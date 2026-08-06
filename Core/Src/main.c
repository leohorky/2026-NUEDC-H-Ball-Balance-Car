/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Emm_V5.h"
#include "stdio.h"
#include "pid.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
volatile uint8_t mode=0,last_mode=0,start=0;
volatile uint8_t SW1_state=1,SW2_state=1;
volatile int16_t last_out=0,now_out=0;
volatile uint8_t flag=0,flag1=1;
volatile uint32_t tim=0,tim_1=0,tim_2=0;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
const int16_t pulse_up[151] = {
    0, 27, 54, 81, 108, 135, 162, 189, 217, 244,
    271, 299, 326, 354, 382, 409, 437, 465, 493, 521,
    549, 577, 605, 633, 661, 690, 718, 747, 775, 804,
    832, 861, 890, 919, 948, 977, 1006, 1035, 1065, 1094,
    1124, 1153, 1183, 1212, 1242, 1272, 1302, 1332, 1362, 1393,
    1423, 1453, 1484, 1515, 1545, 1576, 1607, 1638, 1669, 1701,
    1732, 1763, 1795, 1827, 1858, 1890, 1922, 1955, 1987, 2019,
    2052, 2085, 2117, 2150, 2183, 2216, 2250, 2283, 2317, 2351,
    2385, 2419, 2453, 2487, 2522, 2556, 2591, 2626, 2662, 2697,
    2733, 2768, 2804, 2840, 2877, 2913, 2950, 2987, 3024, 3061,
    3099, 3136, 3174, 3212, 3251, 3290, 3329, 3368, 3407, 3447,
    3487, 3527, 3568, 3608, 3650, 3691, 3733, 3775, 3817, 3860,
    3903, 3946, 3990, 4034, 4079, 4124, 4169, 4215, 4262, 4308,
    4356, 4403, 4451, 4500, 4550, 4599, 4650, 4701, 4753, 4805,
    4858, 4912, 4966, 5022, 5078, 5135, 5193, 5252, 5312, 5373,
    5435
};
const int16_t pulse_down[151] = {
    0, -27, -54, -80, -107, -134, -160, -187, -213, -240,
    -266, -293, -319, -345, -372, -398, -424, -450, -476, -502,
    -528, -554, -580, -606, -632, -658, -683, -709, -735, -760,
    -786, -811, -837, -862, -888, -913, -939, -964, -989, -1015,
    -1040, -1065, -1090, -1115, -1141, -1166, -1191, -1216, -1241, -1266,
    -1291, -1316, -1340, -1365, -1390, -1415, -1440, -1464, -1489, -1514,
    -1539, -1563, -1588, -1612, -1637, -1661, -1686, -1710, -1735, -1759,
    -1784, -1808, -1833, -1857, -1881, -1906, -1930, -1954, -1978, -2003,
    -2027, -2051, -2075, -2099, -2123, -2147, -2172, -2196, -2220, -2244,
    -2268, -2292, -2316, -2340, -2364, -2387, -2411, -2435, -2459, -2483,
    -2507, -2531, -2555, -2578, -2602, -2626, -2650, -2673, -2697, -2721,
    -2744, -2768, -2792, -2816, -2839, -2863, -2886, -2910, -2934, -2957,
    -2981, -3004, -3028, -3051, -3075, -3099, -3122, -3146, -3169, -3193,
    -3216, -3240, -3263, -3286, -3310, -3333, -3357, -3380, -3404, -3427,
    -3450, -3474, -3497, -3521, -3544, -3567, -3591, -3614, -3637, -3661,
    -3684
};
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* UART4 receives one signed 8-bit vision value per interrupt. */
volatile int8_t now_position = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void Set_positon(int16_t p){
	p=p-8;
	if(p>=0){
		Emm_V5_Pos_Control(1,1,1000,150,pulse_up[p],1,0);
		//Emm_V5_QPos_Control(1,pulse_up[p]);
	}
	else if(p<0){
		Emm_V5_Pos_Control(1,0,1000,150,-pulse_down[-p],1,0);
	}

}


void LED_state(void){
	  if(mode==0){
		  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_SET);
		  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET);
	  }
	  else if(mode==1){
		  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET);
	  }
	  else if(mode==2){
	  		  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_SET);
	  		  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET);
	  	  }
}


