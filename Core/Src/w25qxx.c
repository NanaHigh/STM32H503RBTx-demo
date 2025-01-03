#include "w25qxx.h"

void spi_write_one_byte(uint8_t data) {
  HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi1, (uint8_t*)&data, 1, W25Q_TIMEOUT);
  while(status != HAL_OK)
  {
    status = HAL_SPI_Transmit(&hspi1, (uint8_t*)&data, 1, W25Q_TIMEOUT);
  }
}

void spi_write_bytes(uint8_t* data, uint16_t len) {
  HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi1, data, len, W25Q_TIMEOUT);
  while(status != HAL_OK)
  {
    HAL_SPI_Transmit(&hspi1, data, len, W25Q_TIMEOUT);
  }
}

uint8_t spi_read_one_byte() {
  uint8_t data = 0;
  HAL_StatusTypeDef status = HAL_SPI_Receive(&hspi1, (uint8_t*)&data, 1, W25Q_TIMEOUT);
  while(status != HAL_OK)
  {
    status = HAL_SPI_Receive(&hspi1, (uint8_t*)&data, 1, W25Q_TIMEOUT);
  }
  return data;
}

void spi_read_bytes(uint8_t* data, uint16_t len) {
  HAL_StatusTypeDef status = HAL_SPI_Receive(&hspi1, data, len, W25Q_TIMEOUT);
  while(status != HAL_OK)
  {
    status = HAL_SPI_Receive(&hspi1, data, len, W25Q_TIMEOUT);
  }
}

uint16_t w25q_id(void){
  uint16_t id = 0;
  W25Q_CS_LOW;
  spi_write_one_byte(W25Q_ManufactDeviceID);
  spi_write_one_byte(0x00);
  spi_write_one_byte(0x00);
  spi_write_one_byte(0x00);
  id = spi_read_one_byte() << 8;
  id |= spi_read_one_byte();
  W25Q_CS_HIGH;
  return id;
}

uint64_t w25q_serial_num(void) {
  uint8_t temp = 0;
  uint64_t serial_num = 0;
  
  W25Q_CS_LOW;
  spi_write_one_byte(W25Q_UniqueID);
  spi_write_one_byte(0x00);
  spi_write_one_byte(0x00);
  spi_write_one_byte(0x00);
  for(uint8_t i = 0; i < 8; i++){
    temp = spi_read_one_byte();
    serial_num = (serial_num << 8) | temp;
  }
  W25Q_CS_HIGH;
  
  return serial_num;
}

uint16_t w25q_detect(void){
  uint16_t id = w25q_id();
  uint16_t max_block = 0;
  switch(id){
  case W25Q80: 
    printf("W25Q80 Detected.\r\nW25Q80");
    max_block = 16;
    break;
  case W25Q16: 
    printf("W25Q16 Detected.\r\nW25Q16 ");
    max_block = 32;
    break;
  case W25Q32: 
    printf("W25Q32 Detected.\r\nW25Q32 ");
    max_block = 64;
    break;
  case W25Q64: 
    printf("W25Q64 Detected.\r\nW25Q64 ");
    max_block = 128;
    break;
  case W25Q128: 
    printf("W25Q128 Detected.\r\nW25Q128 ");
    max_block = 256;
    break;
  case W25Q256: 
    printf("W25Q256 Detected.\r\nW25Q256 ");
    max_block = 512;
    break;
  default:
    printf("W25QXX Not Found.\r\n");
    return 0;
  }
  printf("SN: 0x%llX.\r\n", w25q_serial_num());
  return id;
}         

void w25q_init(void){
  W25Q_CS_LOW;
  spi_write_one_byte(W25Q_Enable_Reset);
  spi_write_one_byte(W25Q_Reset_Device);
  W25Q_CS_HIGH;
}

void w25q_write_enable(void) {
  W25Q_CS_LOW;
  spi_write_one_byte(W25Q_WriteEnable);
  W25Q_CS_HIGH;
}

