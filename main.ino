#include <Wire.h>
#include <SPI.h>
#include <Lepton.h>
#include <TFT_eSPI.h>
 
/* Lepton Pinmap
 * SDA -> 21
 * SCL -> 22
 * CS -> 5
 * CLK(SCK) -> 18
 * MISO -> 19
 * MOSI -> 23
 */
Lepton lepton(21, 22, 5);
const int leptonWidth = 60;
const int leptonHeight = 80;
uint16_t frameData[leptonWidth * leptonHeight];

/* LCD Pinmap (HSPI)
 * LCD : 320*240
 * CS -> 15
 * RESET -> 4
 * DC -> 2
 * SDI(MOSI) -> 13
 * CLK(SCK) -> 14
 * LED -> VCC
 * SDOK(MISO) -> 12
 * T_CLK -> SCK
 * T_CS -> 34
 * T_DIN -> MOSI
 * T_DO -> MISO
 * // also need to allow USE_HSPI_PORT option in User_Setup.h
 */
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite videoSprite = TFT_eSprite(&tft);
const int videoScale = 4;
byte renderData[leptonWidth * videoScale][leptonHeight * videoScale];

// color map from : https://github.com/groupgets/LeptonModule/blob/master/software/raspberrypi_video/Palettes.cpp
const byte colormap_rainbow[] = {1, 3, 74, 0, 3, 74, 0, 3, 75, 0, 3, 75, 0, 3, 76, 0, 3, 76, 0, 3, 77, 0, 3, 79, 0, 3, 82, 0, 5, 85, 0, 7, 88, 0, 10, 91, 0, 14, 94, 0, 19, 98, 0, 22, 100, 0, 25, 103, 0, 28, 106, 0, 32, 109, 0, 35, 112, 0, 38, 116, 0, 40, 119, 0, 42, 123, 0, 45, 128, 0, 49, 133, 0, 50, 134, 0, 51, 136, 0, 52, 137, 0, 53, 139, 0, 54, 142, 0, 55, 144, 0, 56, 145, 0, 58, 149, 0, 61, 154, 0, 63, 156, 0, 65, 159, 0, 66, 161, 0, 68, 164, 0, 69, 167, 0, 71, 170, 0, 73, 174, 0, 75, 179, 0, 76, 181, 0, 78, 184, 0, 79, 187, 0, 80, 188, 0, 81, 190, 0, 84, 194, 0, 87, 198, 0, 88, 200, 0, 90, 203, 0, 92, 205, 0, 94, 207, 0, 94, 208, 0, 95, 209, 0, 96, 210, 0, 97, 211, 0, 99, 214, 0, 102, 217, 0, 103, 218, 0, 104, 219, 0, 105, 220, 0, 107, 221, 0, 109, 223, 0, 111, 223, 0, 113, 223, 0, 115, 222, 0, 117, 221, 0, 118, 220, 1, 120, 219, 1, 122, 217, 2, 124, 216, 2, 126, 214, 3, 129, 212, 3, 131, 207, 4, 132, 205, 4, 133, 202, 4, 134, 197, 5, 136, 192, 6, 138, 185, 7, 141, 178, 8, 142, 172, 10, 144, 166, 10, 144, 162, 11, 145, 158, 12, 146, 153, 13, 147, 149, 15, 149, 140, 17, 151, 132, 22, 153, 120, 25, 154, 115, 28, 156, 109, 34, 158, 101, 40, 160, 94, 45, 162, 86, 51, 164, 79, 59, 167, 69, 67, 171, 60, 72, 173, 54, 78, 175, 48, 83, 177, 43, 89, 179, 39, 93, 181, 35, 98, 183, 31, 105, 185, 26, 109, 187, 23, 113, 188, 21, 118, 189, 19, 123, 191, 17, 128, 193, 14, 134, 195, 12, 138, 196, 10, 142, 197, 8, 146, 198, 6, 151, 200, 5, 155, 201, 4, 160, 203, 3, 164, 204, 2, 169, 205, 2, 173, 206, 1, 175, 207, 1, 178, 207, 1, 184, 208, 0, 190, 210, 0, 193, 211, 0, 196, 212, 0, 199, 212, 0, 202, 213, 1, 207, 214, 2, 212, 215, 3, 215, 214, 3, 218, 214, 3, 220, 213, 3, 222, 213, 4, 224, 212, 4, 225, 212, 5, 226, 212, 5, 229, 211, 5, 232, 211, 6, 232, 211, 6, 233, 211, 6, 234, 210, 6, 235, 210, 7, 236, 209, 7, 237, 208, 8, 239, 206, 8, 241, 204, 9, 242, 203, 9, 244, 202, 10, 244, 201, 10, 245, 200, 10, 245, 199, 11, 246, 198, 11, 247, 197, 12, 248, 194, 13, 249, 191, 14, 250, 189, 14, 251, 187, 15, 251, 185, 16, 252, 183, 17, 252, 178, 18, 253, 174, 19, 253, 171, 19, 254, 168, 20, 254, 165, 21, 254, 164, 21, 255, 163, 22, 255, 161, 22, 255, 159, 23, 255, 157, 23, 255, 155, 24, 255, 149, 25, 255, 143, 27, 255, 139, 28, 255, 135, 30, 255, 131, 31, 255, 127, 32, 255, 118, 34, 255, 110, 36, 255, 104, 37, 255, 101, 38, 255, 99, 39, 255, 93, 40, 255, 88, 42, 254, 82, 43, 254, 77, 45, 254, 69, 47, 254, 62, 49, 253, 57, 50, 253, 53, 52, 252, 49, 53, 252, 45, 55, 251, 39, 57, 251, 33, 59, 251, 32, 60, 251, 31, 60, 251, 30, 61, 251, 29, 61, 251, 28, 62, 250, 27, 63, 250, 27, 65, 249, 26, 66, 249, 26, 68, 248, 25, 70, 248, 24, 73, 247, 24, 75, 247, 25, 77, 247, 25, 79, 247, 26, 81, 247, 32, 83, 247, 35, 85, 247, 38, 86, 247, 42, 88, 247, 46, 90, 247, 50, 92, 248, 55, 94, 248, 59, 96, 248, 64, 98, 248, 72, 101, 249, 81, 104, 249, 87, 106, 250, 93, 108, 250, 95, 109, 250, 98, 110, 250, 100, 111, 251, 101, 112, 251, 102, 113, 251, 109, 117, 252, 116, 121, 252, 121, 123, 253, 126, 126, 253, 130, 128, 254, 135, 131, 254, 139, 133, 254, 144, 136, 254, 151, 140, 255, 158, 144, 255, 163, 146, 255, 168, 149, 255, 173, 152, 255, 176, 153, 255, 178, 155, 255, 184, 160, 255, 191, 165, 255, 195, 168, 255, 199, 172, 255, 203, 175, 255, 207, 179, 255, 211, 182, 255, 216, 185, 255, 218, 190, 255, 220, 196, 255, 222, 200, 255, 225, 202, 255, 227, 204, 255, 230, 206, 255, 233, 208,
0};
const byte colormap_grayscale[] = {0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 21, 22, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 30, 30, 30, 31, 31, 31, 32, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35, 35, 36, 36, 36, 37, 37, 37, 38, 38, 38, 39, 39, 39, 40, 40, 40, 41, 41, 41, 42, 42, 42, 43, 43, 43, 44, 44, 44, 45, 45, 45, 46, 46, 46, 47, 47, 47, 48, 48, 48, 49, 49, 49, 50, 50, 50, 51, 51, 51, 52, 52, 52, 53, 53, 53, 54, 54, 54, 55, 55, 55, 56, 56, 56, 57, 57, 57, 58, 58, 58, 59, 59, 59, 60, 60, 60, 61, 61, 61, 62, 62, 62, 63, 63, 63, 64, 64, 64, 65, 65, 65, 66, 66, 66, 67, 67, 67, 68, 68, 68, 69, 69, 69, 70, 70, 70, 71, 71, 71, 72, 72, 72, 73, 73, 73, 74, 74, 74, 75, 75, 75, 76, 76, 76, 77, 77, 77, 78, 78, 78, 79, 79, 79, 80, 80, 80, 81, 81, 81, 82, 82, 82, 83, 83, 83, 84, 84, 84, 85, 85, 85, 86, 86, 86, 87, 87, 87, 88, 88, 88, 89, 89, 89, 90, 90, 90, 91, 91, 91, 92, 92, 92, 93, 93, 93, 94, 94, 94, 95, 95, 95, 96, 96, 96, 97, 97, 97, 98, 98, 98, 99, 99, 99, 100, 100, 100, 101, 101, 101, 102, 102, 102, 103, 103, 103, 104, 104, 104, 105, 105, 105, 106, 106, 106, 107, 107, 107, 108, 108, 108, 109, 109, 109, 110, 110, 110, 111, 111, 111, 112, 112, 112, 113, 113, 113, 114, 114, 114, 115, 115, 115, 116, 116, 116, 117, 117, 117, 118, 118, 118, 119, 119, 119, 120, 120, 120, 121, 121, 121, 122, 122, 122, 123, 123, 123, 124, 124, 124, 125, 125, 125, 126, 126, 126, 127, 127, 127, 128, 128, 128, 129, 129, 129, 130, 130, 130, 131, 131, 131, 132, 132, 132, 133, 133, 133, 134, 134, 134, 135, 135, 135, 136, 136, 136, 137, 137, 137, 138, 138, 138, 139, 139, 139, 140, 140, 140, 141, 141, 141, 142, 142, 142, 143, 143, 143, 144, 144, 144, 145, 145, 145, 146, 146, 146, 147, 147, 147, 148, 148, 148, 149, 149, 149, 150, 150, 150, 151, 151, 151, 152, 152, 152, 153, 153, 153, 154, 154, 154, 155, 155, 155, 156, 156, 156, 157, 157, 157, 158, 158, 158, 159, 159, 159, 160, 160, 160, 161, 161, 161, 162, 162, 162, 163, 163, 163, 164, 164, 164, 165, 165, 165, 166, 166, 166, 167, 167, 167, 168, 168, 168, 169, 169, 169, 170, 170, 170, 171, 171, 171, 172, 172, 172, 173, 173, 173, 174, 174, 174, 175, 175, 175, 176, 176, 176, 177, 177, 177, 178, 178, 178, 179, 179, 179, 180, 180, 180, 181, 181, 181, 182, 182, 182, 183, 183, 183, 184, 184, 184, 185, 185, 185, 186, 186, 186, 187, 187, 187, 188, 188, 188, 189, 189, 189, 190, 190, 190, 191, 191, 191, 192, 192, 192, 193, 193, 193, 194, 194, 194, 195, 195, 195, 196, 196, 196, 197, 197, 197, 198, 198, 198, 199, 199, 199, 200, 200, 200, 201, 201, 201, 202, 202, 202, 203, 203, 203, 204, 204, 204, 205, 205, 205, 206, 206, 206, 207, 207, 207, 208, 208, 208, 209, 209, 209, 210, 210, 210, 211, 211, 211, 212, 212, 212, 213, 213, 213, 214, 214, 214, 215, 215, 215, 216, 216, 216, 217, 217, 217, 218, 218, 218, 219, 219, 219, 220, 220, 220, 221, 221, 221, 222, 222, 222, 223, 223, 223, 224, 224, 224, 225, 225, 225, 226, 226, 226, 227, 227, 227, 228, 228, 228, 229, 229, 229, 230, 230, 230, 231, 231, 231, 232, 232, 232, 233, 233, 233, 234, 234, 234, 235, 235, 235, 236, 236, 236, 237, 237, 237, 238, 238, 238, 239, 239, 239, 240, 240, 240, 241, 241, 241, 242, 242, 242, 243, 243, 243, 244, 244, 244, 245, 245, 245, 246, 246, 246, 247, 247, 247, 248, 248, 248, 249, 249, 249, 250, 250, 250, 251, 251, 251, 252, 252, 252, 253, 253, 253, 254, 254, 254, 255, 255, 255,
0};
const byte colormap_ironblack[] = {255, 255, 255, 253, 253, 253, 251, 251, 251, 249, 249, 249, 247, 247, 247, 245, 245, 245, 243, 243, 243, 241, 241, 241, 239, 239, 239, 237, 237, 237, 235, 235, 235, 233, 233, 233, 231, 231, 231, 229, 229, 229, 227, 227, 227, 225, 225, 225, 223, 223, 223, 221, 221, 221, 219, 219, 219, 217, 217, 217, 215, 215, 215, 213, 213, 213, 211, 211, 211, 209, 209, 209, 207, 207, 207, 205, 205, 205, 203, 203, 203, 201, 201, 201, 199, 199, 199, 197, 197, 197, 195, 195, 195, 193, 193, 193, 191, 191, 191, 189, 189, 189, 187, 187, 187, 185, 185, 185, 183, 183, 183, 181, 181, 181, 179, 179, 179, 177, 177, 177, 175, 175, 175, 173, 173, 173, 171, 171, 171, 169, 169, 169, 167, 167, 167, 165, 165, 165, 163, 163, 163, 161, 161, 161, 159, 159, 159, 157, 157, 157, 155, 155, 155, 153, 153, 153, 151, 151, 151, 149, 149, 149, 147, 147, 147, 145, 145, 145, 143, 143, 143, 141, 141, 141, 139, 139, 139, 137, 137, 137, 135, 135, 135, 133, 133, 133, 131, 131, 131, 129, 129, 129, 126, 126, 126, 124, 124, 124, 122, 122, 122, 120, 120, 120, 118, 118, 118, 116, 116, 116, 114, 114, 114, 112, 112, 112, 110, 110, 110, 108, 108, 108, 106, 106, 106, 104, 104, 104, 102, 102, 102, 100, 100, 100, 98, 98, 98, 96, 96, 96, 94, 94, 94, 92, 92, 92, 90, 90, 90, 88, 88, 88, 86, 86, 86, 84, 84, 84, 82, 82, 82, 80, 80, 80, 78, 78, 78, 76, 76, 76, 74, 74, 74, 72, 72, 72, 70, 70, 70, 68, 68, 68, 66, 66, 66, 64, 64, 64, 62, 62, 62, 60, 60, 60, 58, 58, 58, 56, 56, 56, 54, 54, 54, 52, 52, 52, 50, 50, 50, 48, 48, 48, 46, 46, 46, 44, 44, 44, 42, 42, 42, 40, 40, 40, 38, 38, 38, 36, 36, 36, 34, 34, 34, 32, 32, 32, 30, 30, 30, 28, 28, 28, 26, 26, 26, 24, 24, 24, 22, 22, 22, 20, 20, 20, 18, 18, 18, 16, 16, 16, 14, 14, 14, 12, 12, 12, 10, 10, 10, 8, 8, 8, 6, 6, 6, 4, 4, 4, 2, 2, 2, 0, 0, 0, 0, 0, 9, 2, 0, 16, 4, 0, 24, 6, 0, 31, 8, 0, 38, 10, 0, 45, 12, 0, 53, 14, 0, 60, 17, 0, 67, 19, 0, 74, 21, 0, 82, 23, 0, 89, 25, 0, 96, 27, 0, 103, 29, 0, 111, 31, 0, 118, 36, 0, 120, 41, 0, 121, 46, 0, 122, 51, 0, 123, 56, 0, 124, 61, 0, 125, 66, 0, 126, 71, 0, 127, 76, 1, 128, 81, 1, 129, 86, 1, 130, 91, 1, 131, 96, 1, 132, 101, 1, 133, 106, 1, 134, 111, 1, 135, 116, 1, 136, 121, 1, 136, 125, 2, 137, 130, 2, 137, 135, 3, 137, 139, 3, 138, 144, 3, 138, 149, 4, 138, 153, 4, 139, 158, 5, 139, 163, 5, 139, 167, 5, 140, 172, 6, 140, 177, 6, 140, 181, 7, 141, 186, 7, 141, 189, 10, 137, 191, 13, 132, 194, 16, 127, 196, 19, 121, 198, 22, 116, 200, 25, 111, 203, 28, 106, 205, 31, 101, 207, 34, 95, 209, 37, 90, 212, 40, 85, 214, 43, 80, 216, 46, 75, 218, 49, 69, 221, 52, 64, 223, 55, 59, 224, 57, 49, 225, 60, 47, 226, 64, 44, 227, 67, 42, 228, 71, 39, 229, 74, 37, 230, 78, 34, 231, 81, 32, 231, 85, 29, 232, 88, 27, 233, 92, 24, 234, 95, 22, 235, 99, 19, 236, 102, 17, 237, 106, 14, 238, 109, 12, 239, 112, 12, 240, 116, 12, 240, 119, 12, 241, 123, 12, 241, 127, 12, 242, 130, 12, 242, 134, 12, 243, 138, 12, 243, 141, 13, 244, 145, 13, 244, 149, 13, 245, 152, 13, 245, 156, 13, 246, 160, 13, 246, 163, 13, 247, 167, 13, 247, 171, 13, 248, 175, 14, 248, 178, 15, 249, 182, 16, 249, 185, 18, 250, 189, 19, 250, 192, 20, 251, 196, 21, 251, 199, 22, 252, 203, 23, 252, 206, 24, 253, 210, 25, 253, 213, 27, 254, 217, 28, 254, 220, 29, 255, 224, 30, 255, 227, 39, 255, 229, 53, 255, 231, 67, 255, 233, 81, 255, 234, 95, 255, 236, 109, 255, 238, 123, 255, 240, 137, 255, 242, 151, 255, 244, 165, 255, 246, 179, 255, 248, 193, 255, 249, 207, 255, 251, 221, 255, 253, 235, 255, 255, 24,
0};
int colorMode = 0;

