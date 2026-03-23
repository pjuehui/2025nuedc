#include "hardware/ad9959.h"
#define ACC_FRE_FACTOR 8.58993459  // (2^(32)-1)/500_000_000

uint8_t CPOW0_DATA[2] = {0x00,0x00};//default Value = 0x0000   @ = POW/2^14*360
uint8_t FR1_DATA[3] = {0xD0,0x00,0x00};//VCO gain control[23]=1系统时钟高于255Mhz; PLL[22:18]=10100,20倍频,20*25M=500MHZ; Charge pump control = 75uA
uint8_t FR2_DATA[2] = {0x00,0x00};  // 双方向扫描，即从起始值扫到结束值后，又从结束值扫到起始值
//uint8_t FR2_DATA[2] = {0x80,0x00};// 单方向扫描，即从起始值扫到结束值后，又从起始值扫到结束值，以此类推

uint8_t CFR_DATA[3] = {0x00,0x03,0x02};//default Value = 0x000302      //通道功能寄存器

uint8_t CH0_ADD[1] = {0x10};     // 开 CH0
uint8_t CH1_ADD[1] = {0x20};      // 开 CH1
uint8_t CH2_ADD[1] = {0x40};      // 开 CH2
uint8_t CH3_ADD[1] = {0x80};      // 开 CH3

uint32_t SinFre[5] = {50, 5000000, 50, 50};
uint32_t SinAmp[5] = {1023, 1023/2, 1023, 1023};
uint32_t SinPhr[5] = {0, 4095, 4095*3, 4095*2};

void AD9959_Init(void)
{
  uint8_t data_init[3]={0xd0,0x00,0x00};
  Init_IO();
  IntReset_IO();
  WriteData_AD9959(FR1_ADD,3,data_init,1);
}
//*****************************
//IO初始化过程
//不使用额外功能时的初始化
//*****************************
void Init_IO(void)
{
   AD9959_PWR_L;
    CS_H;
    SCLK_L;
    UPDATE_L;
    SDIO0_L;
}


void IntReset_IO(void)  //IntReset  复位操作
{
  Reset_L;
    ad9959delay(1);
    Reset_H;
    ad9959delay(30);
    Reset_L;
}

/*--------------------------------------------
函数功能：控制器通过SPI向AD9959写数据
RegisterAddress: 寄存器地址
NumberofRegisters: 所含字节数
*RegisterData: 数据起始地址
temp: 是否更新IO寄存器
----------------------------------------------*/
void WriteData_AD9959(uint8_t RegisterAddress, uint8_t NumberofRegisters, uint8_t *RegisterData,uint8_t temp)
{
    uint8_t ControlValue = 0;
    uint8_t ValueToWrite = 0;
    uint8_t RegisterIndex = 0;
    uint8_t i = 0;
    ControlValue = RegisterAddress;  //0x03,从高位往低位写
    SCLK_L;
    CS_L;
    ad9959delay(1);
        for(i=0; i<8; i++)
    {
        SCLK_L;
        if(0x80 == (ControlValue & 0x80))   //上升沿将数值传递给寄存器
        {SDIO0_H;
        ad9959delay(2);
        }
        else
        {SDIO0_L;
        ad9959delay(2);
            }
        SCLK_H;
        ad9959delay(5);
        ControlValue <<= 1;
    }
    SCLK_L;
    ad9959delay(5);
//写入数据   //循环这个字节数，将数进行传递
        for (RegisterIndex=0; RegisterIndex<NumberofRegisters; RegisterIndex++)
    {
        ValueToWrite = RegisterData[RegisterIndex];
        for (i=0; i<8; i++)
        {
            SCLK_L;
            if(0x80 == (ValueToWrite & 0x80))
            SDIO0_H;
            else
            SDIO0_L;
            SCLK_H;
            ValueToWrite <<= 1;
        }
        SCLK_L;
    }
    if(temp==1)
    AD9959_IO_Updata();
  CS_H;

}
//**************************
//temp为中间变量，temp=1的作用相当于IO_UPDATA,省去了再写一遍代码
//**************************
void WriteDATA_AD9959(uint8_t RegisterAddress, uint8_t NumberofRegisters, uint8_t *RegisterData)
{
    uint8_t ControlValue = 0;
    uint8_t ValueToWrite = 0;
    uint8_t RegisterIndex = 0;
    uint8_t i = 0;
    ControlValue = RegisterAddress;  //0x03,从高位往低位写
    SCLK_L;
    CS_L;
    ad9959delay(1);
        for(i=0; i<8; i++)
    {
        SCLK_L;
        if(0x80 == (ControlValue & 0x80))   //上升沿将数值传递给寄存器
        {SDIO0_H;
        ad9959delay(2);
        }
        else
        {SDIO0_L;
        ad9959delay(2);
            }
        SCLK_H;
        ad9959delay(5);
        ControlValue <<= 1;
    }
    SCLK_L;
    ad9959delay(5);
//写入数据   //循环这个字节数，将数进行传递
        for (RegisterIndex=0; RegisterIndex<NumberofRegisters; RegisterIndex++)
    {
        ValueToWrite = RegisterData[RegisterIndex];
        for (i=0; i<8; i++)
        {
            SCLK_L;
            if(0x80 == (ValueToWrite & 0x80))
            SDIO0_H;
            else
            SDIO0_L;
            SCLK_H;
            ValueToWrite <<= 1;
        }
        SCLK_L;
    }
  CS_H;
}


