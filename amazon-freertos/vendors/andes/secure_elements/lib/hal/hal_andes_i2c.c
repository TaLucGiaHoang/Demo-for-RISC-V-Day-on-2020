/**
 * hal_andes_i2c.c
 *
 *  Created on: Oct 19, 2020
 *      Author: HoangSHC
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
// andes driver
#include "Driver_I2C.h"
#include "ae250.h"
#include "i2c_ae250.h"

#include "atca_hal.h"
#include "atca_device.h"
#include "hal_andes_i2c.h"
#include "atca_config.h"

#if 1
extern void vLoggingPrint( const char * pcFormat );
extern void vLoggingPrintf( const char * pcFormat, ... );
#define DEBUG_LOG    //vLoggingPrintf
#else
#define DEBUG_LOG(x)
#endif

/* I2C driver */
extern NDS_DRIVER_I2C Driver_I2C0;
static NDS_DRIVER_I2C* pDrv_I2C = &Driver_I2C0;

/**
 * \defgroup hal_ Hardware abstraction layer (hal_)
 *
 * \brief
 * These methods define the hardware abstraction layer for communicating with a CryptoAuth device
 *
   @{ */

/**
 * \brief
 * Logical to physical bus mapping structure
 */
static ATCAI2CMaster_t i2c_hal_data[MAX_I2C_BUSES];   // map logical, 0-based bus number to index


/* I2C driver of Corvette-F1 */
//extern NDS_DRIVER_VERSION i2c_get_version(void);
//extern NDS_I2C_CAPABILITIES i2c_get_capabilities(void);
extern int32_t i2c0_initialize(NDS_I2C_SignalEvent_t cb_event);
extern int32_t i2c0_uninitialize();
extern int32_t i2c0_power_control(NDS_POWER_STATE state);
extern int32_t i2c0_master_transmit(uint32_t addr, const uint8_t* data, uint32_t num, bool xfer_pending);
extern int32_t i2c0_master_receive(uint32_t addr, uint8_t* data, uint32_t num, bool xfer_pending);
//extern int32_t i2c0_slave_transmit(const uint8_t* data, uint32_t num);
//extern int32_t i2c0_slave_receive(uint8_t* data, uint32_t num);
extern int32_t i2c0_get_datacount();
extern int32_t i2c0_control(uint32_t control, uint32_t arg);

NDS_I2C_STATUS i2c0_get_status();

static void driver_i2c0_uninit(void){
	pDrv_I2C->Uninitialize();
}

static int32_t driver_i2c0_init(char baud, uint32_t addr){
	DEBUG_LOG("driver_i2c0_init\r\n");
	int32_t Status = NDS_DRIVER_OK;

	// default slave mode
	Status = i2c0_initialize(NULL);
	if(Status != NDS_DRIVER_OK){
		return Status;
	}

	Status = i2c0_control(NDS_I2C_BUS_CLEAR, 0);
	if(Status != NDS_DRIVER_OK){
		return Status;
	}

	/* Set full power control */
	Status = i2c0_power_control(NDS_POWER_FULL);
	if(Status != NDS_DRIVER_OK){
	return Status;
	}

	/* Set I2C Bus Speed */
	if(baud == 100000) {
		Status = i2c0_control(NDS_I2C_BUS_SPEED, NDS_I2C_BUS_SPEED_STANDARD); ///< Standard Speed (100kHz)
	} else if(baud == 400000) {
		Status = i2c0_control(NDS_I2C_BUS_SPEED, NDS_I2C_BUS_SPEED_FAST); ///< Fast Speed     (400kHz)
	} else if(baud == 1000000) {
		Status = i2c0_control(NDS_I2C_BUS_SPEED, NDS_I2C_BUS_SPEED_FAST_PLUS); ///< Fast+ Speed    (  1MHz)
	} else if(baud == 3400000) {
		Status = i2c0_control(NDS_I2C_BUS_SPEED, NDS_I2C_BUS_SPEED_HIGH); ///< High Speed     (3.4MHz)
	} else {
		Status = i2c0_control(NDS_I2C_BUS_SPEED, NDS_I2C_BUS_SPEED_STANDARD); ///< Standard Speed (100kHz)
	}
	if(Status != NDS_DRIVER_OK){
		return Status;
	}

	/* Set slave address (7-bit) */
//	Status = i2c0_control(NDS_I2C_OWN_ADDRESS, 0x00 & (~NDS_I2C_ADDRESS_10BIT));
	Status = i2c0_control(NDS_I2C_OWN_ADDRESS, addr/*ATCA_I2C_ECC_ADDRESS*/ & (~NDS_I2C_ADDRESS_10BIT));
	if(Status != NDS_DRIVER_OK){
		return Status;
	}

	return Status;
}

