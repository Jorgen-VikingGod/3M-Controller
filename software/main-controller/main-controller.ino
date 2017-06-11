#include "config.h"
#include "helper.h"

<<<<<<< HEAD
/*
#include <displaylib.h>      // https://github.com/olikraus/display_Arduino
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#if defined(__MK20DX256__)
#include <i2c_t3.h>
#elif defined(U8X8_HAVE_HW_I2C)
#include <Wire.h>
#endif
*/
=======
#include <WiFi.h>
#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();

#if _display
>>>>>>> 9a84e43... esp32 version only
#include <SPI.h>
#include <Ucglib.h>

#include "i2c_devices.h"
#include "i2c_button_device.h"
#include "i2c_slider_device.h"
#include "i2c_encoder_device.h"

typedef enum {
  MLS_UNKNOWN = 0,
  MLS_INITIAL_DEVICES,
  MLS_PROCESS
} MainLoopState;
MainLoopState eState = MLS_UNKNOWN;

volatile boolean interruptEnabled;

// initial device list class
DeviceList devices(ENABLE_PIN, ENABLE_LOOP_PIN);

<<<<<<< HEAD
// initial display
//display_SH1106_128X64_NONAME_F_4W_HW_SPI display(display_R0, OLED_CS_PIN, OLED_DC_PIN, OLED_RST_PIN);
//Ucglib_SSD1331_18x96x64_UNIVISION_HWSPI display(OLED_DC_PIN, OLED_CS_PIN, OLED_RST_PIN);
Ucglib_ILI9163_18x128x128_HWSPI display(OLED_DC_PIN, OLED_CS_PIN, OLED_RST_PIN);
=======
#if _display
OLED display(OLED_CS_PIN, OLED_DC_PIN, OLED_RST_PIN);
#endif
>>>>>>> 9a84e43... esp32 version only


CRGB rgbMap[6] = {0xFF0000, 0xFFFF00, 0x00FF00, 0x00FFFF, 0x0000FF, 0xFF00FF};
uint8_t hue0 = 0;
uint8_t hue1 = 42;
uint8_t hue2 = 85;
uint8_t hue3 = 128;
uint8_t hue4 = 171;
uint8_t hue5 = 213;
uint8_t hueDelta = 1;

bool bToggle = false;
bool bLastState = false;
bool bLastState2 = false;
uint8_t speedIdx = 3;
uint8_t speedList[10] = {0, 1, 2, 5, 10, 25, 50, 100, 175, 255};
uint8_t notes[6] = {60, 62, 64, 65, 67, 69};
static uint8_t thisdelay = 25;

unsigned long previous_millis = 0;

const int ledPin = 13;
static RgbColor colTemp = RgbColor(0);


void onSuccessInitialCallback(I2C_Device* device) {
  DEBUG_PRINT(F("Main: onSuccessInitialCallback - device initialed: "));
  DEBUG_PRINTLN(device->deviceID());
}

void onButtonPressCallback(RGBButton* btn) {
  Serial.print(F("Main: onButtonPressCallback ("));
  uint8_t buttonID = btn->id();
  ButtonDevice *buttonDevice = btn->device();
  int channelOffset = buttonDevice->channelOffset();
  Serial.print(channelOffset+buttonID);
  Serial.print(F(", "));
  Serial.print(buttonDevice->deviceID());
  Serial.println(F("): pressed"));
  btn->setColor(rgbMap[buttonID]); 
<<<<<<< HEAD
  usbMIDI.sendNoteOn(notes[buttonID], 99, 1);
  buttonDevice->sendCurrentColorStream();
}