unsigned long startTime;
unsigned long lastMillis;
unsigned long frameCount;
unsigned int framesPerSecond;
unsigned long lastRenderTime = 0;

const boolean isDebug = true;

const int UI_VIDEO_VIEW   = 11000;
const int UI_SETTING_VIEW = 12000;

int uiState = UI_VIDEO_VIEW;
boolean needToInvalidate = true;

unsigned long lastTouchCheckTime = 0;
uint16_t touchX = -1, touchY = -1;

const int SCALE_MODE_NNI = 1001; // Nearest Neighbor Interpolation
const int SCALE_MODE_BLI = 1002; // Biliniear Interpolation
int scaleMode = SCALE_MODE_BLI;

const int AUTO_TEMP_MIN = 300 * 100; // 26'C == 300K
const int AUTO_TEMP_MAX = 310 * 100; // 37'C == 310K
boolean autoTemp = false;

void setup() {
  Serial.begin(115200);

  setupLepton();
  setupTft();
}

void setupLepton() {
  lepton.begin();
  
  uint16_t serialNumber[4];
  lepton.doGetCommand(Lepton::CMD_SYS_FLIR_SERIAL_NUMBER, serialNumber);
  if (isDebug) {
    Serial.printf("FLIR Serial Number: %4x %4x %4x %4x\n", serialNumber[0], serialNumber[1], serialNumber[2], serialNumber[3]);
  }
  
  lepton.syncFrame();
  if (isDebug) {
    Serial.println("sync done");
  }
}

