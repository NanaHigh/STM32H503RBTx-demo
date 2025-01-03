#include "icm42688.h"

void icm42688_write_one_bit(uint8_t reg_addr, uint8_t reg_data){
  HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&hi2c1,ICM42688_ADDRESS<<1,reg_addr,1,&reg_data,1,ICM42688_TIMEOUT);
  while(status != HAL_OK){
    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&hi2c1,ICM42688_ADDRESS<<1,reg_addr,1,&reg_data,1,ICM42688_TIMEOUT);
  }
}

uint8_t icm42688_read_one_bit(uint8_t reg_addr){
  uint8_t data = 0;
  HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1,ICM42688_ADDRESS<<1,reg_addr,1,&data,1,ICM42688_TIMEOUT);
  while(status != HAL_OK){
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1,ICM42688_ADDRESS<<1,reg_addr,1,&data,1,ICM42688_TIMEOUT);
  }
  return data;
}

uint8_t icm42688_detect(void) {
  uint8_t id = 0;
  id = icm42688_read_one_bit(ICM42688_WHO_AM_I);
  if(id == 0x47){
    printf("ICM42688 Detected.\r\n");
    return 0;
  }else{
    printf("ICM42688 Not Found.\r\n");
    return 1;
  }
}

icm42688_cfg_inv icm42688_init(icm42688_afs afs, icm42688_aodr aodr, icm42688_gfs gfs, icm42688_godr godr) {
  icm42688_cfg_inv inv = {0};
  
  switch (afs) {
  case ICM42688_AFS_2G:
    inv.acc_inv = 2000.0 / 32768.0;
    break;
  case ICM42688_AFS_4G:
    inv.acc_inv = 4000.0 / 32768.0;
    break;
  case ICM42688_AFS_8G:
    inv.acc_inv = 6000.0 / 32768.0;
    break;
  case ICM42688_AFS_16G:
    inv.acc_inv = 18000.0 / 32768.0;
    break;
  case NUM_ICM42688__AFS:
    inv.acc_inv = 1.0;
    break;
  }
  
  switch (gfs) {
  case ICM42688_GFS_15_625DPS:
    inv.gyro_inv = 15.625 / 32768.0;
    break;
  case ICM42688_GFS_31_25DPS:
    inv.gyro_inv = 31.25 / 32768.0;
    break;
  case ICM42688_GFS_62_5DPS:
    inv.gyro_inv = 62.5 / 32768.0;
    break;
  case ICM42688_GFS_125DPS:
    inv.gyro_inv = 125.0 / 32768.0;
    break;
  case ICM42688_GFS_250DPS:
    inv.gyro_inv = 250.0 / 32768.0;
    break;
  case ICM42688_GFS_500DPS:
    inv.gyro_inv = 500.0 / 32768.0;
    break;
  case ICM42688_GFS_1000DPS:
    inv.gyro_inv = 1000.0 / 32768.0;
    break;
  case ICM42688_GFS_2000DPS:
    inv.gyro_inv = 2000.0 / 32768.0;
    break;
  case NUM_ICM42688_GFS:
    inv.gyro_inv = 1.0;
    break;
  }
  
  // 复位
  icm42688_write_one_bit(ICM42688_PWR_MGMT0, 0x00);
  HAL_Delay(10);
  
  // 配置寄存器
  icm42688_write_one_bit(ICM42688_ACCEL_CONFIG0, (uint8_t)afs << 5 | (uint8_t) aodr + 1);
  icm42688_write_one_bit(ICM42688_GYRO_ACCEL_CONFIG0, (uint8_t)gfs << 5 | (uint8_t) godr + 1);

  // 设置GYRO_MODE,ACCEL_MODE为低噪声模式.
  icm42688_write_one_bit(ICM42688_PWR_MGMT0, 0x0F);
  
  return inv;
}

icm42688_cfg_inv icm42688_default_init(void){
  return icm42688_init(ICM42688_AFS_16G, ICM42688_AODR_1000HZ, ICM42688_GFS_2000DPS, ICM42688_GODR_1000HZ);
}

icm42688_raw_val icm42688_get_data(icm42688_cfg_inv inv){
  icm42688_raw_val val = {0};
  int8_t lsb = 0;
  int8_t msb = 0;
  
  // 温度
  lsb = icm42688_read_one_bit(ICM42688_TEMP_DATA0);
  msb = icm42688_read_one_bit(ICM42688_TEMP_DATA1);
  val.temperature = (msb << 8 | lsb) / 132.48 + 25.0;
  
  // 加速度
  lsb = icm42688_read_one_bit(ICM42688_ACCEL_DATA_X0);
  msb = icm42688_read_one_bit(ICM42688_ACCEL_DATA_X1);
  val.acc[0] = inv.acc_inv * (msb << 8 | lsb);
  lsb = icm42688_read_one_bit(ICM42688_ACCEL_DATA_Y0);
  msb = icm42688_read_one_bit(ICM42688_ACCEL_DATA_Y1);
  val.acc[1] = inv.acc_inv * (msb << 8 | lsb);
  lsb = icm42688_read_one_bit(ICM42688_ACCEL_DATA_Z0);
  msb = icm42688_read_one_bit(ICM42688_ACCEL_DATA_Z1);
  val.acc[2] = inv.acc_inv * (msb << 8 | lsb);
  
  // 角速度
  lsb = icm42688_read_one_bit(ICM42688_GYRO_DATA_X0);
  msb = icm42688_read_one_bit(ICM42688_GYRO_DATA_X1);
  val.gyro[0] = inv.gyro_inv * (msb << 8 | lsb);
  lsb = icm42688_read_one_bit(ICM42688_GYRO_DATA_Y0);
  msb = icm42688_read_one_bit(ICM42688_GYRO_DATA_Y1);
  val.gyro[1] = inv.gyro_inv * (msb << 8 | lsb);
  lsb = icm42688_read_one_bit(ICM42688_GYRO_DATA_Y0);
  msb = icm42688_read_one_bit(ICM42688_GYRO_DATA_Y1);
  val.gyro[2] = inv.gyro_inv * (msb << 8 | lsb);
  
  return val;
}



