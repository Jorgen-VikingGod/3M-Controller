/*
 * i2c_button_device.h
 *
 *  Created on: Jan 30, 2017
 *      Author: Juergen Skrotzky (JorgenVikingGod@gmail.com)
 */

#ifndef I2C_BUTTON_DEVICE_H_
#define I2C_BUTTON_DEVICE_H_

#include "helper.h"
#include "i2c_device.h"
#include <FastLED.h>

#define MAX_BUTTONS 21

class ButtonDevice;

class RGBButton {
  public:
    RGBButton(uint8_t btnID, ButtonDevice *pParent, void (*pPressCallbackFunction)(RGBButton*), void (*pReleaseCallbackFunction)(RGBButton*), int iClickDelay = 100, int iDoubleClickDelay = 20) {
      m_id                = btnID;
      m_parent            = pParent;
      m_pPressCallback    = pPressCallbackFunction;
      m_pReleaseCallback  = pReleaseCallbackFunction;
      m_clickDelay        = iClickDelay;
      m_doubleClickDelay  = iDoubleClickDelay;
      m_buttonPressed     = false;
      m_lastPress         = 0;
      m_lastRelease       = 0;
      m_rgb               = CRGB::Black;
      DEBUG_PRINT(F("RGBButton: RGBButton("));
      DEBUG_PRINT(btnID);
      DEBUG_PRINT(F(", pCallbackFunction, "));
      DEBUG_PRINT(iClickDelay);
      DEBUG_PRINT(F(", "));
      DEBUG_PRINT(iDoubleClickDelay);
      DEBUG_PRINTLN(F("): "));
    }
    ~RGBButton() {
      DEBUG_PRINTLN(F("RGBButton: ~RGBButton: "));
    }
    uint8_t id() {
      DEBUG_PRINT(F("RGBButton: id: "));
      DEBUG_PRINTLN(m_id);
      return m_id;
    }
    ButtonDevice* device() {
      return m_parent;
    }
    
    CRGB& color() {
      DEBUG_PRINT(F("RGBButton: color: ("));
      DEBUG_PRINT(m_rgb.r);
      DEBUG_PRINT(F(", "));
      DEBUG_PRINT(m_rgb.g);
      DEBUG_PRINT(F(", "));
      DEBUG_PRINT(m_rgb.b);
      DEBUG_PRINTLN(F("): "));
      return m_rgb;
    }
    void setColor(const CRGB &rgbCol) {
      DEBUG_PRINT(F("RGBButton: setColor("));
      DEBUG_PRINT(rgbCol.r);
      DEBUG_PRINT(F(", "));
      DEBUG_PRINT(rgbCol.g);
      DEBUG_PRINT(F(", "));
      DEBUG_PRINT(rgbCol.b);
      DEBUG_PRINTLN(F("): "));
      m_rgb = rgbCol;
    }
    /*
    uint32_t color() {
      uint8_t red   = (m_rgb >> 16) & 0xFF;
      uint8_t green = (m_rgb >>  8) & 0xFF;
      uint8_t blue  = (m_rgb >>  0) & 0xFF;
      DEBUG_PRINT(F("RGBButton: "));
      DEBUG_PRINT_HEX(red);
      DEBUG_PRINT_HEX(green);
      DEBUG_PRINT_HEX(blue);
      DEBUG_PRINTLN(F(" color(): "));
      return m_rgb;
    }
    uint8_t red() {
      uint8_t red = (m_rgb >> 16) & 0xFF;
      DEBUG_PRINT(F("RGBButton: "));
      DEBUG_PRINT_HEX(red);
      DEBUG_PRINTLN(F(" red(): "));
      return red;
    }
    uint8_t green() {
      uint8_t green = (m_rgb >>  8) & 0xFF;
      DEBUG_PRINT(F("RGBButton: "));
      DEBUG_PRINT_HEX(green);
      DEBUG_PRINTLN(F(" green(): "));
      return green;
    }
    uint8_t blue() {
      uint8_t blue  = (m_rgb >>  0) & 0xFF;
      DEBUG_PRINT(F("RGBButton: "));
      DEBUG_PRINT_HEX(blue);
      DEBUG_PRINTLN(F(" blue(): "));
      return blue;
    }
    void setColor(uint32_t colorcode) {
      uint8_t red   = (colorcode >> 16) & 0xFF;
      uint8_t green = (colorcode >>  8) & 0xFF;
      uint8_t blue  = (colorcode >>  0) & 0xFF;
      DEBUG_PRINT(F("RGBButton: setColor("));
      DEBUG_PRINT_HEX(red);
      DEBUG_PRINT_HEX(green);
      DEBUG_PRINT_HEX(blue);
      DEBUG_PRINTLN(F("): "));
      m_rgb = colorcode;
    }
    */
    bool isPressed() {
      DEBUG_PRINT(F("RGBButton: isPressed: "));
      DEBUG_PRINTLN(m_buttonPressed);
      return m_buttonPressed;
    }
    void press() {
      DEBUG_PRINTLN(F("RGBButton: press: "));
      
      unsigned long current = millis();
      /*if (m_buttonPressed) {

      } else {

      }*/
      
      if (m_pPressCallback) {
        m_pPressCallback(this);
      }
      
      m_lastPress = current;
      m_buttonPressed = true;
    }
    void release() {
      DEBUG_PRINTLN(F("RGBButton: press: "));
      unsigned long current = millis();
      /*if (m_buttonPressed) {
        if ((current - m_lastPress) > m_clickDelay) {

        } else if ((current - m_lastPress) > m_doubleClickDelay) {
          if (m_pCallback) {
            m_pCallback(this);
          }
        }
      } else {

      }
      */
      
      if (m_pReleaseCallback) {
        m_pReleaseCallback(this);
      }
      
      m_lastRelease = current;
      m_buttonPressed = false;
    }
    void setPressCallback(void (*pPressCallbackFunction)(RGBButton*)) {
      DEBUG_PRINTLN(F("RGBButton: setPressCallback: "));
      m_pPressCallback = pPressCallbackFunction;
    }
    void setReleaseCallback(void (*pReleaseCallbackFunction)(RGBButton*)) {
      DEBUG_PRINTLN(F("RGBButton: setReleaseCallback: "));
      m_pReleaseCallback = pReleaseCallbackFunction;
    }
  private:
    void (*m_pPressCallback)(RGBButton*);
    void (*m_pReleaseCallback)(RGBButton*);
  protected:
    uint8_t m_id;
    ButtonDevice *m_parent;
    unsigned long m_clickDelay;
    unsigned long m_doubleClickDelay;
    bool m_buttonPressed;
    unsigned long m_lastPress;
    unsigned long m_lastRelease;
    CRGB m_rgb;
    //uint32_t m_rgb;
};

