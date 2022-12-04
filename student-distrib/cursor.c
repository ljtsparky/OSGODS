#include "cursor.h"
#include "lib.h"
//reference https://wiki.osdev.org/Text_Mode_Cursor
void disable_cursor()
{
	outb(0x0A,0x3D4);
	outb(0x20,0x3D5);
}

void update_cursor(int x, int y)
{
	uint16_t pos = y * 80 + x;
	outb( 0x0F,0x3D4);
	outb( (uint8_t) (pos & 0xFF) ,0x3D5);
	outb( 0x0E,0x3D4 );
	outb( (uint8_t) ((pos >> 8) & 0xFF)  ,0x3D5 );
}
