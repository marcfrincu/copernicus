#pragma once
#ifndef __linux__
#include <windows.h>
#endif
#include <GL/glut.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <float.h> /* for DBL_MAX */

#include "include/objects.h"
#include "include/copernicus.h"
#include "include/util.h"
#include "include/camera.h"
#include "include/postscript.h"
#include "include/particles.h"

#include "vec3d.c"

#define NO_CLOSEST_STARS 20

GLuint texture_id[NO_TEXTURES];
GLuint texture_dso_id[NO_DSO_TEXTURES];

typedef struct {
	int index;
	double distance_from_camera;
} closest_star_info;
closest_star_info closest_star[NO_CLOSEST_STARS];

void draw_legend ( FILE* file, double scala );

void draw_planet ( double size, int segs, int tex_index, unsigned char r,
                   unsigned char g,unsigned char b )
{
	GLfloat mat_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0 };
	GLfloat mat_emission[] = { 0.0f, 0.0f, 0.0f, 1.0 };
	GLfloat mat_diffuse[] = { 0.6f, 0.6f, 0.6f, 1.0 };
	GLfloat mat_specular[] = { 0.8f, 0.8f, 0.8f, 1.0 };
	GLfloat mat_all[] = { 0.8f, 0.8f, 0.8f, 1.0 };

    glPushMatrix ( );
        glPushAttrib ( GL_CURRENT_BIT );
			glMaterialfv ( GL_FRONT, GL_AMBIENT, mat_ambient );
			glMaterialfv ( GL_FRONT, GL_DIFFUSE, mat_diffuse );
			glMaterialfv ( GL_FRONT, GL_SPECULAR, mat_specular );
			glMaterialf  ( GL_FRONT, GL_SHININESS, 5.0f );
			glMaterialfv ( GL_FRONT, GL_EMISSION, mat_emission );

			glRotatef  ( 90.0, 1.0, 0.0, 0.0 );
            glColor3ub ( r, g, b );

			glEnable ( GL_TEXTURE_2D );

			GLUquadricObj*  q = gluNewQuadric ( );
			gluQuadricDrawStyle ( q, GLU_FILL );
			gluQuadricNormals   ( q, GLU_SMOOTH );
			gluQuadricTexture   ( q, GL_TRUE );

			glBindTexture ( GL_TEXTURE_2D, texture_id[tex_index] );

			gluSphere ( q, size, segs, segs );
			gluDeleteQuadric ( q );

			glDisable (GL_TEXTURE_2D );

			glMaterialfv ( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_all );
			glMaterialfv ( GL_FRONT, GL_SPECULAR, mat_all );
			glMaterialfv ( GL_FRONT, GL_EMISSION, mat_all );

        glPopAttrib ( );
    glPopMatrix ( );
}

void draw_moon ( double size, int segs, GLuint texture, unsigned char r,
                   unsigned char g,unsigned char b )
{
	GLfloat mat_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0 };
	GLfloat mat_emission[] = { 0.0f, 0.0f, 0.0f, 1.0 };
	GLfloat mat_diffuse[] = { 0.6f, 0.6f, 0.6f, 1.0 };
	GLfloat mat_specular[] = { 0.8f, 0.8f, 0.8f, 1.0 };
	GLfloat mat_all[] = { 1.f, 1.f, 0.f, 1.0 };

    glPushMatrix ( );
        glPushAttrib ( GL_CURRENT_BIT );
			glMaterialfv ( GL_FRONT, GL_AMBIENT, mat_ambient );
			glMaterialfv ( GL_FRONT, GL_DIFFUSE, mat_diffuse );
			glMaterialfv ( GL_FRONT, GL_SPECULAR, mat_specular );
			glMaterialf  ( GL_FRONT, GL_SHININESS, 5.0f );
			glMaterialfv ( GL_FRONT, GL_EMISSION, mat_emission );

			glRotatef  ( 90.0, 1.0, 0.0, 0.0 );
            glColor3ub ( r, g, b );

			glEnable ( GL_TEXTURE_2D );

			GLUquadricObj*  q = gluNewQuadric ( );
			gluQuadricDrawStyle ( q, GLU_FILL );
			gluQuadricNormals   ( q, GLU_SMOOTH );
			gluQuadricTexture   ( q, GL_TRUE );

			glBindTexture ( GL_TEXTURE_2D, texture );

			gluSphere ( q, size, segs, segs );
			gluDeleteQuadric ( q );

			glDisable (GL_TEXTURE_2D );

			glMaterialfv ( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_all );
			glMaterialfv ( GL_FRONT, GL_SPECULAR, mat_all );
			glMaterialfv ( GL_FRONT, GL_EMISSION, mat_all );

        glPopAttrib ( );
    glPopMatrix ( );
}


void draw_sun ( double size, int tex_index, char spectrum )
{
    GLfloat sun_glow_G[] = { 0.8f, 0.8f, 0.0f, 1.0f }, sun_glow_O[] = { 1.0f, 1.0f, 1.0f, 1.0f }, sun_glow_B[] = { 0.5f, 0.5f, 1.0f, 1.0f }, sun_glow_A[] = { 0.0f, 0.0f, 1.0f, 1.0f }, sun_glow_M[] = { 1.0f, 0.0f, 0.0f, 1.0f }, sun_glow_K[] = { 0.8f, 0.0f, 0.0f, 1.0f };

    glPushMatrix ( );
        glDisable(GL_BLEND);
        glPushAttrib ( GL_CURRENT_BIT );

        switch ( spectrum )
        {
            case 'O': glMaterialfv( GL_FRONT, GL_EMISSION, sun_glow_O ); break;
            case 'B': glMaterialfv( GL_FRONT, GL_EMISSION, sun_glow_B ); break;
            case 'A': glMaterialfv( GL_FRONT, GL_EMISSION, sun_glow_A ); break;
            case 'F': glMaterialfv( GL_FRONT, GL_EMISSION, sun_glow_G ); break;
            case 'G': glMaterialfv( GL_FRONT, GL_EMISSION, sun_glow_G ); break;
            case 'K': glMaterialfv( GL_FRONT, GL_EMISSION, sun_glow_K ); break;
            case 'M': glMaterialfv( GL_FRONT, GL_EMISSION, sun_glow_M ); break;
            default: glMaterialfv( GL_FRONT, GL_EMISSION, sun_glow_G ); break;
        }

        glRotatef ( 90.0, 1.0, 0.0, 0.0 );
        glColor3f ( 255, 255, 255 );
        glEnable ( GL_TEXTURE_2D );

        GLUquadricObj*  q = gluNewQuadric ( );
        gluQuadricDrawStyle ( q, GLU_FILL );
        gluQuadricNormals   ( q, GLU_SMOOTH );
        gluQuadricTexture   ( q, GL_TRUE );

        glBindTexture ( GL_TEXTURE_2D, texture_id[tex_index] );

        gluSphere ( q, size, 40, 40 );
        gluDeleteQuadric ( q );

        glPopAttrib ( );
        glEnable(GL_BLEND);
    glPopMatrix();
}

