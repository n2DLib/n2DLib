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

extern Fixed fixcos(Fixed angle);
extern void rotate(int x, int y, Fixed ca, Fixed sa, Rect* out);

extern int currentBuf;
extern unsigned short *doubleBuf[2];

extern int initDoubleBuff();
extern void switchBuffers();
extern void switchBuffersRestore();
extern void deinitDoubleBuff();
extern inline void clearBuffer();
extern inline void setPixel(int, int, unsigned short);
extern inline void setPixelRGB(int, int, unsigned char, unsigned char, unsigned char);
extern void drawSprite(unsigned short*, int, int);
extern void drawSpritePart(unsigned short*, int, int, Rect);
extern void drawSpriteRotated(unsigned short*, Rect, Fixed);
void drawLine(int, int, int, int, uint8_t, uint8_t, uint8_t)
extern void drawPolygon(uint8_t, uint8_t, uint8_t, int, ...);

#define BUFF_BASE_ADDRESS doubleBuf[currentBuf]
#define BUFF_BYTES_SIZE (320*240*2)

#endif
