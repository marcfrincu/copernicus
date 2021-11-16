/*
 * Copernicus Space Simulator v1.6 for OpenGL
 * Written by Marc Frincu (c) 2005-2010
 * Email: mfrincu@info.uvt.ro
 * This software is distributed under the terms of this General Public License.
 * For details see the license file attached to the archive
 */

 /*
 TODO wish list:
 1) GUI in GL: change date and time, goto for star with proper name and DSOs, ps starcharts, etc. (TODO 5)
 2) 3DS loader capable of loading objects with multiple textures. Test case: constellation images
 3) Build starchart generator: for stars, galaxies, solar system, polar and stereographic projection etc.
 4) Particle system for comet tails (DONE), black holes, planetary rings
 5) Extra solar system planets. When near a star with planets display them (TODO 2)
 6) Lens flares for Sun (DONE) and stars (DONE).
 7) Shadows (TODO 6)
 8) Bump maps (TODO 4)
 9) Billboards for stars (DONE)
 10) Add comets (TODO 1)
 11) Add Moon (DONE)
 12) GoTo system (TODO 3)
 13) Add DSO images (DONE)
 14) Collision detection (DONE)

 */

#ifndef __linux__
#include <windows.h>
#endif
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <float.h>
#include <string.h>

#include "include/tgaload.h"
#include "include/3dsloader.h"
#include "include/camera.h"
#include "include/util.h"
#include "include/copernicus.h"
#include "include/objects.h"
#include "include/astro.h"
#include "include/rings.h"
#include "include/particles.h"

#include "vec3d.c"

/* Variables for managing the screen and camera movement */
int screen_width, screen_height;
int  vp_centre_x, vp_centre_y;
int  old_x, old_y;
int  depressed_button = 0;
float aspect_ratio;
double camera_speed_km;
double old_camera_distance, old_day_number;

const char *selected_object_name = "";

