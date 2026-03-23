#include "hardware/PE4302.h"
#include "hardware/delay.h"
#include "ti_msp_dl_config.h"

void PE4302_Set(unsigned char db)
{
    unsigned char i;
    unsigned char W_DB;
    db = db&0X3F;

    // db = db^0X3F;	//数据位取反
    //PE_CLK_0;
    DL_GPIO_clearPins(PE4302_PORT, PE4302_PE_CLK_PIN);
    delay_us(500);
    //PE_LE_0_EN;//LE为低，才能将新数据移入寄存器
    DL_GPIO_clearPins(PE4302_PORT, PE4302_PE_LE_0_PIN);
    delay_us(500);
    for(i = 0; i < 6; i++)
    {
        W_DB = (db>>5);
        if(W_DB == 1)
        {
            //PE_DAT_1;
            DL_GPIO_setPins(PE4302_PORT, PE4302_PE_DAT_PIN);
        }
        else
        {
            //PE_DAT_0;
            DL_GPIO_clearPins(PE4302_PORT, PE4302_PE_DAT_PIN);
        }
        delay_us(500);
        //PE_CLK_1;//移入寄存器
        DL_GPIO_setPins(PE4302_PORT, PE4302_PE_CLK_PIN);
        delay_us(500);
        db = (db << 1)&0X3F;
        //PE_CLK_0;
        DL_GPIO_clearPins(PE4302_PORT, PE4302_PE_CLK_PIN);
        delay_us(500);
    }
    //PE_LE_0_DIS;//数据进设备
    DL_GPIO_setPins(PE4302_PORT, PE4302_PE_LE_0_PIN);
    delay_us(500);
    //PE_DAT_0;
    DL_GPIO_clearPins(PE4302_PORT, PE4302_PE_DAT_PIN);
    //PE_LE_0_EN;
    DL_GPIO_clearPins(PE4302_PORT, PE4302_PE_LE_0_PIN);
}

