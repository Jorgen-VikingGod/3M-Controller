/*
 * i2c_slider_device.h
 *
 *  Created on: Feb 13, 2017
 *      Author: Juergen Skrotzky (JorgenVikingGod@gmail.com)
 */

#ifndef I2C_SLIDER_DEVICE_H_
#define I2C_SLIDER_DEVICE_H_

#include "helper.h"
#include "i2c_device.h"

class SliderDevice : public I2C_Device {
  public:
    typedef enum {
      DEVICE_STATUS   = 0x00,            // 0x00 = 1 Byte
      DEVICE_REGISTER = 0x60             // 0x60 to n = 1 to n Bytes
    } SliderDeviceHeader;
    SliderDevice(uint8_t devAddr = 0x08)
      : I2C_Device(devAddr) {
      DEBUG_PRINTLN(F("SliderDevice: SliderDevice: "));
    } // SliderDevice
    SliderDevice(I2C_Device *parent) {
      m_deviceAddress = parent->deviceAddress();
      m_deviceStatus  = parent->deviceStatus();
      m_manufactureID = parent->manufactureID();
      m_deviceType    = parent->deviceType();
      m_softwareID    = parent->softwareID();
      m_deviceID      = new uint8_t[11];
      parent->deviceID(m_deviceID);
    } // SliderDevice copy constructor
    virtual ~SliderDevice() {
      DEBUG_PRINTLN(F("SliderDevice: ~SliderDevice: "));
    } // ~SliderDevice()
};

#endif /* I2C_SLIDER_DEVICE_H_ */