//volatile uint8_t tmp[16] = {0};
//static void Wait_Write_Complete(uint64_t ms){
//	uint64_t i;
//	for (i = 0; i < (ms * CPUFREQ)/1000; ++i) {
//		tmp[i % 16] = (i % 16);
//	}
//}
static int32_t driver_i2c0_write(uint32_t addr, uint32_t len, uint8_t* buf, bool xfer_pending){
//#define MAX_WRITE		256 // 8-bit datacnt 0x00-0xff
//#define WRITE_TIME		2//(ms)
	DEBUG_LOG("driver_i2c0_write\r\n");

	// I2C stop enable.
	// xfer_pending: Transfer operation is pending - Stop condition will not be generated.
	// The bus is busy when a START condition is on bus and it ends when a STOP condition is seen.
	// 10-bit slave address must set STOP bit.
	i2c0_master_transmit(addr, buf, len, xfer_pending); // enable stop bit
//	while(i2c0_get_status().busy);
	atca_delay_ms(3); // wait write complete

	if(i2c0_get_datacount() != len){
		return -1;
	}
//	Wait_Write_Complete(WRITE_TIME);
	return 0;
}

static int driver_i2c0_read(unsigned int addr, unsigned int len, unsigned char buf[], bool xfer_pending) {
//#define MAX_READ		256 // 8-bit datacnt 0x00-0xff
	int ret;

	// xfer_pending is true => no stop condition, still busy
	// since xfer_pending is true, no stop condition and busy bit will always be pull high
	// 10-bit slave address must set STOP bit
	// read data from the I2C
	pDrv_I2C->MasterReceive(addr, buf, len, xfer_pending);
	while(pDrv_I2C->GetStatus().busy);

	if(pDrv_I2C->GetDataCount() != len){
		return -1;
	}

	return 0;
}


/**
 * \brief discover i2c buses available for this hardware
 * this maintains a list of logical to physical bus mappings freeing the application
 * of the a-priori knowledge
 * \param[in] i2c_buses - an array of logical bus numbers
 * \param[in] max_buses - maximum number of buses the app wants to attempt to discover
 * \return ATCA_SUCCESS
 */

ATCA_STATUS hal_i2c_discover_buses(int i2c_buses[], int max_buses)
{
	DEBUG_LOG("hal_i2c_discover_buses - unimplemented\r\n");
    return ATCA_UNIMPLEMENTED;
}

/**
 * \brief discover any CryptoAuth devices on a given logical bus number
 * \param[in]  bus_num  logical bus number on which to look for CryptoAuth devices
 * \param[out] cfg     pointer to head of an array of interface config structures which get filled in by this method
 * \param[out] found   number of devices found on this bus
 * \return ATCA_SUCCESS
 */

ATCA_STATUS hal_i2c_discover_devices(int bus_num, ATCAIfaceCfg cfg[], int *found)
{
	DEBUG_LOG("hal_i2c_discover_devices - unimplemented\r\n");
    return ATCA_UNIMPLEMENTED;
}

/**
 * \brief hal_i2c_init manages requests to initialize a physical interface.  it manages use counts so when an interface
 * has released the physical layer, it will disable the interface for some other use.
 * You can have multiple ATCAIFace instances using the same bus, and you can have multiple ATCAIFace instances on
 * multiple i2c buses, so hal_i2c_init manages these things and ATCAIFace is abstracted from the physical details.
 */

/** \brief initialize an I2C interface using given config
 * \param[in] hal - opaque ptr to HAL data
 * \param[in] cfg - interface configuration
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_i2c_init(void *hal, ATCAIfaceCfg *cfg)
{
    DEBUG_LOG("hal_i2c_init\r\n");

    if (cfg->atcai2c.bus >= MAX_I2C_BUSES)
    {
    	DEBUG_LOG("cfg->atcai2c.bus invalid value\r\n");
        return ATCA_COMM_FAIL;
    }
    ATCAI2CMaster_t* data = &i2c_hal_data[0]; //&i2c_hal_data[cfg->atcai2c.bus];

    if (data->ref_ct <= 0)
    {
        driver_i2c0_init(cfg->atcai2c.baud, cfg->atcai2c.slave_address/*ATCA_I2C_ECC_ADDRESS*/);
        // store this for use during the release phase
        data->bus_index = cfg->atcai2c.bus;
        // buses are shared, this is the first instance
        data->ref_ct = 1;
    }
    else
    {
        // Bus is already is use, increment reference counter
        data->ref_ct++;
    }

    ((ATCAHAL_t*)hal)->hal_data = data;

    DEBUG_LOG("hal_i2c_init ATCA_SUCCESS\r\n");
    return ATCA_SUCCESS;
}