void setupTft() {
  tft.init();

  tft.setRotation(1);

  videoSprite.setColorDepth(8);
  videoSprite.createSprite(
    leptonHeight * videoScale,
    leptonWidth * videoScale
  );
  videoSprite.fillSprite(TFT_BLUE);
}

void loop() {
  startTime = millis();

  if (uiState == UI_VIDEO_VIEW && !lepton.readFrame(frameData)) {
    Serial.println("skip");
    return;
  } else {
    invalidateVideo();
  }

  static uint16_t color;

  // in video view, check touch once per second. it costs lot.
  if (uiState != UI_VIDEO_VIEW || millis() - lastTouchCheckTime >= 3000) {
    if (!tft.getTouch(&touchX, &touchY)) {
      touchX = -1;
      touchY = -1;
    }
    lastTouchCheckTime = millis();
  }
  render();

  handleTouch();

  measureFrame();
}

void invalidateVideo() {
  if (uiState != UI_VIDEO_VIEW) return;
  
  uint16_t avg = 0;
  uint16_t minValue = 65535;
  uint16_t maxValue = 0;

  if (autoTemp) {
    for (int i = 0; i < leptonWidth; i++){
      for(int j = 0; j < leptonHeight; j++){
        uint16_t pixelData = frameData[i * leptonHeight + j];
        avg += (double(pixelData) / (leptonWidth * leptonHeight));
        
        if (pixelData !=0) {
          if (minValue > pixelData) {
            minValue = pixelData;
          }
          if (maxValue < pixelData) {
            maxValue = pixelData;
          }
        }
      }
    }
  } else {
    minValue = AUTO_TEMP_MIN;
    maxValue = AUTO_TEMP_MAX;
  }

  double scale = 0;
  if (maxValue != 0 && minValue != 0) {
    scale = 255.0 / (maxValue - minValue);
  } else {
    scale = 1.0;
  }
  
  if (isDebug) {
    Serial.print("avg : ");
    Serial.println(avg);
    Serial.print("maxValue : ");
    Serial.println(maxValue);
    Serial.print("minValue : ");
    Serial.println(minValue);
    Serial.print("scale : ");
    Serial.println(scale);
  }
  
  unsigned long sum2 = 0;
  
  for (int i = 0; i < leptonWidth; i++) {
    for (int j = 0; j < leptonHeight; j++) {
       uint16_t pixelData = ((frameData[i * leptonHeight + j] ) - minValue) * scale;

      if (pixelData < 0) {
        pixelData = 0;
      } else if (pixelData > 255) {
        pixelData = 255;
      }
      sum2 += pixelData;

      for(int k = 0; k < videoScale; k++) {
        for(int l = 0; l < videoScale; l++) {
          renderData[i * videoScale + k][j * videoScale + l] = pixelData;
        }
      }
    }
  }

  if (scaleMode == SCALE_MODE_BLI) {
    int frameLength = videoScale * 2;
    for (int i = 0; i < leptonWidth * videoScale; i+=(frameLength - 1)) {
      for (int j = 0; j < leptonHeight * videoScale; j+=(frameLength - 1)) {

        uint16_t originValues[4] = {
          renderData[i][j],
          renderData[i][j+(frameLength - 1)],
          renderData[i+(frameLength - 1)][j+(frameLength - 1)],
          renderData[i+(frameLength - 1)][j+(frameLength - 1)],
        };
        for (int k = 0; k < frameLength; k++) {
          for (int l = 0; l < frameLength; l++) {
            if (i * videoScale + k >= leptonWidth * videoScale || j * videoScale + l >= leptonHeight * videoScale) continue;
            
            renderData[i + k][j + l] = (
              originValues[0] * ((frameLength - k) * (frameLength - l)) +
              originValues[1] * (k * (frameLength - l)) +
              originValues[2] * ((frameLength - k) * l) +
              originValues[3] * (k * l)
            ) / ((frameLength) * (frameLength));
          }
        }
        
      }
    }
  }

  if (isDebug) {
    Serial.print("==");
    Serial.print("avg2 : " );
    Serial.println(sum2 / (leptonWidth * leptonHeight));
  }
  needToInvalidate = true;
}