void draw_orbit ( double xCenter, double yCenter, double zCenter, double radius) {

	double x, z, angle;

	glPushMatrix ();
        glDisable ( GL_LIGHTING );
		glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		glEnable ( GL_BLEND );

		glShadeModel ( GL_FLAT );
	  	glDepthFunc ( GL_LEQUAL );

		glLineWidth ( 1 ) ;

		glRotatef ( 90.0, 1.0, 0.0, 0.0 );

		glBegin(GL_LINE_LOOP);
		glColor3f ( 0, 0, 50 );
		for (int i = 0; i <= 360; i++) {
			angle = radian_correction * i;
			x = radius * cos ( angle );
			z = radius * sin ( angle );
			glVertex3d ( xCenter + x, yCenter, zCenter + z );
		}
		glEnd();

		glDisable ( GL_BLEND );
		glShadeModel ( GL_SMOOTH );
        glEnable ( GL_LIGHTING );
	glPopMatrix ();

}

void draw_sun_billboard ( double size, int texture, double object_pos[] )
{
	double p1[3], p2[3], p3[3], p4[3];
	double alfa, beta, cosalfa, cosbeta, sinalfa, sinbeta;
	double x, y, z;
	double cam[3], cam_object_dir[3];

    cam[0] = camera.coords.posx;
    cam[1] = camera.coords.posy;
    cam[2] = camera.coords.posz;

    // Compute the Camera-Object vector
    vdiff ( cam_object_dir, cam, object_pos );
    vnorm ( cam_object_dir );

   // printf("%d %f %f %f %f %f %f %f\n", tex_index, cam_object_dir[0], cam_object_dir[1], cam_object_dir[2], object_pos[0], object_pos[1], object_pos[2]);

    p1[0] = - size;
    p1[1] = - size;
    p1[2] = 0;
    p2[0] = + size;
    p2[1] = - size;
    p2[2] = 0;
    p3[0] = + size;
    p3[1] = + size;
    p3[2] = 0;
    p4[0] = - size;
    p4[1] = + size;
    p4[2] = 0;

    //p1[0] = p4[0] = p1[1] = p2[1] = scale;
    //p2[0] = p3[0] = p3[1] = p4[1] = -scale;
    //p1[2] = p2[2] = p3[2] = p4[2] = 0.0;

    alfa = atan2 ( cam_object_dir[2], cam_object_dir[0]) - radian_correction * 180.0f / 2.0;
    beta = atan2 ( cam_object_dir[1], sqrt ( pow ( cam_object_dir[0], 2 ) + pow ( cam_object_dir[2], 2 )) );

	//alfa = atan2 ( camera.coords.posz, camera.coords.posx) - radian_correction * 180.0f / 2.0;
	//beta = atan2 ( camera.coords.posy, sqrt ( camera.coords.posx * camera.coords.posx + camera.coords.posz * camera.coords.posz ) );

	cosalfa = cos ( alfa );
	cosbeta = cos ( beta );
	sinalfa = sin ( alfa );
	sinbeta = sin ( beta );

	ROTATE ( p1 );
	ROTATE ( p2 );
	ROTATE ( p3 );
	ROTATE ( p4 );

	glBindTexture ( GL_TEXTURE_2D, texture );
	glBlendFunc ( GL_ONE, GL_ONE );

	glEnable ( GL_BLEND );
	glEnable ( GL_TEXTURE_2D );
	glDepthMask ( GL_FALSE );

	glPushMatrix ( );
       // glTranslatef ( object_pos[0], object_pos[1], object_pos[2] );
        glBegin ( GL_QUADS );
            glTexCoord2f ( 0.0, 0.0 );
            glVertex3f ( p1[0], p1[1], p1[2] );
            glTexCoord2f ( 0.0, 1.0 );
            glVertex3f ( p2[0], p2[1], p2[2] );
            glTexCoord2f ( 1.0, 1.0 );
            glVertex3f (p3[0], p3[1], p3[2] );
            glTexCoord2f ( 1.0, 0.0 );
            glVertex3f ( p4[0], p4[1], p4[2] );
        glEnd ();

       /* glBegin ( GL_QUADS );
            glTexCoord2f ( 0.0, 1.0 );
            glVertex3f ( p1[0], p1[1], p1[2] );
            glTexCoord2f ( 1.0, 1.0 );
            glVertex3f ( p2[0], p2[1], p2[2] );
            glTexCoord2f ( 1.0, 0.0 );
            glVertex3f (p3[0], p3[1], p3[2] );
            glTexCoord2f ( 0.0, 0.0 );
            glVertex3f ( p4[0], p4[1], p4[2] );
        glEnd ( );
        */
	glPopMatrix ( );
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);

}

