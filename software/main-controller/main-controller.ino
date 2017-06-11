#include <Arduino.h>
#include "config.h"
#include "helper.h"

#if defined(ESP32)
#include <WiFi.h>
#endif

#if !defined(__MK20DX256__)
#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();
#endif

#if _display
#include <SPI.h>
#include <FTOLED.h>
#include "fonts/SystemFont5x7.h"
#endif

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

volatile boolean interruptEnabled = false;

// initial device list class
DeviceList devices(ENABLE_PIN, ENABLE_LOOP_PIN);

// initial display
//display_SH1106_128X64_NONAME_F_4W_HW_SPI display(display_R0, OLED_CS_PIN, OLED_DC_PIN, OLED_RST_PIN);
//Ucglib_SSD1331_18x96x64_UNIVISION_HWSPI display(OLED_DC_PIN, OLED_CS_PIN, OLED_RST_PIN);
//Ucglib_ILI9163_18x128x128_HWSPI display(OLED_DC_PIN, OLED_CS_PIN, OLED_RST_PIN);
//Ucglib_SSD1351_18x128x128_HWSPI display(OLED_DC_PIN, OLED_CS_PIN, OLED_RST_PIN);
#if _display
OLED display(OLED_CS_PIN, OLED_DC_PIN, OLED_RST_PIN);
#endif


RgbColor rgbMap[6] = {RgbColor(255,0,0), RgbColor(255,255,0), RgbColor(0,255,0), RgbColor(0,255,255), RgbColor(0,0,255), RgbColor(255,0,255)};
float hue0 = 0.00;
float hue1 = 0.16;
float hue2 = 0.33;
float hue3 = 0.5;
float hue4 = 0.67;
float hue5 = 0.84;
float hueDelta = 0.01;

bool bToggle = false;
bool bLastState = false;
bool bLastState2 = false;
uint8_t speedIdx = 3;
uint8_t speedList[10] = {0, 1, 2, 5, 10, 25, 50, 100, 175, 255};
uint8_t notes[6] = {60, 62, 64, 65, 67, 69};
static uint8_t thisdelay = 25;

unsigned long previous_millis = 0;



void onSuccessInitialCallback(I2C_Device* device) {
  DEBUG_PRINT(F("Main: onSuccessInitialCallback - device initialed: "));
  DEBUG_PRINTLN(device->deviceID());
}


void onButtonPressCallback(RGBButton* btn) {
  DEBUG_PRINT(F("Main: onButtonPressCallback ("));
  uint8_t buttonID = btn->id();
  ButtonDevice *buttonDevice = btn->device();
  int channelOffset = buttonDevice->channelOffset();
  DEBUG_PRINT(channelOffset+buttonID);
  DEBUG_PRINT(F(", "));
  DEBUG_PRINT(buttonDevice->deviceID());
  DEBUG_PRINTLN(F("): pressed"));
  btn->setColor(rgbMap[buttonID]); 
  buttonDevice->sendCurrentColorStream();
#if defined(__MK20DX256__)
  usbMIDI.sendNoteOn(notes[buttonID], 99, 1);
#endif

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
  //noInterrupts();
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
#if defined(__MK20DX256__)
  usbMIDI.sendNoteOff(notes[buttonID], 0, 1);
#endif

  if (buttonID == 4) {
    speedIdx++;
    if (speedIdx>9)
      speedIdx = 0;
    thisdelay = speedList[speedIdx];
  }
  if (buttonID == 5) {
    bToggle = !bToggle;
  }
  //interrupts();
}

void onEnableLoopPinChanged() {
  DEBUG_PRINTLN(F("Main: onEnableLoopPinChanged: "));
}

void handle_interrupts() {
  while(1) {
    if (digitalRead(INTERRUPT_PIN) == LOW) {
      ButtonDevice *buttonDevice = devices.buttonDevice(0);
      if (buttonDevice) {
        uint8_t devState = buttonDevice->fetchDeviceStatus();
        if (devState != 0x00) {
          buttonDevice->fetchButtonStates();
        } // if (devState != 0x00) {
        else {
          DEBUG_PRINTLN(F("handle_interrupts > devState != 0x00"));
        }
      } // if (buttonDevice) {
    } // if (digitalRead(INTERRUPT_PIN) == LOW) {
  } // while(1) {
} // handle_interrupts

void onInterruptPinLow() {
  interruptEnabled = true;
  //digitalWrite(5, HIGH);
  //gpio_intr_disable((gpio_num_t)INTERRUPT_PIN);
  gpio_set_intr_type((gpio_num_t)INTERRUPT_PIN, GPIO_INTR_DISABLE);
}

const int ledPin = 13;

static RgbColor colTemp = RgbColor(0);

