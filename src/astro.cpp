#include "include/astro.h"
#include "include/util.h"
#include "include/tgaload.h"
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
When computing the Cartesian Coordinates (x, y, z) from Equatorial ones (RA and Dec) the axis are
defined as follows:
   +x: towards Dec = 0 degrees, RA = 0.0 hours (the vernal equinox)
   +y: towards Dec = 0 degrees, RA = 6.0 hours
   +z: towards Dec = +90.0 degrees (north celestial pole)

   origin: of this coordinate system is at the Sun.
*/

asteroid_info centaur[NO_CENTAURS], transneptune[NO_TRANSNEPTUNES], asteroid[NO_ASTEROIDS];
moon_info moon[NO_MOONS];
dso_info dso_object[NO_DSO_OBJECTS];
constellation_info constellation[NO_CONSTELLATION_LINES];
constellation_boundary_info constellation_boundary[NO_CONSTELLATION_BOUNDARIES];
constellation_name_info constellation_name[NO_CONSTELLATION];
beyer_info beyer[NO_BEYER];
constellation_info *p_constellation = constellation;
constellation_name_info *p_constellation_name = constellation_name;
constellation_boundary_info *p_constellation_boundary = constellation_boundary;
beyer_info *p_beyer = beyer;
dso_info *p_dso_object = dso_object;
asteroid_info *p_centaur = centaur, *p_transneptune = transneptune,*p_asteroid = asteroid;

planet_info planet[NO_PLANETS];
planet_info *p_planet = planet;

star_info star[NO_STARS];
star_info *p_star = star;

int date_changed;
int year, month, day, hour;
double lst, julian_date, my_long, my_lat;
long actual_no_stars;
long actual_no_dsos;
long actual_no_transneptunes;
long actual_no_centaurs;
long actual_no_asteroids;

static void compute_beyer_name_coordinates ( beyer_info *beyer, int index, double lst, double phi, float scale_factor );
static void compute_constellation_name_coordinates ( constellation_name_info *constellation_name, int index, double lst, double phi, float scale_factor );
static void compute_constellation_line_coordinates ( constellation_info *constellation, int index, double lst, double phi, float scale_factor );
static void compute_constellation_boundary_coordinates ( constellation_boundary_info *constellation_boundary, double r1, double d1, double r2, double d2, int index, double lst, double phi, float scale_factor );
static coordinates_info compute_asteroid_coordinates ( double N, double i, double w, double a, double e, double M, int epoch );
static coordinates_info compute_moon_coordinates ( double N, double i, double w, double a, double e, double M, int epoch );
static moon_info compute_earth_moon_coordinates ( double SM,  double SW, double SN, double SXs, double SYs, double crt_lat, double lst );

double date()
{
	double d;
	if (date_changed == 1)
	{
		d = 367 * ( year )  - ( 7 *  ( ( year) + ( ( ( month ) + 9 ) / 12 ) ) ) / 4 + ( 275 * ( month ) ) / 9 + day - 730530;
		d = d + hour / 24.0;
		lst = rev ( 100.46 + 0.985647 * d + my_long + hour * 15.04107 );
		lst = lst / 15.04107;
		julian_date = compute_julian_date ( year, month, day );
	}
	else
	{
		time_t currentTime;
		struct tm *timeStruct;
		time ( &currentTime );
		timeStruct = localtime ( &currentTime );
		d = 367 * ( 2000 + timeStruct->tm_year % 100 ) - ( 7 * ( ( 2000 + timeStruct->tm_year % 100 ) + ( ( ( timeStruct->tm_mon + 1 ) + 9) / 12 ) ) ) / 4 + ( 275 * ( timeStruct->tm_mon + 1 ) ) / 9 + timeStruct->tm_mday - 730530;
		d = d + ( timeStruct->tm_hour + timeStruct->tm_min / 60.0 + timeStruct->tm_sec / 3600.0 ) / 24.0;
		double decimalhours = timeStruct->tm_hour + timeStruct->tm_min / 60.0 + timeStruct->tm_sec / 3600.0;
		lst = rev ( 100.46 + 0.985647 * d + my_long + decimalhours * 15.04107 );
		lst = lst / 15.04107;
		julian_date = compute_julian_date ( 2000 + timeStruct->tm_year % 100, timeStruct->tm_mon, timeStruct->tm_mday );
	}
	return d;
}

double compute_julian_date ( int year, int month, int day )
{
	if ( ( year < 1 ) || ( year > 9999 ) )
		return -1.0;
	else
	{
		if ( month < 3 )
		{
			month = month + 12;
			year = year - 1;
		}
		switch ( month )
		{
			case 3:
			case 5:
			case 7:
			case 8:
			case 10:
			case 12:
			case 13: if ( ( day < 1 ) || ( day > 31 ) )
							return -1.0;
						break;
			case 4:
			case 6:
			case 9:
			case 11: if ( ( day < 1 ) || ( day > 30 ) )
							return -1.0;
						break;
			case 14:
			{
				switch ( day )
				{
					case 29: if ( ( year + 1 ) % 4 != 0 )
									return -1.0;
							 	else
									return -1.0;
					default: break;
				}
			}
			default: break;
		}
		return int ( 365.25 * ( year + 4716 ) ) + int ( 30.6001 * ( month + 1 ) ) + day - 1524.5;
	}
}
void compute_sun_coordinates ( planet_info *planet, double lst, double phi )
{
	double OblEcl, d = date();
	double N, incl, w, a, e, M;
	double E, xv, yv, v, r, rs, RS, lonsun;

	double h, glat, x, y, z, sina;
	double zalt = 1.570796;

	N = planet[0].N;
	incl = planet[0].i;
	w = planet[0].w;
	a = planet[0].a;
	e = planet[0].e;
	M = planet[0].M;

	OblEcl = ( 23.4393 - 0.0000003563 * d ) * radian_correction;

	E = M + e * sin ( M ) * ( 1.0 + e * cos( M ) );

	if ( E > 0.05 )
	{
		E = E - ( E - e * sin ( E ) - M ) / ( 1 - e * cos ( E ) );
	}

	xv = cos ( E ) - e;
	yv = sin ( E ) * sqrt ( 1.0 - ( e * e ) );

	v = atan2 ( yv, xv );
	r = sqrt ( xv * xv + yv * yv );
	rs = r;
	RS = rs;
	lonsun = v + w;
	planet[0].xs = rs * cos ( lonsun );
	planet[0].ys = rs * sin( lonsun );

	planet[0].xe = planet[0].xs;
	planet[0].ye = planet[0].ys * cos ( OblEcl );
	planet[0].ze = planet[0].ys * sin ( OblEcl );

	planet[0].RA = atan2 ( planet[0].ye, planet[0].xe );
	planet[0].Dec = atan2 ( planet[0].ze, sqrt ( planet[0].xe * planet[0].xe + planet[0].ye * planet[0].ye ) );

	h = lst - planet[0].RA;
	glat = phi;

	sina = sin ( planet[0].Dec ) * sin ( glat ) + cos ( planet[0].Dec ) * cos ( glat ) * cos ( h );
	a = asin ( sina );
	y = -cos ( planet[0].Dec ) * cos ( glat ) * sin ( h );
	x = sin ( planet[0].Dec ) - sin ( glat ) * sina;
	z = atan2 ( y, x );
	if ( a > 0 )
	{
		x = cos ( z ) * tan ( ( zalt - a ) / 2 );
		y = sin ( z ) * tan ( ( zalt - a ) / 2 );
	}

	planet[0].xp = x;
	planet[0].yp = y;
}

