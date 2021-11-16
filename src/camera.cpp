#include "include/camera.h"
#include "include/util.h"
#include "vec3d.c"

#include <math.h>
#include <stdio.h>

double cam_eye[3], cam_pos[3], cam_up[3];

double m_Frustum[6][4];

world_camera camera;
time_camera delta_t;

static int check_collision ( double pi[], double qi[], double vp[], double vq[], double rp, double rq )
{
    double a[3], b[3];

    vdiff ( a, pi, qi );
    vdiff ( b, vp, vq );

    double b_2 = vdot ( b, b );

    if ( b_2 != 0 )
    {

        double t = ( -vdot( a, b ) - sqrt ( vdot ( a, b ) * vdot ( a, b ) - vdot ( b, b ) * ( vdot ( a, a ) - ( rp + rq ) * ( rp + rq ) ) ) ) / vdot ( b, b );

        if ( t >= 0 && t < 1 )
        {
            return 1;
        }
    }
    return 0;
}

void move_camera ( planet_info *planet, asteroid_info *asteroid, asteroid_info *centaur, asteroid_info *transneptune, dso_info *dso, star_info *star, float scale_factor )
{

   float newx, newy, newz;

   polar_to_cartesian ( camera.azimuth, camera.speed, camera.elevation,
                        &newx, &newy, &newz );

    /* start collision detection */
   float tmp_newx, tmp_newy, tmp_newz;
   polar_to_cartesian ( camera.azimuth, camera.speed, camera.elevation,
                        &tmp_newx, &tmp_newy, &tmp_newz );

   double pi[3] = { camera.coords.posx, camera.coords.posy, camera.coords.posz };
   double vp[3] = { tmp_newx + delta_t.elapsedTime, tmp_newy + delta_t.elapsedTime, tmp_newz + delta_t.elapsedTime };
   double rp = 1;

   double qi[] = { 0, 0, 0 };
   double vq[3] = { 0, 0, 0 };
   double rq = 0;

   int i,j;
   for ( i=1; i<NO_PLANETS; i++ )
   {
       qi[0] = p_planet[i].xe * scale_factor;
       qi[1] = p_planet[i].ye * scale_factor;
       qi[2] = p_planet[i].ze * scale_factor;
       rq = p_planet[i].size;

       if (check_collision (pi, qi, vp, vq, rp, rq))
       {
           camera.speed = 0;
           return;
       }

       for ( j=0; j<planet[i].no_moons; j++)
       {
           qi[0] = p_planet[i].xe * scale_factor + p_planet[i].moon[j].x * planet[i].moon_scale_factor;
           qi[1] = p_planet[i].ye * scale_factor + p_planet[i].moon[j].y * planet[i].moon_scale_factor;
           qi[2] = p_planet[i].ze * scale_factor + p_planet[i].moon[j].z * planet[i].moon_scale_factor;
           rq = p_planet[i].moon[j].size;

           if (check_collision (pi, qi, vp, vq, rp, rq))
           {
               camera.speed = 0;
               return;
           }
       }

    for (i=0; i<actual_no_asteroids; i++)
	{
       qi[0] = asteroid[i].x * scale_factor;
       qi[1] = asteroid[i].y * scale_factor;
       qi[2] = asteroid[i].z * scale_factor;
       rq = 5;

       if (check_collision (pi, qi, vp, vq, rp, rq))
       {
           camera.speed = 0;
           return;
       }
	}
	for (i=0; i<actual_no_transneptunes; i++)
	{
       qi[0] = transneptune[i].x * scale_factor;
       qi[1] = transneptune[i].y * scale_factor;
       qi[2] = transneptune[i].z * scale_factor;
       rq = 5;
	}
	for (i=0; i<actual_no_centaurs; i++)
	{
       qi[0] = centaur[i].x * scale_factor;
       qi[1] = centaur[i].y * scale_factor;
       qi[2] = centaur[i].z * scale_factor;
       rq = 5;
	}
	// TODO: add test for comets
/*	for (i=0; i<actual_no_stars; i++)
	{

	}
*/
   }
   /*end collision*/

   // Replace old x, y, z coords for camera
   camera.coords.posx += ( newx * delta_t.elapsedTime );
   camera.coords.posy += ( newy * delta_t.elapsedTime );
   camera.coords.posz += ( newz * delta_t.elapsedTime );
}