void AD9959_IO_Updata(void)  //IO_Update  更新数据
{
     UPDATE_L;
     ad9959delay(2);
     UPDATE_H;
     ad9959delay(4);
     UPDATE_L;
} 


void AD9959_Write_frequence(uint8_t channel,uint32_t frequence)
{
 uint8_t CFTW0_DATA[4] ={0x00,0x00,0x00,0x00};  //中间变量
 uint32_t temp;
 temp=(uint32_t)frequence*ACC_FRE_FACTOR;      //将输入频率因子分为四个字节
  CFTW0_DATA[3]=(uint8_t)temp;
  CFTW0_DATA[2]=(uint8_t)(temp>>8);
  CFTW0_DATA[1]=(uint8_t)(temp>>16);
    CFTW0_DATA[0]=(uint8_t)(temp>>24);
   if(channel==0)
      {
            WriteData_AD9959(CSR_ADD,1,CH0_ADD,1);//控制寄存器写入CH0通道
      WriteData_AD9959(CFTW0_ADD,4,CFTW0_DATA,1);//CTW0 address 0x04.输出CH0设定频率
        }
      else if(channel==1)
      {
            WriteData_AD9959(CSR_ADD,1,CH1_ADD,1);//控制寄存器写入CH1通道
      WriteData_AD9959(CFTW0_ADD,4,CFTW0_DATA,1);//CTW0 address 0x04.输出CH1设定频率
      }
      else if(channel==2)
      {
            WriteData_AD9959(CSR_ADD,1,CH2_ADD,1);//控制寄存器写入CH2通道
      WriteData_AD9959(CFTW0_ADD,4,CFTW0_DATA,1);//CTW0 address 0x04.输出CH2设定频率
      }
      else if(channel==3)
      {
            WriteData_AD9959(CSR_ADD,1,CH3_ADD,1);//控制寄存器写入CH3通道
      WriteData_AD9959(CFTW0_ADD,4,CFTW0_DATA,1);//CTW0 address 0x04.输出CH3设定频率
      }
 
}

