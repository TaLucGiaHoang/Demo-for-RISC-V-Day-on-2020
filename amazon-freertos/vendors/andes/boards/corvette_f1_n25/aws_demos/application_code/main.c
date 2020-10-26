/*
 * Amazon FreeRTOS V1.1.4
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */
#include <stdio.h>
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

/* Platfrom includes. */
#include "uart.h"
#include "platform.h"
#include "mbedtls/platform.h"

/* Demo includes */
#include "aws_demo.h"

/* AWS library includes. */
#include "iot_system_init.h"
#include "iot_logging_task.h"
#include "iot_wifi.h"
#include "aws_clientcredential.h"
#include "aws_application_version.h"
#include "aws_dev_mode_key_provisioning.h"

/* Logging Task Defines. */
#define mainLOGGING_MESSAGE_QUEUE_LENGTH		( 15 )
#define mainLOGGING_TASK_STACK_SIZE			( configMINIMAL_STACK_SIZE * 8 )

/* Unit test defines. */
#define mainTEST_RUNNER_TASK_STACK_SIZE			( configMINIMAL_STACK_SIZE * 16 )

/* The task delay for allowing the lower priority logging task to print out Wi-Fi
 * failure status before blocking indefinitely. */
#define mainLOGGING_WIFI_STATUS_DELAY			pdMS_TO_TICKS( 1000 )

/* The name of the devices for xApplicationDNSQueryHook. */
#define mainDEVICE_NICK_NAME				"Corvette-F1 N25"

/* Static arrays for FreeRTOS-Plus-TCP stack initialization for Ethernet network
 * connections are declared below. If you are using an Ethernet connection on your MCU
 * device it is recommended to use the FreeRTOS+TCP stack. The default values are
 * defined in FreeRTOSConfig.h. */

/* Default MAC address configuration.  The application creates a virtual network
 * connection that uses this MAC address by accessing the raw Ethernet data
 * to and from a real network connection on the host PC.  See the
 * configNETWORK_INTERFACE_TO_USE definition for information on how to configure
 * the real network connection to use. */

static uint8_t ucMACAddress[ 6 ] =
{
    configMAC_ADDR0,
    configMAC_ADDR1,
    configMAC_ADDR2,
    configMAC_ADDR3,
    configMAC_ADDR4,
    configMAC_ADDR5
};

/* The default IP and MAC address used by the application.  The address configuration
 * defined here will be used if ipconfigUSE_DHCP is 0, or if ipconfigUSE_DHCP is
 * 1 but a DHCP server could not be contacted.  See the online documentation for
 * more information. */
static const uint8_t ucIPAddress[ 4 ] =
{
    configIP_ADDR0,
    configIP_ADDR1,
    configIP_ADDR2,
    configIP_ADDR3
};
static const uint8_t ucNetMask[ 4 ] =
{
    configNET_MASK0,
    configNET_MASK1,
    configNET_MASK2,
    configNET_MASK3
};
static const uint8_t ucGatewayAddress[ 4 ] =
{
    configGATEWAY_ADDR0,
    configGATEWAY_ADDR1,
    configGATEWAY_ADDR2,
    configGATEWAY_ADDR3
};
static const uint8_t ucDNSServerAddress[ 4 ] =
{
    configDNS_SERVER_ADDR0,
    configDNS_SERVER_ADDR1,
    configDNS_SERVER_ADDR2,
    configDNS_SERVER_ADDR3
};

/**
 * @brief Application task startup hook for applications using Wi-Fi. If you are not
 * using Wi-Fi, then start network dependent applications in the vApplicationIPNetorkEventHook
 * function. If you are not using Wi-Fi, this hook can be disabled by setting
 * configUSE_DAEMON_TASK_STARTUP_HOOK to 0.
 */
void vApplicationDaemonTaskStartupHook( void );

/**
 * @brief Application IP network event hook called by the FreeRTOS+TCP stack for
 * applications using Ethernet. If you are not using Ethernet and the FreeRTOS+TCP stack,
 * start network dependent applications in vApplicationDaemonTaskStartupHook after the
 * network status is up.
 */
void vApplicationIPNetworkEventHook( eIPCallbackEvent_t eNetworkEvent );

/**
 * @brief Initializes the board.
 */
static void prvMiscInitialization( void );
static void prvSetupHardware( void );
/*-----------------------------------------------------------*/

