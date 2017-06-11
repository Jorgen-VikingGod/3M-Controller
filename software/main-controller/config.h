/*
 * config.h
 *
 *  Created on: Feb 16, 2017
 *      Author: Juergen Skrotzky (JorgenVikingGod@gmail.com)
 */

#ifndef CONFIG_H_
#define CONFIG_H_

// Debug mode
// set to 0 to disable debug prints
#define _debug 0
#define _display 0


#if defined(__MK20DX256__)
#define ENABLE_PIN        2
#define ENABLE_LOOP_PIN   3
#define INTERRUPT_PIN     4
#define OLED_CS_PIN       6
#define OLED_RST_PIN      7
#define OLED_DC_PIN       8
#elif defined(STM32L432xx)
// I2C_SDA                4u
// I2C_SCL                5u
// SPI_MOSI               11u
// SPI_MISO               12u
// SPI_SCK                13u
// SPI_SS                 10
#define I2C_SDA           SDA
#define I2C_SCL           SCL
#define ENABLE_PIN        16
#define ENABLE_LOOP_PIN   17
#define INTERRUPT_PIN     4
#define OLED_CS_PIN       SS
#define OLED_RST_PIN      14
#define OLED_DC_PIN       12
#else
// I2C_SDA                21
// I2C_SCL                22
// SPI_MOSI               23
// SPI_MISO               19
// SPI_SCK                18
// SPI_SS                 5
#define I2C_SDA           SDA
#define I2C_SCL           SCL
#define ENABLE_PIN        16
#define ENABLE_LOOP_PIN   17
#define INTERRUPT_PIN     4
#define OLED_CS_PIN       SS
#define OLED_RST_PIN      14
#define OLED_DC_PIN       12
/*
// I2C_SDA                D2
// I2C_SCL                D1
// SPI_MOSI               D7
// SPI_MISO               D6
// SPI_SCK                D5
// SPI_SS                 D8
#define I2C_SDA           D2
#define I2C_SCL           D1
#define ENABLE_PIN        D6
#define ENABLE_LOOP_PIN   D0
#define INTERRUPT_PIN     D4
#define OLED_CS_PIN       D8
#define OLED_RST_PIN      -1
#define OLED_DC_PIN       D3
*/
#endif

/*
 * helper variables for connection (ip, mac, formatted UID)
 */
String strIPAddr;
String strClientID;
String strChipID;
String strHostname;

#endif // CONFIG_H_