void compute_planet_coordinates( planet_info *planet, int i, double lst, double phi )
{
	double OblEcl, E;
	double d = date();
	double N, incl, w, a, e, M;
	double xv, yv, r, v, lonecl, latecl, xh, yh, zh, xs, ys, xg, yg, zg, XE, YE, ZE;
	double Ms, Mj;

	double h, glat, x, y, z, sina;
	double zalt = 1.570796;

	N = planet[i].N;
	incl = planet[i].i;
	w = planet[i].w;
	a = planet[i].a;
	e = planet[i].e;
	M = planet[i].M;

	OblEcl = ( 23.4393 - 0.0000003563 * d ) * radian_correction;

	E = M + e * sin( M ) * ( 1.0 + e * cos ( M ) );

	if ( E > 0.05 )
	{
		E = E - ( E - e * sin ( E ) - M ) / ( 1 - e * cos ( E ) );
	}

	xv = a * ( cos ( E ) - e);
	yv = a * ( sqrt ( 1.0 - e * e ) * sin ( E ) );
	v = atan2 ( yv, xv );
	r = sqrt ( xv * xv + yv * yv );

	planet[i].distFromSun = r;

	planet[i].xe = r * ( cos ( N ) * cos ( v + w ) - sin ( N ) * sin ( v + w ) * cos ( incl ) );
	planet[i].ye = r * ( sin ( N ) * cos ( v + w ) + cos ( N ) * sin ( v + w ) * cos ( incl ) );
	planet[i].ze = r * ( sin ( v + w ) * sin ( incl ) );

	// For the Polar Projection Map
	lonecl = atan2 ( planet[i].ye, planet[i].xe );
	latecl = atan2 ( planet[i].ze, sqrt ( planet[i].xe * planet[i].xe + planet[i].ye * planet[i].ye ) );

	// Add perturbations inflicted by large planets
	switch ( i )
	{
		// Jupiter perturbation by Saturn
		case 5:
			Ms = rev ( 316.967 + 0.0334442282 * d ) * radian_correction;

			lonecl = lonecl - 0.332 * sin ( ( 2 * planet[i].M - 5 * Ms - 67.6 ) * radian_correction );
			lonecl = lonecl - 0.056 * sin ( ( 2 * planet[i].M - 2 * Ms + 21 ) * radian_correction );
			lonecl = lonecl + 0.042 * sin ( ( 3 * planet[i].M - 5 * Ms + 21 ) * radian_correction );
			lonecl = lonecl - 0.036 * sin ( planet[i].M - 2 * Ms );
			lonecl = lonecl + 0.022 * cos ( planet[i].M - Ms );
			lonecl = lonecl + 0.023 * sin ( ( 2 * planet[i].M - 3 * Ms + 52 ) * radian_correction );
			lonecl = lonecl - 0.016 * sin ( ( planet[i].M - 5 * Ms - 69 ) * radian_correction );
			break;
		// Saturn perturbation by Jupiter
		case 6:
			Mj = rev ( 19.895 + 0.0830853001 * d ) * radian_correction;
			Ms = rev ( 316.967 + 0.0334442282 * d ) * radian_correction;

			lonecl = lonecl + 0.812 * sin ( ( 2 * Mj - 5 * planet[i].M - 67.6 ) * radian_correction );
			lonecl = lonecl - 0.229 * cos ( ( 2 * Mj - 4 * planet[i].M - 2 ) * radian_correction );
			lonecl = lonecl + 0.119 * sin ( ( Mj - 2 * planet[i].M - 3 ) * radian_correction );
			lonecl = lonecl + 0.046 * sin ( ( 2 * Mj - 6 * planet[i].M - 69 ) * radian_correction );
			lonecl = lonecl + 0.014 * sin ( ( Mj - 3 * planet[i].M + 32 ) * radian_correction );

			latecl = latecl - 0.020 * cos ( ( 2 * Mj - 4 * Ms - 2 ) * radian_correction );
			latecl = latecl + 0.018 * sin ( ( 2 * Mj - 6 * Ms - 49 ) * radian_correction );
			break;
		// Uranus pertubation by Jupiter and Saturn
		case 7:
			Mj = rev ( 19.895 + 0.0830853001 * d ) * radian_correction;
			Ms = rev ( 316.967 + 0.0334442282 * d ) * radian_correction;

			lonecl = lonecl + 0.040 * sin ( ( Ms - 2 * planet[i].M + 6 ) * radian_correction );
			lonecl = lonecl + 0.035 * sin ( ( Ms - 3 * planet[i].M + 33 ) * radian_correction );
			lonecl = lonecl - 0.015 * sin ( ( Mj - planet[i].M + 20 ) * radian_correction );
			break;
	}


	xh = r * cos ( lonecl ) * cos ( latecl );
	yh = r * sin ( lonecl ) * cos ( latecl );
	zh = r * sin ( latecl );

	xg = xh + xs;
	yg = yh + ys;
	zg = zh;

	XE = xg;
	YE = yg * cos ( OblEcl ) - zg * sin ( OblEcl );
	ZE = yg * sin ( OblEcl ) + zg * cos ( OblEcl );

	planet[i].RA = atan2 ( YE, XE );
	planet[i].Dec = atan2 ( ZE, sqrt ( XE * XE + YE * YE ) );

	h = lst - planet[i].RA;
	glat = phi;

	sina = sin ( planet[i].Dec ) * sin ( glat ) + cos ( planet[i].Dec ) * cos ( glat ) * cos ( h );
	a = asin ( sina );
	y = -cos ( planet[i].Dec ) * cos ( glat ) * sin ( h );
	x = sin ( planet[i].Dec ) - sin ( glat ) * sina;
	z = atan2 ( y, x );

	if ( a > 0 )
	{
		x = cos ( z ) * tan ( ( zalt - a ) / 2 );
		y = sin ( z ) * tan ( ( zalt - a ) / 2 );
	}

	planet[i].xp = x;
	planet[i].yp = y;
}