void render() {
  if (needToInvalidate) {
    if (uiState == UI_VIDEO_VIEW) {
      renderThermalVideo();
    } else {
      renderSettingUi();
    }
  }
  
  renderSystemUi();
  
  needToInvalidate = false;
}

void renderThermalVideo() {
  const byte* colorMap;
  if (colorMode == 0) {
    colorMap = colormap_rainbow;
  } else if (colorMode == 1) {
    colorMap = colormap_grayscale;
  } else if (colorMode == 2) {
    colorMap = colormap_ironblack;
  }
  
  for (int i = 0; i < leptonWidth * videoScale; i++) {
    for (int j = 0; j < leptonHeight * videoScale; j++) {
      uint16_t color = tft.color565(
        colorMap[renderData[i][j] * 3 + 0],
        colorMap[renderData[i][j] * 3 + 1],
        colorMap[renderData[i][j] * 3 + 2]
      );
      
      videoSprite.drawPixel(
        j, 
        i, 
        color
      );
    }
  }
  videoSprite.pushSprite(0, 0);
}

void renderSettingUi() {
  tft.fillScreen(TFT_BLUE);
  
  tft.setTextSize(3);
  tft.drawString("settings", 0, 0);
  
  tft.setTextSize(2);
  tft.drawString("rainbow (" + String((colorMode == 0) ? "o" : " ") + ")", 0, 30);
  tft.drawString("grayscale (" + String((colorMode == 1) ? "o" : " ") + ")", 0, 50);
  tft.drawString("ironblack (" + String((colorMode == 2) ? "o" : " ") + ")", 0, 70);

  tft.drawString("auto temp (" + String((autoTemp == true) ? "o" : " ") + ")", 0, 100);
  tft.drawString("manual temp (" + String((autoTemp == false) ? "o" : " ") + ")", 0, 120);
  
  tft.drawString("scale_nni (" + String((scaleMode == SCALE_MODE_NNI) ? "o" : " ") + ")", 0, 150);
  tft.drawString("scale_bli (" + String((scaleMode == SCALE_MODE_BLI) ? "o" : " ") + ")", 0, 170);
  
  tft.setTextSize(3);
  tft.drawString("back", 250, 210);
}