void draw_planets ( GLenum mode, planet_info *planet, float scale_factor, int slices )
{
    double object_pos[3];
	int i, j;

	// Draw the Sun
	glPushMatrix ();
		glTranslatef ( planet[0].xe, planet[0].ye, planet[0].ze );
		object_pos[0] = planet[0].xe;
		object_pos[1] = planet[0].ye;
		object_pos[2] = planet[0].ze;
		glDisable ( GL_LIGHTING );
		draw_sun_billboard ( planet[0].size * 3, texture_id[GSTARBILLBOARD_TEXTURE], object_pos );
        glEnable ( GL_LIGHTING );

		if (mode == GL_SELECT)
			glPushName ( 0 );

		draw_sun ( planet[0].size * 0.7, GSTAR_TEXTURE, 'G' );

		if (mode == GL_SELECT)
			glPopName ( );

	glPopMatrix ();

	// Draw the planets
	for ( i=1; i< NO_PLANETS; i++ )
	{
		glPushMatrix ();
			glDisable(GL_LIGHTING);
			glColor3ub ( 255, 255, 255 );
			glut_print_string ( planet[i].xe * scale_factor, planet[i].ye * scale_factor, planet[i].ze * scale_factor, planet[i].name );
			glEnable(GL_LIGHTING);

			glTranslatef ( planet[i].xe * scale_factor, planet[i].ye * scale_factor, planet[i].ze * scale_factor);
			glRotatef ( -90 + planet[i].tilt_x, 1.0, 0.0, 0.0 );
			//glRotatef ( planet[i].tilt_z, 0.0, 0.0, 1.0 );

			if (mode == GL_SELECT)
				glPushName ( i );

			draw_planet  ( planet[i].size, slices * 1, i, 255, 255, 255 );

			/* Draw Earth clouds */
			if ( i == 3 )
			{
                    glPushMatrix ( );
                        glRotatef ( (delta_t.lastTime + delta_t.elapsedTime) / 100.f, 1.0, 1.0, 0.0 );
                        glEnable(GL_BLEND);
                        glBlendFunc(GL_ONE, GL_ONE);
                        draw_planet  ( planet[i].size + 0.1, slices * 1, 10, 255, 255, 255 );
                        glDisable(GL_BLEND);
                    glPopMatrix ( );
			}

			if (mode == GL_SELECT)
				glPopName ( );

			for (j=0; j< planet[i].no_moons; j++)
			{
				if ( i != 4 )
				{
					glPushMatrix();
						glDisable(GL_LIGHTING);
						glColor3ub ( 187, 255, 255 );
						glut_print_string ( planet[i].moon[j].x * planet[i].moon_scale_factor, planet[i].moon[j].y * planet[i].moon_scale_factor, planet[i].moon[j].z * planet[i].moon_scale_factor,  planet[i].moon[j].name );
						glEnable(GL_LIGHTING);
						glTranslatef ( planet[i].moon[j].x * planet[i].moon_scale_factor, planet[i].moon[j].y * planet[i].moon_scale_factor, planet[i].moon[j].z * planet[i].moon_scale_factor);
                        draw_moon  ( planet[i].moon[j].size, slices * 1, planet[i].moon[j].texture_id[0], 255, 255, 255 );
					glPopMatrix();
				}
			}

		glPopMatrix ();
	}
}

void draw_mars_satellites( obj_type_ptr deimos, obj_type_ptr phobos, planet_info *planet, float scale_factor )
{
	int j;

	glPushMatrix ( );
		glTranslatef ( planet[4].xe * scale_factor, planet[4].ye * scale_factor, planet[4].ze * scale_factor);
		for (j=0; j< planet[4].no_moons; j++)
		{
			glPushMatrix ( );
			glDisable(GL_LIGHTING);
			glColor3ub ( 187, 255, 255 );
			glut_print_string ( planet[4].moon[j].x * planet[4].moon_scale_factor, planet[4].moon[j].y * planet[4].moon_scale_factor, planet[4].moon[j].z * planet[4].moon_scale_factor,  planet[4].moon[j].name );
			glEnable(GL_LIGHTING);

			glTranslatef ( planet[4].moon[j].x * planet[4].moon_scale_factor, planet[4].moon[j].y * planet[4].moon_scale_factor, planet[4].moon[j].z * planet[4].moon_scale_factor);
			switch (j)
			{
				case 0:
					glScalef (0.005,0.005,0.005 );
					phobos->id_texture = planet[4].moon[j].texture_id[0];
					draw_3DS ( phobos );
					break;
				case 1:
					glScalef (0.005,0.005,0.005 );
					deimos->id_texture = planet[4].moon[j].texture_id[0];
					draw_3DS ( deimos );
					break;
			}
			glPopMatrix ( );
		}
	glPopMatrix ( );
}

void draw_rings ( planet_info *planet, float scale_factor, float slices )
{
	int i;
	for ( i=1; i< NO_PLANETS; i++ )
	{
		glPushMatrix();
			glTranslatef ( planet[i].xe * scale_factor, planet[i].ye * scale_factor, planet[i].ze * scale_factor);
			glRotatef ( planet[i].tilt_x, 1.0, 0.0, 0.0 );
			if ( planet[i].has_rings )
			{
				draw_ring ( planet[i].inner_ring, planet[i].outer_ring, i, RINGS_TEXTURE, 30, scale_factor );
			}
		glPopMatrix();
	}
}

