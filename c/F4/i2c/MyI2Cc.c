#include "MyI2C.h"

/**
 * @brief 一段延迟
 * @param 无
 * @return 无
 * @author HZ12138
 * @date 2022-07-27 08:53:30
 */
void I2C_Delay(void)
{
	int z = 0xff;
	while (z--)
		;
}
/**
 * @brief 写SDA
 * @param H_L:高低电平
 * @return 无
 * @author HZ12138
 * @date 2022-10-21 18:07:18
 */
void I2C_Write_SDA(GPIO_PinState H_L)
{
	HAL_GPIO_WritePin(I2C_SDA_GPIOx, I2C_SDA_Pin, H_L);
}
/**
 * @brief 写SCL
 * @param H_L:高低电平
 * @return 无
 * @author HZ12138
 * @date 2022-10-21 18:07:40
 */
void I2C_Write_SCL(GPIO_PinState H_L)
{
	HAL_GPIO_WritePin(I2C_SCL_GPIOx, I2C_SCL_Pin, H_L);
}
/**
 * @brief 读取SDA
 * @param 无
 * @return SDA的状态
 * @author HZ12138
 * @date 2022-10-21 18:07:56
 */
uint16_t I2C_Read_SDA(void)
{
	return HAL_GPIO_ReadPin(I2C_SDA_GPIOx, I2C_SDA_Pin);
}
/**
 * @brief 读取SCL
 * @param 无
 * @return SDA的状态
 * @author HZ12138
 * @date 2022-10-21 18:07:56
 */
uint16_t I2C_Read_SCL(void)
{
	return HAL_GPIO_ReadPin(I2C_SCL_GPIOx, I2C_SCL_Pin);
}
#ifdef I2C_Host
/**
 * @brief 产生I2C起始信号
 * @param 无
 * @return 无
 * @author HZ12138
 * @date 2022-07-27 08:54:48
 */
void I2C_Start(void)
{
	I2C_Write_SDA(GPIO_PIN_SET);   // 需在SCL之前设定
	I2C_Write_SCL(GPIO_PIN_SET);   // SCL->高
	I2C_Delay();				   // 延时
	I2C_Write_SDA(GPIO_PIN_RESET); // SDA由1->0,产生开始信号
	I2C_Delay();				   // 延时
	I2C_Write_SCL(GPIO_PIN_RESET); // SCL->低
}
/**
 * @brief 产生I2C结束信号
 * @param 无
 * @return 无
 * @author HZ12138
 * @date 2022-07-27 08:57:03
 */
void I2C_End(void)
{
	I2C_Write_SDA(GPIO_PIN_RESET); // 在SCL之前拉低
	I2C_Write_SCL(GPIO_PIN_SET);   // SCL->高
	I2C_Delay();				   // 延时
	I2C_Write_SDA(GPIO_PIN_SET);   // SDA由0->1,产生结束信号
	I2C_Delay();				   // 延时
}
/**
 * @brief 发送应答码
 * @param ack:0 应答 1 不应达
 * @return 无
 * @author HZ12138
 * @date 2022-07-27 09:03:38
 */
void I2C_Send_ACK(uint8_t ack)
{
	if (ack == 1)
		I2C_Write_SDA(GPIO_PIN_SET); // 产生应答电平
	else
		I2C_Write_SDA(GPIO_PIN_RESET);
	I2C_Delay();
	I2C_Write_SCL(GPIO_PIN_SET);   // 发送应答信号
	I2C_Delay();				   // 延时至少4us
	I2C_Write_SCL(GPIO_PIN_RESET); // 整个期间保持应答信号
}
/**
 * @brief 接受应答码
 * @param 无
 * @return 应答码 0 应答 1 不应达
 * @author HZ12138
 * @date 2022-07-27 09:04:28
 */
uint8_t I2C_Get_ACK(void)
{
	uint8_t ret;				 // 用来接收返回值
	I2C_Write_SDA(GPIO_PIN_SET); // 电阻上拉,进入读
	I2C_Delay();
	I2C_Write_SCL(GPIO_PIN_SET); // 进入应答检测
	I2C_Delay();				 // 至少延时4us
	ret = I2C_Read_SDA();		 // 保存应答信号
	I2C_Write_SCL(GPIO_PIN_RESET);
	return ret;
}
/**
 * @brief I2C写1Byte
 * @param dat:1Byte数据
 * @return 应答结果 0 应答 1 不应达
 * @author HZ12138
 * @date 2022-07-27 09:05:14
 */
uint8_t I2C_SendByte(uint8_t dat)
{
	uint8_t ack;
	for (int i = 0; i < 8; i++)
	{
		// 高在前低在后
		if (dat & 0x80)
			I2C_Write_SDA(GPIO_PIN_SET);
		else
			I2C_Write_SDA(GPIO_PIN_RESET);
		I2C_Delay();
		I2C_Write_SCL(GPIO_PIN_SET);
		I2C_Delay(); // 延时至少4us
		I2C_Write_SCL(GPIO_PIN_RESET);
		dat <<= 1; // 低位向高位移动
	}

	ack = I2C_Get_ACK();

	return ack;
}
/**
 * @brief I2C读取1Byte数据
 * @param ack:应答 0 应答 1 不应达
 * @return 接受到的数据
 * @author HZ12138
 * @date 2022-07-27 09:06:13
 */
