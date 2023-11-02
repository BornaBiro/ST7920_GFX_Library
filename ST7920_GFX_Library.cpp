#include <avr/pgmspace.h>
#include <stdlib.h>

#include "Adafruit_GFX.h"
#include "ST7920_GFX_Library.h"

uint8_t buff[1024] __attribute__ ((aligned (4)));		//This array serves as primitive "Video RAM" buffer.  Ensure it's 32-bit aligned.

//This display is split into two halfs. Pages are 16bit long and pages are arranged in that way that are lied horizontaly instead of verticaly, unlike SSD1306 OLED, Nokia 5110 LCD, etc.
//After 8 horizonral page is written, it jumps to half of the screen (Y = 32) and continues until 16 lines of page have been written. After that, we have set cursor in new line.
void ST7920::drawPixel(int16_t x, int16_t y, uint16_t color) {
	if(x<0 || x>=ST7920_WIDTH || y<0 || y>=ST7920_HEIGHT) return;
  	uint8_t y0 = 0, x0 = 0;								//Define and initilize varilables for skiping rows
  	uint16_t data, n;										//Define variable for sending data itno buffer (basicly, that is one line of page)
  	if (y > 31) {											//If Y coordinate is bigger than 31, that means we have to skip into that row, but we have to do that by adding 
    	y -= 32;
    	y0 = 16;
  	}
  	x0 = x % 16;
  	x /= 16;
  	data = 0x8000 >> x0;
  	n = (x * 2) + (y0) + (32 * y);
  	if (!color) {
    	buff[n] &= (~data >> 8);
    	buff[n + 1] &= (~data & 0xFF);
  	}else{
    	buff[n] |= (data >> 8);
    	buff[n + 1] |= (data & 0xFF);
  	}
}

ST7920::ST7920(int8_t CS) : Adafruit_GFX(ST7920_WIDTH, ST7920_HEIGHT) {
  	cs = CS;
}

void ST7920::begin(void) {
	SPI.begin();
	pinMode(cs, OUTPUT);
  	digitalWrite(cs, HIGH);
	ST7920Command(B00001100);
  	digitalWrite(cs, LOW);
}

void ST7920::clearDisplay() {
  	uint32_t* p = (uint32_t*)&buff;
  	for (int i = 0; i < 256; i++) {
    	p[i] = 0;
  	}
}

void ST7920::display() {
  	int x = 0, y = 0, n = 0;
  	digitalWrite(cs, HIGH);
  	ST7920Command(B00100100); //EXTENDED INSTRUCTION SET
  	ST7920Command(B00100110); //EXTENDED INSTRUCTION SET
  	for (y = 0; y < 32; y++) {
    	ST7920Command(0x80 | y);
    	ST7920Command(0x80 | x);
    	for (x = 0; x < 16; x++) {
      		ST7920Data(buff[n]);
      		ST7920Data(buff[n + 1]);
      		n += 2;
    	}
  	}
  	digitalWrite(cs, LOW);
}

void ST7920::invertDisplay() {
  	uint32_t* p = (uint32_t*)&buff;
  	for(int i = 0; i<256; i++) {
    	p[i] = ~p[i];
  	}
}

void ST7920::ST7920Data(uint8_t data) { //RS = 1 RW = 0
  	SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE3));
  	SPI.transfer(B11111010);
  	SPI.transfer((data & B11110000));
 	SPI.transfer((data & B00001111) << 4);
  	SPI.endTransaction();
  	delayMicroseconds(38);
}

void ST7920::ST7920Command(uint8_t data) { //RS = 0 RW = 0
  	SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE3));
  	SPI.transfer(B11111000);
  	SPI.transfer((data & B11110000));
  	SPI.transfer((data & B00001111) << 4);
  	SPI.endTransaction();
  	delayMicroseconds(38);
}
