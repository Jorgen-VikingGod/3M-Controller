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

#define ENABLE_PIN        2
#define ENABLE_LOOP_PIN   3
#define INTERRUPT_PIN     4
#define OLED_CS_PIN       6
#define OLED_RST_PIN      7
#define OLED_DC_PIN       8

/*
 * helper variables for connection (ip, mac, formatted UID)
 */
String strIPAddr;
String strClientID;
String strChipID;
String strHostname;

#endif // CONFIG_H_