void test1(void){
	int8_t position=now_position;
	char buf[16];
	int len;
	len = sprintf(buf,"\r%d", position);
	HAL_UART_Transmit(&huart5,(uint8_t *)buf,len,100);
	HAL_Delay(100);
}

uint8_t change_mode(void){
	return mode!=last_mode;
}

void mode0(void){
	if(change_mode()){	FastPID_Reset();FastPID_SetTarget(&Position,0);last_mode=mode;}
	Set_positon(now_out);
	flag=0;
	HAL_Delay(20);
}
//void mode1(void){
//	if(change_mode()){
//		last_mode=mode;
//		FastPID_Reset();
//		tim_1=tim;
//	}
//	if(tim-tim_1<=8){FastPID_SetTarget(&Position,-50);}
//	else if(tim-tim_1>=9 && tim-tim_1<=13){
//		FastPID_Reset();
//		FastPID_SetTarget(&Position,50);
//	}
//	else if(tim-tim_1>=65 && tim-tim_1<=70){FastPID_Reset();}
//	if(tim-tim_1>=70){
//		FastPID_SetTarget(&Position,-50);
//	}
//	Set_positon(now_out);
//	flag=0;
//	HAL_Delay(20);
//
//}

void mode1(void){
	if(change_mode()){
		last_mode=mode;
		FastPID_Reset();
		tim_1=tim;
	}
	if(flag1==0){
		Set_positon(now_out);
		flag=0;
		HAL_Delay(20);
	}


}
void mode2(void){
	if(change_mode()){
		int8_t position=now_position;
		FastPID_Reset();FastPID_SetTarget(&Position,position);last_mode=mode;tim_2=tim;
	}
	Set_positon(now_out);
	flag=0;
	HAL_Delay(20);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_UART5_Init();
  MX_UART4_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  FastPID_Init();
  FastPID_SetParam(&Position,1.9f, 0.09f, 19.0f);
  FastPID_SetTarget(&Position,0);
  //Emm_V5_Modify_MicroStep(MOTOR_ADDR,true,0); //修改细分
  Emm_V5_Origin_Trigger_Return(1, 0, 0);  //触发单圈自动就近归零
  HAL_Delay(200);

  //Emm_V5_Origin_Set_O(1,1); //仅执行一�?,存储零点位置
  if(HAL_UART_Receive_DMA(&huart4,
                          (uint8_t *)&now_position,
                          1U) != HAL_OK)
  {
      Error_Handler();
  }
  HAL_TIM_Base_Start_IT(&htim1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  //test1();
	  if(SW1_state==0){
		  HAL_Delay(20);
		  if(HAL_GPIO_ReadPin(SW1_GPIO_Port,SW1_Pin)==0){
			  start^=1;
			  SW1_state=1;
			  while(HAL_GPIO_ReadPin(SW1_GPIO_Port,SW1_Pin)==0);
		  }
	  }
	  if(SW2_state==0){
	  		  HAL_Delay(20);
	  		  if(HAL_GPIO_ReadPin(SW2_GPIO_Port,SW2_Pin)==0){
	  			  start=0;
	  			  mode++;
	  			  if(mode>=3){
	  				  mode=0;
	  			  }
	  			  SW2_state=1;
	  			  while(HAL_GPIO_ReadPin(SW2_GPIO_Port,SW2_Pin)==0);
	  		  }
	  	  }

	  if(flag && start){
		  if(mode==0){mode0();}
		  else if(mode==1){mode1();}
		  else if(mode==2){mode2();}
	  }
	  if(mode==1 && start && flag1){
		  Set_positon(-80);
		  HAL_Delay(230);
//		  Set_positon(45);
//		  HAL_Delay(500);
		  FastPID_SetTarget(&Position,-50);
		  flag1=0;
	  }

	  LED_state();

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM1)
    {
		tim++;
		int8_t position1=now_position;
		FastPID_Calc(&Position, position1);
		//if(mode==1){
		//	flag=1;
		//	now_out=Position.out;
		//}
		//else {
			int16_t change=Position.out - last_out;
			if(change==0){
				flag=0;
			}
			else{
				flag=1;
				now_out=Position.out;
				last_out=now_out;
				}
		//}

    }
}



void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	switch(GPIO_Pin)
	    {
			// SW1
			case GPIO_PIN_9:
				SW1_state=0;

			break;
			// SW2
			case GPIO_PIN_15:
				SW2_state=0;
				break;
			default:
				break;
	    }
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