void OnNoteOn(byte channel, byte note, byte velocity) {
  DEBUG_PRINT("Note On, ch=");
  DEBUG_PRINT(channel, DEC);
  DEBUG_PRINT(", note=");
  DEBUG_PRINT(note, DEC);
  DEBUG_PRINT(", velocity=");
  DEBUG_PRINT(velocity, DEC);
  DEBUG_PRINTLN("");

#if _display
/*
  //display.setColor(0, 80, 40, 0);
  //display.setColor(1, 60, 0, 40);
  //display.setColor(2, 20, 0, 20);
  //display.setColor(3, 60, 0, 0); 
  //display.drawGradientBox(0, 0, display.getWidth(), display.getHeight()); 
  display.setColor(0,0,0);
  display.drawBox(0, 0, 80, 64);
  
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
  display.print(velocity);*/
#endif
  
  if (note == 60) {
    colTemp.R = 255; 
  } else if (note == 62) {
    colTemp.G = 255; 
  } else if (note == 64) {
    colTemp.B = 255; 
  }

  ButtonDevice *buttonDevice = devices.buttonDevice(0);
  if (buttonDevice) {
    RGBButton *btn0 = buttonDevice->button(0);
    if (btn0) btn0->setColor(colTemp);
    buttonDevice->sendCurrentColorStream();
  } // if (buttonDevice) {
}

void OnNoteOff(byte channel, byte note, byte velocity) {
  DEBUG_PRINT("Note Off, ch=");
  DEBUG_PRINT(channel, DEC);
  DEBUG_PRINT(", note=");
  DEBUG_PRINT(note, DEC);
  DEBUG_PRINT(", velocity=");
  DEBUG_PRINT(velocity, DEC);
  DEBUG_PRINTLN("");

#if _display
/*
  //display.setColor(0, 80, 40, 0);
  //display.setColor(1, 60, 0, 40);
  //display.setColor(2, 20, 0, 20);
  //display.setColor(3, 60, 0, 0);  
  //display.drawGradientBox(0, 0, display.getWidth(), display.getHeight());
  display.setColor(0,0,0);
  display.drawBox(0, 0, 80, 64);
  
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
  display.print(velocity);*/
#endif
  
  if (note == 60) {
    colTemp.R = 0; 
  } else if (note == 62) {
    colTemp.G = 0; 
  } else if (note == 64) {
    colTemp.B = 0; 
  }

  ButtonDevice *buttonDevice = devices.buttonDevice(0);
  if (buttonDevice) {
    RGBButton *btn0 = buttonDevice->button(0);
    if (btn0) btn0->setColor(colTemp);
    buttonDevice->sendCurrentColorStream();
  } // if (buttonDevice) {
}

void OnVelocityChange(byte channel, byte note, byte velocity) {
  DEBUG_PRINT("Velocity Change, ch=");
  DEBUG_PRINT(channel, DEC);
  DEBUG_PRINT(", note=");
  DEBUG_PRINT(note, DEC);
  DEBUG_PRINT(", velocity=");
  DEBUG_PRINT(velocity, DEC);
  DEBUG_PRINTLN("");

#if _display
/*
  //display.setColor(0, 80, 40, 0);
  //display.setColor(1, 60, 0, 40);
  //display.setColor(2, 20, 0, 20);
  //display.setColor(3, 60, 0, 0); 
  //display.drawGradientBox(0, 0, display.getWidth(), display.getHeight()); 
  display.setColor(0,0,0);
  display.drawBox(0, 0, 128, 64);
  
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
  display.print(velocity);*/
#endif
}

void OnControlChange(byte channel, byte control, byte value) {
  DEBUG_PRINT("Control Change, ch=");
  DEBUG_PRINT(channel, DEC);
  DEBUG_PRINT(", control=");
  DEBUG_PRINT(control, DEC);
  DEBUG_PRINT(", value=");
  DEBUG_PRINT(value, DEC);
  DEBUG_PRINTLN("");

#if _display
  OLED_TextBox box(display, 0, 0, 128, 127);

  box.reset(); // reset the text box contents, without clearing screen (allows flicker-free redraw)
  box.setForegroundColour(DARKBLUE);
  box.print("Control Change");
  box.setForegroundColour(WHITE);
  box.print("channel: ");
  box.print(channel);
  box.print("control: ");
  box.print(control);
  box.print("value: ");
  box.print(value);
  box.print("\n");
#endif
  
  if (control == 0) {
    colTemp.R = value*2;
  } else if (control == 1) {
    colTemp.G = value*2;
  } else if (control == 2) {
    colTemp.B = value*2;
  }

  ButtonDevice *buttonDevice = devices.buttonDevice(0);
  if (buttonDevice) {
    RGBButton *btn0 = buttonDevice->button(0);
    if (btn0) btn0->setColor(colTemp);
    buttonDevice->sendCurrentColorStream();
  } // if (buttonDevice) {
  
  if (channel == 1 && control == 123 && value == 0) {
    //box.setForegroundColour(RED);
    //box.print("\n");
    //box.print("<MIDI> reset!");
  }
}