void initialize_planets ( planet_info *planet, double lst, double phi )
{
	double d = date();
	int i;

	// Sun
	planet[0].N = 0;
	planet[0].i = 0;
	planet[0].w = rev ( 282.9404 + 0.0000470935 * d ) * radian_correction;
	planet[0].a = 1;
	planet[0].e = 0.016709 - 0.000000001151 * d;
	planet[0].M = rev ( 356.047 + 0.9856002585 * d ) * radian_correction;
	planet[0].size = 327;
	planet[0].tilt_x = 0.0;
	planet[0].tilt_z = 0.0;
	planet[0].name = "Sun";
	// Mercury
	planet[1].N = rev ( 48.3313 + 0.0000324587 * d ) * radian_correction;
	planet[1].i = rev ( 7.0047 + 0.00000005 * d ) * radian_correction;
	planet[1].w = rev ( 29.1241 + 0.0000101444 * d ) * radian_correction ;
	planet[1].a = 0.387098 + 0.0000000000180698 * d;
	planet[1].e = 0.205635 + 0.000000000559 * d;
	planet[1].M = rev ( 168.6562 + 4.0923344368 * d ) * radian_correction;
	planet[1].size = 1.17;
	planet[1].tilt_x = 7.01;
	planet[1].tilt_z = 1407.509;
	planet[1].name = "Mercury";
	// Venus
	planet[2].N = rev ( 76.6799 + 0.000024659 * d ) * radian_correction;
	planet[2].i = rev ( 3.3946 + 0.0000000275 * d ) * radian_correction;
	planet[2].w = rev ( 54.891 + 0.0000138374 * d ) * radian_correction ;
	planet[2].a = 0.72333 + 0.0000000000251882 * d;
	planet[2].e = 0.006773 - 0.000000001302 * d;
	planet[2].M = rev ( 48.0052 + 1.6021302244 * d ) * radian_correction;
	planet[2].size = 2.28;
	planet[2].tilt_x = 178.72;
	planet[2].tilt_z = 5832.47;
	planet[2].name = "Venus";
	// Earth
	planet[3].N = rev ( -11.26064 - 0.00013863 * d ) * radian_correction;
	planet[3].i = rev ( 0.0005 - 0.000000356985 * d ) * radian_correction;
	planet[3].w = rev ( 102.94719 - planet[3].N + 0.00000911309 * d ) * radian_correction;
	planet[3].a = 1.00000011 - 0.00000000000136893 * d;
	planet[3].e = 0.01671022 - 0.00000000104148 * d;
	planet[3].M = rev ( 100.46435 - 102.94719 + 0.985609101 * d ) * radian_correction;
	planet[3].size = 3.0;
	planet[3].tilt_x = -23.45;
	planet[3].tilt_z = 23.934;
	planet[3].name = "Earth";
	// Mars
	planet[4].N = rev ( 49.5574 + 0.0000211081 * d ) * radian_correction;
	planet[4].i = rev ( 1.8497 - 0.0000000178 * d ) * radian_correction;
	planet[4].w = rev ( 286.5016 + 0.0000292961 * d ) * radian_correction ;
	planet[4].a = 1.523688-0.000000001977 * d;
	planet[4].e = 0.093405 + 0.000000002516 * d;
	planet[4].M = rev ( 18.6021 + 0.5240207766 * d ) * radian_correction;
	planet[4].size = 1.68;
	planet[4].tilt_x = 25.19;
	planet[4].tilt_z = 24.622;
	planet[4].name = "Mars";
	// Jupiter
	planet[5].N = rev ( 100.4542 + 0.0000276854 * d ) * radian_correction;
	planet[5].i = rev ( 1.303 - 0.0000001557 * d ) * radian_correction;
	planet[5].w = rev ( 273.8777 + 0.0000164505 * d ) * radian_correction ;
	planet[5].a = 5.20256 + 0.0000000166289 * d;
	planet[5].e = 0.048498 + 0.000000004469 * d;
	planet[5].M = rev ( 19.895 + 0.0830853001 * d ) * radian_correction;
	planet[5].size = 33.6;
	planet[5].tilt_x = 3.13;
	planet[5].tilt_z = 0.0;
	planet[5].name = "Jupiter";
	planet[5].has_rings = 1;
	planet[5].inner_ring = 57.68;
	planet[5].outer_ring = 60.74;
	// Saturn
	planet[6].N = rev ( 113.6634 + 0.000023898 * d ) * radian_correction;
	planet[6].i = rev ( 2.4886 - 0.0000001081 * d ) * radian_correction;
	planet[6].w = rev ( 339.3939 + 0.0000297661 * d ) * radian_correction ;
	planet[6].a = 9.55475- 0.00000008255439999999999 * d;
	planet[6].e = 0.055546 - 0.000000009499 * d;
	planet[6].M = rev ( 316.967 + 0.0334442282 * d ) * radian_correction;
	planet[6].size = 28.2;
	planet[6].tilt_x = 26.73;
	planet[6].tilt_z = 10.656;
	planet[6].name = "Saturn";
	planet[6].has_rings = 1;
	planet[6].inner_ring = 28.2 + 3.12;
	planet[6].outer_ring = 28.2 + 56.83;
	// Uranus
	planet[7].N = rev ( 74.0005 + 0.000013978 * d ) * radian_correction;
	planet[7].i = rev ( 0.7733 + 0.000000019 * d ) * radian_correction;
	planet[7].w = rev ( 96.6612 + 0.000030565 * d ) * radian_correction ;
	planet[7].a = 19.18171 + 0.0000000416222 * d;
	planet[7].e = 0.047318 + 0.00000000745 * d;
	planet[7].M = rev ( 142.5905 + 0.011725806 * d ) * radian_correction;
	planet[7].size = 12.0;
	planet[7].tilt_x = 97.77;
	planet[7].tilt_z = 17.27;
	planet[7].name = "Uranus";
	planet[7].has_rings = 1;
	planet[7].inner_ring = 15.06;
	planet[7].outer_ring = 17.82;
	// Neptune
	planet[8].N = rev ( 131.7806 + 0.000030173 * d ) * radian_correction;
	planet[8].i = rev ( 1.77 - 0.000000255 * d ) * radian_correction;
	planet[8].w = rev ( 272.8461 - 0.000006027 * d ) * radian_correction ;
	planet[8].a = 30.05826  - 0.0000000342768 * d;
	planet[8].e = 0.008606 + 0.00000000215 * d;
	planet[8].M = rev ( 260.2471 + 0.005995147 * d ) * radian_correction;
	planet[8].size = 11.4;
	planet[8].tilt_x = 28.32;
	planet[8].tilt_z = 16.11;
	planet[8].name = "Neptune";
	planet[8].has_rings = 1;
	planet[8].inner_ring = 19.77;
	planet[8].outer_ring = 29.66;

	#ifdef DEBUG
	printf ( "Day number : %lf\n", d );
	printf ( "Orbital elements of the Sun and the 8 planets\n" );
	for (i=0; i<NO_PLANETS; i++)
		printf ( "\tObject: %s : N=%lf i=%lf w=%lf a=%lf e=%lf M=%lf\n", planet[i].name, planet[i].N, planet[i].i, planet[i].w, planet[i].a, planet[i].e, planet[i].M );
	#endif

	#ifdef DEBUG
	printf ( "3D position of the Sun and 8 planets and their equatorial coordinates\n" );
	#endif
	compute_sun_coordinates ( planet, lst, phi  );
	#ifdef DEBUG
	printf ( "\tObject: %s : Xe=%lf Ye=%lf Ze=%lf RA=%lf Dec=%lf\n", planet[0].name, planet[0].xe, planet[0].ye, planet[0].ze, planet[0].RA, planet[0].Dec );
	#endif
	for (i=1; i<NO_PLANETS; i++) {
		compute_planet_coordinates ( planet, i, lst, phi );
		#ifdef DEBUG
		printf ( "\tPlanet: %s : Xe=%lf Ye=%lf Ze=%lf r=%lf RA=%lf Dec=%lf\n", planet[i].name, planet[i].xe, planet[i].ye, planet[i].ze, planet[i].distFromSun, planet[i].RA, planet[i].Dec );
		#endif
	}
}

