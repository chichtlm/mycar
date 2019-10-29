/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "core_cm3.h"
#include "stm32f1xx_ll_gpio.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SYS_TICK_PERIOD 72000000

static void usart_send(void);
static void set_blink(uint16_t period);
static void led_task(void);
static void motor_1_forward(void);
static void motor_2_forward(void);
static void motor_3_forward(void);
static void motor_4_forward(void);


static void motor_1_reverse(void);
static void motor_1_brake(void);

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define USART_DEBUG huart2


/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t led_period = 0;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
    set_blink(1000);

    __HAL_TIM_SET_COUNTER(&htim2, 0);
    __HAL_TIM_SET_COUNTER(&htim3, 0);
    __HAL_TIM_SET_COUNTER(&htim4, 0);
    
    HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    //usart_send();

    led_task();
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enables the Clock Security System 
  */
  HAL_RCC_EnableCSS();
}

/* USER CODE BEGIN 4 */

static void usart_send(void)
{
    uint8_t buffer[5]={1,2,3,4,5};

    HAL_UART_Transmit(&huart2, buffer, 5, 10);
    
}


static void set_blink(uint16_t period)
{
    LL_GPIO_ResetOutputPin(GPIOD, LL_GPIO_PIN_2);
	//led_times = times*2;
	led_period = period/2;
}


static uint16_t timer_cnt2 = 0;
static uint16_t timer_cnt3 = 0;
static uint16_t timer_cnt4 = 0;
static uint16_t timer_cnt = 0;



static void led_task(void)
{
    static uint32_t next;

    static uint32_t i = 0;
    uint32_t curr;
    
    curr = HAL_GetTick();
    if(curr > next)
    {
        HAL_GPIO_TogglePin(GPIOD,GPIO_PIN_2);
        next = curr + led_period;

        i++;
        if(0==(i%2))
        {
           motor_1_forward(); 
           motor_2_forward();
           motor_3_forward();
           motor_4_forward();

           timer_cnt2 = (uint16_t)__HAL_TIM_GET_COUNTER(&htim2);
            __HAL_TIM_SET_COUNTER(&htim2, 0);

            timer_cnt3 = (uint16_t)__HAL_TIM_GET_COUNTER(&htim3);
            __HAL_TIM_SET_COUNTER(&htim3, 0);

            timer_cnt4 = (uint16_t)__HAL_TIM_GET_COUNTER(&htim4);
            __HAL_TIM_SET_COUNTER(&htim4, 0);

           printf("\r\ntimer_cnt:%d,%d,%d",timer_cnt2,timer_cnt3,timer_cnt4);
        }
        else
        {
            //motor_1_reverse();
        }
    }
}

static void motor_1_forward(void)
{
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 80);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
}

static void motor_2_forward(void)
{
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 80);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
}

static void motor_3_forward(void)
{
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 80);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_3);
}

static void motor_4_forward(void)
{
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 50);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET); 
}



static void motor_1_reverse(void)
{
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 80);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
    //HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_4);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET); 
}


static void motor_1_brake(void)
{
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 0);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_4);
    //HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_4);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET); 
}


int fputc(int ch,FILE *f)
{
    uint8_t buffer[1] = {ch};
    HAL_UART_Transmit(&USART_DEBUG, buffer, 1, 5);
}

int fgetc(FILE *f)
{   
    uint8_t buffer;
    HAL_UART_Receive(&USART_DEBUG, &buffer, 1, 5);
  
    return buffer;
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
