/*
  Lepton.h - Library for accessing the Lepton thermal imaging camera (http://lepton.flir.com/)
  Created by Matt Williams, 17-19 June 2016
  BSD license - check license.txt for more information
 */

#ifndef Lepton_h
#define Lepton_h

#include "Arduino.h"

class Lepton {
public:
  // Registers for use with readRegister and writeRegister.
  static const uint16_t REG_POWER = 0;
  static const uint16_t REG_STATUS = 2;
  static const uint16_t REG_COMMAND_ID = 4;
  static const uint16_t REG_DATA_LEN = 6;
  static const uint16_t REG_DATA_BASE = 8;

  // Automatic Gain Control commands
  static const uint16_t CMD_AGC_ENABLE = 0x0100;
  static const uint16_t CMD_AGC_ROI = 0x0108;
  static const uint16_t CMD_AGC_HISTOGRAM_STATISTICS = 0x010c;
  static const uint16_t CMD_AGC_HEQ_DAMPING_FACTOR = 0x0124;
  static const uint16_t CMD_AGC_HEQ_CLIP_LIMIT_HIGH = 0x012c;
  static const uint16_t CMD_AGC_HEQ_CLIP_LIMIT_LOW = 0x0130;
  static const uint16_t CMD_AGC_HEQ_EMPTY_COUNT = 0x013c;
  static const uint16_t CMD_AGC_HEQ_SCALE_FACTOR = 0x0144;
  static const uint16_t CMD_AGC_HEQ_CALC_ENABLE_STATE = 0x0148;

  // System commands
  static const uint16_t CMD_SYS_PING = 0x0200;
  static const uint16_t CMD_SYS_STATUS = 0x0204;
  static const uint16_t CMD_SYS_FLIR_SERIAL_NUMBER = 0x0208;
  static const uint16_t CMD_SYS_CAMERA_UPTIME = 0x020C;
  static const uint16_t CMD_SYS_AUX_TEMPERATURE_KELVIN = 0x0210;
  static const uint16_t CMD_SYS_FPA_TEMPERATURE_KELVIN = 0x0214;
  static const uint16_t CMD_SYS_TELEMETRY_ENABLE = 0x0218;
  static const uint16_t CMD_SYS_TELEMETRY_LOCATION = 0x021c;
  static const uint16_t CMD_SYS_FRAMES_TO_AVERAGE = 0x0224;
  static const uint16_t CMD_SYS_CUST_SERIAL_NUMBER = 0x0228;
  static const uint16_t CMD_SYS_SCENE_STATISTICS = 0x022c;
  static const uint16_t CMD_SYS_SCENE_ROI = 0x0230;
  static const uint16_t CMD_SYS_THERMAL_SHUTDOWN_COUNT = 0x0234;
  static const uint16_t CMD_SYS_SHUTTER_POSITION = 0x0238;
  static const uint16_t CMD_SYS_FFC_SHUTTER_MODE = 0x023c;
  static const uint16_t CMD_SYS_FFC_NORMALIZATION = 0x0240;
  static const uint16_t CMD_SYS_FFC_STATUS = 0x0244;

  // Video commands
  static const uint16_t CMD_VID_USER_LUT = 0x0308;
  static const uint16_t CMD_VID_FOCUS_CALC_ENABLE = 0x030c;
  static const uint16_t CMD_VID_FOCUS_ROI = 0x0310;
  static const uint16_t CMD_VID_FOCUS_METRIC_THRESHOLD = 0x0314;
  static const uint16_t CMD_VID_FOCUS_METRIC = 0x0318;
  static const uint16_t CMD_VID_FREEZE_ENABLE = 0x0324;

  static const uint16_t STATUS_BIT_BUSY = 1;
  static const uint16_t STATUS_BIT_BOOT_MODE = 2;
  static const uint16_t STATUS_BIT_BOOT_STATUS = 4;

  // Create a new Lepton instance
  Lepton(int sdaPin, int sclPin, int ssPin, int width, int height);

  // Start Lepton access
  void begin();

  // Read a (16-bit) register
  uint16_t readRegister(uint16_t reg);

  // Write a (16-bit) register
  void writeRegister(uint16_t reg, uint16_t value);

  // Do a get command, and get the resulting data
  uint16_t doGetCommand(uint16_t commandIdBase, uint16_t* data);

  // Do a set command, using the provided data
  void doSetCommand(uint16_t commandIdBase, uint16_t* data, uint16_t dataLen);

  // Do a run command, using the provided data and returning the result in the same buffer
  uint16_t doRunCommand(uint16_t commandIdBase, uint16_t* data, uint16_t dataLen);

  // (Re-)synchronize the frame stream
  void syncFrame();
  
  void end();

  // Read a frame into a 80 * 60 uint16_t buffer
  int readFrame(uint16_t* data);

  // Read completely raw frame data - for debugging purposes
  void readFrameRaw(uint16_t* data);

private:
  // Device ID on the I2C interface
  static const uint8_t DEVICE_ID = 0x2A;

  // Command types
  static const uint16_t TYPE_GET = 0;
  static const uint16_t TYPE_SET = 1;
  static const uint16_t TYPE_RUN = 2;

  // Start I2C transmission relating to a specific register
  void startTransmission(uint16_t reg);

  // Transmit a word over I2C
  void transmitWord(uint16_t value);

  // End I2C transmission
  void endTransmission();

  // Read a word over I2C
  uint16_t readWord();

  // Set the current register over I2C
  void setRegister(uint16_t reg);

  // Wait for the camera to become idle, e.g. after performing a command
  void waitIdle();

  // Read data from a buffer
  uint16_t readData(uint16_t* data);
  
  // Write data to a buffer
  void writeData(uint16_t* data, uint16_t dataLen);

  // Read a word of frame data over SPI
  uint16_t readFrameWord();

  // Wait for the next frame over SPI
  uint16_t waitNextFrame();

  // Dump a buffer as hex to serial - for debugging purposes
  void dumpHex(uint16_t *data, int dataLen);

  // SDA, SCL and SPI CS pins
  int _sdaPin;
  int _sclPin;
  int _ssPin;

  int _width;
  int _height;
};

#endif