void initialize_other_ss_objects ( planet_info *planet, asteroid_info *asteroid, asteroid_info *centaur, asteroid_info *transneptune, double lst, double phi )
{
	static char line[150], *tmp;
	char filename[255];
	FILE *f = NULL;
	long i, junk;
	moon_info m_earth[NO_MOONS], m_mars[NO_MOONS], m_jupiter[NO_MOONS], m_saturn[NO_MOONS], m_uranus[NO_MOONS], m_neptune[NO_MOONS];
	moon_info *moon_earth = m_earth, *moon_jupiter = m_jupiter, *moon_mars = m_mars, *moon_saturn = m_saturn, *moon_uranus = m_uranus, *moon_neptune = m_neptune;
    coordinates_info coords;

	#ifdef DEBUG
	printf ( "Other Solar System Objects (Moons, Asteroids -- Centaurs, Transneptunes --, Comets) information:\n" );
	#endif

    f = fopen ( "data/asteroids.dat", "rt" );
	if ( f == NULL ) {
		printf ( "Error opening the asteroids.dat file" );
		exit ( 1 );
	}

	i = 0;
	while ( fgets ( line, 150, f ) != NULL  && i < NO_ASTEROIDS )
	{
		tmp = line;

		sscanf ( tmp, "%d %s %d %d %lf %lf %lf %lf %ld %lf %lf %lf %lf\n", &junk, asteroid[i].name, &asteroid[i].year, &asteroid[i].month, &asteroid[i].day, &asteroid[i].M, &asteroid[i].a, &asteroid[i].e, &asteroid[i].w, &asteroid[i].N, &asteroid[i].i, &asteroid[i].H, &asteroid[i].q );

		coords = compute_asteroid_coordinates ( asteroid[i].N, asteroid[i].i, asteroid[i].w, asteroid[i].a, asteroid[i].e, asteroid[i].M, asteroid[i].year );

		asteroid[i].x = coords.x;
		asteroid[i].y = coords.y;
		asteroid[i].z = coords.z;

		i++;
	}

	actual_no_asteroids = i;

	#ifdef DEBUG
	printf ( "\tNo. of Asteroids read: %ld\n", i );
	#endif

	f = fopen ( "data/centaurs.dat", "rt" );
	if ( f == NULL ) {
		printf ( "Error opening the centaurs.dat file" );
		exit ( 1 );
	}

	i = 0;
	while ( fgets ( line, 150, f ) != NULL )
	{
		tmp = line;

		sscanf ( tmp, "%d %s %lf %lf %lf %ld %lf %lf %lf %lf %lf %lf\n", &centaur[i].year, centaur[i].name, &centaur[i].q, &centaur[i].Q, &centaur[i].H, &centaur[i].epoch, &centaur[i].M, &centaur[i].w, &centaur[i].N, &centaur[i].i, &centaur[i].e, &centaur[i].a );

		coords = compute_asteroid_coordinates ( centaur[i].N, centaur[i].i, centaur[i].w, centaur[i].a, centaur[i].e, centaur[i].M, 2004 );

		centaur[i].x = coords.x;
		centaur[i].y = coords.y;
		centaur[i].z = coords.z;

		i++;
	}

	actual_no_centaurs = i;

	#ifdef DEBUG
	printf ( "\tNo. of Centaurs read: %ld\n", i );
	#endif

	f = fopen ( "data/transneptunes.dat", "rt" );
	if ( f == NULL ) {
		printf ( "Error opening the transneptunes.dat file" );
		exit ( 1 );
	}

	i = 0;
	while ( fgets ( line, 150, f ) != NULL )
	{
		tmp = line;

		sscanf ( tmp, "%d %s %lf %lf %lf %ld %lf %lf %lf %lf %lf %lf\n", &transneptune[i].year, transneptune[i].name, &transneptune[i].q, &transneptune[i].Q, &transneptune[i].H, &transneptune[i].epoch, &transneptune[i].M, &transneptune[i].w, &transneptune[i].N, &transneptune[i].i, &transneptune[i].e, &transneptune[i].a );

		coords = compute_asteroid_coordinates ( transneptune[i].N, transneptune[i].i, transneptune[i].w, transneptune[i].a, transneptune[i].e, transneptune[i].M, 2004 );

		transneptune[i].x = coords.x;
		transneptune[i].y = coords.y;
		transneptune[i].z = coords.z;

		i++;
	}

	actual_no_transneptunes = i;

	#ifdef DEBUG
	printf ( "\tNo. of Transneptunes read: %ld\n", i );
	#endif

    moon_earth[0] = compute_earth_moon_coordinates ( planet[0].M, planet[0].w, planet[0].N, planet[0].xs, planet[0].ys, phi, lst );

    glGenTextures ( 1, moon_earth[0].texture_id );
    moon_earth[0].texture_id[0] = tgaLoadAndBind ( "images/moon.tga", TGA_LOW_QUALITY );

    planet[3].no_moons = 1;
    planet[3].moon = moon_earth;
    planet[3].moon[0].size = planet[3].size / 4.f;
    planet[3].moon_scale_factor = 1/5.f;
    strcpy ( planet[3].moon[0].name, "Moon" );

    #ifdef DEBUG
    printf("\tMoon: Luna: Xe=%lf Ye=%lf Ze=%lf\n", planet[3].moon[0].x, planet[3].moon[0].y, planet[3].moon[0].z);
    #endif

	f = fopen ( "data/moonmars.dat", "rt" );
	if ( f == NULL ) {
		printf ( "Error opening the moonmars.dat file" );
		exit ( 1 );
	}

	i = 0;
	while ( fgets ( line, 150, f ) != NULL )
	{
		tmp = line;

		sscanf ( tmp, "%s %lf %lf %lf %lf %lf %lf %d %s\n", moon_mars[i].name, &moon_mars[i].N, &moon_mars[i].i, &moon_mars[i].w, &moon_mars[i].a, &moon_mars[i].e, &moon_mars[i].M, &moon_mars[i].epoch, moon_mars[i].texture_name );

		glGenTextures ( 1, moon_mars[i].texture_id );

		sprintf ( filename, "images/%s", moon_mars[i].texture_name );
		moon_mars[i].texture_id[0] = tgaLoadAndBind ( filename, TGA_LOW_QUALITY );
		coords = compute_moon_coordinates ( moon_mars[i].N, moon_mars[i].i, moon_mars[i].w, moon_mars[i].a, moon_mars[i].e, moon_mars[i].M, moon_mars[i].epoch );

		moon_mars[i].x = coords.x;
		moon_mars[i].y = coords.y;
		moon_mars[i].z = coords.z;
        moon_mars[i].size = 0.3;
		#ifdef DEBUG
		printf ( "\tMoon: %s : Xe=%lf Ye=%lf Ze=%lf\n", moon_mars[i].name, moon_mars[i].x, moon_mars[i].y, moon_mars[i].z );
		#endif

		i++;
	}

	planet[4].no_moons = i;
	planet[4].moon = moon_mars;
	planet[4].moon_scale_factor = 100000;

	#ifdef DEBUG
	printf ( "\tNo. of Mars moons read: %ld\n", i );
	#endif

	f = fopen ( "data/moonjup.dat", "rt" );
	if ( f == NULL ) {
		printf ( "Error opening the moonjup.dat file" );
		exit ( 1 );
	}

	i = 0;
	while ( fgets ( line, 150, f ) != NULL )
	{

		tmp = line;

		sscanf ( tmp, "%s %lf %lf %lf %lf %lf %lf %d %s\n", moon_jupiter[i].name, &moon_jupiter[i].N, &moon_jupiter[i].i, &moon_jupiter[i].w, &moon_jupiter[i].a, &moon_jupiter[i].e, &moon_jupiter[i].M, &moon_jupiter[i].epoch, moon_jupiter[i].texture_name  );

		glGenTextures ( 1, moon_jupiter[i].texture_id );

		sprintf ( filename, "images/%s", moon_jupiter[i].texture_name );
		moon_jupiter[i].texture_id[0] = tgaLoadAndBind ( filename, TGA_LOW_QUALITY );

		coords = compute_moon_coordinates ( moon_jupiter[i].N, moon_jupiter[i].i, moon_jupiter[i].w, moon_jupiter[i].a, moon_jupiter[i].e, moon_jupiter[i].M, moon_jupiter[i].epoch);

		moon_jupiter[i].x = coords.x;
		moon_jupiter[i].y = coords.y;
		moon_jupiter[i].z = coords.z;
        moon_jupiter[i].size = 0.3;
		#ifdef DEBUG
		printf ( "\tMoon: %s : Xe=%lf Ye=%lf Ze=%lf\n", moon_jupiter[i].name, moon_jupiter[i].x, moon_jupiter[i].y, moon_jupiter[i].z );
		#endif

		i++;
	}

	planet[5].no_moons = i;
	planet[5].moon = moon_jupiter;
	planet[5].moon_scale_factor = 30000;

	#ifdef DEBUG
	printf ( "\tNo. of Jupiter moons read: %ld\n", i );
	#endif

	f = fopen ( "data/moonsat.dat", "rt" );
	if ( f == NULL ) {
		printf ( "Error opening the moonsat.dat file" );
		exit ( 1 );
	}

	i = 0;
	while ( fgets ( line, 150, f ) != NULL )
	{
		tmp = line;

		sscanf ( tmp, "%s %lf %lf %lf %lf %lf %lf %d %s\n", moon_saturn[i].name, &moon_saturn[i].N, &moon_saturn[i].i, &moon_saturn[i].w, &moon_saturn[i].a, &moon_saturn[i].e, &moon_saturn[i].M, &moon_saturn[i].epoch, moon_saturn[i].texture_name  );

    //printf("%d", moon_saturn[i].texture_id);

		//glGenTextures ( 1, moon_saturn[i].texture_id );

		sprintf ( filename, "images/%s", moon_saturn[i].texture_name );
		moon_saturn[i].texture_id[0] = tgaLoadAndBind ( filename, TGA_LOW_QUALITY );

		coords = compute_moon_coordinates ( moon_saturn[i].N, moon_saturn[i].i, moon_saturn[i].w, moon_saturn[i].a, moon_saturn[i].e, moon_saturn[i].M, moon_saturn[i].epoch );

		moon_saturn[i].x = coords.x;
		moon_saturn[i].y = coords.y;
		moon_saturn[i].z = coords.z;
        moon_saturn[i].size = 0.3;
		#ifdef DEBUG
		printf ( "\tMoon: %s : Xe=%lf Ye=%lf Ze=%lf\n", moon_saturn[i].name, moon_saturn[i].x, moon_saturn[i].y, moon_saturn[i].z );
		#endif

		i++;
	}

	planet[6].no_moons = i;
	planet[6].moon = moon_saturn;
	planet[6].moon_scale_factor = 30000;

	#ifdef DEBUG
	printf ( "\tNo. of Saturn moons read: %ld\n", i );
	#endif

	f = fopen ( "data/moonuran.dat", "rt" );
	if ( f == NULL ) {
		printf ( "Error opening the moonuran.dat file" );
		exit ( 1 );
	}

	i = 0;
	while ( fgets ( line, 150, f ) != NULL )
	{
		tmp = line;

		sscanf ( tmp, "%s %lf %lf %lf %lf %lf %lf %d %s\n", moon_uranus[i].name, &moon_uranus[i].N, &moon_uranus[i].i, &moon_uranus[i].w, &moon_uranus[i].a, &moon_uranus[i].e, &moon_uranus[i].M, &moon_uranus[i].epoch, moon_uranus[i].texture_name );

		//glGenTextures ( 1, moon_uranus[i].texture_id );

		sprintf ( filename, "images/%s", moon_uranus[i].texture_name );
		moon_uranus[i].texture_id[0] = tgaLoadAndBind ( filename, TGA_LOW_QUALITY );

		coords = compute_moon_coordinates ( moon_uranus[i].N, moon_uranus[i].i, moon_uranus[i].w, moon_uranus[i].a, moon_uranus[i].e, moon_uranus[i].M, moon_uranus[i].epoch );

		moon_uranus[i].x = coords.x;
		moon_uranus[i].y = coords.y;
		moon_uranus[i].z = coords.z;
        moon_uranus[i].size = 0.3;
		#ifdef DEBUG
		printf ( "\tMoon: %s : Xe=%lf Ye=%lf Ze=%lf\n", moon_uranus[i].name, moon_uranus[i].x, moon_uranus[i].y, moon_uranus[i].z );
		#endif

		i++;
	}

	planet[7].no_moons = i;
	planet[7].moon = moon_uranus;
	planet[7].moon_scale_factor = 10000;

	#ifdef DEBUG
	printf ( "\tNo. of Uranus moons read: %ld\n", i );
	#endif

	f = fopen ( "data/moonnept.dat", "rt" );
	if ( f == NULL ) {
		printf ( "Error opening the moonuran.dat file" );
		exit ( 1 );
	}

	i = 0;
	while ( fgets ( line, 150, f ) != NULL )
	{
		tmp = line;

		sscanf ( tmp, "%s %lf %lf %lf %lf %lf %lf %d %s\n", moon_neptune[i].name, &moon_neptune[i].N, &moon_neptune[i].i, &moon_neptune[i].w, &moon_neptune[i].a, &moon_neptune[i].e, &moon_neptune[i].M, &moon_neptune[i].epoch, moon_neptune[i].texture_name  );

		//glGenTextures ( 1, moon_neptune[i].texture_id );

		sprintf ( filename, "images/%s", moon_neptune[i].texture_name );
		moon_neptune[i].texture_id[0] = tgaLoadAndBind ( filename, TGA_LOW_QUALITY );

		coords = compute_moon_coordinates ( moon_neptune[i].N, moon_neptune[i].i, moon_neptune[i].w, moon_neptune[i].a, moon_neptune[i].e, moon_neptune[i].M, moon_neptune[i].epoch );

		moon_neptune[i].x = coords.x;
		moon_neptune[i].y = coords.y;
		moon_neptune[i].z = coords.z;
        moon_neptune[i].size = 0.3;
		#ifdef DEBUG
		printf ( "\tMoon: %s : Xe=%lf Ye=%lf Ze=%lf\n", moon_neptune[i].name, moon_neptune[i].x, moon_neptune[i].y, moon_neptune[i].z );
		#endif

		i++;
	}

	planet[8].no_moons = i;
	planet[8].moon = moon_neptune;
	planet[8].moon_scale_factor = 30000;

	#ifdef DEBUG
	printf ( "\tNo. of Neptune moons read: %ld\n", i );
	#endif

}
coordinates_info compute_asteroid_coordinates ( double N, double i, double w, double a, double e, double M, int epoch )
{
    coordinates_info coords;

	double E, E1, OblEcl, xv, yv, v, r;
	double d = date();
	N = rev ( N + 0.013967 * ( 2000 - epoch ) + 0.0000382394 * d ) * radian_correction;
	i = rev ( i ) * radian_correction;
	w = rev ( w ) * radian_correction;
	M = rev ( M + 360.0 / ( 1.00004024 * pow ( a , 1.5 ) ) ) * radian_correction;

	E = M + e * sin ( M ) * ( 1.0 +e * cos ( M ) );
	OblEcl = rev ( 23.4393 - 0.0000003563 * d ) * radian_correction;

	E1 = E - ( E - e * sin ( E ) - M ) / ( 1 - e * cos ( E ) );
	while ( ( E * 1 / radian_correction - E1 * 1 / radian_correction ) > 0.005 )
	{
		E = E1;
		E1 = E - ( E - e * sin ( E ) - M ) / ( 1 - e * cos ( E ) );
	}
	E = E1;

	xv = a * ( cos ( E ) - e );
	yv = a * ( sqrt ( 1.0 - ( e * e ) ) * sin ( E ) );
	v = atan2 ( yv, xv );
	r = sqrt ( xv * xv + yv * yv );

	coords.x = r* ( cos ( N ) * cos ( v + w ) - sin ( N ) * sin ( v + w ) * cos ( i ) );
	coords.y = r * ( sin ( N ) * cos ( v + w ) + cos ( N ) * sin ( v + w ) * cos( i ) );
	coords.z = r * ( sin ( v + w ) * sin ( i ) );

	return coords;
}

