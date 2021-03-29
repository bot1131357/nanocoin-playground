// Display.h

#ifndef _DISPLAY_h
#define _DISPLAY_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

typedef enum
{
	RIGHT_ALIGNMENT = 0,
	LEFT_ALIGNMENT,
	CENTER_ALIGNMENT,
} Text_alignment;

namespace EINK
{
	void init(void);
	void print(const String& str, int16_t x, int16_t y, uint8_t alignment, uint8_t size = 0);
	void drawQR(const char* str, int16_t x, int16_t y);
	void drawFrame(int16_t edge);
	void drawBitmap(const char* filename, int16_t x, int16_t y, bool with_color);
	void update();
	void clear();
}

#endif