void onButtonReleaseCallback(RGBButton* btn) {
  Serial.print(F("Main: onButtonReleaseCallback ("));
  uint8_t buttonID = btn->id();
  ButtonDevice *buttonDevice = btn->device();
  int channelOffset = buttonDevice->channelOffset();
  Serial.print(channelOffset+buttonID);
  Serial.print(F(", "));
  Serial.print(buttonDevice->deviceID());
  Serial.println(F("): released"));
  btn->setColor(CRGB::Black);
  usbMIDI.sendNoteOff(notes[buttonID], 0, 1);
  buttonDevice->sendCurrentColorStream();
=======
  buttonDevice->sendCurrentColorStream();
  MIDI.sendNoteOn(notes[buttonID], 99, 1);

#if _display
  OLED_TextBox box(display, 0, 0, 128, 127);
  box.reset(); // reset the text box contents, without clearing screen (allows flicker-free redraw)
  box.setForegroundColour(DARKBLUE);
  box.print("Control Change");
  box.setForegroundColour(WHITE);
  box.print("buttonID: ");
  box.print(buttonID);
  box.print("channelOffset: ");
  box.print(channelOffset);
  box.print("deviceID: ");
  box.print(buttonDevice->deviceID());
  box.print("\n");
#endif
}

void onButtonReleaseCallback(RGBButton* btn) {
  DEBUG_PRINT(F("Main: onButtonReleaseCallback ("));
  uint8_t buttonID = btn->id();
  ButtonDevice *buttonDevice = btn->device();
  int channelOffset = buttonDevice->channelOffset();
  DEBUG_PRINT(channelOffset+buttonID);
  DEBUG_PRINT(F(", "));
  DEBUG_PRINT(buttonDevice->deviceID());
  DEBUG_PRINTLN(F("): released"));
  btn->setColor(RgbColor(0));
  buttonDevice->sendCurrentColorStream();
  MIDI.sendNoteOff(notes[buttonID], 0, 1);
  if (buttonID == 4) {
    speedIdx++;
    if (speedIdx>9)
      speedIdx = 0;
    thisdelay = speedList[speedIdx];
  }
  if (buttonID == 5) {
    bToggle = !bToggle;
  }
>>>>>>> 9a84e43... esp32 version only
}

void onEnableLoopPinChanged() {
  DEBUG_PRINTLN(F("Main: onEnableLoopPinChanged: "));
}

<<<<<<< HEAD
void handle_interrupts() {
  while(1) {
    if (digitalRead(INTERRUPT_PIN) == LOW) {
      Serial.println(F("Main: handle_interrupts: "));
      ButtonDevice *buttonDevice = devices.buttonDevice(0);
      if (buttonDevice) {
        uint8_t devState = buttonDevice->fetchDeviceStatus();
        Serial.print(F("fetchDeviceStatus: "));
        Serial.println(devState, HEX);
        if (devState != 0x00) {
          buttonDevice->fetchButtonStates();
        } // if (devState != 0x00) {
      } // if (buttonDevice) {
    } // if (digitalRead(INTERRUPT_PIN) == LOW) {
  } // while(1) {
} // handle_interrupts

void onInterruptPinRising() {
  interruptEnabled = true;
  /*
  noInterrupts();
  Serial.println(F("Main: onInterruptPinRising: "));
  interruptEnabled = true;

  ButtonDevice *buttonDevice = devices.buttonDevice(0);
  if (buttonDevice) {
    uint8_t devState = buttonDevice->fetchDeviceStatus();
    Serial.print(F("fetchDeviceStatus: "));
    Serial.println(devState, HEX);
    if (devState != 0x00) {
      buttonDevice->fetchButtonStates();
    }
  } // if (buttonDevice) {
  
  interrupts();
  */
}

const int ledPin = 13;

static CRGB colTemp = CRGB::Black;