moon_info compute_earth_moon_coordinates ( double SM,  double SW, double SN, double SXs, double SYs, double crt_lat, double lst )
{
    moon_info moon;

    double N, i, w, a, e, M;
	double E, E1, OblEcl, xv, yv, v, r;
	double LS, LM, D, F;
	double d = date();

    N = rev ( 125.1228 - 0.0529538083 * d ) * radian_correction;
	i = 5.1454 * radian_correction;
	w = rev ( 318.0634 + 0.1643573223 * d ) * radian_correction;
	a = 60.2666;
	e = 0.054900;
	M = rev ( 115.3654 + 13.0649929509 * d ) * radian_correction;

    LS = SM + SW;
	// Mean Longitude
	LM = M + w + N;
	// Mean Elongation
	D = LM - LS;
	// Latitude argument
	F = LM - SN;

	E = M + e * sin ( M ) * ( 1.0 +e * cos ( M ) );
	OblEcl = rev ( 23.4393 - 0.0000003563 * d ) * radian_correction;

	E1 = E - ( E - e * sin ( E ) - M ) / ( 1 - e * cos ( E ) );
	while ( ( E * 1 / radian_correction - E1 * 1 / radian_correction ) > 0.005 )
	{
		E = E1;
		E1 = E - ( E - e * sin ( E ) - M ) / ( 1 - e * cos ( E ) );
	}
	E = E1;

	xv = a * ( cos ( E ) - e );
	yv = a * ( sqrt ( 1.0 - ( e * e ) ) * sin ( E ) );
	v = atan2 ( yv, xv );
	r = sqrt ( xv * xv + yv * yv );

	moon.x = r * ( cos ( N ) * cos ( v + w ) - sin ( N ) * sin ( v + w ) * cos ( i ) );
    moon.y = r * ( sin ( N ) * cos ( v + w ) + cos ( N ) * sin ( v + w ) * cos( i ) );
	moon.z = r * ( sin ( v + w ) * sin ( i ) );

	double lonecl = atan2 ( moon.y, moon.x );
    double latecl = atan2 ( moon.z, sqrt ( pow ( moon.x, 2) + pow ( moon.y, 2 ) ) );

    lonecl = lonecl	* 1	/ radian_correction
		+ ( -1.274 * sin ( M - 2 * D ) + 0.658 * sin ( 2 * D )
		- 0.186 * sin ( SM ) - 0.059
		* sin ( 2 * M - 2 * D ) - 0.057
		* sin ( M - 2 * D + SM ) + 0.053
		* sin ( M + 2 * D ) + 0.046
		* sin ( 2 * D - SM) + 0.041
		* sin ( M - SM) - 0.035 * sin ( D )
		- 0.031 * sin ( M + SM ) - 0.015
		* sin ( 2 * F - 2 * D ) + 0.011 * sin ( M - 4 * D ) );
	latecl = latecl * 1	/ radian_correction
		+ ( -0.173 * sin ( F - 2 * D ) - 0.055
		* sin ( M - F - 2 * D ) - 0.046
		* sin ( M + F - 2 * D ) + 0.033
		* sin ( F + 2 * D ) + 0.017 * sin ( 2 * M + F ) );

	lonecl = lonecl * radian_correction;
	latecl = latecl * radian_correction;

    r = r + ( -0.58 * cos ( M - 2 * D ) - 0.46 * cos ( 2 * D ) );

	moon.x = r * cos ( lonecl ) * cos ( latecl );
	moon.y = r * sin ( lonecl ) * cos ( latecl );
    moon.z = r * sin ( latecl );

    double xg = moon.x + SXs;
	double yg = moon.y + SYs;
	double zg = moon.z;

    double xe = xg;
	double ye = yg
        * cos ( OblEcl ) - zg
        * sin ( OblEcl );
	double ze = yg
        * sin ( OblEcl ) + zg
        * cos ( OblEcl );

	moon.RA = atan2(ye, xe);
	moon.Dec = atan2(ze, sqrt ( pow ( xe, 2 ) + pow ( ye, 2 ) ) );

    // The previously computed position was the geocentric one.
	// For the topocentric we need to take into account the flattening of
	// Earth:
	double mpar = 1 / a;
	double gclat = ( crt_lat * 1 / radian_correction - 0.1924 * sin(2 * crt_lat ) )	* radian_correction;
    double rho = 0.99833 + 0.00167 * cos ( 2 * crt_lat );
	double ha = lst - moon.RA;
	double g = atan ( tan ( gclat ) / cos ( ha ) );
	moon.RA = moon.RA - mpar * rho * cos ( gclat ) * sin ( ha )	/ cos ( moon.Dec );
    moon.Dec = moon.Dec - mpar * rho * sin ( gclat ) * sin ( g - moon.Dec) / sin ( g );

	//moon.rg = r;
	//moon.rh = 1;

	return moon;
}

