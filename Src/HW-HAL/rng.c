#include "rng.h"

RNG_HandleTypeDef RngHandle;

uint32_t Get_random(void)
{
    uint16_t random_data=0;
    RngHandle.Instance = RNG;
    if (HAL_RNG_Init(&RngHandle) != HAL_OK)
    {
    }
    else
    {
        random_data = HAL_RNG_GetRandomNumber(&RngHandle);
    }
    HAL_RNG_DeInit(&RngHandle);
   
    if(random_data%2==0)//生成16位随机数/65535占的百分比*时间范围*2换算RTC时间，
    {
        random_data=((random_data*2.048/65536)*Time_random);
        RTC_SleepTime=(RTC_SleepTime+random_data)	;
    }
    else
    {	
        random_data=((random_data*2.048/65536)*Time_random);
        RTC_SleepTime=(RTC_SleepTime-random_data)	;
    }

    return random_data;
}







