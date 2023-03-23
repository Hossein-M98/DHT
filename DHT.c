/**
 **********************************************************************************
 * @file   DHT.c
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

/* Includes ---------------------------------------------------------------------*/
#include "DHT.h"
#include <stdio.h>



/* Private Constants ------------------------------------------------------------*/
#define DHT11_Temperature_Min   0
#define DHT11_Temperature_Max   50
#define DHT11_Humidity_Min      20
#define DHT11_Humidity_Max      90
#define DHT11_Delay_Read        50

#define DHT22_Temperature_Min   -40
#define DHT22_Temperature_Max   80
#define DHT22_Humidity_Min      0
#define DHT22_Humidity_Max      100
#define DHT22_Delay_Read        20




/**
 ==================================================================================
                           ##### Private Functions #####                           
 ==================================================================================
 */

static DHT_Result_t
DHT_ReadRaw(DHT_Handler_t *Handler, uint8_t *Data)
{
  DHT_Result_t DHT_Result = DHT_OK;
  uint8_t Buffer[5] = {0};
  uint8_t Retries = 0;


  //----- Step 1 - Start communication -----
  if (DHT_Result == DHT_OK)
  {
    // Request Data
    Handler->PlatformSetDir(1);
    Handler->PlatformWrite(0);
    switch (Handler->Type)
    {
      case DHT_TYPE_DHT11:
        Handler->PlatformDelayMs(DHT11_Delay_Read);
        break;

      case DHT_TYPE_DHT22:
        Handler->PlatformDelayMs(DHT22_Delay_Read);
        break;
    }

    // Setup DHT_PIN as input (with pull-up resistor) so as to read Data
    Handler->PlatformWrite(1);
    Handler->PlatformSetDir(0);

    // Wait for response for 20-40us
    Retries = 0;
    while (Handler->PlatformRead())
    {
      Handler->PlatformDelayUs(2);
      Retries += 2;
      if (Retries > 60)
      {
        DHT_Result = DHT_TIMEOUT; // Timeout error
        break;
      }
    }
  }
  //----------------------------------------

  //----- Step 2 - Wait for response -----
  if (DHT_Result == DHT_OK)
  {
    // Response sequence began
    // Wait for the first response to finish (low for ~80us)
    Retries = 0;
    while (!Handler->PlatformRead())
    {
      Handler->PlatformDelayUs(2);
      Retries += 2;
      if (Retries > 100)
      {
        DHT_Result = DHT_TIMEOUT; // Timeout error
        break;
      }
    }
    // Wait for the last response to finish (high for ~80us)
    Retries = 0;
    while (Handler->PlatformRead())
    {
      Handler->PlatformDelayUs(2);
      Retries += 2;
      if (Retries > 100)
      {
        DHT_Result = DHT_TIMEOUT; // Timeout error
        break;
      }
    }
  }
  //--------------------------------------

  //----- Step 3 - Data transmission -----
  if (DHT_Result == DHT_OK)
  {
    // Reading 5 bytes, bit by bit
    for (uint8_t i = 0; i < 5; i++)
    {
      for (int8_t j = 7; j >= 0; j--)
      {
        // There is always a leading low level of 50 us
        Retries = 0;
        while (!Handler->PlatformRead())
        {
          Handler->PlatformDelayUs(2);
          Retries += 2;
          if (Retries > 70)
          {
            DHT_Result = DHT_TIMEOUT; // Timeout error
            j = -1;                   // Break inner for-loop
            i = 5;                    // Break outer for-loop
            break;                    // Break while loop
          }
        }

        if (DHT_Result == DHT_OK)
        {
          // We read Data bit || 26-28us means '0' || 70us means '1'
          Handler->PlatformDelayUs(35); // Wait for more than 28us

          if (Handler->PlatformRead()) // If HIGH
            Buffer[i] |= (1 << j);     // bit = '1'

          Retries = 0;
          while (Handler->PlatformRead())
          {
            Handler->PlatformDelayUs(2);
            Retries += 2;
            if (Retries > 100)
            {
              DHT_Result = DHT_TIMEOUT; // Timeout error
              break;
            }
          }
        }
      }
    }
  }
  //--------------------------------------

  //----- Step 4 - Check checksum and return Data -----
  if (DHT_Result == DHT_OK)
  {
    if (((uint8_t)(Buffer[0] + Buffer[1] + Buffer[2] + Buffer[3])) != Buffer[4])
    {
      DHT_Result = DHT_CHECKSUM_ERROR; // Checksum error
    }
    else
    {
      // Build returning array
      // Buffer[0] = Humidity		(int)
      // Buffer[1] = Humidity		(dec)
      // Buffer[2] = Temperature		(int)
      // Buffer[3] = Temperature		(dec)
      // Buffer[4] = Checksum
      for (uint8_t i = 0; i < 4; i++)
      {
        Data[i] = Buffer[i];
      }
    }
  }
  //---------------------------------------------------

  return DHT_Result;
}

