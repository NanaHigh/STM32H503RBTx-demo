#ifndef __W25QXX_H
#define __W25QXX_H

#include "stdint.h"
#include "stdio.h"
#include "gpio.h"
#include "spi.h"

#define W25Q_CS_HIGH HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, 1)
#define W25Q_CS_LOW HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, 0)

// Command
#define W25Q_WriteEnable 0x06 
#define W25Q_WriteDisable 0x04 
#define W25Q_ReadStatusReg1 0x05 
#define W25Q_ReadStatusReg2 0x35 
#define W25Q_WriteStatusReg 0x01 
#define W25Q_WriteVolatileEnable 0x50
#define W25Q_ReadData 0x03 
#define W25Q_FastReadData 0x0B 
#define W25Q_FastReadDual 0x3B 
#define W25Q_PageProgram 0x02 
#define W25Q_BlockErase 0xD8 
#define W25Q_SectorErase 0x20 
#define W25Q_ChipErase 0xC7 
#define W25Q_PowerDown 0xB9 
#define W25Q_ReleasePowerDown 0xAB 
#define W25Q_DeviceID 0xAB 
#define W25Q_ManufactDeviceID 0x90
#define W25Q_UniqueID 0x4B
#define W25Q_JedecDeviceID 0x09
#define W25Q_Enable_Reset 0x66
#define W25Q_Reset_Device 0x99
#define W25Q_Enter_4_Addr 0xb7
#define W25Q_Read_4_Addr 0x13
#define W25Q_Write_4_Addr 0x02
#define W25Q_Exit_4_Addr 0xe9

#define W25Q_TIMEOUT 10

#define	W25Q_SECTOR_SIZE 4096

typedef enum{
  W25Q80 = 0XEF13,
  W25Q16,
  W25Q32,
  W25Q64,
  W25Q128,
  W25Q256
}w25q_type;

void spi_write_one_byte(uint8_t data);
void spi_write_bytes(uint8_t* data, uint16_t len);
uint8_t spi_read_one_byte();
void spi_read_bytes(uint8_t* data, uint16_t len);
uint16_t w25q_id(void);
uint64_t w25q_serial_num(void);
uint16_t w25q_detect(void);
void w25q_init(void);
void w25q_write_enable(void);
void w25q_write_disable(void);
void w25q_write_volatile_enable(void);
uint8_t w25q_sr1(void);
uint8_t w25q_sr2(void);
void w25q_write_sr1(uint8_t data);
void w25q_busy_wait(void);
void w25q_power_down(void);
void w25q_wakeup(void);
void w25q_erase_chip(void);
void w25q_erase_sector(uint32_t addr);
void w25q_erase_block(uint32_t addr);
uint32_t w25q_block_addr(uint8_t block);
uint32_t w25q_sector_addr(uint16_t sector);
uint32_t w25q_page_addr(uint16_t page);
uint32_t w25q_block2sector_addr(uint8_t block, uint8_t sub_sector);
uint32_t w25q_block2sector2page_addr(uint8_t block, uint8_t sub_sector, uint8_t sub_page);
void w25q_split_addr(uint32_t in_addr, uint8_t* out_addr);
uint8_t w25q_read_one_byte(uint32_t addr);
void w25q_read_bytes(uint32_t addr, uint8_t* data, uint16_t len);
void w25q_fast_read_bytes(uint32_t addr, uint8_t* data, uint16_t len);
void w25q_write_by_page(uint32_t addr, uint8_t* data, uint16_t len);
void w25q_write_by_sector(uint32_t addr, uint8_t* data, uint16_t len);

#endif // __W25QXX_H