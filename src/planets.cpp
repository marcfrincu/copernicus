/*

        Paul Groves.

   Semi-accurate (orbital periods are correct, but orbits are perfectly
   circular) simulation of the inner-solar system using OpenGL.

   ----------------------------------------------------------------------
   Click left and drag the mouse around to change the camera angle.
        Click right and drag the mouse up and down to change your speed.
   You can press + and - on the numpad to speed up and slow down too.
   Press 'r' to reset the camera and 's' to come to a halt.

   Notes:

   New rotation function borrowed from Mesa lets us update the planets using
   the system clock and NOT frame rate as before.

   The previous versions textures have been replaced by .bmp files and are
   loaded with my new bmp library.  If your card doesn't support palettised
   textures, this won't work.  Mail me and I'll send you the 24bit textures :)
   ----------------------------------------------------------------------

*/

#include <windows.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "planets.h"
#include "bitmap.h"
#include "vector.c"

/*      Set up planet variables - make 'em global to keep it simple */
object_t        mercury;
object_t        venus;
object_t        earth;
object_t        moon;
object_t        mars;

/* Place for tex_ids */
GLuint          texture_id[MAX_NO_TEXTURES];

/*      Stores timing information for consistant frame rate independent movement */
time_delta_type         delta_t;

world_object_type camera;

/* Handle the warping mouse (IE it never hits the edge of the screen!)  */
int                     vp_centre_x;
int                     vp_centre_y;
int                     old_x;
int                     old_y;
int                depressed_button = 0;
bool                    mouse_drag = false;


/*      Set up a variable to rotate each planet (days not accurate) */
float           day_rotation             = 0.0;


void start_mouse_drag ( int x, int y )
{
        /* Store cursor position  */
        old_x = x;
        old_y = y;

        glutSetCursor   ( GLUT_CURSOR_NONE );
        glutWarpPointer ( vp_centre_x, vp_centre_y );
}

#pragma argsused
void stop_mouse_drag ( int x, int y )
{
        /* Restore old settings */
        glutSetCursor   ( GLUT_CURSOR_LEFT_ARROW );
        glutWarpPointer ( old_x, old_y );
}

void CalculateDeltaT ( void )
{
        delta_t.currentTime = ( float ) timeGetTime ( );
   delta_t.elapsedTime = ( delta_t.currentTime - delta_t.lastTime ) / 1000.0;
        delta_t.lastTime    = delta_t.currentTime;
}

void draw_star( void )
{
   float x, y, z, elevation, azimuth;

   elevation  = rand ( );
   azimuth    = rand ( );

        y = STAR_RADIUS * sin ( elevation );
        x = STAR_RADIUS * cos ( elevation ) * cos ( azimuth );
        z = STAR_RADIUS * cos ( elevation ) * sin ( azimuth );

        glColor3ub ( 200, 200, 200 );
   glVertex3f ( x, y, z );
}

void make_star_list ( void )
{
        randomize ( );

   glNewList ( 1, GL_COMPILE );

   glBegin ( GL_POINTS );
        for ( int i = 0; i < MAX_STARS; i++ )
        draw_star ( );
   glEnd ( );
   glEndList ( );
}

void initial_camera_position ( void )
{
   camera.coords.posx = 0.0;
   camera.coords.posy = 20.0;
   camera.coords.posz = -200.0;

   camera.upx = 0.0;
   camera.upy = 1.0;
   camera.upz = 0.0;

   camera.azimuth   = 1.0f;
   camera.elevation = 0.0f;

   camera.speed = 0.0f;
}

void init ( void )
{
        /* Image declarations for texture maps */
   image_t      moon_map;
   image_t      earth_map;
   image_t      venus_map;
   image_t      mars_map;

        /*      Prepare lights and emissions */
   float   ambient_light[] = {0.3, 0.3, 0.45, 1.0};
   float           source_light[]  = {0.9, 0.8, 0.8, 1.0};

   glEnable ( GL_DEPTH_TEST );
   glEnable ( GL_CULL_FACE );
   glShadeModel ( GL_SMOOTH );

   // Setup for Binding the textures to the texture_ids
   glGenTextures ( 4, texture_id );
   glEnable ( GL_TEXTURE_2D );

   bmpGetColorEXT ( );

   glBindTexture ( GL_TEXTURE_2D, texture_id[0] );
   bmpLoadImage  ( "moon.bmp", &moon_map, BMP_FREE );

   glBindTexture ( GL_TEXTURE_2D, texture_id[1] );
   bmpLoadImage  ( "venus.bmp", &venus_map, BMP_FREE );

   glBindTexture (GL_TEXTURE_2D, texture_id[2] );
   bmpLoadImage  ("earth.bmp", &earth_map, BMP_FREE );

   glBindTexture ( GL_TEXTURE_2D, texture_id[3] );
   bmpLoadImage  ( "mars.bmp", &mars_map, BMP_FREE );

   // Enable lighting
   glEnable       ( GL_LIGHTING );
   glLightModelfv ( GL_LIGHT_MODEL_AMBIENT,ambient_light );
   glLightfv      ( GL_LIGHT0,GL_DIFFUSE,source_light );
   glEnable       ( GL_LIGHT0 );

   //   Enable material attributes
   glEnable ( GL_COLOR_MATERIAL );
   glColorMaterial ( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );

   glClearColor ( 0.0, 0.0, 0.10, 0.0 );

   make_star_list ( );

   /* Init camera variables */
   initial_camera_position ( );

   /* Mesa orbiting code uses the centre of current modelview matrix as a
      pivot to orbit around - this means we have to supply a starting point
      for the planets in some plane (x in this case.)
   */
   mercury.posx = 25.0;
   venus.posx    = 65.0;
   earth.posx    = 150.0;
   mars.posx     = 250.0;

   // Distance to orbit from the EARTH...
   moon.posx    = 25.0;
}