void OnNoteOn(byte channel, byte note, byte velocity) {
  Serial.print("Note On, ch=");
  Serial.print(channel, DEC);
  Serial.print(", note=");
  Serial.print(note, DEC);
  Serial.print(", velocity=");
  Serial.print(velocity, DEC);
  Serial.println();

  display.setColor(0, 80, 40, 0);
  display.setColor(1, 60, 0, 40);
  display.setColor(2, 20, 0, 20);
  display.setColor(3, 60, 0, 0);  
  display.drawGradientBox(0, 0, display.getWidth(), display.getHeight());
  
  display.setFont(ucg_font_helvB10_hr);
  display.setColor(0, 255, 0);
  display.setPrintPos(5, 15);
  display.print("Note On");
  
  display.setFont(ucg_font_helvB08_hr);
  display.setColor(255, 255, 255);
  display.setPrintPos(5, 35);
  display.print("channel: ");
  display.print(channel);
  display.setPrintPos(5, 45);
  display.print("note: ");
  display.print(note);
  display.setPrintPos(5, 55);
  display.print("velocity: ");
  display.print(velocity);
=======
void onInterruptPinLow() {
  interruptEnabled = true;
  gpio_set_intr_type((gpio_num_t)INTERRUPT_PIN, GPIO_INTR_DISABLE);
}

void OnNoteOn(byte channel, byte note, byte velocity) {
  DEBUG_PRINT("Note On, ch=");
  DEBUG_PRINT(channel, DEC);
  DEBUG_PRINT(", note=");
  DEBUG_PRINT(note, DEC);
  DEBUG_PRINT(", velocity=");
  DEBUG_PRINT(velocity, DEC);
  DEBUG_PRINTLN("");
>>>>>>> 9a84e43... esp32 version only
  
  if (note == 60) {
    colTemp.red = 255; 
  } else if (note == 62) {
    colTemp.green = 255; 
  } else if (note == 64) {
    colTemp.blue = 255; 
  }

  ButtonDevice *buttonDevice = devices.buttonDevice(0);
  if (buttonDevice) {
    RGBButton *btn0 = buttonDevice->button(0);
    if (btn0) btn0->setColor(colTemp);
    buttonDevice->sendCurrentColorStream();
  } // if (buttonDevice) {
}

void OnNoteOff(byte channel, byte note, byte velocity) {
<<<<<<< HEAD
  Serial.print("Note Off, ch=");
  Serial.print(channel, DEC);
  Serial.print(", note=");
  Serial.print(note, DEC);
  Serial.print(", velocity=");
  Serial.print(velocity, DEC);
  Serial.println();

  display.setColor(0, 80, 40, 0);
  display.setColor(1, 60, 0, 40);
  display.setColor(2, 20, 0, 20);
  display.setColor(3, 60, 0, 0);  
  display.drawGradientBox(0, 0, display.getWidth(), display.getHeight());
  
  display.setFont(ucg_font_helvB10_hr);
  display.setColor(255, 0, 0);
  display.setPrintPos(5, 15);
  display.print("Note Off");
  
  display.setFont(ucg_font_helvB08_hr);
  display.setColor(255, 255, 255);
  display.setPrintPos(5, 35);
  display.print("channel: ");
  display.print(channel);
  display.setPrintPos(5, 45);
  display.print("note: ");
  display.print(note);
  display.setPrintPos(5, 55);
  display.print("velocity: ");
  display.print(velocity);
  
=======
  DEBUG_PRINT("Note Off, ch=");
  DEBUG_PRINT(channel, DEC);
  DEBUG_PRINT(", note=");
  DEBUG_PRINT(note, DEC);
  DEBUG_PRINT(", velocity=");
  DEBUG_PRINT(velocity, DEC);
  DEBUG_PRINTLN("");

>>>>>>> 9a84e43... esp32 version only
  if (note == 60) {
    colTemp.red = 0; 
  } else if (note == 62) {
    colTemp.green = 0; 
  } else if (note == 64) {
    colTemp.blue = 0; 
  }

  ButtonDevice *buttonDevice = devices.buttonDevice(0);
  if (buttonDevice) {
    RGBButton *btn0 = buttonDevice->button(0);
    if (btn0) btn0->setColor(colTemp);
    buttonDevice->sendCurrentColorStream();
  } // if (buttonDevice) {
}

void OnVelocityChange(byte channel, byte note, byte velocity) {
<<<<<<< HEAD
  Serial.print("Velocity Change, ch=");
  Serial.print(channel, DEC);
  Serial.print(", note=");
  Serial.print(note, DEC);
  Serial.print(", velocity=");
  Serial.print(velocity, DEC);
  Serial.println();

  display.setColor(0, 80, 40, 0);
  display.setColor(1, 60, 0, 40);
  display.setColor(2, 20, 0, 20);
  display.setColor(3, 60, 0, 0);  
  display.drawGradientBox(0, 0, display.getWidth(), display.getHeight());
  
  display.setFont(ucg_font_helvB10_hr);
  display.setColor(255, 0, 0);
  display.setPrintPos(5, 15);
  display.print("Velocity Change");
  
  display.setFont(ucg_font_helvB08_hr);
  display.setColor(255, 255, 255);
  display.setPrintPos(5, 35);
  display.print("channel: ");
  display.print(channel);
  display.setPrintPos(5, 45);
  display.print("note: ");
  display.print(note);
  display.setPrintPos(5, 55);
  display.print("velocity: ");
  display.print(velocity);
}

void OnControlChange(byte channel, byte control, byte value) {
  Serial.print("Control Change, ch=");
  Serial.print(channel, DEC);
  Serial.print(", control=");
  Serial.print(control, DEC);
  Serial.print(", value=");
  Serial.print(value, DEC);
  Serial.println();

  display.setColor(0, 80, 40, 0);
  display.setColor(1, 60, 0, 40);
  display.setColor(2, 20, 0, 20);
  display.setColor(3, 60, 0, 0);  
  display.drawGradientBox(0, 0, display.getWidth(), display.getHeight());
  
  display.setFont(ucg_font_helvB10_hr);
  display.setColor(50, 200, 255);
  display.setPrintPos(5, 15);
  display.print("Control Change");
  
  display.setFont(ucg_font_helvB08_hr);
  display.setColor(255, 255, 255);
  display.setPrintPos(5, 35);
  display.print("channel: ");
  display.print(channel);
  display.setPrintPos(5, 45);
  display.print("control: ");
  display.print(control);
  display.setPrintPos(5, 55);
  display.print("value: ");
  display.print(value);
  
=======
  DEBUG_PRINT("Velocity Change, ch=");
  DEBUG_PRINT(channel, DEC);
  DEBUG_PRINT(", note=");
  DEBUG_PRINT(note, DEC);
  DEBUG_PRINT(", velocity=");
  DEBUG_PRINT(velocity, DEC);
  DEBUG_PRINTLN("");
}

void OnControlChange(byte channel, byte control, byte value) {
  DEBUG_PRINT("Control Change, ch=");
  DEBUG_PRINT(channel, DEC);
  DEBUG_PRINT(", control=");
  DEBUG_PRINT(control, DEC);
  DEBUG_PRINT(", value=");
  DEBUG_PRINT(value, DEC);
  DEBUG_PRINTLN("");

>>>>>>> 9a84e43... esp32 version only
  if (control == 0) {
    colTemp.red = value*2;
  } else if (control == 1) {
    colTemp.green = value*2;
  } else if (control == 2) {
    colTemp.blue = value*2;
  }

  ButtonDevice *buttonDevice = devices.buttonDevice(0);
  if (buttonDevice) {
    RGBButton *btn0 = buttonDevice->button(0);
    if (btn0) btn0->setColor(colTemp);
    buttonDevice->sendCurrentColorStream();
  } // if (buttonDevice) {
  
  if (channel == 1 && control == 123 && value == 0) {
<<<<<<< HEAD
    display.setColor(255, 0, 0);
    display.setPrintPos(5, 65);
    display.print("<MIDI> reset!");
=======
    // midi reset
>>>>>>> 9a84e43... esp32 version only
  }
}

void OnProgramChange(byte channel, byte program) {
  Serial.print("Program Change, ch=");
  Serial.print(channel, DEC);
  Serial.print(", program=");
  Serial.print(program, DEC);
  Serial.println();
}

void OnAfterTouch(byte channel, byte pressure) {
  Serial.print("After Touch, ch=");
  Serial.print(channel, DEC);
  Serial.print(", pressure=");
  Serial.print(pressure, DEC);
  Serial.println();
}

void OnPitchChange(byte channel, int pitch) {
  Serial.print("Pitch Change, ch=");
  Serial.print(channel, DEC);
  Serial.print(", pitch=");
  Serial.print(pitch, DEC);
  Serial.println();
}

void setup() {  
  //if (_debug) {
    Serial.begin(9600);
  //}
  pinMode(18, INPUT_PULLUP);
  pinMode(19, INPUT_PULLUP);

  // initial I2C library with internal pullups and 1MHz clock
<<<<<<< HEAD
  Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_INT, I2C_RATE_1000);
  
  // initial display with 180 degree screen rotation
  display.begin(UCG_FONT_MODE_TRANSPARENT);
  display.setFont(ucg_font_ncenR14_hr);
  display.clearScreen();
  
  // register callbacks for usbMIDI events
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleVelocityChange(OnVelocityChange);
  usbMIDI.setHandleControlChange(OnControlChange);
  usbMIDI.setHandleProgramChange(OnProgramChange);
  usbMIDI.setHandleAfterTouch(OnAfterTouch);
  usbMIDI.setHandlePitchChange(OnPitchChange);

  // register enable and interrupt pins and attach interrupts
  DEBUG_PRINTLN(F("Main: Initial interrupt pins"));
  // interrupt pin to get informed if some devices through interrupts
  pinMode(INTERRUPT_PIN, INPUT);
  attachInterrupt(INTERRUPT_PIN, onInterruptPinRising, FALLING);
=======
  uint64_t chipid=ESP.getEfuseMac();//The chip ID is essentially its MAC address(length: 6 bytes).
  DEBUG_PRINTF("ESP32 Chip ID = %04X",(uint16_t)(chipid>>32));//print High 2 bytes
  DEBUG_PRINTF("%08X\n",(uint32_t)chipid);//print Low 4bytes.
  WiFi.mode(WIFI_MODE_NULL);
  //pinMode(21, INPUT_PULLUP);    // SDA
  //pinMode(22, INPUT_PULLUP);    // SCL
  Wire.begin(SDA, SCL, 400000L);
  //Wire.setClock(1000000L);

  // register callbacks for usbMIDI events
  MIDI.setHandleNoteOff(OnNoteOff);
  MIDI.setHandleNoteOn(OnNoteOn);
  MIDI.setHandleControlChange(OnControlChange);
  MIDI.setHandleProgramChange(OnProgramChange);
  // Initiate MIDI communications, listen to all channels
  MIDI.begin(MIDI_CHANNEL_OMNI);
  // register enable and interrupt pins and attach interrupts
  DEBUG_PRINTLN(F("Main: Initial interrupt pins"));
  // interrupt pin to get informed if some devices through interrupts
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(INTERRUPT_PIN, onInterruptPinLow, ONLOW);
>>>>>>> 9a84e43... esp32 version only

  eState = MLS_INITIAL_DEVICES;
}