coordinates_info compute_moon_coordinates ( double N, double i, double w, double a, double e, double M, int epoch )
{
    coordinates_info coords;

	double E, E1, OblEcl, xv, yv, v, r;
	double d = date();


	N = rev ( N + 0.013967 * ( 2000 - epoch ) + 0.0000382394 * d ) * radian_correction;
	i = rev ( i ) * radian_correction;
	w = rev ( w ) * radian_correction;
	a = a / AU_TO_KM ;
	M = rev ( M + 360.0 / ( 1.00004024 * pow ( a , 1.5 ) ) ) * radian_correction;

	E = M + e * sin ( M ) * ( 1.0 +e * cos ( M ) );
	OblEcl = rev ( 23.4393 - 0.0000003563 * d ) * radian_correction;

	E1 = E - ( E - e * sin ( E ) - M ) / ( 1 - e * cos ( E ) );
	while ( ( E * 1 / radian_correction - E1 * 1 / radian_correction ) > 0.005 )
	{
		E = E1;
		E1 = E - ( E - e * sin ( E ) - M ) / ( 1 - e * cos ( E ) );
	}
	E = E1;

	xv = a * ( cos ( E ) - e );
	yv = a * ( sqrt ( 1.0 - ( e * e ) ) * sin ( E ) );
	v = atan2 ( yv, xv );
	r = sqrt ( xv * xv + yv * yv );

	coords.x = r * ( cos ( N ) * cos ( v + w ) - sin ( N ) * sin ( v + w ) * cos ( i ) );
    coords.y = r * ( sin ( N ) * cos ( v + w ) + cos ( N ) * sin ( v + w ) * cos( i ) );
	coords.z = r * ( sin ( v + w ) * sin ( i ) );

    return coords;
}

void initialize_dso ( dso_info *dso, double lst, double phi, float scale_factor )
{
	static char line[150], *tmp;
	FILE *f = NULL;
	long i;
	float scale_factor_stellar = /*3.3 * 63241.1f **/ scale_factor;
	double h, sina, glat, x, y, z, zalt, a;
	double r1, d1;
	double rvect;
	double nudge = 2 / 100.0;

	f = fopen ( "data/dso.dat", "rt" );
	if ( f == NULL ) {
		printf ( "Error opening the dso.dat file" );
		exit ( 1 );
	}


	#ifdef DEBUG
	printf ( "Deep Space Object (DSO) information:\n" );
	#endif

	i = 0;
	while ( fgets ( line, 150, f ) != NULL )
	{
		tmp = line;

		sscanf ( tmp, "%s %lf %lf %lf %s\n", dso[i].name, &dso[i].RA, &dso[i].Dec, &dso[i].dist, dso[i].type );

        dso[i].dist /= 	3.26156;

		r1 = dso[i].RA;
		d1 = dso[i].Dec;
        r1 = r1 * radian_correction;
        d1 = d1 * radian_correction;

		rvect = dso[i].dist * scale_factor_stellar * cos ( d1 );
		dso[i].x = rvect * cos ( r1 );
		dso[i].y = rvect * sin ( r1 );
		dso[i].z = dso[i].dist * scale_factor_stellar * sin ( d1 );

		h = lst - r1;
		glat = phi;
		sina = sin ( d1 ) * sin ( glat ) + cos ( d1 ) * cos ( glat ) * cos ( h );
		a = asin ( sina );
		y = -cos ( d1 ) * cos ( glat ) * sin ( h );
		x = sin ( d1 ) - sin ( glat ) * sina;
		z = atan2 ( y, x );

		if (a > 0)
		{
				x = cos ( z ) * tan ( ( zalt - a ) / 2 );
				y = sin ( z ) * tan ( ( zalt - a ) / 2 );
				if  ( x > 0 )
					 x = x + 1 * nudge;
				else
					 x = x + -1 * nudge;
				if ( y > 0 )
					 y = y + 1 * nudge;
				else
					 y = y + -1 * nudge;

	         dso[i].visible = 1;
		}
		dso[i].xp = x;
		dso[i].yp = y;

		i++;
	}

	actual_no_dsos = --i;

	#ifdef DEBUG
	printf ( "\tNo. of DSOs read: %ld\n", actual_no_dsos );
	#endif

    fclose ( f );

}

void initialize_stars ( star_info *star, float scale_factor )
{
	static char line[150], *tmp;
	FILE *f = NULL;
	long i;
	float rvect;
	float scale_factor_stellar = /*3.3 * 63241.1f **/ scale_factor;

	f = fopen ( "data/stars_details.dat", "rt" );
	if ( f == NULL ) {
		printf ( "Error opening the stars_details.dat file" );
		exit ( 1 );
	}


	#ifdef DEBUG
	printf ( "Star information:\n" );
	#endif

	i = 0;
	while ( fgets ( line, 150, f ) != NULL )
	{
		tmp = line;
		sscanf ( tmp, "%s %s %s %s %s %s %s %lf %lf %lf %lf %lf %s %lf\n", star[i].id, star[i].hip, star[i].hd, star[i].hr, star[i].gliese, star[i].bayer_flamsteed, star[i].name, &star[i].RA, &star[i].Dec, &star[i].dist, &star[i].magn, &star[i].abs_magn, star[i].spectrum, &star[i].bv );

		rvect = star[i].dist * scale_factor_stellar * cos ( star[i].Dec * radian_correction );
		star[i].x = rvect * cos ( star[i].RA * radian_correction * 15 );
		star[i].y = rvect * sin ( star[i].RA * radian_correction * 15 );
		star[i].z = star[i].dist * scale_factor_stellar * sin ( star[i].Dec * radian_correction );
		i++;
	}

	actual_no_stars = --i;

	#ifdef DEBUG
	printf ( "\tNo. of stars read: %ld\n", actual_no_stars );
	#endif

   fclose ( f );
}