void draw_closest_star_names ( star_info *star, int show_star )
{
    double object_pos[3];
    int i = 0, j = 0, index = 0, texture_index = 0;
    char *star_name, changed = 0;
    double d = 0.0, MAX_CLOSEST_DISTANCE = DBL_MAX;
    int texture_bb_id = GSTARBILLBOARD_TEXTURE;

    /*
        This code fragment computes the list of the closest NO_CLOSEST STARS to the camera
        in one step using the closest_star array as the final destination
    */

    for ( j=0; j<NO_CLOSEST_STARS; j++ )
        closest_star[j].distance_from_camera = MAX_CLOSEST_DISTANCE;
        closest_star[j].index = -1;

     /* For all stars */
    for ( i=0; i<actual_no_stars; i++ )
	{
	    /* Compute the farthest closest star so far; only if we have previously modified the list */
        MAX_CLOSEST_DISTANCE = closest_star[0].distance_from_camera;
        index = 0;
        for ( j=1; j<NO_CLOSEST_STARS; j++ )
            if ( closest_star[j].distance_from_camera > MAX_CLOSEST_DISTANCE )
            {
                MAX_CLOSEST_DISTANCE = closest_star[j].distance_from_camera;
                index = j;
            }

        /* Compute the distance from camera for each star */
        d = distance ( camera.coords.posx, camera.coords.posy, camera.coords.posz, star[i].x, star[i].y, star[i].z );
        /* If its distance is smaller then the most distant closest star replace it */
        if ( d < MAX_CLOSEST_DISTANCE ) {
            closest_star[index].index = i;
            closest_star[index].distance_from_camera = d;
            changed = 1;
        }
	}


	for (i=0; i<NO_CLOSEST_STARS; i++ )
	{
	    glDisable(GL_LIGHTING);
        glPushMatrix ( );
        index = closest_star[i].index;
	    star_name = "";
        if (strcmp(star[index].name,"n/a")==0)
        {
            if (strcmp(star[index].bayer_flamsteed,"n/a")==0)
            {
                if (strcmp(star[index].gliese,"n/a")==0)
                {
                    if (strcmp(star[index].hd,"n/a")!=0)
                    {
                        star_name = (char*)malloc(strlen(star[index].hd)+1);
                        strcpy(star_name, star[index].hd);
                    }
                }
                else {
                    star_name = (char*)malloc(strlen(star[index].gliese)+1);
                    strcpy(star_name, star[index].gliese);
                }
            }
            else {
                star_name = (char*)malloc(strlen(star[index].bayer_flamsteed)+1);
                strcpy(star_name, star[index].bayer_flamsteed);
            }
        }
        else {
            star_name = (char*)malloc(strlen(star[index].name)+1);
            strcpy(star_name, star[index].name);
        }

        switch (star[index].spectrum[0])  // Color depending on the spectral type
        {
            case 'O':   texture_index = OSTAR_TEXTURE; texture_bb_id = OSTARBILLBOARD_TEXTURE; break;
            case 'B':   texture_index = OSTAR_TEXTURE; texture_bb_id = OSTARBILLBOARD_TEXTURE; break;
            case 'A':   texture_index = ASTAR_TEXTURE; texture_bb_id = ASTARBILLBOARD_TEXTURE; break;
            case 'F':   texture_index = GSTAR_TEXTURE; texture_bb_id = GSTARBILLBOARD_TEXTURE; break;
            case 'G':   texture_index = GSTAR_TEXTURE; texture_bb_id = GSTARBILLBOARD_TEXTURE; break;
            case 'K':   texture_index = MSTAR_TEXTURE; texture_bb_id = MSTARBILLBOARD_TEXTURE; break;
            case 'M':   texture_index = MSTAR_TEXTURE; texture_bb_id = MSTARBILLBOARD_TEXTURE; break;
            case 'C':   texture_index = MSTAR_TEXTURE; texture_bb_id = MSTARBILLBOARD_TEXTURE; break;
            case 'R':
            case 'N':
            case 'S':   texture_index = MSTAR_TEXTURE; texture_bb_id = MSTARBILLBOARD_TEXTURE; break;
            default :   texture_index = GSTAR_TEXTURE; texture_bb_id = GSTARBILLBOARD_TEXTURE; break;
        }
        ;

        glColor3ub ( 255, 0, 0 );
        glut_print_string ( star[index].x, star[index].y, star[index].z, star_name );

        if ( show_star )
        {
            glPushMatrix ( );
                object_pos[0] = star[index].x;
                object_pos[1] = star[index].y;
                object_pos[2] = star[index].z;
                glTranslatef ( star[index].x, star[index].y, star[index].z );
                draw_sun_billboard ( 30 * 3.8, texture_id[texture_bb_id], object_pos );
                draw_sun ( 30, texture_index, star[index].spectrum[0] );
            glPopMatrix ( );

            if ( distance ( camera.coords.posx, camera.coords.posy, camera.coords.posz, star[index].x, star[index].y, star[index].z ) < STAR_SYSTEM_RANGE )
            {
                make_lens_flares ( star[index].x, star[index].y, star[index].z, 0 );
            }
        }
        glPopMatrix();
        glEnable(GL_LIGHTING);
    }
}


void draw_star_names ( star_info *star )
{
    int i=0;
    char *star_name;
    glPushMatrix ( );
	for (i=0; i<actual_no_stars; i++ )
	{
	    star_name = "";
        if (strcmp(star[i].name,"n/a")!=0)
        {
            star_name = (char*)malloc(strlen(star[i].name)+1);
            strcpy(star_name, star[i].name);
        }

        glDisable(GL_LIGHTING);
        glColor3ub ( 255, 255, 255 );
        glut_print_string ( star[i].x, star[i].y, star[i].z, star_name );
        glEnable(GL_LIGHTING);
    }
    glPopMatrix();
}

void draw_stars ( star_info *star, float scale_factor )
{
	long i, j;
	float RGB[3];
	int no_batches;
	const int stars_in_batch = 3500;


	glPushMatrix ( );
		glDisable ( GL_LIGHTING );
		no_batches = actual_no_stars / stars_in_batch;
		j = 0;
		while ( j <= no_batches )
		{
			glBegin ( GL_POINTS );
			for (i = j * stars_in_batch ; i < (j+1) * stars_in_batch ; i++)
			{
				switch (star[i].spectrum[0])  // Color depending on the spectral type
				{
					case 'O':   RGB[0] = 0.8f;  RGB[1] = 1.0f; RGB[2] = 1.3f;  break;
					case 'B':   RGB[0] = 0.9f;  RGB[1] = 1.0f; RGB[2] = 1.2f;  break;
					case 'A':   RGB[0] = 0.95f; RGB[1] = 1.0f; RGB[2] = 1.15f; break;
					case 'F':   RGB[0] = 1.05f; RGB[1] = 1.0f; RGB[2] = 1.05f; break;
					case 'G':   RGB[0] = 1.3f;  RGB[1] = 1.0f; RGB[2] = 0.9f;  break;
					case 'K':   RGB[0] = 1.15f; RGB[1] = 0.95f;RGB[2] = 0.8f;  break;
					case 'M':   RGB[0] = 1.15f; RGB[1] = 0.85f;RGB[2] = 0.8f;  break;
					case 'C':   RGB[0] = 1.3f;  RGB[1] = 0.85f;RGB[2] = 0.6f;  break;
					case 'R':
					case 'N':
					case 'S':   RGB[0] = 1.5f;  RGB[1] = 0.8f; RGB[2] = 0.2f;  break;
					default :   RGB[0] = 1.0f;  RGB[1] = 1.0f; RGB[2] = 1.0f;  break;
				}
				glColor3ub ( RGB[0] * 100, RGB[1] * 100, RGB[2] * 100 );
				glVertex3f ( star[i].x, star[i].y, star[i].z );
            }
			glEnd ();
			j++;
		}
		glEnable ( GL_LIGHTING );

	glPopMatrix ( );
}

