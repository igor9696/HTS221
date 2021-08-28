/*
 * LPS22HB.c
 *
 *  Created on: 25 sie 2021
 *      Author: igur
 */


#include "LPS22HB.h"

static uint8_t read8(LPS22HB_t* sensor, uint8_t reg_addr)
{
	uint8_t tmp;
	HAL_I2C_Mem_Read(sensor->i2c_handler, sensor->DEVICE_ADDR, reg_addr, 1, &tmp, 1, 1000);
	return tmp;
}

static void write8(LPS22HB_t* sensor, uint8_t reg_addr, uint8_t* data)
{
	HAL_I2C_Mem_Write(sensor->i2c_handler, sensor->DEVICE_ADDR, reg_addr, 1, data, 1, 1000);
}


static int32_t read24(LPS22HB_t* sensor, uint8_t reg_addr)
{
	uint8_t buff[3];
	HAL_I2C_Mem_Read(sensor->i2c_handler, sensor->DEVICE_ADDR, reg_addr, 1, buff, 3, 1000);
	return (int32_t)(buff[2] << 16) | (buff[1] << 8) | buff[0];
}

static int16_t read16(LPS22HB_t* sensor, uint8_t reg_addr)
{
	uint8_t buff[2];
	HAL_I2C_Mem_Read(sensor->i2c_handler, sensor->DEVICE_ADDR, reg_addr, 1, buff, 2, 1000);
	return (int16_t)(buff[1] << 8) | (buff[0]);
}

////////////////////////////////////////////////////////////////////////////






uint8_t LPS22HB_Init(LPS22HB_t* sensor, uint8_t device_address, I2C_HandleTypeDef* i2c_handler)
{
	sensor->i2c_handler = i2c_handler;
	sensor->DEVICE_ADDR = device_address << 1;

	// check sensor ID
	uint8_t sensor_id;
	sensor_id = read8(sensor, LPS_WHO_AM_I);
	if(sensor_id != LPS_ID)
	{
		return 1; // error
	}


	return 0;
}


void LPS22HB_GetDataOneHot(LPS22HB_t* sensor, int32_t* pressure_data, uint16_t* temperature_data)
{
	// set ONE-SHOT bit in CTRL_REG2 to 1
	uint8_t tmp;
	tmp = read8(sensor, LPS_CTRL_REG2);
	tmp |= 0x01;
	write8(sensor, LPS_CTRL_REG2, &tmp);

	// pull data from registers
	int32_t press_raw = 0;
	int16_t temp_raw = 0;

	press_raw = read24(sensor, PRESS_OUT_XL);
	temp_raw = read16(sensor, TEMP_OUT_L);

	// calculations
	*pressure_data = press_raw / PRESSURE_ACCURACY;
	*temperature_data = temp_raw / TEMPERATURE_ACCURACY;
}

void LPS22HB_GetData(LPS22HB_t* sensor, int16_t* pressure_data, int16_t* temperature_data)
{
	// pull data from registers
	int32_t press_raw = 0;
	int16_t temp_raw = 0;

	press_raw = read24(sensor, PRESS_OUT_XL);
	temp_raw = read16(sensor, TEMP_OUT_L);

	// calculations
	*pressure_data = press_raw / PRESSURE_ACCURACY;
	*temperature_data = temp_raw / TEMPERATURE_ACCURACY;
}




void LPS22HB_Set_ODR(LPS22HB_t* sensor, uint8_t ODR)
{
	if(ODR > 5) ODR = 5;

	uint8_t tmp;
	tmp = read8(sensor, LPS_CTRL_REG1);
	tmp &= 0x8F; // mask ODR bits

	tmp |= (ODR << 4);
	write8(sensor, LPS_CTRL_REG1, &tmp);
}

void LPS22HB_Set_DRDY_Signal(LPS22HB_t* sensor, uint8_t active_level, uint8_t output_type, uint8_t enable)
{
	uint8_t tmp;
	tmp = read8(sensor, LPS_CTRL_REG3);

	tmp &= 0x38; // xx111xxx clear register

	if(enable != 0)
	{
		tmp |= 0x04; // set 1 on DRDY bit
	}

	if(output_type == DRDY_OpenDrain)
	{
		tmp |= (1 << 6); // set 1 on PP_OD bit
	}

	if(active_level == DRDY_ActiveLow)
	{
		tmp |= (1 << 7); // set 1 on INT_H_L bit
	}

	write8(sensor, LPS_CTRL_REG3, &tmp);
}



