#include "astro.h"
#include "copernicus.h"
#include "3dsloader.h"
#include "particles.h"

#define STAR_SYSTEM_RANGE 1000
#define NO_TEXTURES 26
#define NO_DSO_TEXTURES 300
#define DETAIL_LIMIT 500

/* this macro by Sebastien Metrot */
#define ROTATE(V) \
do {               \
   y = V[1] * cosbeta + V[2] * sinbeta; \
   z = -V[1] * sinbeta + V[2] * cosbeta;  \
   x = V[0] * cosalfa - z * sinalfa;  \
   z = V[0] * sinalfa + z * cosalfa;  \
   V[0] = x; V[1] = y; V[2] = z;         \
} while (0);

extern GLuint texture_id[NO_TEXTURES];
extern GLuint texture_dso_id[NO_DSO_TEXTURES];

void draw_sun ( double size, int tex_index, char spectrum );
void draw_planet ( double size, int segs, int tex_index, unsigned char r, unsigned char g,unsigned char b );
void draw_orbit ( double xCenter, double yCenter, double zCenter, double radius );
void draw_sun_billboard ( double size, int tex_index, double object_pos[] );
void draw_planets ( GLenum mode, planet_info *planet, float scale_factor, int slices );
void draw_rings ( planet_info *planet, float scale_factor, float slices );
void draw_closest_star_names ( star_info *star, int show_star );
void draw_star_names ( star_info *star );
void draw_stars ( star_info *star, float scale_factor );
void draw_constellation_3d ( constellation_info *constellation, constellation_boundary_info *constellation_boundary, constellation_name_info *constellation_name, beyer_info *beyer );
void draw_dso_objects ( dso_info *dso, float scale_factor );
void draw_asteroids ( obj_type_ptr asteroid, asteroid_info *asteroid, asteroid_info *centaur, asteroid_info *transneptune, float scale );
void draw_mars_satellites( obj_type_ptr deimos, obj_type_ptr phobos, planet_info *planet, float scale_factor );
void draw_starchart ( const char* filename, star_info *star, dso_info *dso, double starmap_scale_factor, int width, int height, int offset );
void draw_flare ( float r, float g, float b, float a, float scale, double flare_pos[], int text_id );
void make_lens_flares ( float x, float y, float z, float size );
void draw_comet_tail ( particle *particles, int text_id, float sun_pos[], float object_pos[], float scale_factor );