void initialize_constellations ( constellation_info *constellation, constellation_boundary_info *constellation_boundary, constellation_name_info *constellation_name, beyer_info *beyer, double lst, double phi, float scale_factor )
{
	FILE *f = NULL;
	char line[150], *tmp;
	int i, j, r1, r2, d1, d2, new_constellation;

	#ifdef DEBUG
	printf ( "Reading constellation coordinates, boundaries, names and important stars\n" );
	#endif

	f = fopen ( "data/conlines.dat", "rt" );
    if ( f == NULL )
	{
		printf ( "Error opening the conlines.dat file." );
		exit ( 1 );
	}

	i = 0;
	while ( fgets ( line, 150, f ) != NULL )
	{
		tmp = line;
		sscanf ( tmp, "%s %d %d %d %d\n", constellation[i].name, &constellation[i].r1, &constellation[i].d1, &constellation[i].r2, &constellation[i].d2 );
		compute_constellation_line_coordinates ( constellation, i, lst, phi, scale_factor );
		i++;
	}

	fclose ( f );

	#ifdef DEBUG
	printf("\tNo. of constellations lines read: %d\n", --i);
	#endif

	f = fopen ( "data/cbounds.dat", "rt" );
   if ( f == NULL )
	{
		printf ( "Error opening the cbounds.dat file." );
		exit ( 1 );
	}

	i = 0;
	j = 0;
	r1 = 0;
	r2 = 0;
	d1 = 0;
	d2 = 0;
	new_constellation = 0;
	while ( fgets ( line, 150, f ) != NULL )
	{
		tmp = line;
		sscanf ( tmp, "%d %d %d\n", &new_constellation, &r2, &d2 );
		if ( i-1 > -1 && new_constellation)
		{
			compute_constellation_boundary_coordinates ( constellation_boundary, r1, d1, r2, d2, j, lst, phi, scale_factor );
			j++;
		}
		i++;
		r1 = r2;
		d1 = d2;
	}

	fclose ( f );

	#ifdef DEBUG
	printf("\tNo. of constellations boundaries read: %d\n", --j);
	#endif

	f = fopen ( "data/cnames.dat", "rt" );
   if ( f == NULL )
	{
		printf ( "Error opening the cnames.dat file." );
		exit ( 1 );
	}

	i = 0;
	while ( fgets ( line, 150, f ) != NULL )
	{
		tmp = line;
		sscanf ( tmp, "%d %d %s", &constellation_name[i].r, &constellation_name[i].d, constellation_name[i].name );
		compute_constellation_name_coordinates ( constellation_name, i, lst, phi, scale_factor );
		i++;
	}

	fclose ( f );

	#ifdef DEBUG
	printf("\tNo. of constellation names read: %d\n", --i);
	#endif

	f = fopen ( "data/beyer.dat", "rt" );
	if ( f == NULL )
	{
		printf ( "Error opening the beyer.dat file." );
		exit ( 1 );
	}

	i = 0;
	while ( fgets ( line, 150, f ) != NULL )
	{
		tmp = line;
		sscanf ( tmp, "%s %f %f", beyer[i].name, &beyer[i].r, &beyer[i].d );
		compute_beyer_name_coordinates ( beyer, i, lst, phi, scale_factor );
		i++;
	}

	fclose ( f );

	#ifdef DEBUG
	printf("\tNo. of Beyer stars in constellations read: %d\n", --i);
	#endif

}

static void compute_constellation_line_coordinates ( constellation_info *constellation, int index, double lst, double phi, float scale_factor )
{
	int plotted = 0;
	double h, sina, glat, x, y, x1, y1, z1, x2, y2, z2, a1, a2, aq, bq, cq, dg, xr1, xr2, yr1, yr2, atop, ztop, abot, zbot, p, q, M, c;
	double r1, r2, d1, d2;
	double rvect, sphere_radius;
	double zalt = 1.570796;
	sphere_radius = scale_factor /** scale_factor / 2*/;

	r1 = constellation[index].r1;
	r2 = constellation[index].r2;
	d1 = constellation[index].d1;
	d2 = constellation[index].d2;

	r1 = r1 / 1000 * radian_correction * 15;
	d1 = d1 / 100 * radian_correction;

	r2 = r2 / 1000 * radian_correction * 15;
	d2 = d2 / 100 * radian_correction;

	rvect = sphere_radius * cos ( d1 );
	constellation[index].x1 = rvect * cos ( r1 );
	constellation[index].y1 = rvect * sin ( r1 );
	constellation[index].z1 = sphere_radius * sin ( d1 );
	rvect = sphere_radius * cos ( d2 );
	constellation[index].x2 = rvect * cos ( r2 );
	constellation[index].y2 = rvect * sin ( r2 );
	constellation[index].z2 = sphere_radius * sin ( d2 );

	// First end
	h = lst - r1;
	glat = phi;

	sina = sin ( d1 ) * sin ( glat ) + cos ( d1 ) * cos ( glat ) * cos ( h );
	a1 = asin ( sina );
	y = -cos ( d1 ) * cos ( glat ) * sin ( h );
	x = sin ( d1 ) - sin ( glat ) * sina;
	z1 = atan2 ( y, x );

	// Second end
	h = lst - r2;
	glat = phi;

	sina = sin ( d2 ) * sin ( glat ) + cos ( d2 ) * cos ( glat ) * cos ( h );
	a2 = asin ( sina );
	y = -cos ( d2 ) * cos ( glat ) * sin ( h );
	x = sin ( d2 ) - sin ( glat ) * sina;
	z2 = atan2 ( y, x );

	// Case II
	// Both ends are above the horizon
	if ( ( a1 > 0 ) && ( a2 > 0 ) )
	{
		x1 = cos ( z1 ) * tan ( ( zalt - a1 ) / 2 );
		y1 = sin ( z1 ) * tan ( ( zalt - a1 ) / 2 );
		x2 = cos ( z2 ) * tan ( ( zalt - a2 ) / 2 );
		y2 = sin ( z2 ) * tan ( ( zalt - a2 ) / 2 );
		plotted = 1;
		constellation[index].visible = 1;
	}
	// Case III
	// One end above ant the other bellow the horizon
	if ( ( plotted != 1 ) && ( ( a1 * a2 ) < 0 ) )
	{
		if ( a1 > 0 )
		{
			atop = a1;
			ztop = z1;
			abot = a2;
			zbot = z2;
		}
		else
		{
			atop = a2;
			ztop = z2;
			abot = a1;
			zbot = z1;
		}
		x1 = cos ( ztop ) * tan ( ( zalt - atop ) / 2 );
		y1 = sin ( ztop ) * tan ( ( zalt - atop ) / 2 );
		x1 = cos ( zbot ) * tan ( ( zalt - abot ) / 2 );
		y1 = sin ( zbot ) * tan ( ( zalt - abot ) / 2 );

		M = ( y1 - y2 ) / ( x1 - x2 );
		c = y1 - M * x1;

		aq = M * M + 1;
		bq = 2 * c * M;
		cq = c * c - 1;

		// Case V		dg = bq * bq - 4 * aq * cq;
		xr1 = ( -bq + sqrt ( dg ) ) / ( 2 * aq );
		xr2 = ( -bq - sqrt ( dg ) ) / ( 2 * aq );
		yr1 = M * xr1 + c;
		yr2 = M * xr2 + c;
		p = between ( x1, x2, xr1 );
		q = between ( y1, y2, yr1 );

		if ( ( p > 0 ) && ( q > 0 ) )
		{
			x2 = xr1;
			y2 = yr1;
		}
		p = between ( x1, x2, xr2 );
		q = between ( y1, y2, yr2 );
		if ( ( p > 0 ) && ( q > 0 ) )
		{
			x2 = xr2;
			y2 = yr2;
		}
		plotted = 1;
		constellation[index].visible = 1;
	}
	constellation[index].xp1 = x1;
	constellation[index].yp1 = y1;
	constellation[index].zp1 = 0.0;
	constellation[index].xp2 = x2;
	constellation[index].yp2 = y2;
	constellation[index].zp2 = 0.0;
}

