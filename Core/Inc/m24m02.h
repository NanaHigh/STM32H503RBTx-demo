#ifndef __M24M02_H
#define __M24M02_H

#include "stdint.h"
#include "stdio.h"
#include "i2c.h"

#define M24M02_TIMEOUT 10

uint8_t m24m02_detect(void);
void m24m02_mem_write(uint16_t reg_addr, uint8_t* reg_data, uint16_t len);
void m24m02_mem_read(uint16_t reg_addr, uint8_t* reg_data, uint16_t len);

#endif /* __M24M02_H */