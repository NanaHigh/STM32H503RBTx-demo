#include <LowLevelIOInterface.h>
#include <stdint.h>
#include <stdio.h>
#include "usart.h"
#pragma module_name = "?__write"

extern UART_HandleTypeDef hlpuart1;

/// IAR新版本串口打印需要

size_t __write(int handle, const unsigned char * buffer, size_t size)
{
  if (buffer == 0)
  {
    return 0;
  }
  if (handle != _LLIO_STDOUT && handle != _LLIO_STDERR)
  {
    return _LLIO_ERROR;
  }
  
  /* Sending in normal mode */
  if(HAL_UART_Transmit(&hlpuart1, (uint8_t *)buffer, size, 10) == HAL_OK)
  {
    return size;
  }
  else
  {
    return _LLIO_ERROR;
  }
}
