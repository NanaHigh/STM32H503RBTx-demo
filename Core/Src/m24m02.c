#include "m24m02.h"

uint8_t M24M02_MEM_ADDRESS = 0x50;
//uint8_t M24M02_ID_ADDRESS = 0x58;

void m24m02_mem_write(uint16_t reg_addr, uint8_t* reg_data, uint16_t len){
  HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&hi2c1,M24M02_MEM_ADDRESS<<1,reg_addr,2,reg_data,len,M24M02_TIMEOUT);
  while(status != HAL_OK){
    status = HAL_I2C_Mem_Write(&hi2c1,M24M02_MEM_ADDRESS<<1,reg_addr,2,reg_data,len,M24M02_TIMEOUT);
  }
}

void m24m02_mem_read(uint16_t reg_addr, uint8_t* reg_data, uint16_t len){
  HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1,M24M02_MEM_ADDRESS<<1,reg_addr,2,reg_data,len,M24M02_TIMEOUT);
  while(status != HAL_OK)
  {
    status = HAL_I2C_Mem_Read(&hi2c1,M24M02_MEM_ADDRESS<<1,reg_addr,2,reg_data,len,M24M02_TIMEOUT);
  }
}

uint8_t m24m02_detect(void) {
  uint8_t w_data[3] = {0x01, 0x02, 0x03};
  uint8_t r_data[3] = {0};
  m24m02_mem_write(0x0001, w_data, 3);
  HAL_Delay(5);
  m24m02_mem_read(0x0001, r_data, 3);
  for(uint8_t i = 0; i < 3; i++){
    if (w_data[i] != r_data[i]) {
      printf("M24M02 Not Found.\r\n");
      return 1;
    }
  }
  printf("M24M02 Detected.\r\n");
  return 0;
}