void AD9959_Selfwrite_frequence(uint8_t channel,uint32_t frequence)
{
 uint8_t CFTW0_DATA[4] ={0x00,0x00,0x00,0x00};  //中间变量
 uint32_t temp;
 if(frequence==20000){
  temp=FREQ_CON_20K; 
  CFTW0_DATA[3]=(uint8_t)temp;
  CFTW0_DATA[2]=(uint8_t)(temp>>8);
  CFTW0_DATA[1]=(uint8_t)(temp>>16);
    CFTW0_DATA[0]=(uint8_t)(temp>>24);
 }
 if(frequence==25000){
  temp=FREQ_CON_25K; 
  CFTW0_DATA[3]=(uint8_t)temp;
  CFTW0_DATA[2]=(uint8_t)(temp>>8);
  CFTW0_DATA[1]=(uint8_t)(temp>>16);
    CFTW0_DATA[0]=(uint8_t)(temp>>24);
 }
 if(frequence==30000){
  temp=FREQ_CON_30K; 
  CFTW0_DATA[3]=(uint8_t)temp;
  CFTW0_DATA[2]=(uint8_t)(temp>>8);
  CFTW0_DATA[1]=(uint8_t)(temp>>16);
    CFTW0_DATA[0]=(uint8_t)(temp>>24);
 }
 if(frequence==35000){
  temp=FREQ_CON_35K; 
  CFTW0_DATA[3]=(uint8_t)temp;
  CFTW0_DATA[2]=(uint8_t)(temp>>8);
  CFTW0_DATA[1]=(uint8_t)(temp>>16);
    CFTW0_DATA[0]=(uint8_t)(temp>>24);
 }
 if(frequence==40000){
  temp=FREQ_CON_40K; 
  CFTW0_DATA[3]=(uint8_t)temp;
  CFTW0_DATA[2]=(uint8_t)(temp>>8);
  CFTW0_DATA[1]=(uint8_t)(temp>>16);
    CFTW0_DATA[0]=(uint8_t)(temp>>24);
 }
 if(frequence==45000){
  temp=FREQ_CON_45K; 
  CFTW0_DATA[3]=(uint8_t)temp;
  CFTW0_DATA[2]=(uint8_t)(temp>>8);
  CFTW0_DATA[1]=(uint8_t)(temp>>16);
    CFTW0_DATA[0]=(uint8_t)(temp>>24);
 }
 if(frequence==50000){
  temp=FREQ_CON_50K; 
  CFTW0_DATA[3]=(uint8_t)temp;
  CFTW0_DATA[2]=(uint8_t)(temp>>8);
  CFTW0_DATA[1]=(uint8_t)(temp>>16);
    CFTW0_DATA[0]=(uint8_t)(temp>>24);
 }
 if(frequence==55000){
  temp=FREQ_CON_55K; 
  CFTW0_DATA[3]=(uint8_t)temp;
  CFTW0_DATA[2]=(uint8_t)(temp>>8);
  CFTW0_DATA[1]=(uint8_t)(temp>>16);
    CFTW0_DATA[0]=(uint8_t)(temp>>24);
 }
 if(frequence==60000){
  temp=FREQ_CON_60K; 
  CFTW0_DATA[3]=(uint8_t)temp;
  CFTW0_DATA[2]=(uint8_t)(temp>>8);
  CFTW0_DATA[1]=(uint8_t)(temp>>16);
    CFTW0_DATA[0]=(uint8_t)(temp>>24);
 }
 if(frequence==65000){
  temp=FREQ_CON_65K; 
  CFTW0_DATA[3]=(uint8_t)temp;
  CFTW0_DATA[2]=(uint8_t)(temp>>8);
  CFTW0_DATA[1]=(uint8_t)(temp>>16);
    CFTW0_DATA[0]=(uint8_t)(temp>>24);
 }
 if(frequence==70000){
  temp=FREQ_CON_70K; 
  CFTW0_DATA[3]=(uint8_t)temp;
  CFTW0_DATA[2]=(uint8_t)(temp>>8);
  CFTW0_DATA[1]=(uint8_t)(temp>>16);
    CFTW0_DATA[0]=(uint8_t)(temp>>24);
 }
 if(frequence==75000){
  temp=FREQ_CON_75K; 
  CFTW0_DATA[3]=(uint8_t)temp;
  CFTW0_DATA[2]=(uint8_t)(temp>>8);
  CFTW0_DATA[1]=(uint8_t)(temp>>16);
    CFTW0_DATA[0]=(uint8_t)(temp>>24);
 }
 if(frequence==80000){
  temp=FREQ_CON_80K; 
  CFTW0_DATA[3]=(uint8_t)temp;
  CFTW0_DATA[2]=(uint8_t)(temp>>8);
  CFTW0_DATA[1]=(uint8_t)(temp>>16);
    CFTW0_DATA[0]=(uint8_t)(temp>>24);
 }
 if(frequence==85000){
  temp=FREQ_CON_85K; 
  CFTW0_DATA[3]=(uint8_t)temp;
  CFTW0_DATA[2]=(uint8_t)(temp>>8);
  CFTW0_DATA[1]=(uint8_t)(temp>>16);
    CFTW0_DATA[0]=(uint8_t)(temp>>24);
 }
 if(frequence==90000){
  temp=FREQ_CON_90K; 
  CFTW0_DATA[3]=(uint8_t)temp;
  CFTW0_DATA[2]=(uint8_t)(temp>>8);
  CFTW0_DATA[1]=(uint8_t)(temp>>16);
    CFTW0_DATA[0]=(uint8_t)(temp>>24);
 }
 if(frequence==95000){
  temp=FREQ_CON_95K; 
  CFTW0_DATA[3]=(uint8_t)temp;
  CFTW0_DATA[2]=(uint8_t)(temp>>8);
  CFTW0_DATA[1]=(uint8_t)(temp>>16);
    CFTW0_DATA[0]=(uint8_t)(temp>>24);
 }
 if(frequence==100000){
  temp=FREQ_CON_100K; 
  CFTW0_DATA[3]=(uint8_t)temp;
  CFTW0_DATA[2]=(uint8_t)(temp>>8);
  CFTW0_DATA[1]=(uint8_t)(temp>>16);
    CFTW0_DATA[0]=(uint8_t)(temp>>24);
 }
   if(channel==0)
      {
            WriteData_AD9959(CSR_ADD,1,CH0_ADD,1);//控制寄存器写入CH0通道
      WriteData_AD9959(CFTW0_ADD,4,CFTW0_DATA,1);//CTW0 address 0x04.输出CH0设定频率
        }
      else if(channel==1)
      {
            WriteData_AD9959(CSR_ADD,1,CH1_ADD,1);//控制寄存器写入CH1通道
      WriteData_AD9959(CFTW0_ADD,4,CFTW0_DATA,1);//CTW0 address 0x04.输出CH1设定频率
      }
      else if(channel==2)
      {
            WriteData_AD9959(CSR_ADD,1,CH2_ADD,1);//控制寄存器写入CH2通道
      WriteData_AD9959(CFTW0_ADD,4,CFTW0_DATA,1);//CTW0 address 0x04.输出CH2设定频率
      }
      else if(channel==3)
      {
            WriteData_AD9959(CSR_ADD,1,CH3_ADD,1);//控制寄存器写入CH3通道
      WriteData_AD9959(CFTW0_ADD,4,CFTW0_DATA,1);//CTW0 address 0x04.输出CH3设定频率
      }
 
} 

