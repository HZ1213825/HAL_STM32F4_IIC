#ifndef __MYI2C_H
#define __MYI2C_H
#include "main.h"
#include "Print.h"

#define I2C_Address 0x54
#define I2C_SCL_GPIOx GPIOA
#define I2C_SCL_Pin GPIO_PIN_0
#define I2C_SDA_GPIOx GPIOA
#define I2C_SDA_Pin GPIO_PIN_1
#define I2C_SDA_Pinx 1 // GPIO_PIN_x 写x

/*
IIC主机
需要:2个GPIO
    开漏上拉输出模式
*/

// #define I2C_Host

/*
IIC从机
需要:2个GPIO
    边沿中断,上拉
*/

#define I2C_Slave

extern uint8_t I2C_Slave_Add;
extern uint8_t Reg_Add;
extern uint8_t I2C_Slave_RX_Buf;

// IIC所有操作函数
uint8_t I2C_SendByte(uint8_t dat); // I2C发送一个字节
uint8_t I2C_ReadByte(uint8_t ack); // I2C读取一个字节
uint8_t I2C_Write_Reg(uint8_t reg, uint8_t data);
uint8_t I2C_Read_Reg(uint8_t reg);
uint8_t I2C_Read_Len(uint8_t reg, uint8_t len, uint8_t *buf);
uint8_t I2C_Write_Len(uint8_t reg, uint8_t len, uint8_t *buf);
void I2C_Slave_IRQ_SCL(void);
void I2C_Slave_IRQ_SDA(void);
#endif
