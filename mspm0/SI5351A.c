#include "SI5351A.h"
#include "hardware/delay.h"

// 私有函数声明
static void I2C_Start(void);
static void I2C_Stop(void);
static void I2C_WriteByte(uint8_t data);
static void I2C_WriteRegister(uint8_t reg, uint8_t data);

//-----------------------------------------------------------------
// I2C基本操作
//-----------------------------------------------------------------
static void I2C_Start(void)
{
    SI5351_SDA_H();
    SI5351_SCL_H();
    delay_us(4);
    SI5351_SDA_L();
    delay_us(4);
    SI5351_SCL_L();
    delay_us(4);
}

static void I2C_Stop(void)
{
    SI5351_SCL_L();
    delay_us(4);
    SI5351_SDA_L();
    delay_us(4);
    SI5351_SCL_H();
    delay_us(4);
    SI5351_SDA_H();
    delay_us(4);
}

// static void I2C_WriteByte(uint8_t data)
// {
//     uint8_t i;
//     for(i = 0; i < 8; i++) {
//         SI5351_SCL_L();
//         delay_us(2);
//         (data & 0x80) ? SI5351_SDA_H() : SI5351_SDA_L();
//         data <<= 1;
//         delay_us(2);
//         SI5351_SCL_H();
//         delay_us(4);
//     }
    
//     // 等待ACK
//     SI5351_SCL_L();
//     SI5351_SDA_H();
//     delay_us(4);
//     SI5351_SCL_H();
//     delay_us(4);
//     while(SI5351_PORT->DATAIN & SI5351_SI5351_SDA_PIN); // 等待ACK
//     SI5351_SCL_L();
// }

static void I2C_WriteByte(uint8_t data)//IIC总线发送信数据
{
		uint8_t i;
    for(i=0;i<8;i++) //发送一个字节数据 
    {     
        SI5351_SCL_L();
        delay_us(2);
        (data & 0x80) ? SI5351_SDA_H() : SI5351_SDA_L();
        data <<= 1;
        delay_us(2);
        SI5351_SCL_H();
        delay_us(4);
		}  
    SI5351_SCL_L();
    SI5351_SDA_H();
    delay_us(4);
    SI5351_SCL_H();
    delay_us(4);
    
    // 检测 ACK（检查 SDA 是否被拉低）
    uint16_t timeout = 1000;  // 超时计数
    while ((DL_GPIO_getRawInterruptStatus(SI5351_PORT, SI5351_SI5351_SDA_PIN) && timeout--)) {
        delay_us(1);
    }
		SI5351_SCL_L();
}


static void I2C_WriteRegister(uint8_t reg, uint8_t data)
{
    I2C_Start();
    I2C_WriteByte(SI5351_I2C_ADDR);
    I2C_WriteByte(reg);
    I2C_WriteByte(data);
    I2C_Stop();
}

//-----------------------------------------------------------------
// SI5351初始化
//-----------------------------------------------------------------
void Si5351_Init(void)
{
    // // 初始化GPIO
    // DL_GPIO_initDigitalOutput(SI5351_PORT, SI5351_SCL_PIN, 
    //                         DL_GPIO_OUTPUT_OPEN_DRAIN, DL_GPIO_OUTPUT_PULL_UP);
    // DL_GPIO_initDigitalOutput(SI5351_PORT, SI5351_SDA_PIN, 
    //                         DL_GPIO_OUTPUT_OPEN_DRAIN, DL_GPIO_OUTPUT_PULL_UP);
    
    // DL_IOMUX_setPinMux(SI5351_SCL_IOMUX);
    // DL_IOMUX_setPinMux(SI5351_SDA_IOMUX);
    
    // 初始状态
    SI5351_SCL_H();
    SI5351_SDA_H();
    
    // 使能所有输出
    I2C_WriteRegister(3, 0x00);
}