/*
        Draw nice lines or dots to give some visual cue when rotating
        the user view
*/
void do_orbit_dots ( float radius )
{
   float angle;

   // Copy the current matrix and start a new one
   glPushMatrix ( );
   glRotatef ( 90.0, 1.0, 0.0, 0.0 );
   glBegin   ( GL_LINE_LOOP );
   glColor3f ( 0.0, 0.2, 0.0 );
   for ( int i = 0; i <= 24; i++ )
   {
      angle = 3.14159 / 12.0 * i;
      glVertex2f ( ( radius*cos ( angle )), ( radius*sin ( angle )) );
   }
   glEnd ( );
   glPopMatrix ( );
}


/*
   Functions to draw the sun and planets, each function needs to call
   push / pop matrix and do a rotate because GLU spheres are created along
   the Z axis (for no good reason...)
*/
void draw_sun ( void )
{
   float sun_glow[] = {0.7, 0.5, 0.0, 1.0};

   glPushMatrix ( );
   glPushAttrib ( GL_CURRENT_BIT );
   glMaterialfv ( GL_FRONT, GL_EMISSION, sun_glow );
   glRotatef ( 90.0, 1.0, 0.0, 0.0 );
   glColor3f ( 0.8, 0.7, 0.0 );
   glutSolidSphere (20, 15, 15 );
   glPopAtrib ( );
   glPopMatrix();
}

void draw_planet ( int size, int segs, int tex_index, unsigned char r,
                   unsigned char g,unsigned char b )
{
   glPushMatrix ( );
   glRotatef  ( 90.0, 1.0, 0.0, 0.0 );
   glRotatef  ( day_rotation, 0.0, 0.0, 1.0 );
   glColor3ub ( r, g, b );

   glEnable ( GL_TEXTURE_2D );
   glBindTexture ( GL_TEXTURE_2D, texture_id[tex_index] );
   GLUquadricObj*  q = gluNewQuadric ( );
   gluQuadricDrawStyle ( q, GLU_FILL );
   gluQuadricNormals   ( q, GLU_SMOOTH );
   gluQuadricTexture   ( q, GL_TRUE );
   gluSphere ( q, size, segs, segs );
   gluDeleteQuadric ( q );
   glDisable (GL_TEXTURE_2D );
   glPopMatrix ( );
}

void draw_star_field ( void )
{
   // Move to the location of the 'camera'
   glDisable ( GL_LIGHTING );
   glPushMatrix ( );
   glTranslatef ( camera.coords.posx, camera.coords.posy, camera.coords.posz );
   glCallList   ( 1 );
   glPopMatrix ( );
   glEnable ( GL_LIGHTING );
}

void move_camera ( void )
{

   float newx, newy, newz;

   polar_to_cartesian ( camera.azimuth, camera.speed, camera.elevation,
                        newx, newy, newz );

   // Replace old x, y, z coords for camera
   camera.coords.posx += ( newx * delta_t.elapsedTime );
   camera.coords.posy += ( newy * delta_t.elapsedTime );
   camera.coords.posz += ( newz * delta_t.elapsedTime );
}


void aim_camera( void )
{
   float centerx, centery, centerz;

   glLoadIdentity ( );

   // calc new eye vector
   polar_to_cartesian ( camera.azimuth, 100.0f,
                        camera.elevation, centerx, centery, centerz );

   // calc new up vector
   polar_to_cartesian ( camera.azimuth, 100.0f, camera.elevation+90,
                        camera.upx, camera.upy, camera.upz );

   gluLookAt ( camera.coords.posx, camera.coords.posy, camera.coords.posz,
               camera.coords.posx + centerx, camera.coords.posy + centery,
               camera.coords.posz + centerz, camera.upx, camera.upy, camera.upz );
}