/*---------------------------------------
函数功能：设置通道输出相位
Channel:  输出通道
Phase:    输出相位,范围：0~16383(对应角度：0°~360°)
---------------------------------------*/
void AD9959_Write_Phase(uint8_t channel,uint16_t phase)
{
 uint16_t P_temp=0;
  P_temp=(uint16_t)phase;
    CPOW0_DATA[1]=(uint16_t)P_temp;
    CPOW0_DATA[0]=(uint16_t)(P_temp>>8);
    if(channel==0)
  {
        WriteData_AD9959(CSR_ADD,1,CH0_ADD,1);
    WriteData_AD9959(CPOW0_ADD,2,CPOW0_DATA,1);
  }
  else if(channel==1)
  {
        WriteData_AD9959(CSR_ADD,1,CH1_ADD,1);
    WriteData_AD9959(CPOW0_ADD,2,CPOW0_DATA,1);
  }
  else if(channel==2)
  {
        WriteData_AD9959(CSR_ADD,1,CH2_ADD,1);
    WriteData_AD9959(CPOW0_ADD,2,CPOW0_DATA,1);
  }
  else if(channel==3)
  {
        WriteData_AD9959(CSR_ADD,1,CH3_ADD,1);
    WriteData_AD9959(CPOW0_ADD,2,CPOW0_DATA,1);
  }
}

/*---------------------------------------
函数功能：设置通道输出幅度
Channel:  输出通道
Amplitude:    输出幅度
---------------------------------------*/
void AD9959_Write_Amplitude(uint8_t channel,uint16_t Amplitude)
{
    uint16_t A_temp;     //=0x23ff;
    uint8_t ACR_DATA[3] = {0x00,0x00,0x00};//default Value = 0x--0000 Rest = 18.91/Iout
  A_temp=Amplitude|0x1000;
    ACR_DATA[2] = (uint8_t)A_temp;  //低位数据
  ACR_DATA[1] = (uint8_t)(A_temp>>8); //高位数据
  if(channel==0)
  {
        WriteData_AD9959(CSR_ADD,1,CH0_ADD,1);
    WriteData_AD9959(ACR_ADD,3,ACR_DATA,1); 
    }
  else if(channel==1)
  {
        WriteData_AD9959(CSR_ADD,1,CH1_ADD,1);
    WriteData_AD9959(ACR_ADD,3,ACR_DATA,1);
    }
  else if(channel==2)
  {
      WriteData_AD9959(CSR_ADD,1,CH2_ADD,1);
    WriteData_AD9959(ACR_ADD,3,ACR_DATA,1);
    }
  else if(channel==3)
  {
        WriteData_AD9959(CSR_ADD,1,CH3_ADD,1);
    WriteData_AD9959(ACR_ADD,3,ACR_DATA,1); 

   }
}

void ad9959delay(uint16_t length)
{
     length = length*36;
      while(length--);
}



