/*
 * FreeRTOS Wi-Fi V1.0.0
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/**
 * @file iot_wifi.c
 * @brief Wi-Fi Interface.
 */

/* Socket and Wi-Fi interface includes. */
#include "FreeRTOS.h"
#include "iot_wifi.h"

/* Wi-Fi configuration includes. */
#include "aws_wifi_config.h"

/* Set up logging for this demo. */
#include "iot_demo_logging.h"

#include "esp32_AT.h"
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_On( void )
{IotLogInfo("iot_wifi.c: %s not implemented\r\n", __func__);
    /* FIX ME. */
#if 0
    esp32_init(115200);
    esp32_create_tasks();
//	/* Restart esp32 module */
//	esp32_reset_module();  // reset esp32
//	esp32_echo_off();  // switch echo off
#endif
    return eWiFiFailure;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Off( void )
{IotLogInfo("iot_wifi.c: %s not implemented\r\n", __func__);
    /* FIX ME. */
    return eWiFiFailure;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_ConnectAP( const WIFINetworkParams_t * const pxNetworkParams )
{IotLogError("iot_wifi.c: %s not implemented\r\n", __func__);
IotLogInfo("SSID = %s\r\n", pxNetworkParams->pcSSID);
IotLogInfo("Password = %s\r\n", pxNetworkParams->pcPassword);

#if 0
    esp32_echo_off();
    esp32_wifi_conn(pxNetworkParams->pcSSID, pxNetworkParams->pcPassword);
#endif
    /* FIX ME. */
    return eWiFiFailure;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Disconnect( void )
{IotLogInfo("iot_wifi.c: %s not implemented\r\n", __func__);
    /* FIX ME. */
    return eWiFiFailure;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Reset( void )
{IotLogInfo("iot_wifi.c: %s not implemented\r\n", __func__);
    /* FIX ME. */
    esp32_reset_module();

    return eWiFiFailure;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Scan( WIFIScanResult_t * pxBuffer,
                            uint8_t ucNumNetworks )
{IotLogInfo("iot_wifi.c: %s not implemented\r\n", __func__);
    /* FIX ME. */
    return eWiFiFailure;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_SetMode( WIFIDeviceMode_t xDeviceMode )
{IotLogInfo("iot_wifi.c: %s not implemented\r\n", __func__);
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetMode( WIFIDeviceMode_t * pxDeviceMode )
{IotLogInfo("iot_wifi.c: %s not implemented\r\n", __func__);
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkAdd( const WIFINetworkProfile_t * const pxNetworkProfile,
                                  uint16_t * pusIndex )
{IotLogInfo("iot_wifi.c: %s not implemented\r\n", __func__);
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkGet( WIFINetworkProfile_t * pxNetworkProfile,
                                  uint16_t usIndex )
{IotLogInfo("iot_wifi.c: %s not implemented\r\n", __func__);
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkDelete( uint16_t usIndex )
{IotLogInfo("iot_wifi.c: %s not implemented\r\n", __func__);
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Ping( uint8_t * pucIPAddr,
                            uint16_t usCount,
                            uint32_t ulIntervalMS )
{IotLogInfo("iot_wifi.c: %s not implemented\r\n", __func__);
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetIP( uint8_t * pucIPAddr )
{IotLogInfo("iot_wifi.c: %s not implemented\r\n", __func__);
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetMAC( uint8_t * pucMac )
{IotLogInfo("iot_wifi.c: %s not implemented\r\n", __func__);
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetHostIP( char * pcHost,
                                 uint8_t * pucIPAddr )
{IotLogInfo("iot_wifi.c: %s not implemented\r\n", __func__);
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_StartAP( void )
{IotLogInfo("iot_wifi.c: %s not implemented\r\n", __func__);
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_StopAP( void )
{IotLogInfo("iot_wifi.c: %s not implemented\r\n", __func__);
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_ConfigureAP( const WIFINetworkParams_t * const pxNetworkParams )
{IotLogInfo("iot_wifi.c: %s not implemented\r\n", __func__);
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_SetPMMode( WIFIPMMode_t xPMModeType,
                                 const void * pvOptionValue )
{IotLogInfo("iot_wifi.c: %s not implemented\r\n", __func__);
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetPMMode( WIFIPMMode_t * pxPMModeType,
                                 void * pvOptionValue )
{IotLogInfo("iot_wifi.c: %s not implemented\r\n", __func__);
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

BaseType_t WIFI_IsConnected(void)
{IotLogInfo("iot_wifi.c: %s not implemented\r\n", __func__);
	/* FIX ME. */
	return pdFALSE;
}

WIFIReturnCode_t WIFI_RegisterNetworkStateChangeEventCallback( IotNetworkStateChangeEventCallback_t xCallback  )
{IotLogInfo("iot_wifi.c: %s not implemented\r\n", __func__);
	/** Needs to implement dispatching network state change events **/
	return eWiFiNotSupported;
}

/*-----------------------------------------------------------*/
#include "FreeRTOS.h"
#include "semphr.h"
#include "event_groups.h"
#include "FreeRTOS_IP.h"
void vApplicationPingReplyHook( ePingReplyStatus_t eStatus,
                                uint16_t usIdentifier )
{IotLogInfo("iot_wifi.c: %s not implemented\r\n", __func__);
    /*handle ping reply. */
    switch( eStatus )
    {
        case eSuccess:
//            /* A valid ping reply has been received. */
//            xQueueSend( xPingReplyQueue, &usIdentifier, pdMS_TO_TICKS( 10 ) );
            break;

        case eInvalidChecksum:
        case eInvalidData:
            /* A reply was received but it was not valid. */
            break;
    }
}
