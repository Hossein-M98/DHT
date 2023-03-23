/**
 **********************************************************************************
 * @file   DHT_platform.h
 * @author Hossein.M (https://github.com/Hossein-M98)
 * @brief  This file contains the platform specific functions for DHT driver
 **********************************************************************************
 *
 * Copyright (c) 2023 Hossein.M (MIT License)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 **********************************************************************************
 */

/* Define to prevent recursive inclusion ----------------------------------------*/
#ifndef	_DHT_PLATFORM_H_
#define _DHT_PLATFORM_H_

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ---------------------------------------------------------------------*/
#include "DHT.h"


/* Functionality Options --------------------------------------------------------*/
/**
 * @brief  Specify the target platform
 * @note   Uncomment the line below according to the target platform
 */ 
// #define DHT_PLATFORM_STM32_HAL
// #define DHT_PLATFORM_ESP32_IDF
// #define DHT_PLATFORM_AVR


#if defined(DHT_PLATFORM_STM32_HAL)
#define DHT_DATA_GPIO_PORT        GPIOA
#define DHT_DATA_GPIO_PIN         GPIO_PIN_0
#elif defined(DHT_PLATFORM_ESP32_IDF)
#define DHT_DATA_GPIO             GPIO_NUM_10
#elif defined(DHT_PLATFORM_AVR)
#define DHT_DATA_GPIO_PORT        PORTA
#define DHT_DATA_GPIO_PIN         PINA
#define DHT_DATA_GPIO_DDR         DDRA
#define DHT_DATA_GPIO_BIT         7
#endif


/**
 ==================================================================================
                             ##### Functions #####                                 
 ==================================================================================
 */

void
DHT_Platform_Init(DHT_Handler_t *Handler);


#ifdef __cplusplus
}
#endif


#endif //! _DHT_PLATFORM_H_
