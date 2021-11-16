#include "astro.h"

#ifndef __linux__
#include <windows.h>
#endif
#include <GL/glut.h>

typedef struct {
	double elapsedTime;
	double currentTime;
	double lastTime;
} time_camera;

typedef struct {
	double posx;
    double posy;
    double posz;
} camera_coordinates;

typedef struct {

	camera_coordinates coords;

    float upx;
	float upy;
    float upz;

	float azimuth;
	float elevation;

	float speed;

} world_camera;

extern double cam_eye[3], cam_pos[3], cam_up[3];

extern world_camera camera;
extern time_camera delta_t;
extern double m_Frustrum[6][4];

void move_camera ( planet_info *planet, asteroid_info *asteroid, asteroid_info *centaur, asteroid_info *transneptune, dso_info *dso, star_info *star, float scale_factor );
void aim_camera ( void );
void initial_camera_position ( double x, double y, double z );

void update_frustum ( void );
int is_point_in_frustum ( double x, double y, double z, double radius );

void aim_camera( float centerx, float centery, float centerz, float length );