/**
 * \brief HAL implementation of I2C post init
 * \param[in] iface  instance
 * \return ATCA_SUCCESS
 */
ATCA_STATUS hal_i2c_post_init(ATCAIface iface)
{
    DEBUG_LOG("hal_i2c_post_init - unimplemented\r\n");
    return ATCA_SUCCESS;
}

/** \brief HAL implementation of I2C send over START
 * \param[in] iface     instance
 * \param[in] txdata    pointer to space to bytes to send
 * \param[in] txlength  number of bytes to send
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_i2c_send(ATCAIface iface, uint8_t *txdata, int txlength)
{
	DEBUG_LOG("hal_i2c_send\r\n");
	// print could make lost data
//	for(int i = 0; i < txlength; i++)
//	{
//		vLoggingPrintf("%x ", txdata[i]);
//	}vLoggingPrintf("\r\n");

    ATCAIfaceCfg *cfg = atgetifacecfg(iface);

    // for this implementation of I2C with CryptoAuth chips, txdata is assumed to have ATCAPacket format

    // other device types that don't require i/o tokens on the front end of a command need a different hal_i2c_send and wire it up instead of this one
    // this covers devices such as ATSHA204A and ATECCx08A that require a word address value pre-pended to the packet
    // txdata[0] is using _reserved byte of the ATCAPacket
    txdata[0] = 0x03;   // insert the Word Address Value, Command token
    txlength++;         // account for word address value byte.

    // send 7-bit address, enable stop bit
    if (driver_i2c0_write(cfg->atcai2c.slave_address, txlength, (unsigned char*)txdata, false) != 0)
    {
        return ATCA_COMM_FAIL;
    }

    return ATCA_SUCCESS;
}

/**
 * \brief HAL implementation of I2C receive function for START I2C
 * \param[in]    iface     Device to interact with.
 * \param[out]   rxdata    Data received will be returned here.
 * \param[inout] rxlength  As input, the size of the rxdata buffer.
 *                         As output, the number of bytes received.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_i2c_receive(ATCAIface iface, uint8_t *rxdata, uint16_t *rxlength)
{
	DEBUG_LOG("hal_i2c_receive\r\n");
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    int count;
    int retries = cfg->rx_retries;
    int status = !ATCA_SUCCESS;
    uint16_t rxdata_max_size = *rxlength;

    *rxlength = 0;
    if (rxdata_max_size < 1)
    {
        return ATCA_SMALL_BUFFER;
    }

    /* Get number of received bytes */
    while (retries-- > 0 && status != ATCA_SUCCESS)
    {
        if (driver_i2c0_read(cfg->atcai2c.slave_address, 1, (unsigned char*)rxdata, false) != 0)
        {
            status = ATCA_COMM_FAIL;
        }
        else
        {
            status = ATCA_SUCCESS;
        }
    }

    if (status != ATCA_SUCCESS)
    {
        return ATCA_COMM_FAIL;
    }
    if (rxdata[0] < ATCA_RSP_SIZE_MIN)
    {
        return ATCA_INVALID_SIZE;
    }
    if (rxdata[0] > rxdata_max_size)
    {
        return ATCA_SMALL_BUFFER;
    }

    //Update receive length with first byte received and set to read rest of the data
    count  = rxdata[0] - 1;

    /* Receive data */
    if (driver_i2c0_read(cfg->atcai2c.slave_address, count, (unsigned char*)&rxdata[1], false) != 0)
    {
        return ATCA_COMM_FAIL;
    }

    *rxlength = rxdata[0];
//    for(int i; i < *rxlength; i++)
//    {
//    	vLoggingPrintf("%x ", rxdata[i]);
//    }
    DEBUG_LOG("hal_i2c_receive ATCA_SUCCESS\r\n");
    return ATCA_SUCCESS;
}