/*NV seed function*/
extern int nv_seed_read_func( unsigned char *buf, size_t buf_len );
extern int nv_seed_write_func( unsigned char *buf, size_t buf_len );
extern void Init_PKCS11();

/*-----------------------------------------------------------*/
#include "atca_device.h"
#include "atca_iface.h"
#include "hal/atca_hal.h"
#include "basic/atca_basic.h"
#include "basic/atca_basic.h"
static ATCADevice _gDevice_test = NULL;
static struct atca_command g_atcab_command_test;
static struct atca_iface g_atcab_iface_test;
static struct atca_device g_atcab_device_test;

void testATCA( void )
{
	vLoggingPrintf("\r\ntestATCA\r\n");
	ATCA_STATUS status = ATCA_GEN_FAIL;

	ATCAIfaceCfg interface_config = {
			.iface_type = 0x0,
			.devtype = ATECC608A,
			.atcai2c.slave_address = ATCA_I2C_ECC_ADDRESS,
			.atcai2c.bus = 0,
			.atcai2c.baud = 400000,
			.wake_delay = 1500,
			.rx_retries = 20,
			.cfg_data = 0x0,
	};
	ATCAIfaceCfg *cfg = &interface_config;

	g_atcab_iface_test.mType = ATCA_I2C_IFACE;
	g_atcab_iface_test.mIfaceCFG = &interface_config;

	cfg->devtype = ATECC608A;

	g_atcab_command_test.dt = ATECC608A;
	g_atcab_command_test.clock_divider = 0;

	// atcab_init(ATCAIfaceCfg *cfg)
    g_atcab_device_test.mCommands = &g_atcab_command_test;
    g_atcab_device_test.mIface = &g_atcab_iface_test;
    status = initATCADevice(cfg, &g_atcab_device_test);
    vLoggingPrintf("initATCADevice %x\r\n", status);
    if (status != ATCA_SUCCESS)
    {
    	vLoggingPrintf("initATCADevice returns %x\r\n", status);
    }
    _gDevice_test = &g_atcab_device_test;

    // set clock manually
    _gDevice_test->mCommands->clock_divider &= ATCA_CHIPMODE_CLOCK_DIV_MASK;


#if 0 // test hal_
    if (cfg->devtype == ATECC608A)
    {
    	ATCADevice ca_dev = &g_atcab_device_test;
        if ((status = atwake(ca_dev->mIface)) != ATCA_SUCCESS)
        {
        	vLoggingPrintf("atwake returns %x\r\n", status);
        }
//        if ((status = atsleep(ca_dev->mIface)) != ATCA_SUCCESS)
//        {
//        	vLoggingPrintf("atsleep returns %x\r\n", status);
//        }
//        vTaskDelay( (TickType_t) 1000 );
//        if ((status = atwake(ca_dev->mIface)) != ATCA_SUCCESS)
//        {
//        	vLoggingPrintf("atwake returns %x\r\n", status);
//        }
//
//
//        if ((status = atidle(ca_dev->mIface)) != ATCA_SUCCESS)
//        {
//        	vLoggingPrintf("atidle returns %x\r\n", status);
//        }
        vTaskDelay( (TickType_t) 1000 );
    }
#endif

#if 0
    //
    vLoggingPrintf("\r\nTest atcab_init\r\n");
    int retries = 2;
    do
    {
        /* If a PKCS11 was killed an left the device in the idle state then
           starting up again will require the device to go back to a known state
           that is accomplished here by retrying the initalization */
        status = atcab_init(cfg);
        vLoggingPrintf("atcab_init returns %x\r\n", status);
    }
    while (retries-- && status);

    // Test atcab_genkey
    uint8_t public_key[64];
    memset(public_key, 0x44, 64); // mark the key with bogus data
    uint8_t slot_id = 0;

    vLoggingPrintf("\r\nTest atcab_genkey slot %d\r\n", slot_id);
    bool is_locked = false;
    status = atcab_is_locked( slot_id, &is_locked);
    vLoggingPrintf("atcab_is_locked %x\r\n", status);
    if (!is_locked)
    	vLoggingPrintf("Config zone must be locked for this test.\r\n");

    vLoggingPrintf("atcab_genkey\r\n");
    status = atcab_genkey(slot_id, public_key);
    vLoggingPrintf("atcab_genkey returns %x\r\n", status);
    if(status == ATCA_SUCCESS)
    {
    	vLoggingPrintf("<public_key> \r\n%s\r\n<public_key>\r\n", public_key);
    }
    vTaskDelay( (TickType_t) 1000 );

    vLoggingPrintf("atcab_get_pubkey\r\n");
    status = atcab_get_pubkey(slot_id, public_key);
    vLoggingPrintf("atcab_get_pubkey returns %x\r\n", status);
	if(status == ATCA_SUCCESS)
	{
		vLoggingPrintf("<public_key> \r\n%s\r\n<public_key>\r\n", public_key);
		vLoggingPrintf(" [%x %x %x %x]\r\n", public_key[0], public_key[1], public_key[2], public_key[3]);
	}
#endif

}
/*-----------------------------------------------------------*/
/**
 * @brief Application runtime entry point.
 */