uint8_t I2C_ReadByte(uint8_t ack)
{
	uint8_t ret = 0;
	I2C_Write_SDA(GPIO_PIN_SET);
	for (int i = 0; i < 8; i++)
	{
		ret <<= 1;
		I2C_Write_SCL(GPIO_PIN_SET);
		I2C_Delay();
		// 高在前低在后
		if (I2C_Read_SDA())
		{
			ret++;
		}
		I2C_Write_SCL(GPIO_PIN_RESET);
		I2C_Delay();
	}

	I2C_Send_ACK(ack);

	return ret;
}
/**
 * @brief I2C连续写
 * @param addr:器件地址
 * @param reg:寄存器地址
 * @param len:长度
 * @param buf:缓冲区地址
 * @return 状态 0成功 其他失败
 * @author HZ12138
 * @date 2022-08-08 15:47:11
 */
uint8_t I2C_Write_Len(uint8_t reg, uint8_t len, uint8_t *buf)
{
	uint8_t i;
	I2C_Start();
	I2C_SendByte(I2C_Address | 0); // 发送器件地址+写命令
	I2C_SendByte(reg);			   // 写寄存器地址
	for (i = 0; i < len; i++)
	{
		I2C_SendByte(buf[i]); // 发送数据
	}
	I2C_End();
	return 0;
}
/**
 * @brief I2C连续读
 * @param addr:器件地址
 * @param reg:寄存器地址
 * @param len:长度
 * @param buf:缓冲区地址
 * @return 状态 0成功 其他失败
 * @author HZ12138
 * @date 2022-08-08 15:47:11
 */
uint8_t I2C_Read_Len(uint8_t reg, uint8_t len, uint8_t *buf)
{
	I2C_Start();
	I2C_SendByte(I2C_Address | 0); // 发送器件地址+写命令
	I2C_SendByte(reg);			   // 写寄存器地址
	I2C_Start();
	I2C_SendByte(I2C_Address | 1); // 发送器件地址+读命令
	while (len)
	{
		if (len == 1)
			*buf = I2C_ReadByte(1); // 读数据,发送nACK
		else
			*buf = I2C_ReadByte(0); // 读数据,发送ACK
		len--;
		buf++;
	}
	I2C_End(); // 产生一个停止条件
	return 0;
}
/**
 * @brief I2C写一个字节
 * @param reg:寄存器地址
 * @param data:数据
 * @return 状态 0成功 其他失败
 * @author HZ12138
 * @date 2022-08-08 15:47:11
 */
uint8_t I2C_Write_Reg(uint8_t reg, uint8_t data)
{
	I2C_Start();
	I2C_SendByte(I2C_Address | 0); // 发送器件地址+写命令
	I2C_SendByte(reg);			   // 写寄存器地址
	I2C_SendByte(data);			   // 发送数据
	I2C_End();
	return 0;
}
/**
 * @brief I2C读一个字节
 * @param reg:寄存器地址
 * @return 读取到的数据
 * @author HZ12138
 * @date 2022-08-08 15:47:11
 */
uint8_t I2C_Read_Reg(uint8_t reg)
{
	uint8_t res;
	I2C_Start();
	I2C_SendByte(I2C_Address | 0); // 发送器件地址+写命令
	I2C_SendByte(reg);			   // 写寄存器地址
	I2C_Start();
	I2C_SendByte(I2C_Address | 1); // 发送器件地址+读命令
	res = I2C_ReadByte(1);		   // 读取数据,发送nACK
	I2C_End();					   // 产生一个停止条件
	return res;
}
#endif
//
//
//
//
//
//
//
//
//
//
//
#ifdef I2C_Slave
uint8_t I2C_Slave_Ins = 0;
uint8_t I2C_Slave_zj = 0;
uint8_t I2C_Slave_num = 0;
uint8_t I2C_Slave_Add = 0;
uint8_t Reg_Add = 0;
uint8_t I2C_Slave_RX_Buf = 0;
uint8_t I2C_Slave_TX_Buf = 0;
uint8_t I2C_Slave_SDA_IRQ_EN = 1;
/**
 * @brief 设置SDA为中断模式
 * @param 无
 * @return 无
 * @author HZ12138
 * @date 2022-12-29 19:56:29
 */
void I2C_Slave_Set_SDA_IT(void)
{
	I2C_SDA_GPIOx->MODER &= ~(3 << (I2C_SDA_Pinx * 2));
	I2C_SDA_GPIOx->MODER |= 0 << I2C_SDA_Pinx * 2;
}
/**
 * @brief 设置SDA为开漏上拉输出
 * @param 无
 * @return 无
 * @author HZ12138
 * @date 2022-12-29 19:56:54
 */
