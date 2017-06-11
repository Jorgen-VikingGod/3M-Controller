/*
 * i2c_device.h
 *
 *  Created on: Jan 27, 2017
 *      Author: Juergen Skrotzky (JorgenVikingGod@gmail.com)
 */

#ifndef I2C_DEVICE_H_
#define I2C_DEVICE_H_

// Include I2C lib
#if defined(__MK20DX256__)
#include <i2c_t3.h>
#else
#include <Wire.h>
#endif
#include "helper.h"

class I2C_Device {
public:
  typedef enum {
    DEVICE_UNKNOWN  = 0x00,
    DEVICE_BUTTONS  = 0x01,
    DEVICE_SLIDERS  = 0x02,
    DEVICE_ENCODERS = 0x03
  } DeviceTypes;

  typedef enum {
    DEVICE_STATUS   = 0x00,             // 0x00 = 1 Byte
    DEVICE_INFO     = 0x30,             // 0x30 = 1 to 15 Bytes
    MANUFACTURE_ID   = DEVICE_INFO,     // 0x30 = 1 Byte
    DEVICE_TYPE,                        // 0x31 = 1 Byte
    SW_ID,                              // 0x32 = 1 Byte
    DEVICE_ID,                          // 0x33 to 0x3E = 1 to 11 Bytes
    DEVICE_CONFIG   = 0x40,             // 0x40 to 0x5F = 1 to 31 Bytes
    SLAVE_ADDRESS   = DEVICE_CONFIG,    // 0x40 = 1 Byte
    IRQ_CONFIG,                         // 0x41 = 1 Byte
    RUN_NEXT_CHIP,                      // 0x42 = 1 Byte
    DEVICE_REGISTER = 0x60              // 0x60 to n = 1 to n Bytes
  } DeviceHeader;
  I2C_Device(I2C_Device *parent) {
    m_channelOffset = parent->channelOffset();
    m_channelCount  = parent->channelCount();
    m_deviceAddress = parent->deviceAddress();
    m_deviceStatus  = parent->deviceStatus();
    m_manufactureID = parent->manufactureID();
    m_deviceType    = parent->deviceType();
    m_softwareID    = parent->softwareID();
    m_deviceID      = new uint8_t[11];
    parent->deviceID(m_deviceID);
  } // I2C_Device copy constructor
  I2C_Device(int chOffset = 0, uint8_t devAddr = 0x08) {
    m_channelOffset = chOffset;
    m_channelCount  = 0;
    m_deviceAddress = devAddr;  // device address
    m_deviceStatus  = 0x00;
    m_manufactureID = 0x00;
    m_deviceType    = 0x00;
    m_softwareID    = 0x00;
    m_deviceID      = new uint8_t[11];
    for (uint8_t i=0;i<11;++i) {m_deviceID[i]= 0x00;}
  } // I2C_Device
  virtual ~I2C_Device() {
    delete[] m_deviceID;
  } // ~I2C_Device()
  /**
   * request data from slave by address at register address and read back the data.
   * deviceAddress: slave address
   * addr:          slave register address
   * buf:           return data buffer
   * length:        data length to read back
   */
  static uint8_t readDataByAddressAndOffset(uint8_t deviceAddress, uint8_t addr, uint8_t* buf, uint8_t length) {
    Wire.beginTransmission(deviceAddress);
    Wire.write(addr);
    Wire.endTransmission(false);  // Don't issue a stop because we need to read the value.
    uint8_t n = Wire.requestFrom(deviceAddress, static_cast<size_t>(length));
    for (uint8_t i = 0; i<n; i++) {
      if (Wire.available()) {
        buf[i] = static_cast<uint8_t>(Wire.read());
      }
    }
    return n;
  } // readDataByAddressAndOffset
  /**
   * wite data to slave at register address.
   * deviceAddress: slave address
   * addr:          slave register address
   * buf:           data buffer to write
   * length:        data length
   */
  static uint8_t writeDataToAddressAndOffset(uint8_t deviceAddress, uint8_t addr, const uint8_t* buf, uint8_t length) {
    Wire.beginTransmission(deviceAddress);
    Wire.write(addr);
    //Wire.write(buf, length);
    for (uint8_t i = 0; i < length; i++) {
      Wire.write(buf[i]);
    }
    Wire.endTransmission();
    return length;
  } // writeDataToAddressAndOffset
  /**
   * wite data to slave at register address.
   * deviceAddress: slave address
   * addr:          slave register address
   * buf:           data buffer to write
   * length:        data length
   */
  static uint8_t writeDataToAddressAndOffset(uint8_t deviceAddress, uint8_t addr, const uint8_t data) {
    Wire.beginTransmission(deviceAddress);
    Wire.write(addr);
    Wire.write(data);
    Wire.endTransmission();
    return 1;
  } // writeDataToAddressAndOffset
  /**
   * request data from slave at register address and read back the data.
   * addr:   slave register address
   * buf:    return data buffer
   * length: data length to read back
   */
  uint8_t readDataByOffset(uint8_t addr, uint8_t* buf, uint8_t length) {
    return readDataByAddressAndOffset(m_deviceAddress, addr, buf, length);
  } // readDataByOffset
  /**
   * wite data to slave at register address.
   * addr:   slave register address
   * buf:    data buffer to write
   * length: data length
   */
  uint8_t writeDataToOffset(uint8_t addr, uint8_t* buf, uint8_t length) {
    return writeDataToAddressAndOffset(m_deviceAddress, addr, buf, length);
  } // writeDataToOffset
  /**
   * wite data to slave at register address.
   * addr:   slave register address
   * data:   1 Byte of data to write
   */
  uint8_t writeDataToOffset(uint8_t addr, uint8_t data) {
    return writeDataToAddressAndOffset(m_deviceAddress, addr, data);
  } // writeDataToOffset
  /**
   * return bool: true on success, false on error
   */
  virtual bool initial() {
    return true;
  } // initial
  /**
   * return device address
   */
  uint8_t deviceAddress() {
    return m_deviceAddress;
  } // deviceAddress
  /**
   * change device address
   */
  bool changeDeviceAddress(uint8_t devAddr = 0x08) {
    /*
    1. chnage slave address in slave register
    2. read back deviceID from slave by new address
    3. compare deviceID with stored one
    4. change stored slave address on success - otherwise return false
    */
    writeDataToOffset(SLAVE_ADDRESS, (devAddr<<1));
    uint8_t temp[11];
    int iDataLength = I2C_Device::readDataByAddressAndOffset(devAddr, DEVICE_ID, temp, 11);
    if (iDataLength < 11) {
      return false;
    } else {
      if (compareBuffer(m_deviceID, temp, 11)) {
        // address change success > set RUN_NEXT_CHIP
        m_deviceAddress = devAddr;
        writeDataToOffset(RUN_NEXT_CHIP, 0x01);
        return true;
      } else {
        return false;
      }
    }
  } // changeDeviceAddress
  /**
   * get complete device info and store in members
   */
  int fetchDeviceInfo() {
    uint8_t temp[15];
    readDataByOffset(DEVICE_INFO, temp, 14);
    m_manufactureID = temp[0];
    m_deviceType = temp[1];
    m_softwareID = temp[2];
    copyBuffer(m_deviceID, temp, 11, 0, 3);
    /*
    DEBUG_PRINT(F("I2C_Device: fetchDeviceInfo > manufactureID: "));
    DEBUG_PRINTLN_HEX(m_manufactureID);
    DEBUG_PRINT(F("I2C_Device: fetchDeviceInfo > softwareID: "));
    DEBUG_PRINTLN_HEX(m_softwareID);
    DEBUG_PRINT(F("I2C_Device: fetchDeviceInfo > deviceType: "));
    DEBUG_PRINTLN_HEX(m_deviceType);
    DEBUG_PRINT(F("I2C_Device: fetchDeviceInfo > deviceID: "));
    DEBUG_PRINTLN(byteToHexString(m_deviceID, 11, " "));
    */
    return 1;
  } // fetchDeviceInfo
  /**
   * get current device status
   */
  uint8_t fetchDeviceStatus() {
    uint8_t temp[1];
    readDataByOffset(DEVICE_STATUS, temp, 1);
    m_deviceStatus = temp[0];
    return m_deviceStatus;
  } // fetchDeviceStatus
  /**
   * get stored device status
   */
  uint8_t deviceStatus() {
    return m_deviceStatus;
  } // deviceStatus
  /**
   * get device type
   */
  uint8_t deviceType() {
    if (m_deviceType == 0x00) {
      uint8_t temp[1];
      readDataByOffset(DEVICE_TYPE, temp, 1);
      m_deviceType = temp[0];
    }
    return m_deviceType;
  } // deviceType
  /**
   * get manufacture id
   */
  uint8_t manufactureID() {
    if (m_manufactureID == 0x00) {
      uint8_t temp[1];
      readDataByOffset(MANUFACTURE_ID, temp, 1);
      m_manufactureID = temp[0];
    }
    return m_manufactureID;
  } // manufactureID
  /**
   * get software id
   */
  uint8_t softwareID() {
    if (m_softwareID == 0x00) {
      uint8_t temp[1];
      readDataByOffset(SW_ID, temp, 1);
      m_softwareID = temp[0];
    }
    return m_softwareID;
  } // softwareID