void aim_camera( void )
{
   float centerx, centery, centerz, length;

   //length = 100.0f;
   length = sqrt ( pow ( camera.coords.posx, 2 ) + pow ( camera.coords.posy, 2 ) + pow ( camera.coords.posz, 2 ) );

   glLoadIdentity ( );

   // Calc new eye vector
   polar_to_cartesian ( camera.azimuth, length,
                        camera.elevation, &centerx, &centery, &centerz );

   cam_eye[0] = centerx;
   cam_eye[1] = centery;
   cam_eye[2] = centerz;

   // Calc new up vector
   polar_to_cartesian ( camera.azimuth, length, camera.elevation + 90,
                        &camera.upx, &camera.upy, &camera.upz );

   cam_pos[0] = camera.upx;
   cam_pos[1] = camera.upy;
   cam_pos[2] = camera.upz;

   gluLookAt ( camera.coords.posx, camera.coords.posy, camera.coords.posz,
		camera.coords.posx + centerx, camera.coords.posy + centery,
		camera.coords.posz + centerz, camera.upx, camera.upy, camera.upz );
}

void aim_camera( float centerx, float centery, float centerz, float length )
{
   glLoadIdentity ( );

   cam_eye[0] = centerx;
   cam_eye[1] = centery;
   cam_eye[2] = centerz;

   // Calc new up vector
   polar_to_cartesian ( camera.azimuth, length, camera.elevation + 90,
                        &camera.upx, &camera.upy, &camera.upz );

   cam_pos[0] = camera.upx;
   cam_pos[1] = camera.upy;
   cam_pos[2] = camera.upz;

   gluLookAt ( camera.coords.posx, camera.coords.posy, camera.coords.posz,
		camera.coords.posx + centerx, camera.coords.posy + centery,
		camera.coords.posz + centerz, camera.upx, camera.upy, camera.upz );
}

void initial_camera_position ( double x, double y, double z )
{
   camera.coords.posx = x;
   camera.coords.posy = y;
   camera.coords.posz = z;

   camera.upx = 0.0;
   camera.upy = 1.0;
   camera.upz = 0.0;

   camera.azimuth   = 1.0f;
   camera.elevation = 0.0f;

   camera.speed = 0.0f;
}