void renderSystemUi() {
  if (isDebug) {
    Serial.print("fps : ");
    Serial.println(framesPerSecond);
  }
  
  tft.setTextSize(1);
  tft.drawString("fps : " + String(framesPerSecond) + " frame : " + String(lastRenderTime) + "ms", 200, 0);
}

void handleTouch() {
  if (touchX == -1 || touchX == 65535 || touchY == -1 || touchY == 65535) return;

  if (uiState == UI_VIDEO_VIEW) {
    uiState = UI_SETTING_VIEW;
  } else {
    handleTouchInSettingUi();
  }

  // after change ui, prevent touch in 1s.
  lastTouchCheckTime = millis() + 1000;

  needToInvalidate = true;
}

void handleTouchInSettingUi() {
  // back button
  if (touchIn(250, 210, 320, 240)) {
    uiState = UI_VIDEO_VIEW;
  }
  // color - rainbow
  else if (touchIn(0, 30, 100, 50)) {
    colorMode = 0;
  }
  // color - grayscale
  else if (touchIn(0, 50, 100, 70)) {
    colorMode = 1;
  }
  // color - ironblack
  else if (touchIn(0, 70, 100, 90)) {
    colorMode = 2;
  }
  // tempMode - enable
  else if (touchIn(0, 100, 100, 120)) {
    autoTemp = true;
  }
  // tempMode = disable
  else if (touchIn(0, 120, 100, 140)) {
    autoTemp = false;
  }
  // scaleMode = nni
  else if (touchIn(0, 150, 100, 170)) {
    scaleMode = SCALE_MODE_NNI;
  }
  // scaleMode = bli
  else if (touchIn(0, 170, 100, 190)) {
    scaleMode = SCALE_MODE_BLI;
  }
}

boolean touchIn(int startX, int startY, int endX, int endY) {
  if (touchX >= startX && touchX <= endX && touchY >= startY && touchY <= endY) {
    return true;
  } else {
    return false;
  }
}

void measureFrame() {
  unsigned long now = millis();
  
  frameCount ++;
  if (now - lastMillis >= 1000) {
    framesPerSecond = frameCount;
    frameCount = 0;
    lastMillis = now;
  }
  
  lastRenderTime = now - startTime;
}
