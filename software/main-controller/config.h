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

<<<<<<< HEAD
#define ENABLE_PIN        2
#define ENABLE_LOOP_PIN   3
#define INTERRUPT_PIN     4
#define OLED_CS_PIN       6
#define OLED_RST_PIN      7
#define OLED_DC_PIN       8
=======
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
>>>>>>> 9a84e43... esp32 version only

/*
 * helper variables for connection (ip, mac, formatted UID)
 */
String strIPAddr;
String strClientID;
String strChipID;
String strHostname;

#endif // CONFIG_H_