void update_frustum ( ) {
    GLfloat	clip[16];
	GLfloat	proj[16];
	GLfloat	modl[16];
	GLfloat	t;

	// Get The Current PROJECTION Matrix From OpenGL
	glGetFloatv( GL_PROJECTION_MATRIX, proj );

	// Get The Current MODELVIEW Matrix From OpenGL
	glGetFloatv( GL_MODELVIEW_MATRIX, modl );

	// Combine The Two Matrices (Multiply Projection By Modelview)
	clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
	clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
	clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
	clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

	clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
	clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
	clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
	clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

	clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
	clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
	clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
	clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

	clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
	clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
	clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
	clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

	// Extract The Numbers For The RIGHT Plane
	m_Frustum[0][0] = clip[ 3] - clip[ 0];
	m_Frustum[0][1] = clip[ 7] - clip[ 4];
	m_Frustum[0][2] = clip[11] - clip[ 8];
	m_Frustum[0][3] = clip[15] - clip[12];

	// Normalize The Result
	t = GLfloat(sqrt( m_Frustum[0][0] * m_Frustum[0][0] + m_Frustum[0][1] * m_Frustum[0][1] + m_Frustum[0][2] * m_Frustum[0][2] ));
	m_Frustum[0][0] /= t;
	m_Frustum[0][1] /= t;
	m_Frustum[0][2] /= t;
	m_Frustum[0][3] /= t;

	// Extract The Numbers For The LEFT Plane
	m_Frustum[1][0] = clip[ 3] + clip[ 0];
	m_Frustum[1][1] = clip[ 7] + clip[ 4];
	m_Frustum[1][2] = clip[11] + clip[ 8];
	m_Frustum[1][3] = clip[15] + clip[12];

	// Normalize The Result
	t = GLfloat(sqrt( m_Frustum[1][0] * m_Frustum[1][0] + m_Frustum[1][1] * m_Frustum[1][1] + m_Frustum[1][2] * m_Frustum[1][2] ));
	m_Frustum[1][0] /= t;
	m_Frustum[1][1] /= t;
	m_Frustum[1][2] /= t;
	m_Frustum[1][3] /= t;

	// Extract The BOTTOM Plane
	m_Frustum[2][0] = clip[ 3] + clip[ 1];
	m_Frustum[2][1] = clip[ 7] + clip[ 5];
	m_Frustum[2][2] = clip[11] + clip[ 9];
	m_Frustum[2][3] = clip[15] + clip[13];

	// Normalize The Result
	t = GLfloat(sqrt( m_Frustum[2][0] * m_Frustum[2][0] + m_Frustum[2][1] * m_Frustum[2][1] + m_Frustum[2][2] * m_Frustum[2][2] ));
	m_Frustum[2][0] /= t;
	m_Frustum[2][1] /= t;
	m_Frustum[2][2] /= t;
	m_Frustum[2][3] /= t;

	// Extract The TOP Plane
	m_Frustum[3][0] = clip[ 3] - clip[ 1];
	m_Frustum[3][1] = clip[ 7] - clip[ 5];
	m_Frustum[3][2] = clip[11] - clip[ 9];
	m_Frustum[3][3] = clip[15] - clip[13];

	// Normalize The Result
	t = GLfloat(sqrt( m_Frustum[3][0] * m_Frustum[3][0] + m_Frustum[3][1] * m_Frustum[3][1] + m_Frustum[3][2] * m_Frustum[3][2] ));
	m_Frustum[3][0] /= t;
	m_Frustum[3][1] /= t;
	m_Frustum[3][2] /= t;
	m_Frustum[3][3] /= t;

	// Extract The FAR Plane
	m_Frustum[4][0] = clip[ 3] - clip[ 2];
	m_Frustum[4][1] = clip[ 7] - clip[ 6];
	m_Frustum[4][2] = clip[11] - clip[10];
	m_Frustum[4][3] = clip[15] - clip[14];

	// Normalize The Result
	t = GLfloat(sqrt( m_Frustum[4][0] * m_Frustum[4][0] + m_Frustum[4][1] * m_Frustum[4][1] + m_Frustum[4][2] * m_Frustum[4][2] ));
	m_Frustum[4][0] /= t;
	m_Frustum[4][1] /= t;
	m_Frustum[4][2] /= t;
	m_Frustum[4][3] /= t;

	// Extract The NEAR Plane
	m_Frustum[5][0] = clip[ 3] + clip[ 2];
	m_Frustum[5][1] = clip[ 7] + clip[ 6];
	m_Frustum[5][2] = clip[11] + clip[10];
	m_Frustum[5][3] = clip[15] + clip[14];

	// Normalize The Result
	t = GLfloat(sqrt( m_Frustum[5][0] * m_Frustum[5][0] + m_Frustum[5][1] * m_Frustum[5][1] + m_Frustum[5][2] * m_Frustum[5][2] ));
	m_Frustum[5][0] /= t;
	m_Frustum[5][1] /= t;
	m_Frustum[5][2] /= t;
	m_Frustum[5][3] /= t;
}

int is_point_in_frustum(double x, double y, double z, double radius)
{
	int i;
	// The Idea Behind This Algorithum Is That If The Point
	// Is Inside All 6 Clipping Planes Then It Is Inside Our
	// Viewing Volume So We Can Return True.
	for(i = 0; i < 6; i++)
	{
		if(m_Frustum[i][0] * x + m_Frustum[i][1] * y + m_Frustum[i][2] * z + m_Frustum[i][3] <= radius)
		{
			return 0;
		}
	}
	return 1;
}
