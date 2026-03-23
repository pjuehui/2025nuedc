#include "hardware/ADS8688.h"
#include "hardware/delay.h"

/**********************************************************************************************************
*	函 数 名: ADS8688_IO_Init
*	功能说明: ADS8688 IO口初始化
*	形    参: 无
*	返 回 值: 无
**********************************************************************************************************/
void ADS8688_IO_Init(void) //ADS8688 IO口初始化
{
  DAISY_L8688;
  CS_H8688;
  RST_H8688;
  CLK_L8688;
  delay_ms(5);
}

/**********************************************************************************************************
*	函 数 名: ADS8688_Init
*	功能说明: ADS8688初始化
*	形    参: 无
*	返 回 值: 无
**********************************************************************************************************/

void ADS8688_Init(uint8_t ch_en) //ADS8688初始化
{	
  ADS8688_IO_Init();
  SOFT_RESET();                                        //软件复位
  delay_ms(25);                                        //手册上没找到具体的，基本上是ns级，这里设置25ms
  Set_Auto_Scan_Sequence(ch_en);                  //使能通道
  ADS8688_WriteReg(CH_PD, ~ch_en);         //通道退出低功耗状态 通道上电
  delay_ms(50);                                       //手册上说退出低功耗至少15ms

  while (ADS8688_ReadReg(AUTO_SEQ_EN) != ch_en) //是否使能成功
  {
    Set_Auto_Scan_Sequence(ch_en);               //使能通道
    ADS8688_WriteReg(CH_PD,~ch_en);       //通道退出低功耗状态 通道上电
    delay_ms(50);
  }
}


/**********************************************************************************************************
*	函 数 名: ADS8688_SPI_Read8Bit
*	功能说明: 从SPI总线读8个bit数据。 不带CS控制。
*	形    参: 无
*	返 回 值: 无
**********************************************************************************************************/
uint8_t ADS8688_SPI_Read8Bit(void)
{
  uint8_t i = 0;
  uint8_t read = 0;
  for (i = 0; i < 8; i++)
  {
    read <<= 1;
    CLK_H8688;
    delay_us(1);
    if (READ_SDO)
     read++;
    CLK_L8688;
    delay_us(1);
  }
  return read;
}


/**********************************************************************************************************
*	函 数 名: ADS8688_SPI_Write8Bit
*	功能说明: 向SPI总线写8个bit数据。 不带CS控制。
*	形    参: data : 数据
*	返 回 值: 无
**********************************************************************************************************/
void ADS8688_SPI_Write8Bit(uint8_t data)
{
  uint8_t i = 0;
  CS_L8688;
  for (i = 0; i < 8; i++)
  {
    if (data & 0x80)
      SDI_H8688;
    else
      SDI_L8688;
    data <<= 1;
    CLK_H8688;
    delay_us(1);
    CLK_L8688;
    delay_us(1);
  }
}



/**********************************************************************************************************
*	函 数 名: ADS8688_WriteCmd
*	功能说明: 写命令寄存器
*	形    参:  cmd : 命令
*	返 回 值: 无
**********************************************************************************************************/
void ADS8688_WriteCmd(uint16_t cmd) //写ADS8688命令寄存器
{
  CS_L8688;
  ADS8688_SPI_Write8Bit(cmd >> 8 & 0XFF);
  ADS8688_SPI_Write8Bit(cmd & 0XFF);  
  ADS8688_SPI_Write8Bit(0X00);
  ADS8688_SPI_Write8Bit(0X00);
  CS_H8688;
}


/*
*********************************************************************************************************
*	函 数 名: SOFT_RESET
*	功能说明: 软件复位
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void SOFT_RESET(void) //软件复位
{
  ADS8688_WriteCmd(RST);
}



/**********************************************************************************************************
*	函 数 名: AUTO_RST_Mode
*	功能说明: 自动模式
*	形    参: 无
*	返 回 值: 无
**********************************************************************************************************/
void AUTO_RST_Mode(void) //自动扫描模式
{
  ADS8688_WriteCmd(AUTO_RST);
}


/**********************************************************************************************************
*	函 数 名: MAN_CH_Mode
*	功能说明: 手动模式通道选择
*	形    参:  ch : 要选择的通道
*	返 回 值: 无
**********************************************************************************************************/
void MAN_CH_Mode(uint16_t ch) //手动模式
{
  ADS8688_WriteCmd(ch);
}


/**********************************************************************************************************
*	函 数 名: ADS8688_ReadReg
*	功能说明: 读指定的寄存器
*	形    参:  addr : 寄存器地址
*	返 回 值: data : 读到的寄存器值。
**********************************************************************************************************/
uint8_t ADS8688_ReadReg(uint8_t addr)  
{
  uint8_t data = 0;
  CS_L8688;
  ADS8688_SPI_Write8Bit(addr << 1);
  data = ADS8688_SPI_Read8Bit();
  data = ADS8688_SPI_Read8Bit();
  CS_H8688;
  return data;
}


/**********************************************************************************************************
*	函 数 名: ADS8688_WriteReg
*	功能说明: 写指定的寄存器
*	形    参:  addr : 寄存器地址
*			  		data : 寄存器值
*	返 回 值: 无
**********************************************************************************************************/
void ADS8688_WriteReg(uint8_t addr, uint8_t data)
{
  CS_L8688;
  ADS8688_SPI_Write8Bit((addr << 1) | 0X01);
  ADS8688_SPI_Write8Bit(data);
  CS_H8688;
}


/**********************************************************************************************************
*	函 数 名: Set_Auto_Scan_Sequence
*	功能说明: 设置自动扫描序列通道
*	形    参:  seq：需要设置自动扫描序列使能寄存器的值
*	返 回 值: 无
**********************************************************************************************************/
void Set_Auto_Scan_Sequence(uint8_t seq)
{
  ADS8688_WriteReg(AUTO_SEQ_EN, seq);
}