void OnProgramChange(byte channel, byte program) {
  DEBUG_PRINT("Program Change, ch=");
  DEBUG_PRINT(channel, DEC);
  DEBUG_PRINT(", program=");
  DEBUG_PRINT(program, DEC);
  DEBUG_PRINTLN("");
}

void OnAfterTouch(byte channel, byte pressure) {
  DEBUG_PRINT("After Touch, ch=");
  DEBUG_PRINT(channel, DEC);
  DEBUG_PRINT(", pressure=");
  DEBUG_PRINT(pressure, DEC);
  DEBUG_PRINTLN("");
}

void OnPitchChange(byte channel, int pitch) {
  DEBUG_PRINT("Pitch Change, ch=");
  DEBUG_PRINT(channel, DEC);
  DEBUG_PRINT(", pitch=");
  DEBUG_PRINT(pitch, DEC);
  DEBUG_PRINTLN("");
}

void setup() {  
  //if (_debug) {
    Serial.begin(115200);
    delay(2000);
  //}

#if _display
  display.begin();
#endif
  //display.selectFont(System5x7);
  // initial I2C library with internal pullups and 1MHz clock
#if defined(__MK20DX256__)  
  Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_INT, I2C_RATE_1000);
#elif defined(ESP32)
  uint64_t chipid=ESP.getEfuseMac();//The chip ID is essentially its MAC address(length: 6 bytes).
  DEBUG_PRINTF("ESP32 Chip ID = %04X",(uint16_t)(chipid>>32));//print High 2 bytes
  DEBUG_PRINTF("%08X\n",(uint32_t)chipid);//print Low 4bytes.
  WiFi.mode(WIFI_MODE_NULL);
  /*
  DEBUG_PRINT(F("Chip-ID: "));
  DEBUG_PRINTLN(ESP.getChipId());
  WiFi.mode(WIFI_OFF);
  */
  //pinMode(21, INPUT_PULLUP);    // SDA
  //pinMode(22, INPUT_PULLUP);    // SCL
  //Wire.begin();
  Wire.begin(SDA, SCL, 400000L); // SDA (21), SCL (22) on ESP32, 400 kHz rate
  //Wire.begin(I2C_SDA, I2C_SCL);
  //Wire.setClock(1000000L);
#else
  Wire.begin();
  Wire.setClock(1000000L);
#endif

#if defined(__MK20DX256__)
  // register callbacks for usbMIDI events
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleVelocityChange(OnVelocityChange);
  usbMIDI.setHandleControlChange(OnControlChange);
  usbMIDI.setHandleProgramChange(OnProgramChange);
  usbMIDI.setHandleAfterTouch(OnAfterTouch);
  usbMIDI.setHandlePitchChange(OnPitchChange);
#else
  // register callbacks for usbMIDI events
  MIDI.setHandleNoteOff(OnNoteOff);
  MIDI.setHandleNoteOn(OnNoteOn);
  MIDI.setHandleControlChange(OnControlChange);
  MIDI.setHandleProgramChange(OnProgramChange);
  // Initiate MIDI communications, listen to all channels
  MIDI.begin(MIDI_CHANNEL_OMNI);
#endif
  // register enable and interrupt pins and attach interrupts
  DEBUG_PRINTLN(F("Main: Initial interrupt pins"));
  // interrupt pin to get informed if some devices through interrupts
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(INTERRUPT_PIN, onInterruptPinLow, ONLOW);

  //pinMode(5, OUTPUT);
  //digitalWrite(5, LOW);

  eState = MLS_INITIAL_DEVICES;
}

