/**
 **********************************************************************************
 * @file   DHT.h
 * @author Hossein.M (https://github.com/Hossein-M98)
 * @brief  DHT driver header file
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
#ifndef _DHT_H_
#define _DHT_H_

#ifdef __cplusplus
extern "C" {
#endif



/* Includes ---------------------------------------------------------------------*/
#include <stdint.h>


/* Exported Data Types ----------------------------------------------------------*/
/**
 * @brief  Library functions result data type
 */
typedef enum DHT_Result_e
{
  DHT_OK   = 0,
  DHT_FAIL = 1,
  DHT_TIMEOUT = 2,
  DHT_CHECKSUM_ERROR = 3,
} DHT_Result_t;

/**
 * @brief  Device type data type
*/
typedef enum DHT_Type_e
{
  DHT_TYPE_DHT11 = 1,
  DHT_TYPE_DHT22 = 2,
} DHT_Type_t;

/**
 * @brief  Handler data type
 * @note   User must initialize this this functions before using library:
 *         - PlatformSetDir (1: Output, 0: Input)
 *         - PlatformWrite
 *         - PlatformRead
 *         - PlatformDelayMs
 *         - PlatformDelayUs
 */
typedef struct DHT_Handler_s
{
  DHT_Type_t Type;
  
  void (*PlatformSetDir)(uint8_t);
  void (*PlatformWrite)(uint8_t);
  uint8_t (*PlatformRead)(void);
  void (*PlatformDelayMs)(uint16_t);
  void (*PlatformDelayUs)(uint16_t);
} DHT_Handler_t;



/**
 ==================================================================================
                               ##### Functions #####                               
 ==================================================================================
 */

/**
 * @brief  Initialization function
 * @param  Handler: Pointer to handler
 * @param  Type: DHT type
 * @retval DHT_Result_t
 *         - DHT_OK: Operation was successful.
 *         - DHT_FAIL: Failed to send or receive data.
 */
DHT_Result_t
DHT_Init(DHT_Handler_t *Handler, DHT_Type_t Type);


/**
 * @brief  Read sample
 * @param  Handler: Pointer to handler
 * @param  Temperature: Pointer to temperature variable
 * @param  Humidity: Pointer to humidity variable
 * @retval 
 */
DHT_Result_t
DHT_ReadSample(DHT_Handler_t *Handler,
               float *Temperature, float *Humidity);



#ifdef __cplusplus
}
#endif


#endif  //! _DHT_H_