void I2C_Slave_Set_SDA_Out(void)
{
	I2C_SDA_GPIOx->MODER &= ~(3 << (I2C_SDA_Pinx * 2));
	I2C_SDA_GPIOx->MODER |= 1 << I2C_SDA_Pinx * 2;
}
/**
 * @brief SCL上升沿服务函数
 * @param 无
 * @return 无
 * @author HZ12138
 * @date 2022-12-29 21:29:43
 */
void I2C_Slave_IRQ_SCL_Rising(void)
{ // SCL上升沿
	switch (I2C_Slave_Ins)
	{
	case 2: // 器件地址解码

		I2C_Slave_zj <<= 1;
		I2C_Slave_zj |= I2C_Read_SDA();
		I2C_Slave_num++;
		if (I2C_Slave_num == 8) // 数据码
		{
			I2C_Slave_Add = I2C_Slave_zj;
			I2C_Slave_zj = 0;
		}
		else if (I2C_Slave_num == 9) // 应答码
		{
			I2C_Slave_num = 0;
			if (I2C_Slave_Add == (I2C_Address & 0xfe))
				I2C_Slave_Ins = 3; // 到寄存器地址读取态
			else if (I2C_Slave_Add == (I2C_Address | 0x01))
			{
				I2C_Write_SDA(GPIO_PIN_SET);
				I2C_Slave_Set_SDA_Out();
				I2C_Slave_Ins = 5;
				I2C_Slave_zj = 0xaa;
				I2C_Slave_num = 0;
				I2C_Slave_SDA_IRQ_EN = 0;
			}
		}

		break;
	case 3: // 寄存器地址读取

		I2C_Slave_zj <<= 1;
		I2C_Slave_zj |= I2C_Read_SDA();
		I2C_Slave_num++;
		if (I2C_Slave_num == 8) // 数据码
		{
			Reg_Add = I2C_Slave_zj;
		}
		else if (I2C_Slave_num == 9) // 应答码
		{
			I2C_Slave_Ins = 4; // 数据读取
			I2C_Slave_zj = 0;
			I2C_Slave_num = 0;
		}
		break;
	case 4: // 数据读取(主机写)
		I2C_Slave_zj <<= 1;
		I2C_Slave_zj |= I2C_Read_SDA();
		I2C_Slave_num++;
		if (I2C_Slave_num == 9) // 应答码
		{
			I2C_Slave_zj = 0;
			I2C_Slave_num = 0;
		}
		break;

	default:
		break;
	}
}
/**
 * @brief SCL下降沿服务函数
 * @param 无
 * @return 无
 * @author HZ12138
 * @date 2022-12-29 21:29:43
 */
void I2C_Slave_IRQ_SCL_Falling(void)
{ // SCL下降沿
	switch (I2C_Slave_Ins)
	{
	case 1: // 准备态
		I2C_Slave_zj = 0;
		I2C_Slave_num = 0;
		I2C_Slave_Ins = 2; // 到器件地址解码
		break;
	case 5: // 数据发送(主机读)

		if (I2C_Slave_zj & 0x80)
			I2C_Write_SDA(GPIO_PIN_SET);
		else
			I2C_Write_SDA(GPIO_PIN_RESET);
		I2C_Slave_zj <<= 1;
		I2C_Slave_num++;
		if (I2C_Slave_num == 9) // 应答码
		{
			I2C_Slave_num = 0;
			I2C_Write_SDA(GPIO_PIN_SET);
			I2C_Slave_Set_SDA_IT();
			I2C_Slave_SDA_IRQ_EN = 1;
			I2C_Slave_Ins = 0;
		}
		break;
	default:
		break;
	}
}
/**
 * @brief 在SCL中断服务函数中调用
 * @param 无
 * @return 无
 * @author HZ12138
 * @date 2022-12-29 21:28:32
 */
void I2C_Slave_IRQ_SCL(void)
{

	if (I2C_Read_SCL() == GPIO_PIN_SET)
	{ // 上升沿
		I2C_Slave_IRQ_SCL_Rising();
	}
	else
	{ // 下降沿
		I2C_Slave_IRQ_SCL_Falling();
	}
}
/**
 * @brief 在SDA中断服务函数中调用
 * @param 无
 * @return 无
 * @author HZ12138
 * @date 2022-12-29 21:28:32
 */
void I2C_Slave_IRQ_SDA(void)
{
	if (I2C_Slave_SDA_IRQ_EN == 1)
	{
		if (I2C_Read_SCL() == GPIO_PIN_SET)
		{
			if (I2C_Read_SDA() == GPIO_PIN_SET)
			{					   // SDA上升沿
								   // 完成态
				I2C_Slave_Ins = 0; // 到空闲态
				I2C_Slave_Set_SDA_IT();
			}
			else
			{ // SDA下降沿
				I2C_Slave_Set_SDA_IT();
				if (I2C_Slave_Ins == 0) // 空闲态
					I2C_Slave_Ins = 1;	// 到准备态
				else
				{
					I2C_Slave_Ins = 1;
				}
			}
		}
	}
}
#endif