void draw_constellation_3d ( constellation_info *constellation, constellation_boundary_info *constellation_boundary, constellation_name_info *constellation_name, beyer_info *beyer )
{
	int i;

	glDisable ( GL_LIGHTING );
	glPushMatrix ( );

		glLineWidth ( 0.1 );
		glColor3ub ( 235, 199, 158 );
		for (i=0; i<NO_CONSTELLATION_LINES; i++)
		{
			glBegin ( GL_LINES );
				glVertex3d ( constellation[i].x1, constellation[i].y1, constellation[i].z1 );
				glVertex3d ( constellation[i].x2, constellation[i].y2, constellation[i].z2 );
			glEnd ( );
		}

		glLineWidth ( 0.1 );
		glLineStipple(1, 0x3F07);
		glEnable(GL_LINE_STIPPLE);

		glColor3ub ( 191, 239, 255 );
		for (i=0; i<NO_CONSTELLATION_BOUNDARIES; i++)
		{
			glBegin ( GL_LINES );
				glVertex3d ( constellation_boundary[i].x1, constellation_boundary[i].y1, constellation_boundary[i].z1 );
				glVertex3d ( constellation_boundary[i].x2, constellation_boundary[i].y2, constellation_boundary[i].z2 );
			glEnd ( );
		}

		glDisable(GL_LINE_STIPPLE);

		glColor3ub ( 240, 230, 140 );
		for (i=0; i<NO_CONSTELLATION; i++)
		{
	      glut_print_string( constellation_name[i].x , constellation_name[i].y , constellation_name[i].z, constellation_name[i].name );
		}

		glColor3ub ( 209, 146, 117 );
		for (i=0; i<NO_BEYER; i++)
		{
	      glut_print_string( beyer[i].x, beyer[i].y, beyer[i].z, beyer[i].name );
		}
	glPopMatrix ( );
	glEnable ( GL_LIGHTING );

}

void draw_dso_objects ( dso_info *dso, float scale_factor )
{
	int i;
	double object_pos[3];

    glDisable ( GL_LIGHTING );
	glPushMatrix ( );

		glColor3ub ( 240, 230, 140 );
		for (i=0; i<actual_no_dsos; i++)
		{
          object_pos[0] = dso[i].x;
          object_pos[1] = dso[i].y;
          object_pos[2] = dso[i].z;
	      glut_print_string( dso[i].x , dso[i].y , dso[i].z, dso[i].name );
	      glPushMatrix ( );
            glTranslatef ( dso[i].x , dso[i].y , dso[i].z );
            draw_sun_billboard ( 1000, texture_dso_id[i], object_pos );
            //draw_sun ( 30, texture_id[0], 'G' );
          glPopMatrix ( );
		}
	glPopMatrix ( );
    glEnable ( GL_LIGHTING );
}

void draw_asteroids ( obj_type_ptr asteroids, asteroid_info *asteroid, asteroid_info *centaur, asteroid_info *transneptune, float scale )
{
	long i;
    double d;

    for (i=0; i<actual_no_asteroids; i++)
	{
		glPushMatrix();
			glDisable(GL_LIGHTING);
			glColor3ub ( 202, 255, 112 );
			glut_print_string ( asteroid[i].x * scale, asteroid[i].y * scale, asteroid[i].z * scale, asteroid[i].name );
			glEnable(GL_LIGHTING);

			glTranslatef ( asteroid[i].x * scale, asteroid[i].y * scale, asteroid[i].z * scale );
			//glScalef (0.05,0.05,0.05 );
			/* Compute the distance from camera for each asteroid */
            d = distance ( camera.coords.posx, camera.coords.posy, camera.coords.posz, asteroid[i].x * scale, asteroid[i].y * scale, asteroid[i].z * scale );
            if (d < DETAIL_LIMIT)
            {
                //printf("%s\n", asteroid[i].name);
                draw_3DS ( asteroids );
            }
		glPopMatrix();

	}

	for (i=0; i<actual_no_transneptunes; i++)
	{
		glPushMatrix();
			glDisable(GL_LIGHTING);
			glColor3ub ( 202, 255, 112 );
			glut_print_string ( transneptune[i].x * scale, transneptune[i].y * scale, transneptune[i].z * scale, transneptune[i].name );
			glEnable(GL_LIGHTING);

			glTranslatef ( transneptune[i].x * scale, transneptune[i].y * scale, transneptune[i].z * scale );
			//glScalef (0.05,0.05,0.05 );
			/* Compute the distance from camera for each asteroid */
            d = distance ( camera.coords.posx, camera.coords.posy, camera.coords.posz, transneptune[i].x * scale, transneptune[i].y * scale, transneptune[i].z * scale );
            if (d < DETAIL_LIMIT)
            {
                //printf("%s\n", transneptune[i].name);
                draw_3DS ( asteroids );
            }
		glPopMatrix();

	}

	for (i=0; i<actual_no_centaurs; i++)
	{
		glPushMatrix();
			glDisable(GL_LIGHTING);
			glColor3ub ( 193,255,193 );
			glut_print_string ( centaur[i].x * scale, centaur[i].y * scale, centaur[i].z * scale, centaur[i].name );
			glEnable(GL_LIGHTING);

			glTranslatef ( centaur[i].x * scale, centaur[i].y * scale, centaur[i].z * scale );
			//glScalef (0.05,0.05,0.05 );
			/* Compute the distance from camera for each asteroid */
            d = distance ( camera.coords.posx, camera.coords.posy, camera.coords.posz, centaur[i].x * scale, centaur[i].y * scale, centaur[i].z * scale );
            if (d < DETAIL_LIMIT)
            {
                //printf("%s\n", centaur[i].name);
                draw_3DS ( asteroids );
            }
            glPopMatrix();
	}
}

