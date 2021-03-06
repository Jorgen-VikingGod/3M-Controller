/*
   i2c_devices.h

    Created on: Feb 08, 2017
        Author: Juergen Skrotzky (JorgenVikingGod@gmail.com)
*/

#ifndef I2C_DEVICES_H_
#define I2C_DEVICES_H_

#include "helper.h"
#include "i2c_device.h"
#include "i2c_button_device.h"
#include "i2c_slider_device.h"
#include "i2c_encoder_device.h"

#define MAX_DEVICES 20

class DeviceList {
  public:
    typedef enum {
      DL_INIT_UNKNOWN = -2,
      DL_INIT_ERROR   = -1,
      DL_INIT_ABORT   = 0,
      DL_INIT_SUCCESS = 1
    } DeviceListStates;
    DeviceList(uint8_t enablePin=0, uint8_t enableLoopPin=0) {
      m_enablePin     = enablePin;
      m_enableLoopPin = enableLoopPin;
      m_pCallback     = NULL;
      pinMode(m_enablePin, OUTPUT);
      pinMode(enableLoopPin, INPUT);
      m_deviceCount = 0;
      for (uint8_t idx = 0; idx < MAX_DEVICES; idx++) {
        m_deviceList[idx] = NULL;
      }
    } // DeviceList
    virtual ~DeviceList() {
      // clear list and delete all device objects
      for (uint8_t idx = 0; idx < m_deviceCount; ++idx) {
        if (m_deviceList[idx]) {
          delete m_deviceList[idx];
          m_deviceList[idx] = NULL;
        }
      }
      m_deviceCount = 0;
    } // ~DeviceList()
    DeviceListStates initialDevices() {
      // activate enable pin to start initial of devices in chain
      digitalWrite(m_enablePin, HIGH);
      // abort if no device in chain (loop back pin is HIGH)
      if (digitalRead(m_enableLoopPin) == HIGH) {
        // reset enable pin
        digitalWrite(m_enablePin, LOW);
        // abort and return error code
        return DL_INIT_ABORT;
      }
      uint8_t device_address = 0x10;
      bool init_device = true;
      // loop until no new device found or enable loop back pin is HIGH
      uint8_t loopback_enabled = LOW;
      int channelOffset = 0;
      while(init_device && loopback_enabled==LOW) {
        I2C_Device localDevice = I2C_Device(channelOffset);
        localDevice.fetchDeviceInfo();
        localDevice.deviceID();
        // change address and increment for next device
        if (localDevice.changeDeviceAddress(device_address)) {
          // add to list
          switch (localDevice.deviceType()) {
            default:
            case I2C_Device::DEVICE_UNKNOWN: {
              I2C_Device *pDevice = new I2C_Device(&localDevice);
              addDevice(pDevice);
              channelOffset += pDevice->channelCount();
              break;
            } case I2C_Device::DEVICE_BUTTONS: {
              ButtonDevice *pDevice = new ButtonDevice(&localDevice);
              addDevice(pDevice);
              channelOffset += pDevice->channelCount();
              break;
            } case I2C_Device::DEVICE_SLIDERS: {
              SliderDevice *pDevice = new SliderDevice(&localDevice);
              addDevice(pDevice);
              channelOffset += pDevice->channelCount();
              break;
            } case I2C_Device::DEVICE_ENCODERS: {
              EncoderDevice *pDevice = new EncoderDevice(&localDevice);
              addDevice(pDevice);
              channelOffset += pDevice->channelCount();
              break;
            }
          } // switch (localDevice.deviceType())
          // go to next address
          device_address++;
          // on success initial execute callback
          if (m_pCallback) {
            m_pCallback(device(m_deviceCount-1));
          }
          init_device = true;
          loopback_enabled = digitalRead(m_enableLoopPin);
        } else {
          // clear list and delete all device objects
          for (uint8_t idx = 0; idx < m_deviceCount; ++idx) {
            if (m_deviceList[idx]) {
              delete m_deviceList[idx];
              m_deviceList[idx] = NULL;
            }
          }
          m_deviceCount = 0;
          init_device = false;
        } // if (device->changeDeviceAddress(device_address)) else clear list
      } // while(init_device && loopback_enabled==LOW)
      // all devices are initialed if enable loop back pin is HIGH
      if (init_device && loopback_enabled==HIGH) {
        return DL_INIT_SUCCESS;
      }
      // error on device init failure
      if (!init_device) {
        return DL_INIT_ERROR;
      }
      // return unknown state if no other case was run
      return DL_INIT_UNKNOWN;
    } // initialDevices
    uint8_t deviceCount() {
      return m_deviceCount;
    } // deviceCount
    uint8_t deviceType(uint8_t idx) {
      uint8_t devType = I2C_Device::DEVICE_UNKNOWN;
      if (idx < m_deviceCount) {
        if (m_deviceList[idx]) {
          devType = m_deviceList[idx]->deviceType();
        }
      }
      return devType;
    } // deviceType
    I2C_Device* device(uint8_t idx) {
      return m_deviceList[idx];
    } // device
    ButtonDevice* buttonDevice(uint8_t idx) {
      return static_cast<ButtonDevice*>(m_deviceList[idx]);
    } // buttonDevice
    SliderDevice* sliderDevice(uint8_t idx) {
      return static_cast<SliderDevice*>(m_deviceList[idx]);
    } // sliderDevice
    EncoderDevice* encoderDevice(uint8_t idx) {
      return static_cast<EncoderDevice*>(m_deviceList[idx]);
    } // encoderDevice
    void setSuccessInitialCallback(void (*pCallbackFunction)(I2C_Device*)) {
      m_pCallback = pCallbackFunction;
    } // setSuccessInitialCallback
  private:
    int addDevice(I2C_Device *dev) {
      if (m_deviceCount+1 > MAX_DEVICES)
        return -1;
      m_deviceList[m_deviceCount] = dev;
      m_deviceCount++;
      return m_deviceCount-1;
    } // addDevice
    void (*m_pCallback)(I2C_Device*);
    uint8_t m_enablePin;
    uint8_t m_enableLoopPin;
    uint8_t m_deviceCount;
    I2C_Device *m_deviceList[MAX_DEVICES];
};

#endif /* I2C_DEVICES_H_ */
