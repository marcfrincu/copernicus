#define MOUSE_SENSITIVITY 0.5

#define GSTAR_TEXTURE 0
#define MERCURY_TEXTURE 1
#define VENUS_TEXTURE 2
#define EARTH_TEXTURE 3
#define MARS_TEXTURE 4
#define JUPITER_TEXTURE 5
#define SATURN_TEXTURE 6
#define URANUS_TEXTURE 7
#define NEPTUNE_TEXTURE 8

#define VENUSCLOUDS_TEXTURE 9
#define EARTHCLOUDS_TEXTURE 10
#define GSTARBILLBOARD_TEXTURE 11
#define RINGS_TEXTURE 12
#define ASTEROID_TEXTURE 13

#define ASTAR_TEXTURE 14
#define BSTAR_TEXTURE 15
#define OSTAR_TEXTURE 16
#define MSTAR_TEXTURE 17

#define HALO_TEXTURE 18
#define LARGE_HALO_TEXTURE 19
#define SMALL_HALO_TEXTURE 20
#define GLOW_TEXTURE 21

#define MSTARBILLBOARD_TEXTURE 22
#define ASTARBILLBOARD_TEXTURE 23
#define OSTARBILLBOARD_TEXTURE 24

#define PARTICLE_TEXTURE 25

void stop_mouse_drag ( int x, int y );
void start_mouse_drag ( int x, int y );
void calculate_delta_t ( void );

void glut_print_string(double x, double y, double z, const char *string);
void draw_2d_stuff ( void );

void init_gl( void );
