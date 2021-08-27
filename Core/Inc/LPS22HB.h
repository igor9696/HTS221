/*
 * LPS22HB.h
 *
 *  Created on: 25 sie 2021
 *      Author: igur
 */

#ifndef INC_LPS22HB_H_
#define INC_LPS22HB_H_

#include "main.h"


#define LPS_DEV_ADDR 			0x5D
#define LPS_ID					0xB1


// Register addresses
#define LPS_WHO_AM_I			0x0F
#define INTERRUPT_CFG			0x0B
#define LPS_CTRL_REG1			0x10
#define LPS_CTRL_REG2			0x11
#define LPS_CTRL_REG3			0x12

// Output data registers
#define PRESS_OUT_XL			0x28
#define PRESS_OUT_L				0x29
#define PRESS_OUT_H				0x2A
#define TEMP_OUT_L				0x2B
#define TEMP_OUT_H				0x2C




#define PRESSURE_ACCURACY		4096 // 4096 LSB/hPa
#define TEMPERATURE_ACCURACY	100  // 100 counts/C


// Output data rates
#define ONEHOT_mode				0
#define rate_1Hz				1
#define rate_10Hz				2
#define rate_25Hz				3
#define rate_50Hz				4
#define rate_75Hz				5


// DRDY Signal properties
#define DRDY_OpenDrain			1
#define DRDY_PushPull			0
#define DRDY_ActiveHigh			0
#define DRDY_ActiveLow			1


typedef struct LPS22HB_t
{
	I2C_HandleTypeDef* 			i2c_handler;
	uint8_t 					DEVICE_ADDR;


}LPS22HB_t;


uint8_t LPS22HB_Init(LPS22HB_t* sensor, uint8_t device_address, I2C_HandleTypeDef* i2c_handler);
void LPS22HB_GetDataOneHot(LPS22HB_t* sensor, int32_t* pressure_data, uint16_t* temperature_data);
void LPS22HB_GetData(LPS22HB_t* sensor, int32_t* pressure_data, uint16_t* temperature_data);
void LPS22HB_Set_ODR(LPS22HB_t* sensor, uint8_t ODR);
void LPS22HB_Set_DRDY_Signal(LPS22HB_t* sensor, uint8_t active_level, uint8_t output_type, uint8_t enable);

#endif /* INC_LPS22HB_H_ */
