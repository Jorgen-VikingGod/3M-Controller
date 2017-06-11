/*
 * i2c_encoder_device.h
 *
 *  Created on: Feb 13, 2017
 *      Author: Juergen Skrotzky (JorgenVikingGod@gmail.com)
 */

#ifndef I2C_ENCODER_DEVICE_H_
#define I2C_ENCODER_DEVICE_H_

#include "helper.h"
#include "i2c_device.h"

class EncoderDevice : public I2C_Device {
  public:
    typedef enum {
      DEVICE_STATUS   = 0x00,            // 0x00 = 1 Byte
      DEVICE_REGISTER = 0x60             // 0x60 to n = 1 to n Bytes
    } EncoderDeviceHeader;
    EncoderDevice(uint8_t devAddr = 0x08)
      : I2C_Device(devAddr) {
    } // EncoderDevice
    EncoderDevice(I2C_Device *parent) {
      m_deviceAddress = parent->deviceAddress();
      m_deviceStatus  = parent->deviceStatus();
      m_manufactureID = parent->manufactureID();
      m_deviceType    = parent->deviceType();
      m_softwareID    = parent->softwareID();
      m_deviceID      = new uint8_t[11];
      parent->deviceID(m_deviceID);
    } // EncoderDevice copy constructor
    virtual ~EncoderDevice() {
    } // ~EncoderDevice()
};

#endif /* I2C_ENCODER_DEVICE_H_ */
