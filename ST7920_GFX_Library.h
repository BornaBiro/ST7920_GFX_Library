#if ARDUINO >= 100
 #include "Arduino.h"
 #define WIRE_WRITE Wire.write
#else
 #include "WProgram.h"
  #define WIRE_WRITE Wire.send
#endif

#include <SPI.h>
#include <Adafruit_GFX.h>

#define ST7920_HEIGHT 	64		//64 pixels tall display
#define ST7920_WIDTH	128		//128 pixels wide display

#define BLACK 1					//Defines color - Black color -> Bit in buffer is set to one
#define WHITE 0					//Defines color - White color -> Bit in buffer is set to zero

class ST7920 : public Adafruit_GFX {
 public:
 	ST7920(int8_t CS);

  	void begin(void);
  	void clearDisplay(void);
  	void invertDisplay(void);
  	void display();

  	void drawPixel(int16_t x, int16_t y, uint16_t color);

 private:
 	int8_t cs;
 	void ST7920Data(uint8_t data);
  	void ST7920Command(uint8_t data);

};


