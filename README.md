# STM32H503RBT6 Development Board Test Demo

## Introduction

This project is a test demo for [my own STM32H503RBT6 development board](https://oshwhub.com/monika13799/stm32h503-development-board).

This project is based on STM32CubeMX and IAR EWARM IDE.

## Features

- STM32H503RBT6 microcontroller
- RGB LED
  - GREEN     (PA0)
  - RED       (PA1)
  - BLUE      (PA2)
- BUZZER
  - BUZZER    (PA13)
- W25QXX FLASH
  - SPI1_SCK  (PC0)
  - SPI1_CS   (PC1)
  - SPI1_MISO (PC2)
  - SPI1_MOSI (PC3)
- M24M02 EEPROM
  - I2C
    - I2C1_SCL  (PB6)
    - I2C1_SDA  (PB7)
  - I3C(Not implemented in this demo)
    - I3C1_SCL  (PB6)
    - I3C1_SDA  (PB7)
- ICM42688 IMU
  - I2C
    - I2C1_SCL  (PB6)
    - I2C1_SDA  (PB7)
  - I3C(Not implemented in this demo)
    - I3C1_SCL  (PB6)
    - I3C1_SDA  (PB7)
- CH340 USB2Serial
  - USART1_TX (PA9)
  - USART1_RX (PA10)
- 4x3 matrix keys implemented by BC6301
  - USART1_RX (PA3)
- USB(Not implemented in this demo)
  - USB_DP  (PA12)
  - USB_DM  (PA11)

## Warning

Only one of the two TypeC ports on the board can be plugged in for power supply or data transmission.
