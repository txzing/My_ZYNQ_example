#include "stdio.h"
#include "xparameters.h"
#include "xgpiops.h"

#define GPIOPS_ID XPAR_XGPIOPS_0_DEVICE_ID   //PS端  GPIO器件 ID

#define MIO_LED0 7   //PS_LED0 连接到 MIO7
#define MIO_LED1 8   //PS_LED1 连接到 MIO8
#define MIO_LED2 0   //PS_LED2 连接到 MIO0

#define MIO_KEY0 12  //PS_KEY0 连接到 MIO7
#define MIO_KEY1 11  //PS_KEY1 连接到 MIO8

#define EMIO_KEY_0 54  //PL_KEY0 连接到EMIO0
#define EMIO_KEY_1 55  //PL_KEY1
#define GPIO_INTR  56  //外部中断

int main()
{
    printf("EMIO TEST!\n");

    XGpioPs gpiops_inst;            //PS端 GPIO 驱动实例
    XGpioPs_Config *gpiops_cfg_ptr; //PS端 GPIO 配置信息

    //根据器件ID查找配置信息
    gpiops_cfg_ptr = XGpioPs_LookupConfig(GPIOPS_ID);
    //初始化器件驱动
    XGpioPs_CfgInitialize(&gpiops_inst, gpiops_cfg_ptr, gpiops_cfg_ptr->BaseAddr);

    //设置LED为输出
    XGpioPs_SetDirectionPin(&gpiops_inst, MIO_LED0, 1);
    XGpioPs_SetDirectionPin(&gpiops_inst, MIO_LED1, 1);
    XGpioPs_SetDirectionPin(&gpiops_inst, MIO_LED2, 1);
    //使能LED输出
    XGpioPs_SetOutputEnablePin(&gpiops_inst, MIO_LED0, 1);
    XGpioPs_SetOutputEnablePin(&gpiops_inst, MIO_LED1, 1);
    XGpioPs_SetOutputEnablePin(&gpiops_inst, MIO_LED2, 1);

    //设置KEY为输入
    XGpioPs_SetDirectionPin(&gpiops_inst, MIO_KEY0, 0);
    XGpioPs_SetDirectionPin(&gpiops_inst, MIO_KEY1, 0);
    XGpioPs_SetDirectionPin(&gpiops_inst, EMIO_KEY_0, 0);
    XGpioPs_SetDirectionPin(&gpiops_inst, EMIO_KEY_1, 0);

    //外部触发引脚
    XGpioPs_SetDirectionPin(&gpiops_inst, GPIO_INTR, 1);
    XGpioPs_SetOutputEnablePin(&gpiops_inst, GPIO_INTR, 1);

    //读取按键状态，用于外部触发引脚翻转
    while(1){
        XGpioPs_WritePin(&gpiops_inst, MIO_LED0,
                ~XGpioPs_ReadPin(&gpiops_inst, MIO_KEY0));

        XGpioPs_WritePin(&gpiops_inst, MIO_LED1,
                ~XGpioPs_ReadPin(&gpiops_inst, MIO_KEY1));

        XGpioPs_WritePin(&gpiops_inst, GPIO_INTR,
                ~XGpioPs_ReadPin(&gpiops_inst, EMIO_KEY_0));

        if(XGpioPs_ReadPin(&gpiops_inst, EMIO_KEY_0) == 0)
        {

            printf("EMIO_KEY_0 TEST!\n");
        }

        if(XGpioPs_ReadPin(&gpiops_inst, EMIO_KEY_1) == 0)
        {

            printf("EMIO_KEY_1 TEST!\n");
        }

    }

    return 0;
}
