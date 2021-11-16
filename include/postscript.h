#ifndef POSTSCRIPT_H_INCLUDED
#define POSTSCRIPT_H_INCLUDED

#endif // POSTSCRIPT_H_INCLUDED

#include <stdio.h>

FILE* open_ps_file ( const char * filename );
void close_ps_file ( FILE* file );
void init_ps_file ( FILE* file, int size_x, int size_y, int offset );
void draw_rect ( FILE* file, double botX, double botY, double topX, double topY );
void draw_filled_circle ( FILE* file, double centerX, double centerY, double radius );
void draw_empty_circle ( FILE* file, double centerX, double centerY, double radius );
void draw_line ( FILE* file, double origX, double origY, double endX, double endY, float line_width );
void draw_text ( FILE* file, const char* text, double origX, double origY, const char* font, float font_size, float angle );
