n2DLib documentation
====================

n2DLib is a C graphics library for the TI-Nspire calculators intended to be used with the Ndless 3rd-party jailbreak.

The lib uses buffering to display smoothly ; that means that all drawing is done to a separate buffer in memory which is then copied to the screen.

All colors, unless specified, must be in R5G6B5 format, meaning a 16-bits number organized this way :

Bit   | 15 14 13 12 11 10 9  8  7  6  5  4  3  2  1  0
Color | R  R  R  R  R  G  G  G  G  G  G  B  B  B  B  B

n2DLib uses a variation on the NTI image format to store images. All images must be arrays of unsigned short organized this way :

Entry |       0        |       1        |       2         |   3-...  |
Use   | Width in pixels|Height in pixels|Transparent color|Image data|

n2DLib also offers the Rect structure that has four members : x, y, w and h.

About fixed-point numbers
=========================

In addition to graphical commands, n2DLib offers a fast alternative to floating-point numbers that is 24.8 fixed-point numbers. Those numbers are integers whose bits 0-7 are treated like a decimal part, and whose bits 8-24 are treated like an integer part. That means 256 is actually 1.0 when used with the appropriate commands. FIxed-point angles are written in binary angles, meaning a full period is [0, 256].

Since fixed-point numbers are special numbers, they can't interact normally with integers. The following array describes what you can and can't do normally :

           |integer|fixed-point|
integer    |  +-*/ |     *     |
fixed-point|   */  |    +-     |

You need a special routine to :
- add an integer to a fixed-point number
- divide an integer by a fixed-point number
- multiply a fixed-point number by another fixed-point number
- divide a fixed-point number by another fixed-point number

See the "Math routines" part of the doc to find how to do all of this.

Summary of commands
===================

==================
GRAPHICAL ROUTINES
==================

-- void clearBufferB()

Fills the buffer with black color.

-- void clearBufferW()

Fills the buffer with white color.

-- void clearBuffer(unsigned short c)

Fills the buffer with the given color.

-- unsigned short getPixel(unsigned short *src, unsigned int x, unsigned int y)

Returns the color of a given pixel in an image, or the transparent color of this image if the coordinates given are out-of-bounds.

-- void deinitBuffering()

Frees memory used by the buffering functionalities. Call this as the very last instruction of your program (excepting return).

-- void drawLine(int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b)

Draws a line between two points of the given 3-components color.

-- void drawPolygon(uint8_t r, uint8_t g, uint8_t b, int pointsNb, ...)

Draws a wired polygon of the given 3-components color using a list of pointsNb coordinates (first x, then y).

-- void drawSprite(unsigned short *src, unsigned int _x, unsigned int _y)

Draws an image to the given coordinates.

-- void drawSpritePart(unsigned short *src, unsigned int _x, unsigned int _y, Rect* part)

Draws part of an image to the given coordinates. The routine draws what of the sprite is in the rectangle given by Rect* part.

-- void drawSpriteRotated(unsigned short* source, Rect* sr, Fixed angle)

Draws a sprite rotated by a given angle at the coordinates given by Rect* sr.

-- void fillCircle(int x, int y, int radius, uint8_t r, uint8_t g, uint8_t b)

Fills a circle of the given 3-components color.

-- void fillEllipse(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b)

Fills an ellipse of the given size with the given 3-components color.

-- void initBuffering()

Initializes the buffering functionalities. Call this as the very first instruction of your program if you want to use n2DLib's buffering.

-- void setPixel(unsigned int x, unsigned int y, unsigned short c)

Sets a pixel to the given color after verifying the pixel is actually in the screen's dimensions.

-- void setPixelRGB(unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b)

Sets a pixel to the given color after verifying the pixel is actually in the screen's dimensions and using three color components.

-- void setPixelUnsafe(unsigned int x, unsigned int y, unsigned short c)

Sets a pixel to the given color, but does not make sure the pixel is in the screen's dimensions. Faster than setPixel, but use only if you know you can't draw out of the screen.

-- void updateScreen()

Copies the content of the buffer to the screen. This does not clear the buffer.

=============
TEXT ROUTINES
=============

-- void drawString(int x, int y, const char *str, unsigned short c)

Draws a string at the given position with the given color using n2DLib's built-in font. Does clipping and supports newlines with \n.

=============
MATH ROUTINES
=============

-- Fixed fixcos(Fixed angle)

Returns the cosinus of a binary angle in fixed-point format.

-- Fixed fixdiv(Fixed a, Fixed b)

Performs a division between two fixed-point numbers.

-- Fixed fixmul(Fixed a, Fixed b)

Performs a multiplication between two fixed-point numbers.

-- Fixed fixsin(Fixed angle)

Returns the sinus of a binary angle in fixed-point format.

-- int fixtoi(Fixed f)

Turns a fixed-point number into an integer.

-- Fixed itofix(int i)

Turns an integer into a fixed-point number.

-- void rotate(int x, int y, Fixed ca, Fixed sa, Rect* out)

Rotates a point on a 2D plane given ca the cosinus of the rotation angle and sa the sinus of the rotation angle. This takes x and y as separate arguments and writes the two resulting coordinates to Rect* out.