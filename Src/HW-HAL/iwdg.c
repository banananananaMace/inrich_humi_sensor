#include "iwdg.h"
IWDG_HandleTypeDef hiwdg;



/*******************************************************
*Function Name 	:MX_IWDG_Init
*Description  	:看门狗初始化
*Input					:
*Output					:
*看门狗溢出时间计算：t=(256*(3749+1))/32k=30s
*******************************************************/

 void MX_IWDG_Init(void)
{

    hiwdg.Instance = IWDG;
    hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
    hiwdg.Init.Window = IWDG_WINDOW_DISABLE;
    hiwdg.Init.Reload = (3749);//30s
    if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
    {
        Error_Handler();
    }

}