int main( void )
{
	 DEV_GPIO->CHANNELDIR |= 0x1E0;                // Set GPIO5~8 direction to output mode.
	 DEV_GPIO->DATAOUT = (1 << 5);                    // Light up LED1
	 DEV_GPIO->DATAOUT = (1 << 8) | (1 << 5);    // Light up LED1 and LED4
	 DEV_GPIO->DATAOUT |= (1 << 6) | (1 << 7);    // Light up LED1 and LED4

   /* Perform any hardware initialization that does not require the RTOS to be
    * running.  */
   prvMiscInitialization();

   /* Create tasks that are not dependent on the Wi-Fi being initialized. */
   xLoggingTaskInitialize( mainLOGGING_TASK_STACK_SIZE,
                           tskIDLE_PRIORITY,
                           mainLOGGING_MESSAGE_QUEUE_LENGTH );

//   FreeRTOS_IPInit( ucIPAddress,
//                    ucNetMask,
//                    ucGatewayAddress,
//                    ucDNSServerAddress,
//                    ucMACAddress );

   /* Start the scheduler.  Initialization that requires the OS to be running,
    * including the Wi-Fi initialization, is performed in the RTOS daemon task
    * startup hook. */
   vTaskStartScheduler();

    return 0;
}
/*-----------------------------------------------------------*/

static void prvMiscInitialization( void )
{
    prvSetupHardware();
}

/*-----------------------------------------------------------*/

void vApplicationDaemonTaskStartupHook( void )
{
#if 0 // original code
    Init_PKCS11();
    mbedtls_platform_set_nv_seed( &nv_seed_read_func, &nv_seed_write_func);
#else // shc test code
///////////
    // test shc
    if( SYSTEM_Init() == pdPASS )//&& xTasksAlreadyCreated == pdFALSE )
    {
//        /* Connect to the Wi-Fi before running the tests. */
//        prvWifiConnect();
        /* A simple example to demonstrate key and certificate provisioning in
         * microcontroller flash using PKCS#11 interface. This should be replaced
         * by production ready key provisioning mechanism. */
        vDevModeKeyProvisioning();
//
//    	testATCA(); // SHC test atecc608a lib
//        /* Start the demo tasks. */
//        DEMO_RUNNER_RunDemos();
    }
#endif
}

/*-----------------------------------------------------------*/

void vApplicationIPNetworkEventHook( eIPCallbackEvent_t eNetworkEvent )
{
//	configPRINT_STRING( "vApplicationIPNetworkEventHook\r\n" );// shc test
#if 0 // shc added to ignore
    if (eNetworkEvent == eNetworkUp)
    {
        configPRINT("Network connection successful.\n\r");
    }

    uint32_t ulIPAddress, ulNetMask, ulGatewayAddress, ulDNSServerAddress;
    char cBuffer[ 16 ];
    static BaseType_t xTasksAlreadyCreated = pdFALSE;

    /* If the network has just come up...*/
    if( eNetworkEvent == eNetworkUp )
    {
        if( SYSTEM_Init() == pdPASS && xTasksAlreadyCreated == pdFALSE )
        {
            /* A simple example to demonstrate key and certificate provisioning in
             * microcontroller flash using PKCS#11 interface. This should be replaced
             * by production ready key provisioning mechanism. */
            vDevModeKeyProvisioning();

            /* Start the demo tasks. */
            DEMO_RUNNER_RunDemos();

            xTasksAlreadyCreated = pdTRUE;
        }

        /* Print out the network configuration, which may have come from a DHCP
        * server. */
        FreeRTOS_GetAddressConfiguration(
            &ulIPAddress,
            &ulNetMask,
            &ulGatewayAddress,
            &ulDNSServerAddress );
        FreeRTOS_inet_ntoa( ulIPAddress, cBuffer );
        FreeRTOS_printf( ( "\r\n\r\nIP Address: %s\r\n", cBuffer ) );

        FreeRTOS_inet_ntoa( ulNetMask, cBuffer );
        FreeRTOS_printf( ( "Subnet Mask: %s\r\n", cBuffer ) );

        FreeRTOS_inet_ntoa( ulGatewayAddress, cBuffer );
        FreeRTOS_printf( ( "Gateway Address: %s\r\n", cBuffer ) );

        FreeRTOS_inet_ntoa( ulDNSServerAddress, cBuffer );
        FreeRTOS_printf( ( "DNS Server Address: %s\r\n\r\n\r\n", cBuffer ) );
    }
#endif
}
/*-----------------------------------------------------------*/

