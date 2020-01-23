/*
    This file is part of Repetier-Firmware.

    Repetier-Firmware is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Repetier-Firmware is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Repetier-Firmware.  If not, see <http://www.gnu.org/licenses/>.

*/

// Additional informations: https://github.com/bigtreetech/BIGTREETECH-SKR-PRO-V1.1
/*
STM32F407ZGT6 processor, 180MHz, 1024kb flash, 192kb RAM
*/
#pragma once

#ifndef STM32F4
#error "Oops! Select BTT_SKR_PRO_V11 in platformio.ini -> default_envs"
#endif

#define KNOWN_BOARD

// Users expect Serial to be usb port!
#undef Serial
#define Serial SerialUSB

#define CPU_ARCH ARCH_ARM
#define MAX_RAM 196608 // BTT_SKR_PRO_V1_1

// Steppers
#define ORIG_X_STEP_PIN PE9
#define ORIG_X_DIR_PIN PF1
#define ORIG_X_ENABLE_PIN PF2
#define ORIG_X_CS_PIN PA15
#define ORIG_X_MIN_PIN PB10
#define ORIG_X_MAX_PIN PE15

#define ORIG_Y_STEP_PIN PE11
#define ORIG_Y_DIR_PIN PE8
#define ORIG_Y_ENABLE_PIN PD7
#define ORIG_Y_CS_PIN PB8
#define ORIG_Y_MIN_PIN PE12
#define ORIG_Y_MAX_PIN PE10

#define ORIG_Z_STEP_PIN PE13
#define ORIG_Z_DIR_PIN PC2
#define ORIG_Z_ENABLE_PIN PC0
#define ORIG_Z_CS_PIN PB9
#define ORIG_Z_MIN_PIN PG8
#define ORIG_Z_MAX_PIN PG5

#define ORIG_E0_STEP_PIN PE14
#define ORIG_E0_DIR_PIN PA0
#define ORIG_E0_ENABLE_PIN PC3
#define ORIG_E0_CS_PIN PB3

#define ORIG_E1_STEP_PIN PD15
#define ORIG_E1_DIR_PIN PE7
#define ORIG_E1_ENABLE_PIN PA3
#define ORIG_E1_CS_PIN PG15

#define ORIG_E2_STEP_PIN PD13
#define ORIG_E2_DIR_PIN PG9
#define ORIG_E2_ENABLE_PIN PF0
#define ORIG_E2_CS_PIN PG12

// Temperature Sensors
#define TEMP_0_PIN PF4 // T0
#define TEMP_2_PIN PF5 // T1
#define TEMP_3_PIN PF6 // T2
#define TEMP_1_PIN PF3 // TB bed

// Heaters / Fans
#define HEATER_0_PIN PB1  // E0 Timer 3 Channel 1
#define HEATER_2_PIN PD14 // E1 Timer 8 Channel 2
#define HEATER_3_PIN PB0  // E2 Timer 3 Channel 3
#define HEATER_1_PIN PD12  // bed Timer 5 Channel 2

#define ORIG_FAN_PIN PC8  // Timer 8 Channel 4
#define ORIG_FAN2_PIN PE5 // Timer 1 Channel 1
#define ORIG_FAN3_PIN PE6 // Timer 1 Channel 1

#ifndef TWI_CLOCK_FREQ
#define TWI_CLOCK_FREQ 400000
#endif

#define EEPROM_SERIAL_ADDR 0x50  // 7 bit i2c address (without R/W bit)
#define EEPROM_PAGE_SIZE 64      // page write buffer size
#define EEPROM_PAGE_WRITE_TIME 7 // page write time in milliseconds (docs say 5ms but that is too short)
#define FLASH_START 0x08000000ul
#define FLASH_SIZE  0x00100000ul // flash size excluding bootloader
#ifndef FLASH_EEPROM_SIZE
#define FLASH_EEPROM_SIZE 0x20000ul // use 128kb flash to prevent early destruction of flash but leave room for firmware
#define FLASH_SECTOR 7              // sector 7 is last 128kb
#endif
#ifndef EEPROM_AVAILABLE
#define EEPROM_AVAILABLE EEPROM_FLASH
#endif

// SPI
#define SCK_PIN PC10
#define MISO_PIN PC11
#define MOSI_PIN PC12

//
// Misc. Functions
//
#ifndef SDSS
#define SDSS PA4
#endif
#define SDPOWER -1
#define ORIG_SDCARDDETECT PB11

#define LED_PIN -1
#define ORIG_PS_ON_PIN -1 <

// LCD / Controller
#ifndef CUSTOM_CONTROLLER_PINS
#if FEATURE_CONTROLLER != CONTROLLER_NONE

#define UI_DISPLAY_RS_PIN PE10
#define UI_DISPLAY_RW_PIN -1
#define UI_DISPLAY_ENABLE_PIN PE9
#define UI_DISPLAY_D4_PIN PE12
#define UI_DISPLAY_D5_PIN PE13
#define UI_DISPLAY_D6_PIN PE14
#define UI_DISPLAY_D7_PIN PE15
#define UI_ENCODER_A PB1
#define UI_ENCODER_B PB2
#define UI_ENCODER_CLICK PE7
#ifndef BEEPER_PIN
#define BEEPER_PIN PE8
#endif
#ifndef SDCARDDETECT
#define SDCARDDETECT PB0
#endif

#endif
#endif