void draw_starchart ( const char* filename, star_info *star, dso_info *dso, double starmap_scale_factor, int width, int height, int offset )
{
    int i;
    double radius;
    char buff[255];
    FILE * file = open_ps_file ( filename );

    init_ps_file ( file, width, height, offset );

    draw_line ( file,  -1, 0, 1, 0, 0.6 );
    draw_line ( file, 0, -1, 0, 1, 0.6 );
    draw_text ( file, "-X", -1, 0, "Helvetica", 12, 0.f );
    draw_text ( file, "X", +1, 0, "Helvetica", 12, 0.f );
    draw_text ( file, "-Y", 0, -1, "Helvetica", 12, 0.f );
    draw_text ( file, "Y", 0, 1, "Helvetica", 12, 0.f );

    starmap_scale_factor *= 20000;

    for (i=0; i<actual_no_dsos; i++)
    {
        switch (dso[i].type[0])
        {
            case 'n':
                draw_empty_circle ( file, dso[i].x / starmap_scale_factor, dso[i].y / starmap_scale_factor, 4.5 );
                draw_empty_circle ( file, dso[i].x / starmap_scale_factor, dso[i].y / starmap_scale_factor, 2.5 );
                break;
            case 'g':
                draw_empty_circle ( file, dso[i].x / starmap_scale_factor, dso[i].y / starmap_scale_factor, 3.5 );
                break;
            case 'c':
                draw_empty_circle ( file, dso[i].x / starmap_scale_factor, dso[i].y / starmap_scale_factor, 4.5 );
                draw_filled_circle ( file, dso[i].x / starmap_scale_factor, dso[i].y / starmap_scale_factor, 2.5 );
                break;
        }
        sprintf( buff, "%s %0.1lf %.1lf", dso[i].name, dso[i].z / starmap_scale_factor, dso[i].dist );
        draw_text ( file, buff, dso[i].x / starmap_scale_factor, dso[i].y / starmap_scale_factor, "Helvetica", 12, 0.f );
    }

    starmap_scale_factor /= 20000;

    for (i=0; i<actual_no_stars; i++)
    {
        if (star[i].x / starmap_scale_factor > 10 || star[i].y / starmap_scale_factor > 10 || star[i].y / starmap_scale_factor < -10 || star[i].x / starmap_scale_factor < -10)
            continue;

        if ( star[i].dist > 50 )
            continue;

        switch ((int)star[i].magn)
        {
            case -2: radius = 10; break;
            case -1: radius = 8; break;
            case 0: radius = 6; break;
            case 1: radius = 4; break;
            case 2: radius = 3; break;
            case 3: radius = 2; break;
            case 4: radius = 1.5; break;
            case 5: radius = 1.0; break;
            case 6: radius = 0.75; break;
            case 7: radius = 0.5; break;
            default: radius = 1.0; break;
        }

        draw_filled_circle ( file, star[i].x / starmap_scale_factor, star[i].y / starmap_scale_factor, radius );

        if ( strcmp ( star[i].name, "n/a" ) != 0 )
        {
            sprintf( buff, "%s %.1lf %.1lf", star[i].name, star[i].z / starmap_scale_factor, star[i].dist );
            draw_text ( file, buff, star[i].x / starmap_scale_factor, star[i].y / starmap_scale_factor, "Helvetica", 12, 0.f );
        }
    }

    draw_legend ( file, 1 );

    close_ps_file ( file );
}

void draw_legend ( FILE* file, double scala )
{
    int width = 2;
    int height = 2;

    draw_rect ( file, width - 1.6 * scala,  height - 0.9 * scala, width - 0.6 * scala, height - 0.1 * scala );
    draw_text ( file, "Starmap showing objects up to a distance of 50 parsecs", width - 2. * scala, height - 0.05 * scala, "Arial", 36, 0 );
    draw_text ( file, "LEGEND", width - 1.3 * scala, height - 0.1 * scala, "Helvetica", 14, 0 );
    draw_text ( file, "Galaxy", width - 1.2 * scala, height - 0.15 * scala, "Helvetica", 14, 0 );
    draw_empty_circle ( file, width - 1.4 * scala, height - 0.15 * scala, 3.5 );
    draw_text ( file, "Nebula", width - 1.2 * scala, height - 0.2 * scala, "Helvetica", 14, 0 );
    draw_empty_circle ( file, width - 1.4 * scala, height - 0.2 * scala, 4.5 );
    draw_empty_circle ( file, width - 1.4 * scala, height - 0.2 * scala, 2.5 );
    draw_text ( file, "Cluster", width - 1.2 * scala, height - 0.25 * scala, "Helvetica", 14, 0 );
    draw_empty_circle ( file, width - 1.4 * scala,  height - 0.25 * scala, 4.5 );
    draw_filled_circle ( file, width - 1.4 * scala, height - 0.25 * scala, 2.5 );
    draw_text ( file, "Magnitude", width - 1.2 * scala, height - 0.3 * scala, "Helvetica", 14, 0 );
    draw_text ( file, "7", width - 1.2 * scala, height - 0.35 * scala, "Helvetica", 14, 0 );
    draw_filled_circle ( file, width - 1.4 * scala, height - 0.35 * scala, 0.5 );
    draw_text ( file, "6", width - 1.2 * scala, height - 4 * scala, "Helvetica", 14, 0);
    draw_filled_circle ( file, width - 1.4 * scala, height - 4 * scala, 0.75 );
    draw_text ( file, "5", width - 1.2 * scala, height - 0.45 * scala, "Helvetica", 14, 0 );
    draw_filled_circle ( file, width - 1.4 * scala, height - 0.45 * scala, 1 );
    draw_text ( file, "4", width - 1.2 * scala, height - 0.5 * scala, "Helvetica", 14, 0 );
    draw_filled_circle ( file, width - 1.4 * scala, height - 0.5 * scala, 1.5 );
    draw_text ( file, "3", width - 1.2 * scala, height - 0.55 * scala, "Helvetica", 14, 0 );
    draw_filled_circle ( file, width - 1.4 * scala, height - 0.55 * scala, 2 );
    draw_text ( file, "2", width - 1.2 * scala, height - 0.6 * scala, "Helvetica", 14, 0 );
    draw_filled_circle ( file, width - 1.4 * scala, height - 0.6 * scala, 3 );
    draw_text ( file, "1", width - 1.2 * scala, height - 0.65 * scala, "Helvetica", 14, 0 );
    draw_filled_circle ( file, width - 1.4 * scala, height - 0.65 * scala, 4 );
    draw_text ( file, "0", width - 1.2 * scala, height - 0.7 * scala, "Helvetica", 14, 0 );
    draw_filled_circle ( file, width - 1.4 * scala, height - 0.7 * scala, 6 );
    draw_text ( file, "-1", width - 1.2 * scala, height - 0.75 * scala, "Helvetica", 14, 0 );
    draw_filled_circle( file, width - 1.4 * scala, height - 0.75 * scala, 8 );
    draw_text ( file, "-2", width - 1.2 * scala, height - 0.8 * scala, "Helvetica", 14, 0 );
    draw_filled_circle ( file, width - 1.4 * scala, height - 0.8 * scala, 10 );
    draw_text ( file, "Copyright Marc Frincu, 2009, mfrincu@info.uvt.ro", width - 1.5 * scala, height - 0.9 * scala, "Arial", 12, 0 );
}