/**
 * \brief method to change the bus speed of I2C
 * \param[in] iface  interface on which to change bus speed
 * \param[in] speed  baud rate (typically 100000 or 400000)
 */

void change_i2c_speed(ATCAIface iface, uint32_t speed)
{
	int32_t Status = NDS_DRIVER_OK;
	/* Set I2C Bus Speed */
	if(speed == 100000) {
		Status = i2c0_control(NDS_I2C_BUS_SPEED, NDS_I2C_BUS_SPEED_STANDARD); ///< Standard Speed (100kHz)
	} else if(speed == 400000) {
		Status = i2c0_control(NDS_I2C_BUS_SPEED, NDS_I2C_BUS_SPEED_FAST); ///< Fast Speed     (400kHz)
	} else if(speed == 1000000) {
		Status = i2c0_control(NDS_I2C_BUS_SPEED, NDS_I2C_BUS_SPEED_FAST_PLUS); ///< Fast+ Speed    (  1MHz)
	} else if(speed == 3400000) {
		Status = i2c0_control(NDS_I2C_BUS_SPEED, NDS_I2C_BUS_SPEED_HIGH); ///< High Speed     (3.4MHz)
	} else {
		Status = i2c0_control(NDS_I2C_BUS_SPEED, NDS_I2C_BUS_SPEED_STANDARD); ///< Standard Speed (100kHz)
	}
}

/**
 * \brief wake up CryptoAuth device using I2C bus
 * \param[in] iface  interface to logical device to wakeup
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_i2c_wake(ATCAIface iface)
{
//	DEBUG_LOG("hal_i2c_wake\r\n");
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    uint32_t bdrt = cfg->atcai2c.baud;
    uint8_t data[4];

    if (bdrt != 100000)    // if not already at 100KHz, change it
    {
//    	DEBUG_LOG("change_i2c_speed 100khz\r\n");
        change_i2c_speed(iface, 100000);
    }

    // Send 0x00 address as wake pulse
    // Dummy Write
    // This command will always return NACK.
    // So, the return code is being ignored.
    driver_i2c0_write(0x00, 0, (unsigned char*)NULL, false);

    atca_delay_ms(3); // wait tWHI + tWLO which is configured based on device type and configuration structure
    //atca_delay_us(cfg->wake_delay);

    // if necessary, revert baud rate to what came in.
    if (bdrt != 100000)
    {
        change_i2c_speed(iface, bdrt);
    }

    // receive the wake up response
    driver_i2c0_read(cfg->atcai2c.slave_address, 4, (unsigned char*)data, false);

    return hal_check_wake(data, 4);
}

/**
 * \brief idle CryptoAuth device using I2C bus
 * \param[in] iface  interface to logical device to idle
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_i2c_idle(ATCAIface iface)
{
	DEBUG_LOG("hal_i2c_idle\r\n");
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    uint8_t data[4];

    data[0] = 0x02;  // idle word address value

    if (driver_i2c0_write(cfg->atcai2c.slave_address, 1, (unsigned char*)data, false) != 0)
    {
        return ATCA_COMM_FAIL;
    }

    DEBUG_LOG("hal_i2c_idle ATCA_SUCCESS\r\n");
    return ATCA_SUCCESS;
}

/**
 * \brief sleep CryptoAuth device using I2C bus
 * \param[in] iface  interface to logical device to sleep
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_i2c_sleep(ATCAIface iface)
{
	DEBUG_LOG("hal_i2c_sleep\r\n");
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    uint8_t data[4];

    data[0] = 0x01;  // sleep word address value

    if (driver_i2c0_write(cfg->atcai2c.slave_address, 1, (unsigned char*)data, false) != 0)
    {
        return ATCA_COMM_FAIL;
    }

    return ATCA_SUCCESS;
}

/**
 * \brief manages reference count on given bus and releases resource if no more refences exist
 * \param[in] hal_data - opaque pointer to hal data structure - known only to the HAL implementation
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_i2c_release(void *hal_data)
{
	DEBUG_LOG("hal_i2c_release\r\n");
    ATCAI2CMaster_t *hal = (ATCAI2CMaster_t*)hal_data;

    //if the use count for this bus has gone to 0 references, disable it.  protect against an unbracketed release
    if (hal && --(hal->ref_ct) <= 0)
    {
        driver_i2c0_uninit();
        hal->ref_ct = 0;
    }
    return ATCA_SUCCESS;
}

/** @} */