/* Variables defining the light properties */
GLfloat   ambient_light[] = { 1.0f, 1.0f, 1.0f, 1.0 };
GLfloat   specular_light[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat   diffuse_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat   light_pos[] = { 0.0f, 0.0f, 0.0f, 1.0 };

int slices = 45;
float scale_factor = 1000.0f;
int planet_list, star_list, star_names_list, ring_list, constellation_list_3d, render_axis_list, render_orbits_list, dso_list, render_asteroid_list, render_mars_moons_list;

int show_star = 0;

double z_far = DBL_MAX - 1;//3000000 * 1000;

int show_axis = 1, show_orbits = 1, show_constellations = 1, show_dso = 1, show_asteroids = 1, show_star_names = 0;

obj_type asteroids, phobos, deimos;

char* pwd;

// Handles the scene rotation - independently from the user navigation
float scene_rotation[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
float scene_azimuth = 0, scene_elevation = 0;

static void compute_camera_speed ( double day_number, double distance );
static void render_axis ( void );
static void render_orbits ( void );

static void handle_pick ( int x, int y );
static void process_hits ( GLint hits, GLuint buffer[] );

int gotoCmd = 0;
float dx,dy,dz,length;

/* function called whenever redisplay ís needed */
void display( void )
{
	calculate_delta_t ( );

	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glLoadIdentity ( );
	draw_2d_stuff ( );

	glPushMatrix ( );
		light_pos[0] = planet[0].xe;
		light_pos[1] = planet[0].ye;
		light_pos[2] = planet[0].ze;

      //  if (!gotoCmd)
       // {
        aim_camera ( );
        move_camera ( p_planet, p_asteroid, p_centaur, p_transneptune, p_dso_object, p_star, scale_factor  );

        glEnable ( GL_LIGHTING );
		glLightfv( GL_LIGHT0, GL_POSITION, light_pos );
        //printf("%f %f\n", camera.azimuth, camera.elevation);
      //  }
     //   else
     //   {
     //           aim_camera ( dx, dy, dz, length );
     //           move_camera ( );
      //  }

        glRotatef ( scene_elevation, 1, 0, 0 );
        glRotatef ( scene_azimuth, 0, 0, 1 );
        glCallList ( ring_list );

		if ( show_star_names )
        {
            glPushMatrix ( );
                glCallList ( star_names_list );
            glPopMatrix ( );
        }

        if  ( show_dso )
		{
			glPushMatrix ( );
				//glCallList ( dso_list );
				draw_dso_objects ( p_dso_object, scale_factor );
			glPopMatrix ( );
		}

		//glCallList(planet_list);
		draw_planets ( GL_RENDER, p_planet, scale_factor, slices );

		glPushMatrix ( );
			glCallList ( star_list );
		glPopMatrix ( );

		if  ( show_constellations )
		{
			glPushMatrix ( );
                glCallList ( constellation_list_3d );
			glPopMatrix ( );
		}
		if  ( show_axis )
			glCallList ( render_axis_list );

		if  ( show_orbits )
			glCallList ( render_orbits_list );

        glPushMatrix ( );
            draw_closest_star_names ( p_star, show_star );
        glPopMatrix ( );

		if ( show_asteroids )
		{
		    draw_asteroids ( &asteroids, p_asteroid, p_centaur, p_transneptune, scale_factor );
			//glPushMatrix ( );
				//glCallList ( render_asteroid_list );
			//glPopMatrix ( );
		}

		glPushMatrix ( );
			glCallList ( render_mars_moons_list );
		glPopMatrix ( );

        make_lens_flares ( planet[0].xe, planet[0].ye, planet[0].ze, 0 );

        glPushMatrix ( );
            float object_pos[3];
            object_pos[0] = planet[3].xe * scale_factor;
            object_pos[1] = planet[3].ye * scale_factor;
            object_pos[2] = planet[3].ze * scale_factor;
          //  draw_comet_tail ( particles, PARTICLE_TEXTURE, light_pos, object_pos, scale_factor );
        glPopMatrix ( );

	glPopMatrix ( );

	glutSwapBuffers ( );

  	glFlush ( );				/* Complete any pending operations */
}

/*
   Standard reshape function, also sets the viewing volume the first time
   its called
*/
void reshape ( int w, int h )
{
   glViewport     ( 0, 0, w, h );
   glMatrixMode   ( GL_PROJECTION );

   glLoadIdentity ( );

   if ( h == 0 )
   {
		aspect_ratio = (float)w;
   }
   else
   {
		aspect_ratio = (float)w / (float)h;
   }

   gluPerspective ( 50.0, aspect_ratio, 0.1, z_far );

   glMatrixMode   ( GL_MODELVIEW );
   glLoadIdentity ( );

   vp_centre_x = w / 2;
   vp_centre_y = h / 2;

   screen_width = w;
   screen_height = h;
}

void mouse ( int button, int state, int x, int y )
{

   if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
   {
		//Process selection
		handle_pick ( x, y );

		start_mouse_drag ( x, y );
		depressed_button = 1;
   }

   if ( button == GLUT_LEFT_BUTTON && state == GLUT_UP )
   {
		stop_mouse_drag ( x, y );
		depressed_button = 0;
   }

   if ( button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN )
   {
		start_mouse_drag ( x, y );
		depressed_button = 2;
   }
   if ( button == GLUT_RIGHT_BUTTON && state == GLUT_UP )
   {
		stop_mouse_drag ( x, y );
		depressed_button = 0;
   }
}

#define BUFSIZE 512

void handle_pick ( int x, int y )
{
	GLuint selectBuf[BUFSIZE];
	GLint viewport[4];
	GLint hits;

	glGetIntegerv ( GL_VIEWPORT, viewport );

	glSelectBuffer ( BUFSIZE, selectBuf );
	glRenderMode ( GL_SELECT );

	glMatrixMode ( GL_PROJECTION );

	glPushMatrix ( );
		glLoadIdentity ( );

	glInitNames ( );
		/* create 1x1 pixel picking region near cursor location */
		gluPickMatrix ( x, screen_height - y, 1.0, 1.0, viewport );
 		gluPerspective ( 50.0, (float) screen_width / (float) screen_height, 0.1, z_far );

		glMatrixMode(GL_MODELVIEW);

		aim_camera ( );
		move_camera ( p_planet, p_asteroid, p_centaur, p_transneptune, p_dso_object, p_star, scale_factor );

		draw_planets ( GL_SELECT, p_planet, scale_factor, slices );

		glMatrixMode ( GL_PROJECTION );
	glPopMatrix ( );

	hits = glRenderMode ( GL_RENDER );
	process_hits ( hits, selectBuf );

	glMatrixMode ( GL_MODELVIEW );

	glFlush ( );


}

void process_hits ( GLint hits, GLuint* buffer )
{
   	unsigned int i, j, selected;
   	GLuint names, *ptr;

   	printf ( "hits = %d\n", hits );
 	ptr = (GLuint *) buffer;
   	for ( i = 0; i < hits; i++ ) {	/*  for each hit  */
      		names = *ptr;
		#ifdef DEBUG
      		printf (" number of names for this hit = %d\n", names);
		#endif
		// we have a hit
		if ( names == 0 )
			selected = 1;
		else
			selected = 0;
			ptr++;
		#ifdef DEBUG
      		printf ( "  z1 is %g;", (float) *ptr / 0x7fffffff ); ptr++;
      		printf ( " z2 is %g\n", (float) *ptr / 0x7fffffff ); ptr++;
      		printf ( "   names are " );
		#endif
      		for ( j = 0; j < names; j++ )
		{ /*  for each name */
			selected_object_name = planet[*ptr].name;
			#ifdef DEBUG
         		printf ("%d ", *ptr );
			#endif
         		ptr++;
      		}
      		printf ( "\n" );
   	}
}

void idle ( void )
{
	glutPostRedisplay ( );
}

void motion ( int x, int y )
{
   	int deltaX = ( x - vp_centre_x );
	int deltaY = ( y - vp_centre_y );

	glutWarpPointer ( vp_centre_x, vp_centre_y );

	/*   fiddle with camera         */

	if ( depressed_button == 1 )
	{
	      camera.elevation += ( deltaY * MOUSE_SENSITIVITY );
	      camera.azimuth   += ( deltaX * MOUSE_SENSITIVITY );
	}
	if ( depressed_button == 2 )
	      camera.speed   += -( deltaY * MOUSE_SENSITIVITY );

	if ( camera.azimuth > 359 )
	      camera.azimuth = 1;

	if ( camera.azimuth < 1 )
	      camera.azimuth = 359;

   	/*
	Call these because the GLUT loop can only handle one message at a time,
      	we're hogging GLUTs message system with mouse coords
    	*/
   	display();

	//glutPostRedisplay();
}

/* called on key press */
void keyboard ( unsigned char key, int x, int y )
{
    double d1, d2;
   	switch ( key ) {
      	case 27:  // Escape key
         exit ( 0 );
         break;
      	case '+':
      	 d1 = distance ( planet[NO_PLANETS-1].xe * scale_factor, planet[NO_PLANETS-1].ye * scale_factor, planet[NO_PLANETS-1].ze * scale_factor, planet[0].xe * scale_factor, planet[0].ye * scale_factor, planet[0].ze * scale_factor );
      	 d2 = distance ( camera.coords.posx, camera.coords.posy, camera.coords.posz, planet[0].xe * scale_factor, planet[0].ye * scale_factor, planet[0].ze * scale_factor );
      	 if ( d1 >= d2 )
      	 {
            camera.speed += 100.0;
            show_star = 0;
      	 }
         else
         {
            if ( camera.speed < ( DBL_MAX / 2 ) )
            {
                camera.speed *= 2;
                show_star = 1;
            }
         }
         glutPostRedisplay ( );
         break;
      	case '=':
         d1 =  ( planet[NO_PLANETS-1].xe * scale_factor, planet[NO_PLANETS-1].ye * scale_factor, planet[NO_PLANETS-1].ze * scale_factor, planet[0].xe * scale_factor, planet[0].ye * scale_factor, planet[0].ze * scale_factor );
      	 d2 = distance ( camera.coords.posx, camera.coords.posy, camera.coords.posz, planet[0].xe * scale_factor, planet[0].ye * scale_factor, planet[0].ze * scale_factor );
      	 if ( d1 >= d2 )
      	 {
            camera.speed += 100.0;
            show_star = 0;
      	 }
         else
         {
            if ( camera.speed < ( DBL_MAX / 2 ) )
            {
                camera.speed *= 2;
                show_star = 1;
            }
         }
         glutPostRedisplay ( );
         break;
      	case '-':
         d1 = distance ( planet[NO_PLANETS-1].xe * scale_factor, planet[NO_PLANETS-1].ye * scale_factor, planet[NO_PLANETS-1].ze * scale_factor, planet[0].xe * scale_factor, planet[0].ye * scale_factor, planet[0].ze * scale_factor );
      	 d2 = distance ( camera.coords.posx, camera.coords.posy, camera.coords.posz, planet[0].xe * scale_factor, planet[0].ye * scale_factor, planet[0].ze * scale_factor );
      	 if ( d1 >= d2 )
      	 {
            camera.speed -= 100.0;
            show_star = 0;
      	 }
         else
         {
            camera.speed /= 2;
            show_star = 1;
         }
         glutPostRedisplay ( );
         break;
     	case 's':
         camera.speed = 0.0f;
			break;
      	case 'i':
         initial_camera_position (0.0, 0.0, -2000.0 );
         scene_elevation = 0;
         scene_azimuth = 0;
         glutPostRedisplay ( );
         break;
      	case 'a':
			if  ( !show_axis )
				show_axis = 1;
			else
				show_axis = 0;

         glutPostRedisplay ( );
         break;
      	case 'o':
			if  ( !show_orbits )
				show_orbits = 1;
			else
				show_orbits = 0;

         glutPostRedisplay ( );
         break;
      	case 'c':
			if  ( !show_constellations )
				show_constellations = 1;
			else
				show_constellations = 0;

         glutPostRedisplay ( );
         break;
      	case 'd':
			if  ( !show_dso)
				show_dso = 1;
			else
				show_dso = 0;

         glutPostRedisplay ( );
         break;
        case 'n':
			show_star_names = !show_star_names;
         glutPostRedisplay ( );
         break;

        case 'q':
			if  ( !show_asteroids )
				show_asteroids = 1;
			else
				show_asteroids = 0;

         glutPostRedisplay ( );
         break;

		case 'l':

            double dir[3], cam[3], pos[3], tmp[3];
            float len, scale;
            int i;


            float azm, alt;

            gotoCmd = 1;

            cam[0] = camera.coords.posx;
            cam[1] = camera.coords.posy;
            cam[2] = camera.coords.posz;

            pos[0] = planet[6].xe * scale_factor;
            pos[1] = planet[6].ye * scale_factor;
            pos[2] = planet[6].ze * scale_factor;



            // dest-cam vector
            vdiff ( dir, pos, cam );
            len = vlen ( dir );


            scale = len / 100;

            cartesian_to_polar (dir[0], dir[1], dir[2], &azm, &alt, &length );

            vnorm ( dir );

            //printf("%f %f\n", azm, alt);
          //  printf("%f %f %f %f\n", dir[0], dir[1], dir[2], len);

         //   for ( i=0; i<len; i=i+scale )
       //     {
//
                vscale ( tmp, dir, scale );
                printf("Tmp: %f %f %f\n", tmp[0],tmp[1],tmp[2]);
                // increment our camera position towards the destination
                vadd ( cam, cam, tmp );
                camera.coords.posx = cam[0];
                camera.coords.posy = cam[1];
                camera.coords.posz = cam[2];


                camera.azimuth = azm;
                camera.elevation = alt - 90;

                printf("Dir: %f %f %f\n", dir[0],dir[1],dir[2]);
                printf("Cam: %f %f %f\n", cam[0],cam[1],cam[2]);


                dx = pos[0];
                dy = pos[1];
                dz = pos[2];

             //   gluLookAt ( camera.coords.posx, camera.coords.posy, camera.coords.posz,
              //     camera.coords.posx + dir[0], camera.coords.posy + dir[1], camera.coords.posz + dir[2], camera.upx, camera.upy, camera.upz );

       //         aim_camera ( dx, dy, dz, length );
        //        move_camera ( );

         //   }
			//initial_camera_position ( (planet[1].xe) * scale_factor + 5, (planet[1].ye) * scale_factor + 5, (planet[1].ze) * scale_factor + 5 );
			//gluLookAt( (planet[1].xe + 40) * scale_factor, (planet[1].ze + 40) * scale_factor, (planet[1].ze + 40) * scale_factor, planet[1].xe * scale_factor, planet[1].ze * scale_factor, planet[1].ze * scale_factor, 0, 1, 0);
			break;
		case 'f':
			glutFullScreen ( );
			break;
        case ',':
            scene_azimuth += 10;
            break;
        case '.':
            scene_azimuth -= 10;
            break;

        case 'm':
            // for 50 parsec range is ok on A0
           // draw_starchart ( "test.ps", p_star, p_dso_object, 40 * 3.3 * 63241.1f * scale_factor, 1000,  1000, 1200 );
            // for 300 parsec range is ok on very large box
            draw_starchart ( "test.ps", p_star, p_dso_object, 40 * 3.3 * 63241.1f * scale_factor, 9000,  9000, 5500 );
            break;
    }
}

void keyboard_special ( int key, int x, int y )
{
   switch ( key ) {
		case GLUT_KEY_RIGHT: // right arrow
			if (camera.azimuth < 359)
				camera.azimuth += 0.1;
			else
				camera.azimuth = 0;
			break;
		case GLUT_KEY_LEFT: // left arrow
			if ( camera.azimuth > 0 )
				camera.azimuth -= 0.1;
			else
				camera.azimuth = 359;
			break;
		case GLUT_KEY_UP: // up arrow
			if ( camera.elevation > 0 )
				camera.elevation -= 1;
			else
				camera.elevation = 359;
			break;
		case GLUT_KEY_DOWN: // down arrow
			if ( camera.elevation < 359 )
				camera.elevation += 1;
			else
				camera.elevation = 0;
			break;

		case GLUT_KEY_PAGE_DOWN:
                scene_elevation += 10;
                break;
        case GLUT_KEY_PAGE_UP:
                scene_elevation -= 10;
                break;
	}
}

void init_gl ( void )
{
	float max[3] = {0.0f, 0.0f, 0.0f};
    int i;
    char file_name[255];

	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glClearColor ( 0.0, 0.0, 0.0, 0.0 );

	glEnable ( GL_DEPTH_TEST );
	glDepthFunc ( GL_LEQUAL );

	glShadeModel ( GL_SMOOTH );

	// For polygon culling
	glCullFace ( GL_BACK );
   	glEnable ( GL_CULL_FACE );

	// For lighting
	glEnable       ( GL_LIGHTING );
   	glLightModelfv ( GL_LIGHT_MODEL_AMBIENT,ambient_light );
	glLightfv      ( GL_LIGHT0, GL_AMBIENT, ambient_light );
	glLightfv      ( GL_LIGHT0, GL_SPECULAR, specular_light );
   	glLightfv      ( GL_LIGHT0, GL_DIFFUSE, diffuse_light );
	glLightfv      ( GL_LIGHT0, GL_POSITION, light_pos );
	glEnable       ( GL_LIGHT0 );

	// For textures
   	glEnable ( GL_TEXTURE_2D );
	glEnable ( GL_ALPHA_TEST );

	initial_camera_position ( 0.0, 0.0, -2000.0 );

	// For textures;
	glPixelStorei ( GL_UNPACK_ALIGNMENT,1 );
	glGenTextures ( NO_TEXTURES, texture_id );

	texture_id [ GSTAR_TEXTURE ] = tgaLoadAndBind ( "images/gstar_small.tga", TGA_LOW_QUALITY );
	texture_id [ MERCURY_TEXTURE ] = tgaLoadAndBind ( "images/mercury_small.tga", TGA_LOW_QUALITY );
	texture_id [ VENUS_TEXTURE ] = tgaLoadAndBind ( "images/venus_small.tga", TGA_LOW_QUALITY );
	texture_id [ VENUSCLOUDS_TEXTURE ] = tgaLoadAndBind ( "images/venusclouds_small.tga", TGA_LOW_QUALITY );
	texture_id [ EARTH_TEXTURE ] = tgaLoadAndBind ( "images/earth_small.tga", TGA_LOW_QUALITY );
	texture_id [ EARTHCLOUDS_TEXTURE ] = tgaLoadAndBind ( "images/earthclouds_small.tga", TGA_LOW_QUALITY );
	texture_id [ MARS_TEXTURE ] = tgaLoadAndBind ( "images/mars_small.tga", TGA_LOW_QUALITY );
	texture_id [ JUPITER_TEXTURE ] = tgaLoadAndBind ( "images/jupiter_small.tga", TGA_LOW_QUALITY );
	texture_id [ SATURN_TEXTURE ] = tgaLoadAndBind ( "images/saturn_small.tga", TGA_LOW_QUALITY );
	texture_id [ URANUS_TEXTURE ] = tgaLoadAndBind ( "images/uranus_small.tga", TGA_LOW_QUALITY );
	texture_id [ NEPTUNE_TEXTURE ] = tgaLoadAndBind ( "images/neptune_small.tga", TGA_LOW_QUALITY );
	texture_id [ GSTARBILLBOARD_TEXTURE ] = tgaLoadAndBind ( "images/gstar_billboard.tga", TGA_LOW_QUALITY );
	texture_id [ RINGS_TEXTURE ] = tgaLoadAndBind ( "images/rings.tga", TGA_LOW_QUALITY );
	texture_id [ ASTEROID_TEXTURE ] = tgaLoadAndBind ( "images/asteroid.tga", TGA_LOW_QUALITY );
	texture_id [ ASTAR_TEXTURE ] = tgaLoadAndBind ( "images/astar.tga", TGA_LOW_QUALITY );
	texture_id [ BSTAR_TEXTURE ] = tgaLoadAndBind ( "images/bstar.tga", TGA_LOW_QUALITY );
	texture_id [ OSTAR_TEXTURE ] = tgaLoadAndBind ( "images/ostar.tga", TGA_LOW_QUALITY );
	texture_id [ MSTAR_TEXTURE ] = tgaLoadAndBind ( "images/mstar.tga", TGA_LOW_QUALITY );
	texture_id [ HALO_TEXTURE ] = tgaLoadAndBind ( "images/lens4.tga", TGA_LOW_QUALITY );
    texture_id [ SMALL_HALO_TEXTURE ] = tgaLoadAndBind ( "images/lens2.tga", TGA_LOW_QUALITY );
	texture_id [ LARGE_HALO_TEXTURE ] = tgaLoadAndBind ( "images/lens3.tga", TGA_LOW_QUALITY );
	texture_id [ GLOW_TEXTURE ] = tgaLoadAndBind ( "images/lens1.tga", TGA_LOW_QUALITY );
	texture_id [ ASTARBILLBOARD_TEXTURE ] = tgaLoadAndBind ( "images/astar_billboard.tga", TGA_LOW_QUALITY );
	texture_id [ MSTARBILLBOARD_TEXTURE ] = tgaLoadAndBind ( "images/mstar_billboard.tga", TGA_LOW_QUALITY );
	texture_id [ OSTARBILLBOARD_TEXTURE ] = tgaLoadAndBind ( "images/ostar_billboard.tga", TGA_LOW_QUALITY );
    texture_id [ PARTICLE_TEXTURE ] = tgaLoadAndBind ( "images/particle.tga", TGA_LOW_QUALITY );

    for (i=0; i<actual_no_dsos; i++)
    {
        sprintf ( file_name, "images/%s.tga", p_dso_object[i].name);
        texture_dso_id [ i ] = tgaLoadAndBind ( file_name, TGA_LOW_QUALITY );
    }

	//tgaSetTexParams (GL_TEXTURE_MIN_FILTER, GL_TEXTURE_MAG_FILTER, GL_COMBINE);

	// For antialiasing
	glEnable ( GL_LINE_SMOOTH );
	glHint ( GL_LINE_SMOOTH_HINT, GL_NICEST );

	//glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, max );
    //glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max[0] );

	planet_list = glGenLists ( 1 );
 	glNewList ( planet_list, GL_COMPILE );
		draw_planets ( GL_RENDER, p_planet, scale_factor, slices );
	glEndList ( );

	dso_list = glGenLists ( 1 );
 	//glNewList ( dso_list, GL_COMPILE );
	//	draw_dso_objects ( p_dso_object, scale_factor );
	//glEndList ( );

	ring_list = glGenLists ( 1 );
 	glNewList ( ring_list, GL_COMPILE );
		draw_rings ( p_planet, scale_factor, slices );
	glEndList ( );

	star_list = glGenLists ( 1 );
	glNewList ( star_list, GL_COMPILE );
		draw_stars ( p_star, 1 );
	glEndList ( );

	star_names_list = glGenLists ( 1 );
	glNewList ( star_names_list, GL_COMPILE );
		draw_star_names ( p_star );
	glEndList ( );

	constellation_list_3d = glGenLists ( 1 );
	glNewList ( constellation_list_3d, GL_COMPILE );
		draw_constellation_3d ( p_constellation, p_constellation_boundary, p_constellation_name, p_beyer );
	glEndList ( );

	render_axis_list = glGenLists ( 1 );
	glNewList ( render_axis_list, GL_COMPILE );
		render_axis ( );
	glEndList ( );

	render_orbits_list = glGenLists ( 1 );
	glNewList ( render_orbits_list, GL_COMPILE );
		render_orbits ( );
	glEndList ( );

	render_asteroid_list = glGenLists ( 1 );
	glNewList ( render_asteroid_list, GL_COMPILE );
		asteroids.id_texture = texture_id [ ASTEROID_TEXTURE ];
	    draw_asteroids ( &asteroids, p_asteroid, p_centaur, p_transneptune, scale_factor );
	glEndList ( );

	render_mars_moons_list = glGenLists ( 1 );
	glNewList ( render_mars_moons_list, GL_COMPILE );
		draw_mars_satellites( &deimos, &phobos, p_planet, scale_factor );
	glEndList ( );

}


int main ( int argc, char *argv[] )
{
	int win;

    pwd = getWorkingDir();

	/* initialize GLUT system */
	glutInit( &argc, argv );

 	//#ifndef __linux__
	glutInitDisplayMode ( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
	//#else
	//glutInitDisplayMode ( GLUT_RGB | GLUT_DEPTH );
	//#endif

  	glutInitWindowSize( 800, 600 );
  	win = glutCreateWindow( "Copernicus v1.6 (c) Marc Frincu 2003-2010 - OpenGL Edition Remastered" );	/* create window */

	my_lat = 45.17;
	my_long = 21.53;

    initialize_constellations ( p_constellation, p_constellation_boundary, p_constellation_name, p_beyer, lst, my_lat, pow ( scale_factor, 2) );
	initialize_planets ( p_planet, lst, my_lat );
	initialize_other_ss_objects ( p_planet, p_asteroid, p_centaur, p_transneptune, lst, my_lat );
	initialize_stars ( p_star, pow ( scale_factor, 2) );
	initialize_dso ( p_dso_object, lst, my_lat, pow ( scale_factor, 2) );

	load_3DS ( &phobos, "3ds/phobos.3ds" );
	load_3DS ( &deimos, "3ds/deimos.3ds" );
	load_3DS ( &asteroids, "3ds/phobos.3ds" );

	init_gl ( );

  	/* from this point on the current window is win */

	glutReshapeFunc ( reshape );
  	glutDisplayFunc ( display );		/* set window's display callback */
	glutMouseFunc    ( mouse );
	glutMotionFunc ( motion );
  	glutKeyboardFunc ( keyboard );		/* set window's key callback */
	glutSpecialFunc ( keyboard_special );
	glutIdleFunc ( idle );
  	glutMainLoop ( );			/* start processing events... */

  	/* execution never reaches this point */

  return 0;
}

void start_mouse_drag ( int x, int y )
{
        /* Store cursor position  */
        old_x = x;
        old_y = y;

        glutSetCursor   ( GLUT_CURSOR_NONE );
        glutWarpPointer ( vp_centre_x, vp_centre_y );
}

void stop_mouse_drag ( int x, int y )
{
        /* Restore old settings */
        glutSetCursor ( GLUT_CURSOR_LEFT_ARROW );
        glutWarpPointer ( old_x, old_y );
}

void calculate_delta_t ( void )
{
	#ifdef __linux__
	time_t t;
	(void) time(&t);
   	delta_t.currentTime = (double) t;
	#else
	delta_t.currentTime = ( float ) timeGetTime ( );
   	#endif

	delta_t.elapsedTime = ( delta_t.currentTime - delta_t.lastTime ) / 1000.0;
   	delta_t.lastTime    = delta_t.currentTime;
}

void glut_print_string ( double x, double y, double z, const char *string )
{
	int i = 0;
	unsigned char v;

	glDisable ( GL_LIGHTING );

	glRasterPos3f(x, y, z);
	glGetBooleanv(GL_CURRENT_RASTER_POSITION_VALID, &v);
	if (v)
		while (string[i])
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, string[i++]);

	glEnable ( GL_LIGHTING );

}

void draw_2d_stuff ( void )
{
	char buffer[255];
	double distance, day_number;


	glDisable ( GL_LIGHTING );
   	glDisable ( GL_TEXTURE_2D );

   	/* Set up viewing matrix  */
   	glMatrixMode   ( GL_PROJECTION );
   	glPushMatrix   ( );
		glLoadIdentity ( );

		glOrtho ( 0.0, ( (double)screen_width ), 0.0,
		               ( (double)screen_height ), -0.0, 0.0 );

		glMatrixMode   ( GL_MODELVIEW );

		glLoadIdentity ( );

		glColor3ub     ( 200, 200, 200 );

		day_number = date();

		sprintf(buffer, "Current day number: %lf", day_number );
		glut_print_string   ( -0.99, 0.95, 0,  buffer );

		distance = DISTANCE ( camera.coords.posx, camera.coords.posy, camera.coords.posz ) / scale_factor;

		compute_camera_speed ( day_number, distance );

        //sprintf(buffer, "Current camera speed: %lf ", camera.speed);
		sprintf(buffer, "Current camera speed: %lf (km/s) %lf (au/s) %lf (ly/s)", camera_speed_km, camera_speed_km / AU_TO_KM, camera_speed_km / LY_TO_KM );
		glut_print_string   ( -0.99, 0.90, 0, buffer );
		sprintf(buffer, "Current distance from Sun: %lf (au)", distance );
		glut_print_string   ( -0.99, 0.85, 0, buffer );
		sprintf(buffer, "Sun brightness (100%% at 1 a.u.): %lf%%", 1.0 / pow ( DISTANCE ( camera.coords.posx, camera.coords.posy, camera.coords.posz ) / scale_factor, 2 ) * 100 );
		glut_print_string   ( -0.99, 0.80, 0, buffer );

		if ( strcmp( selected_object_name, "" ) != 0 )
		{
			sprintf(buffer, "Selected object: %s", selected_object_name);
			glut_print_string   ( -0.99, 0.75, 0, buffer );
		}

		glMatrixMode ( GL_PROJECTION );
	glPopMatrix  ( );

   	glMatrixMode ( GL_MODELVIEW );

   	glEnable ( GL_LIGHTING );
   	glEnable ( GL_TEXTURE_2D );
}

static void compute_camera_speed ( double day_number, double distance )
{
	double diff, diff_time, sign;

	if ( camera.speed == 0 )
	{
		camera_speed_km = 0;
	}

	if ( old_camera_distance != distance && old_day_number != day_number )
	{
		diff = distance - old_camera_distance;
		diff_time = day_number - old_day_number;
		diff_time *= SECONDS_IN_DAY;

		if (diff < 0)
			sign = -1;
		else
			sign = 1;

		camera_speed_km = sign * diff * AU_TO_KM / ( diff_time );
		old_camera_distance = distance;
		old_day_number = day_number;
	}
}

static void render_axis ( void )
{
	double limit = z_far / 1000;

	glDisable ( GL_LIGHTING );
	glDisable ( GL_RENDER );
   	glBegin ( GL_LINES );

   	glColor3f ( 0.0f, 1.0f, 0.0);

	// Three primary axis lines.
   	glVertex3d ( limit, 0 ,0 );
   	glVertex3d ( -1 * limit, 0, 0 );
   	glVertex3d ( 0, limit, 0 );
   	glVertex3d ( 0, -1 * limit, 0 );
   	glVertex3d ( 0, 0, limit );
   	glVertex3d ( 0, 0, -1 * limit );

   	glEnd ( );

	glut_print_string   ( limit, 0 ,0, "X" );
	glut_print_string   ( -1 * limit, 0 ,0, "-X" );
	glut_print_string   ( 0, limit, 0, "Y" );
	glut_print_string   ( 0, -1 * limit, 0, "-Y" );
	glut_print_string   ( 0, 0, limit, "Z" );
	glut_print_string   ( 0, 0, -1 * limit, "-Z" );

   	glEnable ( GL_RENDER );
	glEnable ( GL_LIGHTING );
}

static void render_orbits ( void )
{
	int i;
	for (i=1; i< NO_PLANETS; i++)
		draw_orbit ( planet[0].xe, planet[0].ye, planet[0].ze, planet[i].distFromSun * scale_factor);
}
