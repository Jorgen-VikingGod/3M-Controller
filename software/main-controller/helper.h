/*
   helper.h

    Created on: Jan 30, 2017
        Author: Juergen Skrotzky (JorgenVikingGod@gmail.com)
*/

#ifndef HELPER_H_
#define HELPER_H_

#include "config.h"
/*
#include <TeensyThreads.h>

ThreadWrap(Serial, SerialX);
#define Serial ThreadClone(SerialX)
*/

uint32_t FreeRam(){ // for Teensy 3.0
    uint32_t stackTop;
    uint32_t heapTop;

    // current position of the stack.
    stackTop = (uint32_t) &stackTop;

    // current position of heap.
    void* hTop = malloc(1);
    heapTop = (uint32_t) hTop;
    free(hTop);

    // The difference is the free, available ram.
    return stackTop - heapTop;
}

int freeRam () 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

static const char HEX_CHAR_ARRAY[17] = "0123456789ABCDEF";
/**
 * convert char array (hex values) to readable string by seperator
 * buf:           buffer to convert
 * length:        data length
 * strSeperator   seperator between each hex value
 * return:        formated value as String
 */
static String byteToHexString(unsigned char* buf, unsigned char length, String strSeperator="-") {
  String dataString = "";
  for (unsigned char i = 0; i < length; i++) {
    byte v = buf[i] / 16;
    byte w = buf[i] % 16;
    if (i>0) {
      dataString += strSeperator;
    }
    dataString += String(HEX_CHAR_ARRAY[v]);
    dataString += String(HEX_CHAR_ARRAY[w]);
  }
  dataString.toUpperCase();
  return dataString;
} // byteToHexString

void DEBUG_PRINT_HEX(byte b) {
  if (_debug) {
    byte v = b / 16;
    byte w = b % 16;
    Serial.print(HEX_CHAR_ARRAY[v]);
    Serial.print(HEX_CHAR_ARRAY[w]);
  }
}
void DEBUG_PRINTLN_HEX(byte b) {
  if (_debug) {
    byte v = b / 16;
    byte w = b % 16;
    Serial.print(HEX_CHAR_ARRAY[v]);
    Serial.println(HEX_CHAR_ARRAY[w]);
  }
}

#if !defined(STM32L432xx)
template <typename... Args>
void DEBUG_PRINTF(const char *format, Args &&...args) {
  if (_debug) {
    Serial.printf(format, args...);
  }
}
#endif
template <typename Generic>
void DEBUG_PRINT(Generic text) {
  if (_debug) {
    Serial.print(text);
  }
}
template <typename Generic, typename Format>
void DEBUG_PRINT(Generic text, Format format) {
  if (_debug) {
    Serial.print(text, format);
  }
}
template <typename Generic>
void DEBUG_PRINTLN(Generic text) {
  if (_debug) {
    Serial.println(text);
  }
}
template <typename Generic, typename Format>
void DEBUG_PRINTLN(Generic text, Format format) {
  if (_debug) {
    Serial.println(text, format);
  }
}

inline bool checkBit(unsigned int var, uint8_t pos) {
  return (((var)>>(pos)) & 1);
}

/**
 * copy one buffer to the other
 * dest:          destination buffer
 * source:        source buffer
 * length:        data length
 * destStart:     [optional] start block of destination buffer
 * sourceStart:   [optional] start block of source buffer
 * return:        error code (0 = no error, -1 = error)
 */
static int copyBuffer(uint8_t* dest, uint8_t* source, int length, int destStart=0, int sourceStart=0) {
  if (destStart < 0 || sourceStart < 0) {
    return -1;
  }
  memcpy(&dest[destStart], &source[sourceStart], length);
  return 0;
} // copyBuffer
/**
 * compare two buffers
 * first:         first buffer
 * second:        second buffer
 * length:        data length of buffers
 * return:        error code (0 = no error, -1 = error)
 */
static bool compareBuffer(uint8_t* first, uint8_t* second, int length) {
  bool bReturn = true;
  if (length <= 0) {
    bReturn = false;
  } else { // test each element to be the same. if not, return false
    for (int n=0; n < length; ++n) {
      if (first[n] != second[n]) {
        bReturn = false;
        break;
      }
    }
  }
  DEBUG_PRINT(F("Helper: "));
  DEBUG_PRINT(bReturn ? "true" : "false");
  DEBUG_PRINT(F(" = compareBuffer("));
  DEBUG_PRINT(byteToHexString(first, length, " "));
  DEBUG_PRINT(F(", "));
  DEBUG_PRINT(byteToHexString(second, length, " "));
  DEBUG_PRINT(F(", "));
  DEBUG_PRINT(length);
  DEBUG_PRINTLN(F(")"));
  return bReturn;
} // compareBuffer

inline String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

#endif // HELPER_H_