/**********************************************************************************************************
*	函 数 名: Set_CH_Range
*	功能说明: 设置指定通道测量范围
*	形    参:  ch : 通道
*			  		range : 测量范围
*	返 回 值: 无
**********************************************************************************************************/
void Set_CH_Range(uint8_t ch, uint8_t range)
{
  ADS8688_WriteReg(ch, range);
}


/**********************************************************************************************************
*	函 数 名: Get_AUTO_RST_Mode_Data
*	功能说明: 读取自动扫描模式AD值
*	形    参:data : 数据指针
*			  	chnum : 通道个数
*	返 回 值: 无
**********************************************************************************************************/
void Get_AUTO_RST_Mode_Data(uint16_t *data, uint8_t chnum)
{
  uint8_t i = 0, datal = 0, datah = 0;
  for (i = 0; i < chnum; i++)
  {
    CS_L8688;
    ADS8688_SPI_Write8Bit(0X00);
    ADS8688_SPI_Write8Bit(0X00);
    datah = ADS8688_SPI_Read8Bit();
    datal = ADS8688_SPI_Read8Bit();
    //HAL_Delay(1);
    CS_H8688;
    *(data + i) = datah << 8 | datal;
  }
}


/**********************************************************************************************************
*	函 数 名: Get_MAN_CH_Mode_Data
*	功能说明: 读取手动模式AD值
*	形    参: 无
*	返 回 值: uint16_t,AD值
**********************************************************************************************************/
uint16_t Get_MAN_CH_Mode_Data(void)
{
  uint8_t datah = 0, datal = 0;
  CS_L8688;
  ADS8688_SPI_Write8Bit(0X00);
  ADS8688_SPI_Write8Bit(0X00);
  datah = ADS8688_SPI_Read8Bit();
  datal = ADS8688_SPI_Read8Bit();
  CS_H8688;
  return (datah << 8 | datal);
}
/**********************************************************************************************************
* Bytrain's Part
* 创建：2025.7.5
* 更新：2025.7.5
**********************************************************************************************************/
static uint8_t Flag_Range = 0;
ADS8688_TypeDef ADS = {ADS8688_Init,ADS8688_Use};//须按照顺序定义
/*通道范围选择*/
static void ADS8688_Man_Set(uint8_t ADS8688_Ch_Type,uint8_t range)
{
    switch (ADS8688_Ch_Type) //通道范围选择
    {
    case  ADS8688_CH_0  :
        Set_CH_Range(CHIR_0,range);
        MAN_CH_Mode(MAN_CH_0);
        break;
    case  ADS8688_CH_1  :
        Set_CH_Range(CHIR_1,range);
        MAN_CH_Mode(MAN_CH_1);
        break;
    case  ADS8688_CH_2  :
        Set_CH_Range(CHIR_2,range);
        MAN_CH_Mode(MAN_CH_2);
        break;
    case  ADS8688_CH_3  :
        Set_CH_Range(CHIR_3,range);
        MAN_CH_Mode(MAN_CH_3);
        break;
    case  ADS8688_CH_4  :
        Set_CH_Range(CHIR_4,range);
        MAN_CH_Mode(MAN_CH_4);
        break;
    case  ADS8688_CH_5  :
        Set_CH_Range(CHIR_5,range);
        MAN_CH_Mode(MAN_CH_5);
        break;
    case  ADS8688_CH_6  :
        Set_CH_Range(CHIR_6,range);
        MAN_CH_Mode(MAN_CH_6);
        break;
    case  ADS8688_CH_7  :
        Set_CH_Range(CHIR_7,range);
        MAN_CH_Mode(MAN_CH_7);
        break;
    default: 
        break; 
    }
    switch (range) //范围选择判断，为之后计算选择不同LSB做准备
    {
    case  ADS8688_IR_N2_5V  :
      Flag_Range = 1;
      break;
    case  ADS8688_IR_N1_25V  :
      Flag_Range = 2;
      break;
    case  ADS8688_IR_N0_625V  :
      Flag_Range = 3;
      break;
    case  ADS8688_IR_2_5V  :
      Flag_Range = 4;
      break;        
    case  ADS8688_IR_1_25V  :
      Flag_Range = 5;
      break; 
    default:
      Flag_Range = 0; 
      break; 
    }
}
/*读取数据*/
static float ADS8688_Get_Adcode(void)
{
  uint16_t AdCode = 0;
  float ADS8688_Volt = 0.0;
  AdCode  = Get_MAN_CH_Mode_Data();  
  switch (Flag_Range) //判断选择范围，选择不同LSB和计算方法
  {
  case  1  :
      ADS8688_Volt = (AdCode-32768)*CONST_N2_5V_LSB_mV/1000;
      break;
  case  2  :
      ADS8688_Volt = (AdCode-32768)*CONST_N1_25V_LSB_mV/1000;
      break;       
  case  3  :
      ADS8688_Volt = (AdCode-32768)*CONST_N0_625V_LSB_mV/1000;
      break;
  case  4  :
      ADS8688_Volt = AdCode*CONST_2_5V_LSB_mV/1000;
      break;
  case  5  :
      ADS8688_Volt = AdCode*CONST_1_25V_LSB_mV/1000;
      break;
  default:
      break;               
  }
  return (ADS8688_Volt); 
}
/*选择通道范围并读取输出结果*/
static float ADS8688_Use(uint8_t ADS8688_Ch_Type,uint8_t range)
{
  float ADS8688_Volt =0.0;
  ADS8688_Man_Set(ADS8688_Ch_Type ,range);
  ADS8688_Volt = ADS8688_Get_Adcode();
  return (ADS8688_Volt); 
}





