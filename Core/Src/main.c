/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include "can.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "cmsis_os2.h"
#include "xorshift.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define DATA_LENGTH 8
#define KERNEL_LED_DELAY 1000

#define BATTERY_TX_DELAY 1000
#define MOTOR_TEMP_TX_DELAY 1000

#define MAX_CAN_BATT_TX_DELAY    200

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

osThreadId_t sendBatteryCANMsgHandle;
osThreadId_t sendMotorTempMsgHandle;
osThreadId_t kernelLEDHandle;

uint8_t battery_msg_data[8];

union FloatBytes {
    float float_value;
    uint8_t bytes[4];
} FloatBytes;

union FloatBytes motor_temp;


CAN_TxHeaderTypeDef motor_temp_msg_header = {.StdId = 0x050B,
    .ExtId = 0x0000,
    .IDE = CAN_ID_STD,
    .RTR = CAN_RTR_DATA,
    .DLC = DATA_LENGTH,
    .TransmitGlobalTime = DISABLE
};

const osThreadAttr_t sendBatteryCANMsg_attr = {
    .name = "sendBatteryCANMsg",
    .priority = (osPriority_t) osPriorityHigh,
    .stack_size = 128 * 4
};

const osThreadAttr_t sendMotorTempMsg_attr = {
    .name = "sendMotorCANMsg",
    .priority = (osPriority_t) osPriorityHigh,
    .stack_size = 128 * 4
};

const osThreadAttr_t kernelLED_attr = {
    .name = "kernelLED",
    .priority = (osPriority_t) osPriorityNormal,
    .stack_size = 128 * 4
};


HAL_StatusTypeDef status;
uint8_t error;

uint32_t can_mailbox;

uint32_t free_level;

uint16_t changing_soc = 0;
uint8_t changing_motor_temp = 10;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void sendBatteryCANMsg(void *argument);
void kernelLEDTask(void *argument);
void sendMotorTempMsg(void *argument);

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

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_CAN_Init();
    MX_USART2_UART_Init();
    /* USER CODE BEGIN 2 */

    HAL_CAN_Start(&hcan);

    osKernelInitialize();

    sendBatteryCANMsgHandle = osThreadNew(sendBatteryCANMsg, NULL, &sendBatteryCANMsg_attr);
    sendMotorTempMsgHandle = osThreadNew(sendMotorTempMsg, NULL, &sendMotorTempMsg_attr);
    // kernelLEDHandle = osThreadNew(kernelLEDTask, NULL, &kernelLED_attr);

    osKernelStart();

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
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

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
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
    /** Initializes the CPU, AHB and APB buses clocks
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
}

/* USER CODE BEGIN 4 */


__NO_RETURN void kernelLEDTask (void *argument) {

    while (1) {

        osKernelState_t kernel_status = osKernelGetState();

        if (kernel_status == osKernelRunning) {
            HAL_GPIO_TogglePin(KERNEL_LED_GPIO_Port, KERNEL_LED_Pin);
        }

        osDelay(KERNEL_LED_DELAY);
    }
}


__NO_RETURN void sendMotorTempMsg(void *argument) {
    uint8_t msg_data[8] = {0};

    while (1) {
        motor_temp.float_value = changing_motor_temp;
        changing_motor_temp += 5;

        for (int i = 0; i < 4; i++) {
            msg_data[i] = motor_temp.bytes[i];
        }

        free_level = HAL_CAN_GetTxMailboxesFreeLevel(&hcan);
        status = HAL_CAN_AddTxMessage(&hcan, &motor_temp_msg_header, msg_data, &can_mailbox);

        if (status != HAL_OK) {
            error = 1;
        } else {
            HAL_GPIO_TogglePin(KERNEL_LED_GPIO_Port, KERNEL_LED_Pin);
        }

        osDelay(MOTOR_TEMP_TX_DELAY);
    }
}

__NO_RETURN void sendBatteryCANMsg(void *argument) {
    CAN_TxHeaderTypeDef rand_header;
    
    uint8_t rand_data[8] = {0};
    uint32_t rand_index = 0;
    uint16_t rand_delay = MAX_CAN_BATT_TX_DELAY;
    
    uint8_t rand_soc = 0;
    
    while (1) {
        rand_index = rand(NUM_BATTERY_MSGS);
        rand_header = can_battery_headers[rand_index];

        if (rand_header.StdId == 0x626) {
            rand_soc = rand(100);
            rand_data[0] = rand_soc;
        } else {
            rand_array(&rand_data[0], 8);
        }
        
        free_level = HAL_CAN_GetTxMailboxesFreeLevel(&hcan);
        
        status = HAL_CAN_AddTxMessage(&hcan, &rand_header, rand_data, &can_mailbox);

        if (status != HAL_OK) {
            error = 1;
        } else {
            HAL_GPIO_TogglePin(KERNEL_LED_GPIO_Port, KERNEL_LED_Pin);
        }

        rand_delay = rand(MAX_CAN_BATT_TX_DELAY);
        osDelay(rand_delay);
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
