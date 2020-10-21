/**
 * hal_andes_i2c.h
 *
 *  Created on: Oct 19, 2020
 *      Author: HoangSHC
 */
#ifndef HAL_ANDES_I2C_START_H_
#define HAL_ANDES_I2C_START_H_

#include <stdlib.h>

/** \defgroup hal_ Hardware abstraction layer (hal_)
 *
 * \brief
 * These methods define the hardware abstraction layer for communicating with a CryptoAuth device
 *
   @{ */


#define MAX_I2C_BUSES   1   // Andes Corvette F1 board has 1 I2C

/** \brief this is the hal_data for ATCA HAL for Atmel START SERCOM
 */
typedef struct atcaI2Cmaster
{
    // uint32_t               sercom_core_freq;
    int                    ref_ct;  // reference counter
    // for conveniences during interface release phase
    int bus_index;
} ATCAI2CMaster_t;

void change_i2c_speed(ATCAIface iface, uint32_t speed);

/** @} */

#endif /* HAL_SAMD21_I2C_START_H_ */
