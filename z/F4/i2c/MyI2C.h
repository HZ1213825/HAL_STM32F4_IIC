#ifndef __MYI2C_H
#define __MYI2C_H
#include "main.h"
#define I2C_Address 0x54
#define I2C_SCL_GPIOx GPIOA
#define I2C_SCL_Pin GPIO_PIN_0
#define I2C_SDA_GPIOx GPIOA
#define I2C_SDA_Pin GPIO_PIN_1
// IIC所有操作函数
void I2C_Delay(void);              // I2C延时函数
void I2C_Start(void);              //发送I2C开始信号
void I2C_End(void);                //发送I2C停止信号
uint8_t I2C_SendByte(uint8_t dat); // I2C发送一个字节
uint8_t I2C_ReadByte(uint8_t ack); // I2C读取一个字节
uint8_t I2C_Write_Reg(uint8_t reg, uint8_t data);
uint8_t I2C_Read_Reg(uint8_t reg);
uint8_t I2C_Read_Len(uint8_t reg, uint8_t len, uint8_t *buf);
uint8_t I2C_Write_Len(uint8_t reg, uint8_t len, uint8_t *buf);
#endif
