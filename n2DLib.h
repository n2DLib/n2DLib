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

#ifdef __cplusplus
extern "C" {
#endif

extern Fixed fixcos(Fixed angle);
extern void rotate(int x, int y, Fixed ca, Fixed sa, Rect* out);
extern inline int sq(int);
extern inline Fixed fixsq(Fixed);
extern inline int cube(int);
extern inline Fixed fixcube(Fixed);

extern void initBuffering();
extern void updateScreen();
extern void deinitBuffering();
extern void clearBufferB();
extern void clearBufferW();
extern void clearBuffer(unsigned short);
extern inline unsigned short getPixel(const unsigned short*, unsigned int, unsigned int);
extern inline void setPixelUnsafe(unsigned int, unsigned int, unsigned short);
extern inline void setPixel(unsigned int, unsigned int, unsigned short);
extern inline void setPixelRGB(unsigned int, unsigned int, unsigned char, unsigned char, unsigned char);
extern void fillRect(int, int, int, int, unsigned short);
extern void drawSprite(const unsigned short*, int, int);
extern void drawSpritePart(const unsigned short*, int, int, const Rect*);
extern void drawSpriteRotated(const unsigned short*, const Rect*, Fixed);
extern void drawLine(int, int, int, int, uint8_t, uint8_t, uint8_t);
extern void drawPolygon(uint8_t, uint8_t, uint8_t, int, ...);
extern void fillCircle(int, int, int, uint8_t, uint8_t, uint8_t);
extern void fillEllipse(int, int, int, int, uint8_t, uint8_t, uint8_t);
extern void drawString(int*, int*, int, const char*, unsigned short);
extern void drawDecimal(int*, int*, int, unsigned short);
extern void drawChar(int*, int*, int, char, unsigned short);
extern void drawStringF(int*, int*, int, unsigned short, const char*, ...);

#define BUFF_BYTES_SIZE (320*240*2)
extern unsigned short *BUFF_BASE_ADDRESS;
#ifdef __cplusplus
}
#endif

#endif