void loop() {
  // handle interrupts
  if (interruptEnabled) {
    for (int idx = 0; idx < devices.deviceCount(); ++idx) {
      I2C_Device *dev = devices.device(idx);
      if (dev) {
        //Serial.println("fetchDeviceStatus");
        //uint8_t devState = dev->fetchDeviceStatus();
        //Serial.println(devState, HEX);
        //if (devState != 0x00) {
          switch (devices.deviceType(idx)) {
            default:
            case I2C_Device::DEVICE_UNKNOWN: {
              DEBUG_PRINTLN(F("Main: loop > state MLS_PROCESS > process all devices > interrupt > deviceType > DEVICE_UNKNOWN"));
              break;
            } case I2C_Device::DEVICE_BUTTONS: {
              DEBUG_PRINTLN(F("Main: loop > state MLS_PROCESS > process all devices > interrupt > deviceType > DEVICE_BUTTONS"));
              ButtonDevice *buttonDevice = devices.buttonDevice(idx);
              if (buttonDevice) {
                buttonDevice->fetchButtonStates();
                /*
                RGBButton *btn4 = buttonDevice->button(4);
                if (btn4) {
                  if (bLastState2 && !btn4->isPressed()) {
                    speedIdx++;
                    if (speedIdx>9)
                      speedIdx = 0;
                    thisdelay = speedList[speedIdx];
                  }
                  bLastState2 = btn4->isPressed();
                }
                RGBButton *btn5 = buttonDevice->button(5);
                if (btn5) {
                  if (bLastState && !btn5->isPressed()) {
                    bToggle = !bToggle;
                  }
                  bLastState = btn5->isPressed();
                }
                
                for (uint8_t btnIdx = 0; btnIdx < 6; ++btnIdx) {
                  RGBButton *btn = buttonDevice->button(btnIdx);
                  if (btn) {
                    if (btn->isPressed()) {
                      usbMIDI.sendNoteOn(notes[btnIdx], 99, 1);
                      btn->setColor(rgbMap[btnIdx]); 
                    }else {
                      btn->setColor(CRGB::Black);
                      usbMIDI.sendNoteOff(notes[btnIdx], 0, 1);
                    }
                  } // if (btn) {
                } // for (uint8_t btnIdx = 0; btnIdx < 6; ++btnIdx) {
                */
              } // if (buttonDevice) {
              break;
            } case I2C_Device::DEVICE_SLIDERS: {
              DEBUG_PRINTLN(F("Main: loop > state MLS_PROCESS > process all devices > interrupt > deviceType > DEVICE_SLIDERS"));
              break;
            } case I2C_Device::DEVICE_ENCODERS: {
              DEBUG_PRINTLN(F("Main: loop > state MLS_PROCESS > process all devices > interrupt > deviceType > DEVICE_ENCODERS"));
              break;
            }
          } // switch (devices.deviceType(idx)) {
        //} // if (devState != 0x00) {
      } // if (dev) {
      interruptEnabled = false;
<<<<<<< HEAD
=======
      gpio_set_intr_type((gpio_num_t)INTERRUPT_PIN, GPIO_INTR_LOW_LEVEL);
>>>>>>> 9a84e43... esp32 version only
    } // for (int idx = 0; idx < devices.deviceCount(); ++idx) {
  } // if (interruptEnabled) 

  // main loop state machine
  switch (eState) {
    default:
    case MLS_UNKNOWN: {
      DEBUG_PRINTLN(F("Main: loop > state MLS_UNKNOWN:"));
      break;
    } // case MLS_UNKNOWN:
    case MLS_INITIAL_DEVICES: {
      DEBUG_PRINTLN(F("Main: loop > state MLS_INITIAL_DEVICES:"));
      // initial devices
      devices.setSuccessInitialCallback(&onSuccessInitialCallback);
      DeviceList::DeviceListStates initialState = devices.initialDevices();
      switch (initialState) {
        default:
        case DeviceList::DL_INIT_UNKNOWN: {
          DEBUG_PRINTLN(F("Main: loop > initialDevices > DL_INIT_UNKNOWN"));
          break;
        } case DeviceList::DL_INIT_ABORT: {
          DEBUG_PRINTLN(F("Main: loop > initialDevices > DL_INIT_ABORT"));
          break;
        } case DeviceList::DL_INIT_ERROR: {
          DEBUG_PRINTLN(F("Main: loop > initialDevices > DL_INIT_ERROR"));
          break;
        } case DeviceList::DL_INIT_SUCCESS: {
          DEBUG_PRINTLN(F("Main: loop > initialDevices > DL_INIT_SUCCESS"));
          break;
        }
      } // switch (initialState)
      if (initialState) {
        DEBUG_PRINTLN(F("Main: loop > configure devices: "));
        for (int idx = 0; idx < devices.deviceCount(); ++idx) {
          switch (devices.deviceType(idx)) {
            default:
            case I2C_Device::DEVICE_UNKNOWN: {
              DEBUG_PRINTLN(F("Main: loop > configure devices > deviceType > DEVICE_UNKNOWN"));
              break;
            } case I2C_Device::DEVICE_BUTTONS: {
              DEBUG_PRINTLN(F("Main: loop > configure devices > deviceType > DEVICE_BUTTONS"));
              DEBUG_PRINTLN(FreeRam());
              ButtonDevice *buttonDevice = devices.buttonDevice(idx);
              // add buttons to ButtonDevice buttonDevice
              if (buttonDevice) {
                buttonDevice->initial();
                buttonDevice->setGlobalButtonPressCallback(&onButtonPressCallback);
                buttonDevice->setGlobalButtonReleaseCallback(&onButtonReleaseCallback);
                buttonDevice->setButtonCount(6);
                DEBUG_PRINTLN(FreeRam());
              }
              break;
            } case I2C_Device::DEVICE_SLIDERS: {
              DEBUG_PRINTLN(F("Main: loop > configure devices > deviceType > DEVICE_SLIDERS"));
              break;
            } case I2C_Device::DEVICE_ENCODERS: {
              DEBUG_PRINTLN(F("Main: loop > configure devices > deviceType > DEVICE_ENCODERS"));
              break;
            }
          } // switch (devices.deviceType(idx))
        } // for (int idx = 0; idx < devices.deviceCount(); ++idx)
      } // if (initialState)
      DEBUG_PRINTLN(F("Main: loop > state MLS_INITIAL_DEVICES > state MLS_PROCESS"));
      eState = MLS_PROCESS;
      break;
    } // case MLS_INITIAL_DEVICES:
    case MLS_PROCESS: {
      //#uint8_t interruptFlag = digitalRead(INTERRUPT_PIN);
      DEBUG_PRINTLN(F("Main: loop > state MLS_PROCESS:"));
      for (int idx = 0; idx < devices.deviceCount(); ++idx) {
        switch (devices.deviceType(idx)) {
          default:
          case I2C_Device::DEVICE_UNKNOWN: {
            DEBUG_PRINTLN(F("Main: loop > state MLS_PROCESS > process all devices > deviceType > DEVICE_UNKNOWN"));
            break;
          } case I2C_Device::DEVICE_BUTTONS: {
            DEBUG_PRINTLN(F("Main: loop > state MLS_PROCESS > process all devices > deviceType > DEVICE_BUTTONS"));
            ButtonDevice *buttonDevice = devices.buttonDevice(idx);
            if (buttonDevice) {
              if (bToggle) {
                if (millis() - previous_millis >= thisdelay) {
                //EVERY_N_MILLISECONDS(thisdelay) {
                  hue0+=hueDelta;
                  hue1+=hueDelta;
                  hue2+=hueDelta;
                  hue3+=hueDelta;
                  hue4+=hueDelta;
                  hue5+=hueDelta;
                  RGBButton *btn0 = buttonDevice->button(0);
                  if (btn0) btn0->setColor(colTemp);//CHSV(hue0, 255, 255));
                  RGBButton *btn1 = buttonDevice->button(1);
                  if (btn1) btn1->setColor(CHSV(hue1, 255, 255));
                  RGBButton *btn2 = buttonDevice->button(2);
                  if (btn2) btn2->setColor(CHSV(hue2, 255, 255));
                  RGBButton *btn3 = buttonDevice->button(3);
                  if (btn3) btn3->setColor(CHSV(hue3, 255, 255));
                  RGBButton *btn4 = buttonDevice->button(4);
                  if (btn4) btn4->setColor(CHSV(hue4, 255, 255));
                  RGBButton *btn5 = buttonDevice->button(5);
                  if (btn5) btn5->setColor(CHSV(hue5, 255, 255));
                  previous_millis = millis();
                } // if (millis() - previous_millis >= thisdelay) {
              } // if (!bToggle) else
              buttonDevice->sendCurrentColorStream();
            } // if (buttonDevice) {
            break;
          } case I2C_Device::DEVICE_SLIDERS: {
            DEBUG_PRINTLN(F("Main: loop > state MLS_PROCESS > process all devices > deviceType > DEVICE_SLIDERS"));
            break;
          } case I2C_Device::DEVICE_ENCODERS: {
            DEBUG_PRINTLN(F("Main: loop > state MLS_PROCESS > process all devices > deviceType > DEVICE_ENCODERS"));
            break;
          }
        } // switch (devices.deviceType(idx))
      } // for (int idx = 0; idx < devices.deviceCount(); ++idx)
      break;
    } // case MLS_PROCESS:
  } // switch (eState)
<<<<<<< HEAD
  usbMIDI.read(); // USB MIDI receive
=======
  // Call MIDI.read the fastest you can for real-time performance.
  MIDI.read();
>>>>>>> 9a84e43... esp32 version only
}
