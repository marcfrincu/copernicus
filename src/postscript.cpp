#include "include/postscript.h"

/* radius of unit circle */
int unitr = 1000;
int offset = 400;

FILE* open_ps_file ( const char * filename )
{
    return fopen ( filename,"w" );
}

void close_ps_file ( FILE* file )
{
    fprintf ( file, " showpage\n" );
    fclose ( file );
}

void init_ps_file ( FILE* file, int size_x, int size_y, int _offset )
{
    fprintf ( file, "%%!PS-Adobe-1.0 \n" );

    if (size_x < 0)
        size_x = 1700;
    if (size_y < 0)
        size_y = 1700;
    if ( offset > 0 )
        offset = _offset;

    fprintf ( file, "%%%%BoundingBox: 0 0 %d %d \n", size_x, size_y );
    fprintf ( file, "%%%%EndComments \n" );
}


void draw_rect ( FILE* file, double botX, double botY, double topX, double topY )
{
    fprintf ( file, "newpath 0.2 setlinewidth " );
    fprintf ( file, "%lf %lf moveto ", unitr * botX + offset, unitr * botY + offset );
    fprintf ( file, "%lf %lf lineto ", unitr * botX + offset, unitr * topY + offset );
    fprintf ( file, "%lf %lf moveto ", unitr * topX + offset, unitr * topY + offset );
    fprintf ( file, "%lf %lf moveto ", unitr * topX + offset, unitr * botY + offset );
    fprintf ( file, "%lf %lf moveto ", unitr * botX + offset, unitr * botY + offset );
    fprintf ( file, "closepath stroke \n" );
}

void draw_filled_circle ( FILE* file, double centerX, double centerY, double radius )
{
    fprintf ( file, "newpath 0.20 setlinewidth " );
    fprintf ( file, "%lf %lf %lf 0 360 ", unitr * centerX + offset, unitr * centerY + offset, radius );
    fprintf ( file, " arc fill \n" );
}

void draw_empty_circle ( FILE* file, double centerX, double centerY, double radius )
{
    fprintf ( file, "newpath 0.20 setlinewidth " );
    fprintf ( file, "%lf %lf %lf 0 360 ", unitr * centerX + offset, unitr * centerY + offset, radius );
    fprintf ( file, " arc stroke \n" );
}

void draw_line ( FILE* file, double origX, double origY, double endX, double endY, float line_width )
{
    fprintf ( file, "newpath %f setlinewidth ", line_width );
    fprintf ( file, "%lf %lf moveto ", unitr * origX + offset, unitr * origY + offset );
    fprintf ( file, "%lf %lf lineto ", unitr * endX + offset, unitr * endY + offset );
    fprintf ( file, " stroke \n" );
}

void draw_text ( FILE* file, const char* text, double origX, double origY, const char* font, float font_size, float angle )
{
    if ( font_size < 4 )
        font_size = 4;

    fprintf ( file, "/%s findfont ", font );
    fprintf ( file, "%f scalefont setfont ", font_size );
    fprintf ( file, " newpath " );
    fprintf ( file, "%lf %lf moveto ", unitr * origX + offset, unitr * origY + offset );
    fprintf ( file, "%f rotate ", angle );
    fprintf ( file, "( %s ) show ", text  );
    fprintf ( file, "%f rotate \n", -angle );
}