  /**
   * get device ID as buffer
   * buf: return data buffer from read device ID
   * return: buffer length
   */
  uint8_t deviceID(uint8_t* buf) {
    uint8_t emptyID[11] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    if (m_deviceID && compareBuffer(m_deviceID, emptyID, 11)) {
      readDataByOffset(DEVICE_ID, buf, 11);
      copyBuffer(m_deviceID, buf, 11, 0, 0);
    } else {
      copyBuffer(buf, m_deviceID, 11, 0, 0);
    }
    return sizeof(buf) / sizeof(uint8_t);
  } // deviceID
  /**
   * get device ID as String
   * return: device ID as String
   */
  String deviceID() {
    uint8_t temp[11];
    deviceID(temp);
    return byteToHexString(temp, 11, "-");
  } // deviceID
  int channelOffset() {
    return m_channelOffset;
  } // channelOffset
  int channelCount() {
    return m_channelCount;
  } // channelOffset
protected:
  int     m_channelOffset;
  uint8_t m_channelCount;
  uint8_t m_deviceAddress;
  uint8_t m_deviceStatus;
  uint8_t m_manufactureID;
  uint8_t m_softwareID;
  uint8_t m_deviceType;
  uint8_t* m_deviceID;
};

#endif /* I2C_DEVICE_H_ */