void w25q_write_disable(void) {
  W25Q_CS_LOW;
  spi_write_one_byte(W25Q_WriteDisable);
  W25Q_CS_HIGH;
}

void w25q_write_volatile_enable(void){
  W25Q_CS_LOW;
  spi_write_one_byte(W25Q_WriteVolatileEnable);
  W25Q_CS_HIGH;
}

uint8_t w25q_sr1(void){
  uint8_t data = 0;
  w25q_write_enable();
  W25Q_CS_LOW;
  spi_write_one_byte(W25Q_ReadStatusReg1);
  data = spi_read_one_byte();
  W25Q_CS_HIGH;
  return data;
}

uint8_t w25q_sr2(void){
  uint8_t data = 0;
  W25Q_CS_LOW;
  spi_write_one_byte(W25Q_ReadStatusReg2);
  data = spi_read_one_byte();
  W25Q_CS_HIGH;
  return data;
}

void w25q_write_sr1(uint8_t data){
  W25Q_CS_LOW;
  spi_write_one_byte(0x01);
  spi_write_one_byte(0x01);
  W25Q_CS_HIGH;
}

void w25q_busy_wait(void) {
  uint8_t SR1 = 0;
  SR1 = w25q_sr1();
  while((SR1 & 0x01)==0x01)
  {
    HAL_Delay(1);
    SR1 = w25q_sr1();
  }
}

void w25q_power_down(void) {
  W25Q_CS_LOW;
  spi_write_one_byte(W25Q_PowerDown);
  W25Q_CS_HIGH;
  HAL_Delay(1);
}

void w25q_wakeup(void) {
  W25Q_CS_LOW;
  spi_write_one_byte(W25Q_ReleasePowerDown);
  W25Q_CS_HIGH;
  HAL_Delay(1);
}


void w25q_erase_chip(void) {
  w25q_write_enable();
  w25q_busy_wait();
  W25Q_CS_LOW;
  spi_write_one_byte(W25Q_ChipErase);
  W25Q_CS_HIGH;
  w25q_busy_wait();
}

void w25q_erase_sector(uint32_t addr) {
  uint8_t split_addr[3] = {0};
  w25q_split_addr(addr, split_addr);
  w25q_write_enable();
  w25q_busy_wait();
  
  W25Q_CS_LOW;
  spi_write_one_byte(W25Q_SectorErase);
  spi_write_one_byte(split_addr[0]);
  spi_write_one_byte(split_addr[1]);
  spi_write_one_byte(split_addr[2]);
  W25Q_CS_HIGH;
  w25q_busy_wait();
}

void w25q_erase_block(uint32_t addr) {
  uint8_t split_addr[3] = {0};
  w25q_split_addr(addr, split_addr);
  w25q_write_enable();
  w25q_busy_wait();
  W25Q_CS_LOW;
  spi_write_one_byte(W25Q_BlockErase);
  spi_write_one_byte(split_addr[0]);
  spi_write_one_byte(split_addr[1]);
  spi_write_one_byte(split_addr[2]);
  W25Q_CS_HIGH;
  w25q_busy_wait();
}

uint32_t w25q_block_addr(uint8_t block) {
  return block << 12;
}

uint32_t w25q_sector_addr(uint16_t sector) {
  if (sector > W25Q_SECTOR_SIZE - 1) return 0;
  else return sector << 12;
}

uint32_t w25q_page_addr(uint16_t page) {
  return page << 16;
}

uint32_t w25q_block2sector_addr(uint8_t block, uint8_t sub_sector) {
  if(sub_sector > 15) sub_sector = 0;
  return (block << 16) | (sub_sector << 12);
}

uint32_t w25q_block2sector2page_addr(uint8_t block, uint8_t sub_sector, uint8_t sub_page) {
  if(sub_sector > 15) sub_sector = 0;
  if(sub_page > 15) sub_page = 0;
  return (block << 16) | (sub_sector << 12) | (sub_page << 8);
}