void make_lens_flares ( float x, float y, float z, float size ) {

    double light_cam_dir[3], cam_pos[3];
    double light_center_dir[3];
    double camera_dir[3];
    double flare_pos[3];
    double pt_intersect[3],pt_intersect_untranslated[3];
    double light_pos[3];
	float matrix[16];
    float length = 0.f, length2 = 0.f;

    update_frustum ( );
    if ( is_point_in_frustum ( x, y, z, size ) ) {

        light_pos[0] = x;
        light_pos[1] = y;
        light_pos[2] = z;

        cam_pos[0] = camera.coords.posx;
        cam_pos[1] = camera.coords.posy;
        cam_pos[2] = camera.coords.posz;

        camera_dir[0] = cam_eye[0];
        camera_dir[1] = cam_eye[1];
        camera_dir[2] = cam_eye[2];

        glGetFloatv(GL_MODELVIEW_MATRIX, matrix);

     //   vdiff(camera_dir, camera_dir, cam_pos);
        vnorm(camera_dir);

        //camera_dir[0] = matrix[8];
        //camera_dir[1] = matrix[9];
        //camera_dir[2] = -matrix[10];

    //    printf("Cam dir: %f %f %f\n", camera_dir[0],camera_dir[1],camera_dir[2]);

        // Compute camera-light vector
        vdiff ( light_cam_dir, cam_pos, light_pos );
        length = vlen ( light_cam_dir );

     //   printf("Cam-light dir: %f %f %f %f\n", light_cam_dir[0],light_cam_dir[1],light_cam_dir[2], length);

        vscale ( pt_intersect_untranslated, camera_dir, length);
        vadd ( pt_intersect, pt_intersect_untranslated, cam_pos );

      //  printf("Center dir: %f %f %f\n", pt_intersect[0],pt_intersect[1],pt_intersect[2]);

        // Compute the light-center_scene vector
        vdiff ( light_center_dir, pt_intersect, light_pos );
        length2 = vlen ( light_center_dir );
        vnorm ( light_center_dir );

      //  printf("light-Center dir: %f %f %f %f\n", light_center_dir[0],light_center_dir[1],light_center_dir[2], length2);

        glEnable ( GL_BLEND );
		glBlendFunc ( GL_SRC_ALPHA, GL_ONE );
		glDisable ( GL_DEPTH_TEST );
		glEnable ( GL_TEXTURE_2D );

        vscale ( flare_pos, light_center_dir, length2 * 0.5 );
        vadd ( flare_pos, flare_pos, light_pos );

        draw_flare ( 0.9f, 0.6f, 0.4f, 0.5f, 100.6, flare_pos, SMALL_HALO_TEXTURE );

        vscale ( flare_pos, light_center_dir, length2 * 0.7 );
        vadd ( flare_pos, flare_pos, light_pos );

        draw_flare ( 0.9f, 0.6f, 0.4f, 0.5f, 100.6, flare_pos, GLOW_TEXTURE );

        vscale ( flare_pos, light_center_dir, length2 * 1 );
        vadd ( flare_pos, flare_pos, light_pos );

        draw_flare ( 0.7f, 0.7f, 0.4f, 0.5f, 200.6, flare_pos, LARGE_HALO_TEXTURE );

        vscale ( flare_pos, light_center_dir, length2 * 1.1 );
        vadd ( flare_pos, flare_pos, light_pos );

        draw_flare ( 0.9f, 0.6f, 0.4f, 0.5f, 100.6, flare_pos, GLOW_TEXTURE );

        vscale ( flare_pos, light_center_dir, length2 * 1.7 );
        vadd ( flare_pos, flare_pos, light_pos );

        draw_flare ( 0.9f, 0.2f, 0.1f, 0.5f, 150.6, flare_pos, HALO_TEXTURE );

        glDisable ( GL_BLEND );
		glEnable ( GL_DEPTH_TEST );
		glDisable ( GL_TEXTURE_2D );

    }
}