class ButtonDevice : public I2C_Device {
  public:
    typedef enum {
      DEVICE_STATUS   = 0x00,             // 0x00 = 1 Byte
      BTN_STATES      = 0x01,             // 0x01 to 0x09 = 1 to 9 Bytes (all buttons states)
      BTN_PR_07_00    = BTN_STATES,       // 0x01 = 1 Byte (Button pressed [07 - 00])
      BTN_PR_15_08,                       // 0x02 = 1 Byte (Button pressed [15 - 08])
      BTN_PR_23_16,                       // 0x03 = 1 Byte (Button pressed [23 - 16])
      BTN_RE_07_00,                       // 0x04 = 1 Byte (Button release [07 - 00])
      BTN_RE_15_08,                       // 0x05 = 1 Byte (Button release [15 - 08])
      BTN_RE_23_16,                       // 0x06 = 1 Byte (Button release [23 - 16])
      NUMBER_OF_BTNS  = 0x43,             // 0x43 = 1 Byte (number of buttons: default = 24)
      BTN_MUX_CONTROL,                    // 0x44 = 1 Byte (enable mux)
      BTN_IRQ_MASK_PR_07_00,              // 0x45 = 1 Byte (Button irq pressed mask [07 - 00])
      BTN_IRQ_MASK_PR_15_08,              // 0x46 = 1 Byte (Button irq pressed mask [15 - 08])
      BTN_IRQ_MASK_PR_23_16,              // 0x47 = 1 Byte (Button irq pressed mask [23 - 16])
      BTN_IRQ_MASK_RE_07_00,              // 0x48 = 1 Byte (Button irq released mask [07 - 00])
      BTN_IRQ_MASK_RE_15_08,              // 0x49 = 1 Byte (Button irq released mask [15 - 08])
      BTN_IRQ_MASK_RE_23_16,              // 0x50 = 1 Byte (Button irq released mask [23 - 16])
      DEVICE_REGISTER = 0x60,             // 0x60 to n = 1 to n Bytes
      RGB_COLOR_START = DEVICE_REGISTER   // 0x60 to 0x77 = 24 Bytes of RGB values
    } ButtonDeviceHeader;
    ButtonDevice(I2C_Device *parent) {
      m_deviceAddress = parent->deviceAddress();
      m_deviceStatus  = parent->deviceStatus();
      m_manufactureID = parent->manufactureID();
      m_deviceType    = parent->deviceType();
      m_softwareID    = parent->softwareID();
      m_deviceID      = new uint8_t[11];
      parent->deviceID(m_deviceID);
      m_btnCount      = 0;
      m_channelCount  = MAX_BUTTONS * 3;
    } // ButtonDevice copy constructor
    ButtonDevice(uint8_t devAddr = 0x08)
      : I2C_Device(devAddr), m_btnCount(0) {
      DEBUG_PRINTLN(F("ButtonDevice: ButtonDevice: "));
      for (uint8_t idx = 0; idx < MAX_BUTTONS; idx++) {
        m_buttonList[idx] = NULL;
      }
    } // ButtonDevice
    virtual ~ButtonDevice() {
      DEBUG_PRINTLN(F("ButtonDevice: ~ButtonDevice: "));
    } // ~ButtonDevice
    virtual bool initial() {
      DEBUG_PRINTLN(F("ButtonDevice: initial: "));
      writeDataToOffset(BTN_MUX_CONTROL, 0x01);
      m_btnCount = 0;
      return true;
    } // initial
    void readButtonStates() {
      DEBUG_PRINTLN(F("ButtonDevice: readButtonStates: "));
    } // readButtonStates
    uint8_t buttonCount() {
      DEBUG_PRINT(F("ButtonDevice: buttonCount: "));
      DEBUG_PRINTLN(m_btnCount);
      return m_btnCount;
    } // buttonCount
    void setButtonCount(uint8_t btnCount = MAX_BUTTONS) {
      DEBUG_PRINT(F("ButtonDevice: setButtonCount("));
      DEBUG_PRINT(btnCount);
      DEBUG_PRINTLN(F("): "));
      if (btnCount < MAX_BUTTONS+1) {
        writeDataToOffset(NUMBER_OF_BTNS, 20);//btnCount);
        //writeDataToOffset(NUMBER_OF_BTNS, 24);
        writeDataToOffset(BTN_IRQ_MASK_PR_07_00, 0xFF);
        writeDataToOffset(BTN_IRQ_MASK_PR_15_08, 0xFF);
        writeDataToOffset(BTN_IRQ_MASK_PR_23_16, 0xFF);
        writeDataToOffset(BTN_IRQ_MASK_RE_07_00, 0xFF);
        writeDataToOffset(BTN_IRQ_MASK_RE_15_08, 0xFF);
        writeDataToOffset(BTN_IRQ_MASK_RE_23_16, 0xFF);
        
        /*DEBUG_PRINTLN(m_btnCount);
        for (uint8_t idx = 0; idx < m_btnCount; ++idx) {
          if (m_buttonList[idx]) {
            DEBUG_PRINTLN(FreeRam());
            delete m_buttonList[idx];
            m_buttonList[idx] = NULL;
            DEBUG_PRINTLN(FreeRam());
          }
        }
        */
        for (uint8_t idx = 0; idx < btnCount; ++idx) {
          DEBUG_PRINTLN(FreeRam());
          m_buttonList[idx] = new RGBButton(idx, this, m_pPressCallback, m_pReleaseCallback);
          DEBUG_PRINTLN(FreeRam());
        }
        m_btnCount = btnCount;
      } // if (btnCount < MAX_BUTTONS+1)
    } // setButtonCount
    int addButton(RGBButton *btn) {
      DEBUG_PRINTLN(F("ButtonDevice: addButton: "));
      if (m_btnCount+1 > MAX_BUTTONS)
        return -1;
      m_buttonList[m_btnCount] = btn;
      m_btnCount++;
      return m_btnCount-1;
    } // addButton
    RGBButton* button(uint8_t idx) {
      DEBUG_PRINT(F("ButtonDevice: button("));
      DEBUG_PRINT(idx);
      DEBUG_PRINTLN(F("): "));
      if (idx > MAX_BUTTONS-1 || m_btnCount == 0)
        return NULL;
      return m_buttonList[idx];
    } // button
    void setGlobalButtonPressCallback(void (*pPressCallbackFunction)(RGBButton*)) {
      DEBUG_PRINTLN(F("ButtonDevice: setGlobalButtonPressCallback: "));
      m_pPressCallback = pPressCallbackFunction;
      for (uint8_t idx = 0; idx < m_btnCount; ++idx) {
        if (m_buttonList[idx]) {
          m_buttonList[idx]->setPressCallback(m_pPressCallback);
        }
      }
    } // setGlobalButtonPressCallback
    void setGlobalButtonReleaseCallback(void (*pReleaseCallbackFunction)(RGBButton*)) {
      DEBUG_PRINTLN(F("ButtonDevice: setGlobalButtonReleaseCallback: "));
      m_pReleaseCallback = pReleaseCallbackFunction;
      for (uint8_t idx = 0; idx < m_btnCount; ++idx) {
        if (m_buttonList[idx]) {
          m_buttonList[idx]->setReleaseCallback(m_pReleaseCallback);
        }
      }
    } // setGlobalButtonReleaseCallback
    void fetchButtonStates() {
      DEBUG_PRINTLN(F("ButtonDevice: fetchButtonStates: "));
      uint8_t temp[6];
      readDataByOffset(BTN_STATES, temp, 6);
      for (uint8_t btnIdx = 0; btnIdx < 3; btnIdx++) {
        for (uint8_t idx = 0; idx < 8; ++idx) {
          uint8_t idxOffset = (btnIdx * 8) + idx;
          uint8_t btnPressed = temp[btnIdx];
          uint8_t btnRelease = temp[btnIdx+3];
          if (checkBit(btnPressed, idx)) {
            if (idxOffset < m_btnCount && m_buttonList[idxOffset]) {
              m_buttonList[idxOffset]->press();
            }
          }
          if (checkBit(btnRelease, idx)) {
            if (idxOffset < m_btnCount && m_buttonList[idxOffset]) {
              m_buttonList[idxOffset]->release();
            }
          }
        } // for (uint8_t idx = 0; idx < 8; ++idx) {
      } // for (uint8_t btnIdx = 0; btnIdx < 9; btnIdx += 3) {
    } // fetchButtonStates
    int colorStreamLength() {
      uint8_t bufLength = m_btnCount * 3;
      DEBUG_PRINT(F("ButtonDevice: "));
      DEBUG_PRINT(bufLength);
      DEBUG_PRINTLN(F(" = colorStreamLength()"));
      return bufLength;
    } // colorStreamLength
    int colorStream(uint8_t *buf) {
      uint8_t bufLength = m_btnCount * 3;
      DEBUG_PRINT(F("ButtonDevice: "));
      DEBUG_PRINT(bufLength);
      DEBUG_PRINT(F(" = colorStream("));
      for (uint8_t idx = 0; idx < m_btnCount; ++idx) {
        if (m_buttonList[idx]) {
          CRGB col = m_buttonList[idx]->color();  
          uint8_t idxR = 3*idx+0;
          uint8_t idxG = 3*idx+1;
          uint8_t idxB = 3*idx+2;
          buf[idxR] = col.r;
          DEBUG_PRINT_HEX(buf[idxR]);
          DEBUG_PRINT(F(" "));
          buf[idxG] = col.g;
          DEBUG_PRINT_HEX(buf[idxG]);
          DEBUG_PRINT(F(" "));
          buf[idxB] = col.b;
          DEBUG_PRINT_HEX(buf[idxB]);  
        }
        DEBUG_PRINT(F(" "));
      } // for (uint8_t idx = 0; idx < m_btnCount; ++idx)
      DEBUG_PRINTLN(F(")"));
      return bufLength;
    } // colorStream
    void sendColorStream(uint8_t *buf, int length) {
      DEBUG_PRINTLN(F("ButtonDevice: sendColorStream:"));
      writeDataToOffset(RGB_COLOR_START, buf, length);
    }
    void sendCurrentColorStream() {
      DEBUG_PRINTLN(F("ButtonDevice: sendCurrentColorStream:"));
      int bufLength = colorStreamLength();
      uint8_t buf[bufLength];
      colorStream(buf);
      sendColorStream(buf, bufLength);
    }
  private:
    void (*m_pPressCallback)(RGBButton*);
    void (*m_pReleaseCallback)(RGBButton*);
    uint8_t m_btnCount;
    RGBButton *m_buttonList[MAX_BUTTONS];
};

#endif /* I2C_BUTTON_DEVICE_H_ */