void w25q_split_addr(uint32_t in_addr, uint8_t* out_addr) {
  out_addr[0] = in_addr >> 16;
  out_addr[1] = (in_addr & 0x0000FFFF) >> 8;
  out_addr[2] = in_addr & 0x000000FF;
}

uint8_t w25q_read_one_byte(uint32_t addr) {
  uint8_t split_addr[3] = {0};
  uint8_t data = 0;
  w25q_split_addr(addr, split_addr);
  
  W25Q_CS_LOW;
  spi_write_one_byte(W25Q_ReadData);
  spi_write_one_byte(split_addr[0]);
  spi_write_one_byte(split_addr[1]);
  spi_write_one_byte(split_addr[2]);
  data = spi_read_one_byte();
  W25Q_CS_HIGH;
  
  return data;
}

void w25q_read_bytes(uint32_t addr, uint8_t* data, uint16_t len) {
  uint8_t split_addr[3] = {0};
  w25q_split_addr(addr, split_addr);
  
  W25Q_CS_LOW;
  spi_write_one_byte(W25Q_ReadData);
  spi_write_one_byte(split_addr[0]);
  spi_write_one_byte(split_addr[1]);
  spi_write_one_byte(split_addr[2]);
  spi_read_bytes(data, len);
  W25Q_CS_HIGH;
}

void w25q_fast_read_bytes(uint32_t addr, uint8_t* data, uint16_t len) {
  uint8_t split_addr[3] = {0};
  w25q_split_addr(addr, split_addr);
  
  W25Q_CS_LOW;
  spi_write_one_byte(W25Q_FastReadData);
  spi_write_one_byte(split_addr[0]);
  spi_write_one_byte(split_addr[1]);
  spi_write_one_byte(split_addr[2]);
  spi_write_one_byte(0x00);
  spi_read_bytes(data, len);
  W25Q_CS_HIGH;
}

void w25q_write_by_page(uint32_t addr, uint8_t* data, uint16_t len) {
  uint8_t split_addr[3] = {0};
  w25q_split_addr(addr, split_addr);
  w25q_write_enable();
  
  W25Q_CS_LOW;
  spi_write_one_byte(W25Q_PageProgram);
  spi_write_one_byte(split_addr[0]);
  spi_write_one_byte(split_addr[1]);
  spi_write_one_byte(split_addr[2]);
  spi_write_bytes(data, len);
  W25Q_CS_HIGH;
  
  w25q_busy_wait();
}

void w25q_write_by_sector(uint32_t addr, uint8_t* data, uint16_t len) {
  uint8_t cnt = (len / W25Q_SECTOR_SIZE);
  if(len % W25Q_SECTOR_SIZE) cnt++;
  
  uint32_t start_addr = addr;
  for (uint8_t k=0; k<cnt; k++)
  {
    w25q_erase_sector(start_addr);
    start_addr += W25Q_SECTOR_SIZE;
  }
  
  uint16_t  leftBytes=len % W25Q_SECTOR_SIZE;  //非整数个Page剩余的字节数，即最后一个Page写入的数据
  uint16_t  pgCount=len/W25Q_SECTOR_SIZE;  //前面整数个Page
  uint8_t* buff=data;
  
  for(uint16_t i=0; i<pgCount; i++){	//写入前面pgCount个Page的数据，
    w25q_write_by_page(addr, buff, W25Q_SECTOR_SIZE);  //写一整个Page的数据
    addr += W25Q_SECTOR_SIZE;	//地址移动一个Page
    buff += W25Q_SECTOR_SIZE;		//数据指针移动一个Page大小
  }
  
  if (leftBytes>0)
    w25q_write_by_page(addr, buff, leftBytes);  //最后一个Page，不是一整个Page的数据
}