void draw_flare ( float r, float g, float b, float a, float scale,double flare_pos[], int text_id )
{
    double p1[3], p2[3], p3[3], p4[3];
	double alfa, beta, cosalfa, cosbeta, sinalfa, sinbeta;
	double x, y, z;
	double cam[3], cam_object_dir[3];

    cam[0] = camera.coords.posx;
    cam[1] = camera.coords.posy;
    cam[2] = camera.coords.posz;

    // Compute the Camera-Object vector
    vdiff ( cam_object_dir, cam, flare_pos );
    vnorm ( cam_object_dir );

    p1[0] = - scale;
    p1[1] = - scale;
    p1[2] = 0;
    p2[0] = + scale;
    p2[1] = - scale;
    p2[2] = 0;
    p3[0] = + scale;
    p3[1] = + scale;
    p3[2] = 0;
    p4[0] = - scale;
    p4[1] = + scale;
    p4[2] = 0;

    alfa = atan2 ( cam_object_dir[2], cam_object_dir[0]) - radian_correction * 180.0f / 2.0;
    beta = atan2 ( cam_object_dir[1], sqrt ( pow ( cam_object_dir[0], 2 ) + pow ( cam_object_dir[2], 2 )) );

	cosalfa = cos ( alfa );
	cosbeta = cos ( beta );
	sinalfa = sin ( alfa );
	sinbeta = sin ( beta );

	ROTATE ( p1 );
	ROTATE ( p2 );
	ROTATE ( p3 );
	ROTATE ( p4 );

    glPushMatrix ( );
        glTranslatef ( flare_pos[0], flare_pos[1], flare_pos[2] );

        glBindTexture ( GL_TEXTURE_2D, texture_id[text_id] );
        glColor4f ( r, g, b, a );
        glBegin ( GL_QUADS );
            glTexCoord2f ( 0.0, 0.0 );
            glVertex3f ( p1[0], p1[1], p1[2] );
            glTexCoord2f ( 0.0, 1.0 );
            glVertex3f ( p2[0], p2[1], p2[2] );
            glTexCoord2f ( 1.0, 1.0 );
            glVertex3f (p3[0], p3[1], p3[2] );
            glTexCoord2f ( 1.0, 0.0 );
            glVertex3f ( p4[0], p4[1], p4[2] );
        glEnd ( );
    glPopMatrix ( );
}

void draw_comet_tail ( particle *particles, int text_id, double sun_pos[], double object_pos[], double scale_factor )
{
    int i;
    float x = 0.f, y = 0.f, z = 0.f, scale = 10.f;
    double sun_object_dir[3], cam_object_dir[3], cam[3];
    double p1[3], p2[3], p3[3], p4[3];
	double alfa, beta, cosalfa, cosbeta, sinalfa, sinbeta;

    cam[0] = camera.coords.posx;
    cam[1] = camera.coords.posy;
    cam[2] = camera.coords.posz;

    // Compute Sun-Object vector
    vdiff ( sun_object_dir, object_pos, sun_pos );
    // Normalize vector
    vnorm ( sun_object_dir );

    glShadeModel ( GL_SMOOTH );							    // Enable Smooth Shading
	glClearDepth ( 1.0f );									// Depth Buffer Setup
	glDisable ( GL_DEPTH_TEST );							// Disable Depth Testing
	glEnable ( GL_BLEND );									// Enable Blending
	glBlendFunc ( GL_SRC_ALPHA, GL_ONE );					// Type Of Blending To Perform
	glHint ( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );	// Really Nice Perspective Calculations
	glHint ( GL_POINT_SMOOTH_HINT, GL_NICEST );				// Really Nice Point Smoothing
	glEnable ( GL_TEXTURE_2D );							    // Enable Texture Mapping
	glBindTexture ( GL_TEXTURE_2D, texture_id[text_id] );	// Select Our Texture

    for ( i=0; i<NUM_PARTICLES; i++ )
    {
        if ( particles[i].life <= 0.f )
        {
            // Initialize particle and set its acceleration to be equal with the normalized Sun-Object vector
            initialize_particle ( &particles[i], object_pos, sun_object_dir );
        }
        else
        {
            // Compute the Camera-Object vector
            vdiff ( cam_object_dir, cam, particles[i].p );
            vnorm ( cam_object_dir );

            p1[0] = - scale;
            p1[1] = - scale;
            p1[2] = 0;
            p2[0] = + scale;
            p2[1] = - scale;
            p2[2] = 0;
            p3[0] = + scale;
            p3[1] = + scale;
            p3[2] = 0;
            p4[0] = - scale;
            p4[1] = + scale;
            p4[2] = 0;

            //p1[0] = p4[0] = p1[1] = p2[1] = scale;
            //p2[0] = p3[0] = p3[1] = p4[1] = -scale;
            //p1[2] = p2[2] = p3[2] = p4[2] = 0.0;

            /* Billboarding effect, rotates particle tile so it shows always the same face and angle to camera */
            alfa = atan2 ( cam_object_dir[2], cam_object_dir[0]) - radian_correction * 180.0f / 2.0;
            beta = atan2 ( cam_object_dir[1], sqrt ( pow ( cam_object_dir[0], 2 ) + pow ( cam_object_dir[2], 2 )) );

            cosalfa = cos ( alfa );
            cosbeta = cos ( beta );
            sinalfa = sin ( alfa );
            sinbeta = sin ( beta );

            ROTATE ( p1 );
            ROTATE ( p2 );
            ROTATE ( p3 );
            ROTATE ( p4 );

            glPushMatrix ( );
                glTranslatef ( particles[i].p[0], particles[i].p[1], particles[i].p[2] );

                // Set the particle's color
                glColor4f ( particles[i].c[0], particles[i].c[1], particles[i].c[2], particles[i].life );
                // Draw the particle
                glBegin ( GL_QUADS );
                    glTexCoord2f ( 0.0, 1.0 );
                    glVertex3f ( p1[0], p1[1], p1[2] );
                    glTexCoord2f ( 1.0, 1.0 );
                    glVertex3f ( p2[0], p2[1], p2[2] );
                    glTexCoord2f ( 1.0, 0.0 );
                    glVertex3f (p3[0], p3[1], p3[2] );
                    glTexCoord2f ( 0.0, 0.0 );
                    glVertex3f ( p4[0], p4[1], p4[2] );
                glEnd ( );

            glPopMatrix ( );

            move_particle ( &particles[i] );
            //update_comet_particle_acceleration ( &particles[i], sun_object_dir );
        }

       // printf("%f %f %f %f %f %f %f %f\n", particles[i].life, particles[i].fade, particles[i].p[0], particles[i].p[1], particles[i].p[2], particles[i].a[0], particles[i].a[1], particles[i].a[2]);
    }

    glDisable ( GL_TEXTURE_2D );
    glDisable ( GL_BLEND );
    glEnable ( GL_DEPTH_TEST );

}
