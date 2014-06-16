#ifndef INCLUDE_GRAFX
#define INCLUDE_GRAFX

#include <os.h>

typedef int Fixed;
typedef struct
{
	int x;
	int y;
	int h;
	int w;
} Rect;

#define itofix(x) ((x) << 8)
#define fixtoi(x) ((x) >> 8)
#define fixmul(x, y) ((x)*  (y) >> 8)
#define fixdiv(x, y) (((x) << 8) / (y))

#define fixsin(x) fixcos((x) - 64)

extern "C" {

extern Fixed fixcos(Fixed angle);
extern void rotate(int x, int y, Fixed ca, Fixed sa, Rect* out);

extern int currentBuf;
extern unsigned short *doubleBuf[2];

extern int initBuffering();
extern void updateScreen();
extern void deinitBuffering();
extern void clearBuffer(unsigned short);
#define clearBufferW() memset(BUFF_BASE_ADDRESS, 0xff, BUFF_BYTES_SIZE)
#define clearBufferB() memset(BUFF_BASE_ADDRESS, 0, BUFF_BYTES_SIZE)
extern inline void setPixelUnsafe(unsigned int, unsigned int, unsigned short);
extern inline void setPixel(unsigned int, unsigned int, unsigned short);
extern inline void setPixelRGB(unsigned int, unsigned int, unsigned char, unsigned char, unsigned char);
extern void drawSprite(unsigned short*, int, int);
extern void drawSpritePart(unsigned short*, int, int, Rect);
extern void drawSpriteRotated(unsigned short*, Rect, Fixed);
extern void drawLine(int, int, int, int, uint8_t, uint8_t, uint8_t);
extern void drawPolygon(uint8_t, uint8_t, uint8_t, int, ...);
extern void fillCircle(int, int, int, uint8_t, uint8_t, uint8_t);
extern void fillEllipse(int, int, int, int, uint8_t, uint8_t, uint8_t);

#define BUFF_BYTES_SIZE (320*240*2)
#define ScreenBuffer unsigned short
extern ScreenBuffer* BUFF_BASE_ADDRESS;
}

#endif