/*      Main display function called by GLUT main loop */
void display( void )
{
   /*      Prepare light */
   float   light_pos[] = {0.0, 0.0, 0.0, 1.0};
   int  slices      =  8;

   // Grab the current time and calculate elapsed time since last frame
   CalculateDeltaT ( );

   // Clear previous screen and depth buffer
   glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   aim_camera  ( );
   move_camera ( );

   draw_star_field ( );

   /* Down to business...
      Draw the orbits, light and sun then make new matrices.
      Rotate the new matrix a given amount and translate up the
      X axis then draw the planet.
      Finally after each planet has been drawn pop back to the
      previous matrix
   */
   glPushMatrix ( );
   glDisable  ( GL_LIGHTING );
   do_orbit_dots ( 25.0 );
   do_orbit_dots ( 65.0 );
   do_orbit_dots ( 150.0 );
   do_orbit_dots ( 250.0 );
   glEnable  ( GL_LIGHTING );
   glLightfv ( GL_LIGHT0,GL_POSITION,light_pos );

   draw_sun  ( );

   glPushMatrix ( );
   rotation   ( MERC_DEGS * delta_t.elapsedTime, 0.0, 1.0, 0.0,
               &mercury.posx, &mercury.posy, &mercury.posz );
   glTranslatef ( mercury.posx, mercury.posy, mercury.posz );
   draw_planet  ( 3, 3, 0, 255, 200, 0 );  /* Mercury */
   glPopMatrix ( );

   glPushMatrix ( );
   rotation   ( VENUS_DEGS * delta_t.elapsedTime, 0.0, 1.0, 0.0,
               &venus.posx, &venus.posy, &venus.posz );
   glTranslatef ( venus.posx, venus.posy, venus.posz );
   draw_planet  ( 10, slices, 1, 255, 255, 255 );  /* Venus */
   glPopMatrix ( );

   glPushMatrix ( );
   rotation   ( EARTH_DEGS * delta_t.elapsedTime, 0.0, 1.0, 0.0,
               &earth.posx, &earth.posy, &earth.posz );
   glTranslatef (earth.posx, earth.posy, earth.posz );
   draw_planet  (10, slices, 2, 255, 255, 255 );  /* Earth */

   /* Use the Earth as origin and draw moon  */
   glDisable( GL_LIGHTING );
   do_orbit_dots ( 25.0 );
   glEnable ( GL_LIGHTING );
   rotation ( MOON_DEGS * delta_t.elapsedTime, 0.0, 1.0, 0.0,
             &moon.posx, &moon.posy, &moon.posz );
   glTranslatef ( moon.posx, moon.posy, moon.posz );
   draw_planet  ( 3, 3, 0, 200, 200, 200 );   /* moon */
   glPopMatrix ( );

   /* Back to 'proper' origin, rotate and translate, draw Mars  */
   rotation ( MARS_DEGS * delta_t.elapsedTime, 0.0, 1.0, 0.0,
             &mars.posx, &mars.posy, &mars.posz );
   glTranslatef ( mars.posx, mars.posy, mars.posz );
   draw_planet  ( 8, slices, 3, 255, 255, 255 );
   glPopMatrix ( );

   glutSwapBuffers ( );
}



/*      Idle callback from GLUT main loop.
*/
void idling ( void )
{
   day_rotation += 5;
   if ( day_rotation >= 360 )
      day_rotation = 0.0;

   glutPostRedisplay ( );
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
   if ( h==0 )
      gluPerspective ( 80, ( float ) w, 1.0, 5000.0 );
   else
      gluPerspective ( 80, ( float ) w / ( float ) h, 1.0, 5000.0 );
   glMatrixMode   ( GL_MODELVIEW );
   glLoadIdentity ( );

   vp_centre_x = w / 2;
   vp_centre_y = h / 2;
}




void mouse ( int button, int state, int x, int y )
{
   if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
   {
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

void motion ( int x, int y )
{
   int deltaX ( x - vp_centre_x );
   int deltaY ( y - vp_centre_y );

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

   /* Call these because the GLUT loop can only handle one message at a time,
      we're hogging GLUTs message system with mouse coords
    */
   display ( );
   idling  ( );
}



#pragma argsused
/* Standard GLUT keyboard call back function. */
void keyboard ( unsigned char key, int x, int y )
{
   switch ( key ) {
      case 27:  // Escape key
         exit ( 0 );
         break;
      case '+':
         camera.speed += 10.0;
         glutPostRedisplay ( );
         break;
      case '-':
         camera.speed -= 10.0;
         glutPostRedisplay ( );
         break;
      case 'r':
         initial_camera_position ( );
         glutPostRedisplay ( );
         break;
      case 's':
         camera.speed = 0.0f;
         break;
      default:
         break;
   }
}


/*  Main Loop
 *  Open window with initial window size, title bar,
 *  RGB display mode, and handle input events.
 */
int main ( int argc, char** argv )
{
   glutInit ( &argc, argv );
   glutInitDisplayMode ( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
   glutInitWindowSize  ( 600, 300 );
   glutCreateWindow    ( "Pauly's Planets O'Plenty" );

   init ( );

   glutReshapeFunc  ( reshape  );
   glutMouseFunc    ( mouse    );
   glutMotionFunc   ( motion   );
   glutKeyboardFunc ( keyboard );
   glutDisplayFunc  ( display  );
   glutIdleFunc     ( idling   );
   glutMainLoop     ( );
   return 0;
}
