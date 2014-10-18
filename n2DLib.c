#include "n2DLib.h"
#include "n2DLib_font.h"

#ifdef __cplusplus
extern "C" {
#endif

/*             *
 *  Buffering  *
 *             */

unsigned short *BUFF_BASE_ADDRESS, *ALT_SCREEN_BASE_ADDRESS, *INV_BUFF;
void *SCREEN_BACKUP;

void initBuffering()
{
	BUFF_BASE_ADDRESS = (unsigned short*)malloc(BUFF_BYTES_SIZE);
	if(!BUFF_BASE_ADDRESS) exit(0);
	
	SCREEN_BACKUP = *(void**)0xC0000010;
	
	// Handle monochrome screens-specific shit
	if(is_classic)
		*(int32_t*)(0xC000001C) = (*((int32_t*)0xC000001C) & ~0x0e) | 0x08;
	
	ALT_SCREEN_BASE_ADDRESS = (unsigned short*)malloc(BUFF_BYTES_SIZE);
	if(!ALT_SCREEN_BASE_ADDRESS)
	{
		free(BUFF_BASE_ADDRESS);
		*((int32_t*)0xC000001C) = (*((int32_t*)0xC000001C) & ~0x0e) | 0x04;
		*(void**)0xC0000010 = SCREEN_BACKUP;
		exit(0);
	}
	INV_BUFF = (unsigned short*)malloc(BUFF_BYTES_SIZE);
	if(!INV_BUFF)
	{
		free(ALT_SCREEN_BASE_ADDRESS);
		free(BUFF_BASE_ADDRESS);
		*((int32_t*)0xC000001C) = (*((int32_t*)0xC000001C) & ~0x0e) | 0x04;
		*(void**)0xC0000010 = SCREEN_BACKUP;
		exit(0);
	}
	
	*(void**)0xC0000010 = ALT_SCREEN_BASE_ADDRESS;
}

void updateScreen()
{
	unsigned int *dest, *src, i, c;
	if(has_colors)
	{
		// Screen-access delays make this the fastest method
		memcpy(ALT_SCREEN_BASE_ADDRESS, BUFF_BASE_ADDRESS, BUFF_BYTES_SIZE);
	}
	else
	{
		dest = (unsigned int*)INV_BUFF;
		src = (unsigned int*)BUFF_BASE_ADDRESS;
		for(i = 0; i < 160 * 240; i++)
		{
			c = *src++;
			// c holds two 16-bits colors, decompose them while keeping them that way
			c = ~c;
			*dest++ = (( ((c & 0x1f) << 3) + (((c >> 5) & 0x3f) << 2) + (((c >> 11) & 0x1f) << 3) ) & 0xffff) + 
				((( (((c >> 16) & 0x1f) << 3) + (((c >> 21) & 0x3f) << 2) + (((c >> 27) & 0x1f) << 3)) & 0xffff) << 16);
		}
		memcpy(ALT_SCREEN_BASE_ADDRESS, INV_BUFF, BUFF_BYTES_SIZE);
	}
}

void deinitBuffering()
{
	// Handle monochrome screens-specific shit again
	if(is_classic)
		*((int32_t*)0xC000001C) = (*((int32_t*)0xC000001C) & ~0x0e) | 0x04;
	*(void**)(0xC0000010) = SCREEN_BACKUP;
	free(INV_BUFF);
	free(ALT_SCREEN_BASE_ADDRESS);
	free(BUFF_BASE_ADDRESS);
}

/*         *
 *  Maths  *
 *         */

Fixed fixcos(Fixed angle)
{
	static Fixed cosLUT[] = { 256, 255, 255, 255, 254, 254, 253, 252, 251, 249, 248, 246, 244, 243, 241, 238, 236, 234, 231, 228, 225, 222, 219, 216, 212, 209, 205, 201, 197, 193, 189, 185, 181, 176, 171, 167, 162, 157, 152, 147, 142, 136, 131, 126, 120, 115, 109, 103, 97, 92, 86, 80, 74, 68, 62, 56, 49, 43, 37, 31, 25, 18, 12, 6, 0, -6, -12, -18, -25, -31, -37, -43, -49, -56, -62, -68, -74, -80, -86, -92, -97, -103, -109, -115, -120, -126, -131, -136, -142, -147, -152, -157, -162, -167, -171, -176, -181, -185, -189, -193, -197, -201, -205, -209, -212, -216, -219, -222, -225, -228, -231, -234, -236, -238, -241, -243, -244, -246, -248, -249, -251, -252, -253, -254, -254, -255, -255, -255, -256, -255, -255, -255, -254, -254, -253, -252, -251, -249, -248, -246, -244, -243, -241, -238, -236, -234, -231, -228, -225, -222, -219, -216, -212, -209, -205, -201, -197, -193, -189, -185, -181, -176, -171, -167, -162, -157, -152, -147, -142, -136, -131, -126, -120, -115, -109, -103, -97, -92, -86, -80, -74, -68, -62, -56, -49, -43, -37, -31, -25, -18, -12, -6, 0, 6, 12, 18, 25, 31, 37, 43, 49, 56, 62, 68, 74, 80, 86, 92, 97, 103, 109, 115, 120, 126, 131, 136, 142, 147, 152, 157, 162, 167, 171, 176, 181, 185, 189, 193, 197, 201, 205, 209, 212, 216, 219, 222, 225, 228, 231, 234, 236, 238, 241, 243, 244, 246, 248, 249, 251, 252, 253, 254, 254, 255, 255, 255 };
	return cosLUT[angle & 0xff];
}

void rotate(int x, int y, Fixed ca, Fixed sa, Rect* out)
{
	out->x = fixtoi(fixmul(itofix(x), ca) + fixmul(itofix(y), sa));
	out->y = fixtoi(fixmul(itofix(x), -sa) + fixmul(itofix(y), ca));
}

inline int sq(int x)
{
	return x * x;
}

inline Fixed fixsq(Fixed x)
{
	return fixmul(x, x);
}

inline int cube(int x)
{
	return x * x * x;
}

inline Fixed fixcube(Fixed x)
{
	return fixmul(fixmul(x, x), x);
}

/*            *
 *  Graphics  *
 *            */

void clearBufferB()
{
	int i;
	for(i = 0; i < 160 * 240; i++)
		((unsigned int*)BUFF_BASE_ADDRESS)[i] = 0;
}

void clearBufferW()
{
	int i;
	for(i = 0; i < 160 * 240; i++)
		((unsigned int*)BUFF_BASE_ADDRESS)[i] = 0xffffffff;
}

void clearBuffer(unsigned short c)
{
	int i;
	for(i = 0; i < BUFF_BYTES_SIZE / 2; i++)
			*((unsigned short*)BUFF_BASE_ADDRESS + i) = c;
}

inline unsigned short getPixel(const unsigned short *src, unsigned int x, unsigned int y)
{
	if(x < src[0] && y < src[1])
		return src[x + y * src[0] + 3];
	else
		return src[2];
}

inline void setPixelUnsafe(unsigned int x, unsigned int y, unsigned short c)
{
	*((unsigned short*)BUFF_BASE_ADDRESS + x + y * 320) = c;
}

inline void setPixel(unsigned int x, unsigned int y, unsigned short c)
{
	if(x < 320 && y < 240)
		*((unsigned short*)BUFF_BASE_ADDRESS + x + y * 320) = c;
}

inline void setPixelRGB(unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b)
{
	if(x < 320 && y < 240)
		*((unsigned short*)BUFF_BASE_ADDRESS + x + y * 320) = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
}

void drawHLine(int y, int x1, int x2, unsigned short c)
{
	unsigned int _x1, _x2;
	if((x1 & x2) >> 31 || x1 + x2 >= 640 || (unsigned)y > 239)
	{
		return;
	}
	
	if(x1 < x2)
	{
		_x1 = max(x1, 0);
		_x2 = min(x2, 319);
	}
	else
	{
		_x1 = max(x2, 0);
		_x2 = min(x1, 319);
	}
	for(; _x1 <= _x2; _x1++)
		setPixelUnsafe(_x1, y, c);
}

void drawVLine(int x, int y1, int y2, unsigned short c)
{
	unsigned int _y1, _y2;
	if((y1 & y2) >> 31 || y1 + y2 >= 480 || (unsigned)x > 319)
	{
		return;
	}
	
	if(y1 < y2)
	{
		_y1 = max(y1, 0);
		_y2 = min(y2, 239);
	}
	else
	{
		_y1 = max(y2, 0);
		_y2 = min(y1, 239);
	}
	for(; _y1 <= _y2; _y1++)
		setPixelUnsafe(x, _y1, c);
}

void fillRect(int x, int y, int w, int h, unsigned short c)
{
	unsigned int _x = max(x, 0), _y = max(y, 0), _w = min(320 - _x, w - _x + x), _h = min(240 - _y, h - _y + y), i, j;
	if(_x < 320 && _y < 240)
	{
		for(j = _y; j < _y + _h; j++)
			for(i = _x; i < _x + _w; i++)
				setPixelUnsafe(i, j, c);
	}
}

void drawSprite(const unsigned short *src, int _x, int _y)
{
	int x, y, w = src[0] + _x, h = src[1] + _y, c = 3;
	for(y = _y; y < h; y++)
	{
		for(x = _x; x < w; x++, c++)
		{
			if(src[c] != src[2])
				setPixel(x, y, src[c]);
		}
		if(y > 239) break;
	}
}

void drawSpritePart(const unsigned short *src, int _x, int _y, const Rect* part)
{
	unsigned short c;
	int x, y, w = part->w + _x, h = part->h + _y, z = part->x, t = part->y;
	for(y = _y; y < h; y++, t++)
	{
		for(x = _x, z = part->x; x < w; x++, z++)
		{
			c = getPixel(src, z, t);
			if(c != src[2])
				setPixel(x, y, c);
			if(x > 319) break;
		}
		if(y > 239) break;
	}
}

void drawSpriteScaled(const unsigned short* source, const Rect* info)
{
	Fixed dx = itofix(source[0]) / info->w;
	Fixed dy = itofix(source[1]) / info->h;
	int x, y, _x = info->x + info->w / 2, _y = info->y + info->h / 2;
	Fixed tx = 0, ty = 0;
	unsigned short c;
	
	for(y = info->y - info->h / 2; y < _y; y++, ty += dy)
	{
		for(x = info->x - info->w / 2, tx = 0; x < _x; x++, tx += dx)
		{
			c = getPixel(source, fixtoi(tx), fixtoi(ty));
			if(c != source[2])
				setPixel(x, y, c);
			if(x > 319) break;
		}
		if(y > 239) break;
	}
}

void drawSpriteRotated(const unsigned short* source, const Rect* sr, const Rect* rc, Fixed angle)
{
	Rect* defaultRect = NULL;
	Rect upleft, upright, downleft, downright;
	Rect fr;
	unsigned short currentPixel;
	Fixed dX = fixcos(angle), dY = fixsin(angle);
	
	if(rc == NULL)
	{
		defaultRect->x = source[0] / 2;
		defaultRect->y = source[1] / 2;
		rc = defaultRect;
	}
	
	rotate(-rc->x, -rc->y, dX, dY, &upleft);
	rotate(source[0] - rc->x, -rc->y, dX, dY, &upright);
	rotate(-rc->x, source[1] - rc->y, dX, dY, &downleft);
	rotate(source[0] - rc->x, source[1] - rc->y, dX, dY, &downright);
	
	fr.x = min(min(min(upleft.x, upright.x), downleft.x), downright.x) + sr->x;
	fr.y = min(min(min(upleft.y, upright.y), downleft.y), downright.y) + sr->y;
	fr.w = max(max(max(upleft.x, upright.x), downleft.x), downright.x) + sr->x;
	fr.h = max(max(max(upleft.y, upright.y), downleft.y), downright.y) + sr->y;
	
	Rect cp, lsp, cdrp;
	
	lsp.x = fixmul(itofix(fr.x - sr->x), dX) + fixmul(itofix(fr.y - sr->y), -dY);
	lsp.y = fixmul(itofix(fr.x - sr->x), dY) + fixmul(itofix(fr.y - sr->y), dX);
	
	for(cp.y = fr.y; cp.y <= fr.h; cp.y++)
	{
		cdrp.x = lsp.x;
		cdrp.y = lsp.y;
		
		for(cp.x = fr.x; cp.x <= fr.w; cp.x++)
		{
			if(cp.x >= 0 && cp.x < 320 && cp.y >= 0 && cp.y < 240)
			{
				//~ if(abs(fixtoi(cdrp.x)) <= source[0] / 2 && abs(fixtoi(cdrp.y)) <= source[1] / 2)
				//~ {
					currentPixel = getPixel(source, fixtoi(cdrp.x) + rc->x, fixtoi(cdrp.y) + rc->y);
					if(currentPixel != source[2])
					{
						setPixelUnsafe(cp.x, cp.y, currentPixel);
					}
				//~ }
			}
			cdrp.x += dX;
			cdrp.y += dY;
		}
		
		lsp.x -= dY;
		lsp.y += dX;
	}
}

/*            *
 *  Geometry  *
 *            */
 
void drawLine(int x1, int y1, int x2, int y2, unsigned short c)
{
	int dx = abs(x2-x1);
	int dy = abs(y2-y1);
	int sx = (x1 < x2)?1:-1;
	int sy = (y1 < y2)?1:-1;
	int err = dx-dy;
	int e2;

	while (!(x1 == x2 && y1 == y2))
	{
		setPixel(x1,y1,c);
		e2 = 2*err;
		if (e2 > -dy)
		{		 
			err = err - dy;
			x1 = x1 + sx;
		}
		if (e2 < dx)
		{		 
			err = err + dx;
			y1 = y1 + sy;
		}
	}
}

void drawPolygon(unsigned short c, int nombreDePoints, ...)
// r, g, b, <number of points you want (4 for a square, for instance, not 8 because of x and y...)>, <x1,y1,x2,y2...>
{
	// the number of arguments in the <...> must be even
	int i;
	int* pointsList = (int*)malloc(nombreDePoints*2*sizeof(int));
	
	if (!pointsList) return;
	
	va_list ap;
	int cur_arg = 1;

	va_start(ap, nombreDePoints);
	
	for (i = 0; i < nombreDePoints*2; i++)
	{
		cur_arg = va_arg(ap, int);
		*(pointsList + i) = cur_arg;
	}
	
	for (i = 0; i < nombreDePoints*2 - 2; i+=2)
	{
		drawLine(*(pointsList + i), *(pointsList + i + 1), *(pointsList + i + 2), *(pointsList + i + 3), c);
	}
	drawLine(*(pointsList + nombreDePoints*2 - 2), *(pointsList + nombreDePoints*2 - 1), *(pointsList), *(pointsList + 1), c);
	va_end(ap);
	free(pointsList);
}

void fillCircle(int x, int y, int radius, unsigned short c)
{
	int i,j;
	for(j=-radius; j<=radius; j++)
		for(i=-radius; i<=radius; i++)
			if(i*i+j*j <= radius*radius)
				setPixel(x + i, y + j, c);               
}

/*  /!\ for circle and ellispe, the x and y must be the center of the shape, not the top-left point   /!\  */

void fillEllipse(int x, int y, int w, int h, unsigned short c)
{
	int i,j;
	for(j=-h; j<=h; j++)
		for(i=-w; i<=w; i++)
			if(i*i*h*h+j*j*w*w <= h*h*w*w)
				setPixel(x + i, y + j, c);
}

/*        *
 *  Text  *
 *        */

int isOutlinePixel(unsigned char* charfont, int x, int y)
{
	int xis0 = !x, xis7 = x == 7, yis0 = !y, yis7 = y == 7;
	
	if(xis0)
	{
		if(yis0)
		{
			return !(*charfont & 0x80) && ((*charfont & 0x40) || (charfont[1] & 0x80) || (charfont[1] & 0x40));
		}
		else if(yis7)
		{
			return !(charfont[7] & 0x80) && ((charfont[7] & 0x40) || (charfont[6] & 0x80) || (charfont[6] & 0x40));
		}
		else
		{
			return !(charfont[y] & 0x80) && (
				(charfont[y - 1] & 0x80) || (charfont[y - 1] & 0x40) ||
				(charfont[y] & 0x40) ||
				(charfont[y + 1] & 0x80) || (charfont[y + 1] & 0x40));
		}
	}
	else if(xis7)
	{
		if(yis0)
		{
			return !(*charfont & 0x01) && ((*charfont & 0x02) || (charfont[1] & 0x01) || (charfont[1] & 0x02));
		}
		else if(yis7)
		{
			return !(charfont[7] & 0x01) && ((charfont[7] & 0x02) || (charfont[6] & 0x01) || (charfont[6] & 0x02));
		}
		else
		{
			return !(charfont[y] & 0x01) && (
				(charfont[y - 1] & 0x01) || (charfont[y - 1] & 0x02) ||
				(charfont[y] & 0x02) ||
				(charfont[y + 1] & 0x01) || (charfont[y + 1] & 0x02));
		}
	}
	else
	{
		char b = 1 << (7 - x);
		if(yis0)
		{
			return !(*charfont & b) && (
				(*charfont & (b << 1)) || (*charfont & (b >> 1)) ||
				(charfont[1] & (b << 1)) || (charfont[1] & b) || (charfont[1] & (b >> 1)));
		}
		else if(yis7)
		{
			return !(charfont[7] & b) && (
				(charfont[7] & (b << 1)) || (charfont[7] & (b >> 1)) ||
				(charfont[6] & (b << 1)) || (charfont[6] & b) || (charfont[6] & (b >> 1)));
		}
		else
		{
			return !(charfont[y] & b) && (
				(charfont[y] & (b << 1)) || (charfont[y] & (b >> 1)) ||
				(charfont[y - 1] & (b << 1)) || (charfont[y - 1] & b) || (charfont[y - 1] & (b >> 1)) ||
				(charfont[y + 1] & (b << 1)) || (charfont[y + 1] & b) || (charfont[y + 1] & (b >> 1)));
		}
	}
}

void drawChar(int *x, int *y, int margin, char ch, unsigned short fc, unsigned short olc)
{
	int i, j;
	unsigned char *charSprite;
	if(ch == '\n')
	{
		*x = margin;
		*y += 8;
	}
	else if(*y < 239)
	{
		charSprite = ch * 8 + n2DLib_font;
		// Draw charSprite as monochrome 8*8 image using given color
		for(i = 0; i < 8; i++)
		{
			for(j = 7; j >= 0; j--)
			{
				if((charSprite[i] >> j) & 1)
					setPixel(*x + (7 - j), *y + i, fc);
				else if(isOutlinePixel(charSprite, 7 - j, i))
					setPixel(*x + (7 - j), *y + i, olc);
			}
		}
		*x += 8;
	}
}

void drawString(int *x, int *y, int _x, const char *str, unsigned short fc, unsigned short olc)
{
	int i, max = strlen(str) + 1;
	for(i = 0; i < max; i++)
		drawChar(x, y, _x, str[i], fc, olc);
}

void drawDecimal(int *x, int *y, int n, unsigned short fc, unsigned short olc)
{
	// Ints are in [-2147483648, 2147483647]
	//               |        |
	int divisor =    1000000000, num, numHasStarted = 0;
	
	if(n < 0)
	{
		drawChar(x, y, 0, '-', fc, olc);
		n = -n;
	}
	while(divisor != 0)
	{
		num = n / divisor;
		if(divisor == 1 || num != 0 || numHasStarted)
		{
			numHasStarted = 1;
			drawChar(x, y, 0, num + '0', fc, olc);
		}
		n %= divisor;
		divisor /= 10;
	}
}

void drawStringF(int *x, int *y, int _x, unsigned short fc, unsigned short olc, const char *s, ...)
{
	va_list specialArgs;
	char str[1200] = { 0 };
	// Max nb of chars on-screen
	
	va_start(specialArgs, s);
	vsprintf(str, s, specialArgs);
	drawString(x, y, _x, str, fc, olc);
	va_end(specialArgs);
}

int numberWidth(int n)
{
	// Ints are in [-2147483648, 2147483647]
	int divisor = 10, result = 8;
	
	if(n < 0)
	{
		result += 8;
		n = -n;
	}
	
	while(1)
	{
		if(n < divisor)
			return result;
		result += 8;
		divisor *= 10;
	}
}

int stringWidth(const char* s)
{
	int i, result = 0, size = strlen(s);
	for(i = 0; i < size; i++)
	{
		if(s[i] >= 0x20)
			result += 8;
	}
	return result;
}

/*               *
 * Miscellaneous *
 *               */
int get_key_pressed(t_key* report)
{
	unsigned short rowmap;
	int col, row;
	unsigned short *KEY_DATA = (unsigned short*)0x900E0010;
	int gotKey = 0;
	
	report->row = report->tpad_row = _KEY_DUMMY_ROW;
	report->col = report->tpad_col = _KEY_DUMMY_COL;
	report->tpad_arrow = TPAD_ARROW_NONE;

	// Touchpad and clickpad keyboards have different keymapping
	for(row = 0; row < 8; row++)
	{
		rowmap = is_cx ? KEY_DATA[row] : ~KEY_DATA[row];
		for(col = 1; col <= 0x400; col <<= 1)
		{
			if(rowmap & col)
			{
				gotKey = 1;
				break;
			}
		}
		if(gotKey) break;
	}
	if(gotKey)
	{
		row *= 2;
		row += 0x10;
		if(is_touchpad)
		{
			report->tpad_row = row;
			report->tpad_col = col;
		}
		else
		{
			report->row = row;
			report->col = col;
		}
		return 1;
	}
	else
		return 0;
}

inline int isKey(t_key k1, t_key k2)
{
	return is_touchpad ? (k1.tpad_arrow == k2.tpad_arrow ? k1.tpad_row == k2.tpad_row && k1.tpad_col == k2.tpad_col : 0 )
						: k1.row == k2.row && k1.col == k2.col;
}

#ifdef __cplusplus
}
#endif
