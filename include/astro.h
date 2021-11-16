#pragma once

#include "rings.h"
#include "3dsloader.h"
#include <GL/glut.h>

#define NO_PLANETS 9
#define NO_STARS 87476
#define NO_CONSTELLATION_LINES 645
#define NO_CONSTELLATION_BOUNDARIES 773
#define NO_CONSTELLATION 88
#define NO_BEYER 248
#define NO_DSO_OBJECTS 168
#define NO_MOONS 100
#define NO_CENTAURS 147
#define NO_TRANSNEPTUNES 755
#define NO_ASTEROIDS 1000

#define AU_TO_KM 149597870.691
#define LY_TO_KM 9460730472580.8
#define SECONDS_IN_DAY 86400

typedef struct
{
    double x, y, z;
} coordinates_info;

typedef struct
{
	double q, Q, H, N, i, w, a, e, M;
	double x, y, z, day;
	long epoch;
	int year, month;
	char name[100];
} asteroid_info;

typedef struct
{
	double N, i, w, a, e, M;
	double x, y, z;
	double RA, Dec;
	double size;
	int epoch;
	char name[100], texture_name[100];
	GLuint texture_id[1];
} moon_info;

typedef struct
{
	double RA, Dec, dist;
	double x, y, z, xp, yp;
	char type[100];
	char name[100], texture_name[100];
	GLuint texture_id;
	int visible;
} dso_info;

typedef struct {
	int r1, d1, r2, d2;
	double x1, y1, z1, x2, y2, z2, xp1, yp1, zp1, xp2, yp2, zp2;
	int visible;
	int new_constellation;
} constellation_boundary_info;

typedef struct {
	int r1, d1, r2, d2;
	char name[50];
	double x1, y1, z1, x2, y2, z2, xp1, yp1, zp1, xp2, yp2, zp2;
	int visible;
} constellation_info;

typedef struct {
	int r, d;
	char name[50];
	double x, y, z, xp, yp, zp;
	int visible;
} constellation_name_info;

typedef struct {
	float r, d;
	char name[50];
	double x, y, z, xp, yp, zp;
	int visible;
} beyer_info;

extern asteroid_info centaur[NO_CENTAURS], transneptune[NO_TRANSNEPTUNES], asteroid[NO_ASTEROIDS];
extern moon_info moon[NO_MOONS];
extern dso_info dso_object[NO_DSO_OBJECTS];
extern constellation_boundary_info constellation_boundary[NO_CONSTELLATION_BOUNDARIES];
extern constellation_info constellation[NO_CONSTELLATION_LINES];
extern constellation_name_info constellation_name[NO_CONSTELLATION];
extern beyer_info beyer[NO_BEYER];
extern constellation_info *p_constellation;
extern constellation_boundary_info *p_constellation_boundary;
extern constellation_name_info *p_constellation_name;
extern beyer_info *p_beyer;
extern dso_info *p_dso_object;
extern asteroid_info *p_centaur, *p_transneptune, *p_asteroid;
extern obj_type asteroids, phobos, deimos;

typedef struct {
	char id[50], hip[50], name[50], hd[50], hr[50], gliese[50], bayer_flamsteed[50], spectrum[50];
	double x, y, z, dist;
	double RA, Dec, magn, abs_magn, color[3];
	double bv;
} star_info;

extern star_info star[NO_STARS];
extern star_info *p_star;
extern long actual_no_stars;
extern long actual_no_dsos;
extern long actual_no_transneptunes;
extern long actual_no_centaurs;
extern long actual_no_asteroids;

typedef struct
{
	double N, i, w, a, e, M;
	double magn, phase, elong, RA, Dec;
	double xe, ye, ze;
	double rises, sets, transits;
	int is_circumpolar, is_rising;
	double size, distFromSun;
	double tilt_x, tilt_z;
	const char* name;
	double xp, yp, xs, ys;
	//data for the construction of rings
	int has_rings;
	float inner_ring, outer_ring;
	vertex_data *ring_vertex_list;
	int rings_initialized;
	int *rings_indices;
	//data for the moons
	moon_info *moon;
	int no_moons;
	float moon_scale_factor;
} planet_info;

extern planet_info planet[NO_PLANETS];
extern planet_info *p_planet;

extern int date_changed;
extern int year, month, day, hour;
extern double lst, julian_date, my_long, my_lat;

double date ( void );
double compute_julian_date ( int year, int month, int day );
void compute_sun_coordinates ( planet_info *planet, double lst, double phi  );
void compute_planet_coordinates ( planet_info *planet, int index, double lst, double phi );
void initialize_planets ( planet_info *planet, double lst, double phi );
void initialize_stars ( star_info *star, float scale_factor );
void initialize_constellations ( constellation_info *constellation, constellation_boundary_info *constellation_boundary, constellation_name_info *constellation_name, beyer_info *beyer, double lst, double phi, float scale_factor );
void initialize_dso ( dso_info *dso, double lst, double phi, float scale_factor );
void initialize_other_ss_objects ( planet_info *planet, asteroid_info *asteroid, asteroid_info *centaur, asteroid_info *transneptune, double lst, double phi );