static void compute_constellation_boundary_coordinates ( constellation_boundary_info *constellation_boundary, double r1, double d1, double r2, double d2, int index, double lst, double phi, float scale_factor )
{
	int plotted = 0;
	double h, sina, glat, x, y, x1, y1, z1, x2, y2, z2, a1, a2, aq, bq, cq, dg, xr1, xr2, yr1, yr2, atop, ztop, abot, zbot, p, q, M, c;
	double rvect, sphere_radius;
	double zalt = 1.570796;
	sphere_radius = scale_factor /** scale_factor / 2*/;

	r1 = r1 / 1000 * radian_correction * 15;
	d1 = d1 / 100 * radian_correction;

	r2 = r2 / 1000 * radian_correction * 15;
	d2 = d2 / 100 * radian_correction;

	rvect = sphere_radius * cos ( d1 );
	constellation_boundary[index].x1 = rvect * cos ( r1 );
	constellation_boundary[index].y1 = rvect * sin ( r1 );
	constellation_boundary[index].z1 = sphere_radius * sin ( d1 );
	rvect = sphere_radius * cos ( d2 );
	constellation_boundary[index].x2 = rvect * cos ( r2 );
	constellation_boundary[index].y2 = rvect * sin ( r2 );
	constellation_boundary[index].z2 = sphere_radius * sin ( d2 );

	// First end
	h = lst - r1;
	glat = phi;

	sina = sin ( d1 ) * sin ( glat ) + cos ( d1 ) * cos ( glat ) * cos ( h );
	a1 = asin ( sina );
	y = -cos ( d1 ) * cos ( glat ) * sin ( h );
	x = sin ( d1 ) - sin ( glat ) * sina;
	z1 = atan2 ( y, x );

	// Second end
	h = lst - r2;
	glat = phi;

	sina = sin ( d2 ) * sin ( glat ) + cos ( d2 ) * cos ( glat ) * cos ( h );
	a2 = asin ( sina );
	y = -cos ( d2 ) * cos ( glat ) * sin ( h );
	x = sin ( d2 ) - sin ( glat ) * sina;
	z2 = atan2 ( y, x );

	// Case II
	// Both ends are above the horizon
	if ( ( a1 > 0 ) && ( a2 > 0 ) )
	{
		x1 = cos ( z1 ) * tan ( ( zalt - a1 ) / 2 );
		y1 = sin ( z1 ) * tan ( ( zalt - a1 ) / 2 );
		x2 = cos ( z2 ) * tan ( ( zalt - a2 ) / 2 );
		y2 = sin ( z2 ) * tan ( ( zalt - a2 ) / 2 );
		plotted = 1;
		constellation_boundary[index].visible = 1;
	}
	// Case III
	// One end above ant the other bellow the horizon
	if ( ( plotted != 1 ) && ( ( a1 * a2 ) < 0 ) )
	{
		if ( a1 > 0 )
		{
			atop = a1;
			ztop = z1;
			abot = a2;
			zbot = z2;
		}
		else
		{
			atop = a2;
			ztop = z2;
			abot = a1;
			zbot = z1;
		}
		x1 = cos ( ztop ) * tan ( ( zalt - atop ) / 2 );
		y1 = sin ( ztop ) * tan ( ( zalt - atop ) / 2 );
		x1 = cos ( zbot ) * tan ( ( zalt - abot ) / 2 );
		y1 = sin ( zbot ) * tan ( ( zalt - abot ) / 2 );

		M = ( y1 - y2 ) / ( x1 - x2 );
		c = y1 - M * x1;

		aq = M * M + 1;
		bq = 2 * c * M;
		cq = c * c - 1;

		// Case V		dg = bq * bq - 4 * aq * cq;
		xr1 = ( -bq + sqrt ( dg ) ) / ( 2 * aq );
		xr2 = ( -bq - sqrt ( dg ) ) / ( 2 * aq );
		yr1 = M * xr1 + c;
		yr2 = M * xr2 + c;		p = between ( x1, x2, xr1 );
		q = between ( y1, y2, yr1 );

		if ( ( p > 0 ) && ( q > 0 ) )
		{
			x2 = xr1;
			y2 = yr1;
		}
		p = between ( x1, x2, xr2 );
		q = between ( y1, y2, yr2 );
		if ( ( p > 0 ) && ( q > 0 ) )
		{
			x2 = xr2;
			y2 = yr2;
		}
		plotted = 1;
		constellation_boundary[index].visible = 1;
	}
	constellation_boundary[index].xp1 = x1;
	constellation_boundary[index].yp1 = y1;
	constellation_boundary[index].zp1 = 0.0;
	constellation_boundary[index].xp2 = x2;
	constellation_boundary[index].yp2 = y2;
	constellation_boundary[index].zp2 = 0.0;
}

static void compute_constellation_name_coordinates ( constellation_name_info *constellation_name, int index, double lst, double phi, float scale_factor )
{
	double h, sina, glat, x, y, z, zalt, a;
	double r1, d1;
	double rvect, sphere_radius;
	sphere_radius = scale_factor /** scale_factor / 2*/;

	r1 = constellation_name[index].r;
	d1 = constellation_name[index].d;
   r1 = r1 / 1000 * radian_correction * 15;
   d1 = d1 / 100 * radian_correction;

	rvect = sphere_radius * cos ( d1 );
	constellation_name[index].x = rvect * cos ( r1 );
	constellation_name[index].y = rvect * sin ( r1 );
	constellation_name[index].z = sphere_radius * sin ( d1 );
	constellation_name[index].visible = 1;

	h = lst - r1;
	glat = phi;
	sina = sin ( d1 ) * sin ( glat ) + cos ( d1 ) * cos ( glat ) * cos ( h );
	a = asin ( sina );
	y = -cos ( d1 ) * cos( glat ) * sin ( h );
	x = sin ( d1 ) - sin ( glat ) * sina;
	z = atan2 ( y, x );
   if ( a > 0 )
	{
		x = cos ( z ) * tan ( ( zalt - a ) / 2 );
		y = sin ( z ) * tan ( ( zalt - a ) / 2 );
		constellation_name[index].visible = 1;
   }
	constellation_name[index].xp = x;
	constellation_name[index].yp = y;
	constellation_name[index].zp = z;
}

static void compute_beyer_name_coordinates ( beyer_info *beyer, int index, double lst, double phi, float scale_factor )
{
	double h, sina, glat, x, y, z, zalt, a;
	double r1, d1;
	double rvect, sphere_radius;
	double nudge = 2 / 100.0;
	sphere_radius = scale_factor /** scale_factor / 2*/;

	r1 = beyer[index].r;
	d1 = beyer[index].d;
   r1 = r1 * radian_correction * 15;
   d1 = d1 * radian_correction;

	rvect = sphere_radius * cos ( d1 );
	beyer[index].x = rvect * cos ( r1 );
	beyer[index].y = rvect * sin ( r1 );
	beyer[index].z = sphere_radius * sin ( d1 );
   beyer[index].visible = 1;

	h = lst - r1;
	glat = phi;
	sina = sin ( d1 ) * sin ( glat ) + cos ( d1 ) * cos ( glat ) * cos ( h );
	a = asin ( sina );
	y = -cos ( d1 ) * cos ( glat ) * sin ( h );
	x = sin ( d1 ) - sin ( glat ) * sina;
	z = atan2 ( y, x );

	if (a > 0)
	{
			x = cos ( z ) * tan ( ( zalt - a ) / 2 );
			y = sin ( z ) * tan ( ( zalt - a ) / 2 );
			if  ( x > 0 )
			 x = x + 1 * nudge;
			else
				 x = x + -1 * nudge;
			if ( y > 0 )
				 y = y + 1 * nudge;
			else
				 y = y + -1 * nudge;
         beyer[index].visible = 1;
	}
	beyer[index].xp = x;
	beyer[index].yp = y;
	beyer[index].zp = z;
}
