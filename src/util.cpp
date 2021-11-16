#include <math.h>
#include "include/util.h"
#include <stdlib.h>
#ifdef __linux__
#include <unistd.h>
#else
#include <direct.h>
#endif

double rev ( double x )
{
	double rv;
	rv=x-(int)(x/360)*360;
	if (rv<0){
		rv=rv+360;
	}
	return rv;
}

char* getWorkingDir() {
    char *path = NULL;

    #ifdef __linux__
    size_t size;
    path = getcwd ( path, size );
    #else
    path = _getcwd( NULL, 0 );
    #endif

    return path;
}

void polar_to_cartesian ( float azimuth, float length, float altitude, float *x, float *y, float *z )
{
 /*   altitude = rev ( altitude );
    azimuth = rev ( azimuth );
    *x = length * sin  ( altitude * radian_correction ) * cos ( azimuth );
    *y = length * sin  ( altitude * radian_correction ) * sin ( azimuth );
    *z = length * cos  ( altitude * radian_correction );
*/
	// do x-z calculation
	float theta    = (90 - azimuth) * radian_correction;
	float tantheta = (float) tan (theta);
	float radian_alt = altitude * radian_correction;
	float cospsi = (float) cos (radian_alt);

	(*x) = (float) sqrt ( (length * length) / (tantheta * tantheta + 1));
	(*z) = tantheta * (*x);

	(*x) = - (*x);

	if (((azimuth >= 180.0) && (azimuth <= 360.0)) || (azimuth == 0.0f)){
		(*x) = - (*x);
		(*z) = - (*z);
	}

	// calculate y, and adjust x and z
	(*y) = (float) (sqrt ((*z) * (*z) + (*x) * (*x)) * sin (radian_alt));

	if (length < 0) {
		(*x) = - (*x);
		(*z) = - (*z);
		(*y) = - (*y);
	}
	(*x) = (*x) * cospsi;
	(*z) = (*z) * cospsi;

}

void cartesian_to_polar ( float x, float y, float z, float *azimuth, float *altitude, float *length )
{
    *length = sqrt ( x * x + y * y + z * z );
   // if ( x < 0 )
 //       *azimuth = ( 3.14159265358979323846f - asin ( y / *length ) ) * 1 / radian_correction;
  //  else
  //      *azimuth = asin ( y / *length ) * 1 / radian_correction;
    *azimuth =  atan2 ( y, x ) * 1 / radian_correction;
    *altitude = ( 3.14159265358979323846f / 2 - atan2 ( sqrt ( x * x + y * y ), z ) ) * 1 / radian_correction;
  //  *altitude = acos ( z / ( *length ) ) * 1 / radian_correction;
}

int between ( float a, float b, float x )
{
	float a1 = a, b1 = b,c;
	int betw;

	if ( a1 > b1 )
	{
			c = b1;
			b1 = a1;
			a1 = c;
	}

	if ( ( a1 < x ) && ( x < b1 ) )
			betw = 1;
	else
			betw= 0;

	return betw;
}

double distance (double x1, double y1, double z1, double x2, double y2, double z2 )
{
    return sqrt(pow(x1-x2,2) + pow(y1-y2,2) + pow(z1-z2,2));
}