void loop() {
  // handle interrupts
  if (interruptEnabled) {
    for (int idx = 0; idx < devices.deviceCount(); ++idx) {
      I2C_Device *dev = devices.device(idx);
      if (dev) {
        //uint8_t devState = dev->fetchDeviceStatus();
        //if (devState != 0x00) {
          switch (devices.deviceType(idx)) {
            default:
            case I2C_Device::DEVICE_UNKNOWN: {
              break;
            } case I2C_Device::DEVICE_BUTTONS: {
              ButtonDevice *buttonDevice = devices.buttonDevice(idx);
              if (buttonDevice) {
                buttonDevice->fetchButtonStates();
              } // if (buttonDevice) {
              break;
            } case I2C_Device::DEVICE_SLIDERS: {
              break;
            } case I2C_Device::DEVICE_ENCODERS: {
              break;
            }
          } // switch (devices.deviceType(idx)) {
        //} // if (devState != 0x00) {
      } // if (dev) {
      interruptEnabled = false;
      //digitalWrite(5, LOW);
      //gpio_intr_enable((gpio_num_t)INTERRUPT_PIN);
      gpio_set_intr_type((gpio_num_t)INTERRUPT_PIN, GPIO_INTR_LOW_LEVEL);
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
      if (initialState == DeviceList::DL_INIT_SUCCESS) {
        for (int idx = 0; idx < devices.deviceCount(); ++idx) {
          switch (devices.deviceType(idx)) {
            default:
            case I2C_Device::DEVICE_UNKNOWN: {
              DEBUG_PRINTLN(F("Main: loop > configure devices > deviceType > DEVICE_UNKNOWN"));
              break;
            } case I2C_Device::DEVICE_BUTTONS: {
              ButtonDevice *buttonDevice = devices.buttonDevice(idx);
              // add buttons to ButtonDevice buttonDevice
              if (buttonDevice) {
                buttonDevice->initial();
                buttonDevice->setGlobalButtonPressCallback(&onButtonPressCallback);
                buttonDevice->setGlobalButtonReleaseCallback(&onButtonReleaseCallback);
                buttonDevice->setButtonCount(6);
              }
              break;
            } case I2C_Device::DEVICE_SLIDERS: {
              break;
            } case I2C_Device::DEVICE_ENCODERS: {
              break;
            }
          } // switch (devices.deviceType(idx))
        } // for (int idx = 0; idx < devices.deviceCount(); ++idx)
        DEBUG_PRINTLN(F("Main: loop > state MLS_INITIAL_DEVICES > state MLS_PROCESS"));
        eState = MLS_PROCESS;
      } // if (initialState)
      break;
    } // case MLS_INITIAL_DEVICES:
    case MLS_PROCESS: {
      for (int idx = 0; idx < devices.deviceCount(); ++idx) {
        switch (devices.deviceType(idx)) {
          default:
          case I2C_Device::DEVICE_UNKNOWN: {
            DEBUG_PRINTLN(F("Main: loop > state MLS_PROCESS > process all devices > deviceType > DEVICE_UNKNOWN"));
            break;
          } case I2C_Device::DEVICE_BUTTONS: {
            
            ButtonDevice *buttonDevice = devices.buttonDevice(idx);
            if (buttonDevice) {
              if (bToggle) {
                if (millis() - previous_millis >= thisdelay) {
                  hue0+=hueDelta;
                  hue1+=hueDelta;
                  hue2+=hueDelta;
                  hue3+=hueDelta;
                  hue4+=hueDelta;
                  hue5+=hueDelta;
                  if (hue0 > 1.0) hue0 = 0.0;
                  if (hue1 > 1.0) hue1 = 0.0;
                  if (hue2 > 1.0) hue2 = 0.0;
                  if (hue3 > 1.0) hue3 = 0.0;
                  if (hue4 > 1.0) hue4 = 0.0;
                  if (hue5 > 1.0) hue5 = 0.0;
                  RGBButton *btn0 = buttonDevice->button(0);
                  if (btn0) btn0->setColor(colTemp);//CHSV(hue0, 255, 255));
                  RGBButton *btn1 = buttonDevice->button(1);
                  if (btn1) btn1->setColor(HslColor(hue1, 1.0, 1.0));
                  RGBButton *btn2 = buttonDevice->button(2);
                  if (btn2) btn2->setColor(HslColor(hue2, 1.0, 1.0));
                  RGBButton *btn3 = buttonDevice->button(3);
                  if (btn3) btn3->setColor(HslColor(hue3, 1.0, 1.0));
                  RGBButton *btn4 = buttonDevice->button(4);
                  if (btn4) btn4->setColor(HslColor(hue4, 1.0, 1.0));
                  RGBButton *btn5 = buttonDevice->button(5);
                  if (btn5) btn5->setColor(HslColor(hue5, 1.0, 1.0));
                  previous_millis = millis();
                } // if (millis() - previous_millis >= thisdelay) {
                buttonDevice->sendCurrentColorStream();
              } // if (!bToggle) else
            } // if (buttonDevice) {
            break;
          } case I2C_Device::DEVICE_SLIDERS: {
            
            break;
          } case I2C_Device::DEVICE_ENCODERS: {
            
            break;
          }
        } // switch (devices.deviceType(idx))
      } // for (int idx = 0; idx < devices.deviceCount(); ++idx)
      break;
    } // case MLS_PROCESS:
  } // switch (eState)
#if defined(__MK20DX256__)
  usbMIDI.read(); // USB MIDI receive
#else
  // Call MIDI.read the fastest you can for real-time performance.
  MIDI.read();
#endif
}
