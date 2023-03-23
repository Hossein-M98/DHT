# DHT Library
Basic routines for interfacing a DHT sensor (DHT11, DHT22)


## Hardware Support
It is easy to port this library to any platform. But now it is ready for use in:
- STM32 (HAL)
- ESP32 (esp-idf)
- AVR (ATmega32)

## How To Use
1. Add `DHT.h` and `DHT.c` files to your project.  It is optional to use `DHT_platform.h` and `DHT_platform.c` files (open and config `DHT_platform.h` file).
2. Initialize platform-dependent part of handler.
4. Call `DHT_Init()`.
5. Call other functions and enjoy.

## Example
<details>
<summary>Using DHT_platform files</summary>

```c
#include <stdio.h>
#include "DHT.h"
#include "DHT_platform.h"

int main(void)
{
  DHT_Handler_t Handler = {0};

  DHT_Platform_Init(&Handler);
  DHT_Init(&Handler, DHT_TYPE_DHT22);

  while (1)
  {
    float Temp = 0
    float Hum = 0;

    DHT_ReadSample(&Handler, &Temp, &Hum);
    printf("Temp: %f, Hum: %f\r\n", Temp, Hum);
  }

  return 0;
}
```
</details>


<details>
<summary>Without using DHT_platform files (ATmega32)</summary>

```c
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include "DHT.h"


#define DHT_PORT    PORTB
#define DHT_PIN     PINB
#define DHT_DDR     DDRB
#define DHT_BIT     0


static void
PlatformSetDir(uint8_t Dir)
{
  if (Dir)
    DHT_DDR |= (1 << DHT_BIT);
  else
    DHT_DDR &= ~(1 << DHT_BIT);
}

static void
PlatformWrite(uint8_t Level)
{
  if (Level)
    DHT_PORT |= (1 << DHT_BIT);
  else
    DHT_PORT &= ~(1 << DHT_BIT);
}

static uint8_t
PlatformRead(void)
{
  return ((DHT_PIN >> DHT_BIT) & 0x01);
}

static void
PlatformDelayMs(uint16_t Delay)
{
  for (; Delay > 0; Delay--)
    _delay_ms(1);
}

static void
PlatformDelayUs(uint16_t Delay)
{
  for (; Delay > 0; Delay--)
    _delay_us(1);
}


int main(void)
{
  DHT_Handler_t Handler = {0};

  Handler.PlatformSetDir = PlatformSetDir;
  Handler.PlatformWrite = PlatformWrite;
  Handler.PlatformRead = PlatformRead;
  Handler.PlatformDelayMs = PlatformDelayMs;
  Handler.PlatformDelayUs = PlatformDelayUs;

  DHT_Init(&Handler, DHT_TYPE_DHT22);

  while (1)
  {
    float Temp = 0
    float Hum = 0;

    DHT_ReadSample(&Handler, &Temp, &Hum);

    // We consider that the UART is initialized and STDOUT is redirected to it
    printf("Temp: %f, Hum: %f\r\n", Temp, Hum);
  }

  return 0;
}
```
</details>