//-----------------------------------------------------------------
// 设置输出频率
//-----------------------------------------------------------------
void Si5351_SetFrequency(uint32_t frequency, uint8_t channel)
{
    uint32_t pllFreq;
    uint32_t divider;
    uint8_t mult;
    uint32_t num;
    uint32_t denom = 1048575;
    uint8_t r_div[8] = {0,16,32,48,64,80,96,112};
    uint8_t p_r_div = 0;
    uint8_t synth;
    uint8_t reg_synth;

    // 处理低频情况
    while(frequency < 440000 && p_r_div < 7) {
        frequency *= 2;
        p_r_div++;
    }

    // 计算PLL参数
    divider = 900000000 / frequency;
    if(divider % 2) divider--;
    pllFreq = divider * frequency;
    mult = pllFreq / XTAL_FREQ;
    num = (uint32_t)((pllFreq % XTAL_FREQ) * 1048575.0 / XTAL_FREQ);

    // 设置PLL
    Si5351_SetPLL(SI_SYNTH_PLL_A, mult, num, denom);

    // 选择通道
    switch(channel) {
        case 0:
            synth = SI_SYNTH_MS_0;
            reg_synth = SI_CLK0_CONTROL;
            break;
        case 1:
            synth = SI_SYNTH_MS_1;
            reg_synth = SI_CLK1_CONTROL;
            break;
        case 2:
            synth = SI_SYNTH_MS_2;
            reg_synth = SI_CLK2_CONTROL;
            break;
        default:
            synth = SI_SYNTH_MS_0;
            reg_synth = SI_CLK0_CONTROL;
    }

    // 设置Multisynth
    Si5351_SetMultisynth(synth, divider, r_div[p_r_div]);
    
    // 重置PLL并启用输出
    I2C_WriteRegister(SI_PLL_RESET, 0xA0);
    I2C_WriteRegister(reg_synth, 0x4F | SI_CLK_SRC_PLL_A);
}

//-----------------------------------------------------------------
// 设置PLL参数
//-----------------------------------------------------------------
void Si5351_SetPLL(uint8_t pll, uint8_t mult, uint32_t num, uint32_t denom)
{
    uint32_t P1 = (uint32_t)(128 * mult + 128 * num / denom - 512);
    uint32_t P2 = (uint32_t)(128 * num - denom * (uint32_t)(128 * num / denom));
    uint32_t P3 = denom;

    I2C_WriteRegister(pll + 0, (P3 >> 8) & 0xFF);
    I2C_WriteRegister(pll + 1, P3 & 0xFF);
    I2C_WriteRegister(pll + 2, (P1 >> 16) & 0x03);
    I2C_WriteRegister(pll + 3, (P1 >> 8) & 0xFF);
    I2C_WriteRegister(pll + 4, P1 & 0xFF);
    I2C_WriteRegister(pll + 5, ((P3 >> 12) & 0xF0) | ((P2 >> 16) & 0x0F));
    I2C_WriteRegister(pll + 6, (P2 >> 8) & 0xFF);
    I2C_WriteRegister(pll + 7, P2 & 0xFF);
}

//-----------------------------------------------------------------
// 设置Multisynth参数
//-----------------------------------------------------------------
void Si5351_SetMultisynth(uint8_t synth, uint32_t divider, uint8_t rDiv)
{
    uint32_t P1 = 128 * divider - 512;
    uint32_t P2 = 0;
    uint32_t P3 = 1;

    I2C_WriteRegister(synth + 0, (P3 >> 8) & 0xFF);
    I2C_WriteRegister(synth + 1, P3 & 0xFF);
    I2C_WriteRegister(synth + 2, ((P1 >> 16) & 0x03) | rDiv);
    I2C_WriteRegister(synth + 3, (P1 >> 8) & 0xFF);
    I2C_WriteRegister(synth + 4, P1 & 0xFF);
    I2C_WriteRegister(synth + 5, ((P3 >> 12) & 0xF0) | ((P2 >> 16) & 0x0F));
    I2C_WriteRegister(synth + 6, (P2 >> 8) & 0xFF);
    I2C_WriteRegister(synth + 7, P2 & 0xFF);
}

//-----------------------------------------------------------------
// 控制输出通道使能
//-----------------------------------------------------------------
void Si5351_EnableOutputs(uint8_t channel_0, uint8_t channel_1, uint8_t channel_2)
{
    uint8_t data = ~(channel_0 | (channel_1 << 1) | (channel_2 << 2));
    I2C_WriteRegister(3, data);
}