// Convert temperature Data to float temperature.
static float
DHT_ExtractTemperature(DHT_Handler_t *Handler, uint8_t Data2, uint8_t Data3)
{
  float Temp = 0.0;

  switch (Handler->Type)
  {
  case DHT_TYPE_DHT11:
    Temp = Data2;
    break;

  case DHT_TYPE_DHT22:
    //(Integral<<8 + Decimal) / 10
    Temp = ((Data2 & 0x80) ? ((((Data2 & 0x7F) << 8) | Data3) / (-10.0)) : (((Data2 << 8) | Data3) / 10.0));
    break;
  }

  return Temp;
}

static float
DHT_ExtractHumidity(DHT_Handler_t *Handler, uint8_t Data0, uint8_t Data1)
{
  float Hum = 0.0;

  switch (Handler->Type)
  {
  case DHT_TYPE_DHT11:
    Hum = Data0;
    break;

  case DHT_TYPE_DHT22:
    //(Integral<<8 + Decimal) / 10
    Hum = ((Data0 << 8) | Data1) / 10.0;
    break;
  }

  return Hum;
}



/**
 ==================================================================================
                            ##### Public Functions #####                           
 ==================================================================================
 */

/**
 * @brief  
 * @param  Handler: Pointer to handler
 * @param  Type: DHT type
 * @retval DHT_Result_t
 *         - DHT_OK: Operation was successful.
 *         - DHT_FAIL: Failed to send or receive Data.
 */
DHT_Result_t
DHT_Init(DHT_Handler_t *Handler, DHT_Type_t Type)
{
  if (Handler->PlatformSetDir == NULL ||
      Handler->PlatformWrite == NULL ||
      Handler->PlatformRead == NULL ||
      Handler->PlatformDelayMs == NULL ||
      Handler->PlatformDelayUs == NULL)
    return DHT_FAIL;

  Handler->Type = Type;

  return DHT_OK;
}

/**
 * @brief  Read sample
 * @param  Handler: Pointer to handler
 * @param  Temperature: Pointer to temperature variable
 * @param  Humidity: Pointer to humidity variable
 * @retval 
 */
DHT_Result_t
DHT_ReadSample(DHT_Handler_t *Handler,
               float *Temperature, float *Humidity)
{
  uint8_t Data[4] = {0};
  DHT_Result_t DHT_Result = DHT_OK;

  // Read Data
  DHT_Result = DHT_ReadRaw(Handler, Data);

  // If read successfully
  if (DHT_Result == DHT_OK)
  {
    // Calculate values
    *Temperature = DHT_ExtractTemperature(Handler, Data[2], Data[3]);
    *Humidity = DHT_ExtractHumidity(Handler, Data[0], Data[1]);

    // Check values

    switch (Handler->Type)
    {
    case DHT_TYPE_DHT11:
      if ((*Temperature < DHT11_Temperature_Min) || (*Temperature > DHT11_Temperature_Max))
        DHT_Result = DHT_FAIL;
      else if ((*Humidity < DHT11_Humidity_Min) || (*Humidity > DHT11_Humidity_Max))
        DHT_Result = DHT_FAIL;
      break;

    case DHT_TYPE_DHT22:
      if ((*Temperature < DHT22_Temperature_Min) || (*Temperature > DHT22_Temperature_Max))
        DHT_Result = DHT_FAIL;
      else if ((*Humidity < DHT22_Humidity_Min) || (*Humidity > DHT22_Humidity_Max))
        DHT_Result = DHT_FAIL;
      break;
    }
  }

  return DHT_Result;
}