/**
 * @brief User defined Idle task function.
 *
 * @note Do not make any blocking operations in this function.
 */
void vApplicationIdleHook( void )
{

    static TickType_t xLastPrint = 0;
    TickType_t xTimeNow;
    const TickType_t xPrintFrequency = pdMS_TO_TICKS( 5000 );

    xTimeNow = xTaskGetTickCount();

    if( ( xTimeNow - xLastPrint ) > xPrintFrequency )
    {
        configPRINT( "." );
        xLastPrint = xTimeNow;
    }
}
/*-----------------------------------------------------------*/

/**
* @brief User defined application hook to process names returned by the DNS server.
*/
#if ( ipconfigUSE_LLMNR != 0 ) || ( ipconfigUSE_NBNS != 0 )
    BaseType_t xApplicationDNSQueryHook( const char * pcName )
    {
        BaseType_t xReturn;

        /* Determine if a name lookup is for this node.  Two names are given
         * to this node: that returned by pcApplicationHostnameHook() and that set
         * by mainDEVICE_NICK_NAME. */
        if( strcmp( pcName, pcApplicationHostnameHook() ) == 0 )
        {
            xReturn = pdPASS;
        }
        else if( strcmp( pcName, mainDEVICE_NICK_NAME ) == 0 )
        {
            xReturn = pdPASS;
        }
        else
        {
            xReturn = pdFAIL;
        }

        return xReturn;
    }
#endif /* if ( ipconfigUSE_LLMNR != 0 ) || ( ipconfigUSE_NBNS != 0 ) */
/*-----------------------------------------------------------*/

/**
 * @brief User defined assertion call. This function is plugged into configASSERT.
 * See FreeRTOSConfig.h to define configASSERT to something different.
 */
void vAssertCalled(const char * pcFile,
	uint32_t ulLine)
{
    const uint32_t ulLongSleep = 1000UL;
    volatile uint32_t ulBlockVariable = 0UL;
    volatile char * pcFileName = (volatile char *)pcFile;
    volatile uint32_t ulLineNumber = ulLine;

    (void)pcFileName;
    (void)ulLineNumber;

    printf("vAssertCalled %s, %ld\n", pcFile, (long)ulLine);
    fflush(stdout);

    /* Setting ulBlockVariable to a non-zero value in the debugger will allow
     * this function to be exited. */
    taskDISABLE_INTERRUPTS();
    {
        while (ulBlockVariable == 0UL)
        {
            vTaskDelay( pdMS_TO_TICKS( ulLongSleep ) );
        }
    }
    taskENABLE_INTERRUPTS();
}
/*-----------------------------------------------------------*/

/*
 * Platform dependent function
 */

static void prvSetupHardware( void )
{
    /* Ensure no interrupts execute while the scheduler is in an inconsistent
     * state.  Interrupts are automatically enabled when the scheduler is
     * started. */
    portDISABLE_INTERRUPTS();

    /* Enable UART port to output messages. */
    uart_init(DEV_UART2);
//    uart_set_baudrate(DEV_UART2, 115200);
//
//    /* Enable UART1 port communicate with esp32. */
//	*(unsigned int*)0xF0101004 |= 0xA0; // Change (IO2, IO3) pinmux from (GPIO18, GPIO19) to (UART1_RXD, UART1_TXD)
//	uart_init(DEV_UART1);
//	uart_set_baudrate(DEV_UART1, 115200);


}
