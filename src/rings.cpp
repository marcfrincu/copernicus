/*
    OpenUniverse 1.0
    Copyright (C) 2000  Raul Alonso <amil@las.es>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

	 Modified by Marc Frincu, March 11th 2009 for use in the Copernicus Space Simulator v1.6
	 email: fmarc83@yahoo.com
*/
#ifndef __linux__
#include <windows.h>
#endif
#include <GL/glut.h>
#include <math.h>
#include <malloc.h>

#include "include/rings.h"
#include "include/astro.h"
#include "include/util.h"
#include "include/objects.h"

static void ring ( double, double, int, planet_info *p_planet, int index, float scale_factor );
static int in_range ( double, double, double );

void draw_ring ( float inner_radius, float outer_radius, int planet_index, int tex_index, int slices, float scale_factor )
{
	glBindTexture ( GL_TEXTURE_2D, texture_id[tex_index] );
	glDisable ( GL_CULL_FACE );
	glDisable ( GL_LIGHTING );
	glEnable ( GL_BLEND );
	glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	ring ( inner_radius, outer_radius, slices * 2, p_planet, planet_index, scale_factor );

	glEnable ( GL_LIGHTING );
	glEnable ( GL_CULL_FACE );
	glDisable ( GL_BLEND );
}


static void ring ( double innerRadius, double outerRadius, int slices, planet_info *p_planet, int index, float scale_factor )
{
	GLfloat da, dr;
	GLfloat sa, ca;
	GLfloat r1 = innerRadius;
	GLfloat r2;
	GLint s;
	GLfloat a;
	double b;

	if (!p_planet[index].rings_initialized) {
		p_planet[index].ring_vertex_list =
			(vertex_data *) malloc(sizeof(vertex_data) * (slices + 1) * 2);
		p_planet[index].rings_indices =
			(int *) malloc(sizeof(GLuint) * (slices + 1) * 2);

		da = 2.0 * radian_correction * 180.0f / slices;
		dr = (outerRadius - innerRadius);
		r2 = r1 + dr;

		for (s = 0; s <= slices; s++) {
			if (s == slices)
				a = 0.0;
			else
				a = s * da;
			sa = sin(a);
			ca = cos(a);
			if (s % 2) {
				p_planet[index].ring_vertex_list[s * 2].u = 1.0;
				p_planet[index].ring_vertex_list[s * 2].v = 0.0;
			} else {
				p_planet[index].ring_vertex_list[s * 2].u = 1.0;
				p_planet[index].ring_vertex_list[s * 2].v = 1.0;
			}

			p_planet[index].ring_vertex_list[s * 2].x = r2 * sa;
			p_planet[index].ring_vertex_list[s * 2].y = r2 * ca;
			p_planet[index].ring_vertex_list[s * 2].z = 0.0;
			p_planet[index].ring_vertex_list[s * 2].nx = 0.0;
			p_planet[index].ring_vertex_list[s * 2].ny = 0.0;
			p_planet[index].ring_vertex_list[s * 2].nz = 1.0;
			p_planet[index].rings_indices[s * 2] = s * 2;

			if (a < (radian_correction * 180.0f / 3.0)) {
				p_planet[index].ring_vertex_list[s * 2].r =
					p_planet[index].ring_vertex_list[s * 2].g =
					p_planet[index].ring_vertex_list[s * 2].b = 0.2;
				p_planet[index].ring_vertex_list[s * 2].a = 1.0;
			} else {
				p_planet[index].ring_vertex_list[s * 2].r = 255;
				p_planet[index].ring_vertex_list[s * 2].g = 255;
				p_planet[index].ring_vertex_list[s * 2].b = 255;
				p_planet[index].ring_vertex_list[s * 2].a = 1.0;
			}


			if (s % 2) {
				p_planet[index].ring_vertex_list[s * 2 + 1].u = 0.0;
				p_planet[index].ring_vertex_list[s * 2 + 1].v = 0.0;
			} else {
				p_planet[index].ring_vertex_list[s * 2 + 1].u = 0.0;
				p_planet[index].ring_vertex_list[s * 2 + 1].v = 1.0;
			}

			p_planet[index].ring_vertex_list[s * 2 + 1].x = r1 * sa;
			p_planet[index].ring_vertex_list[s * 2 + 1].y = r1 * ca;
			p_planet[index].ring_vertex_list[s * 2 + 1].z = 0.0;
			p_planet[index].ring_vertex_list[s * 2 + 1].nx = 0.0;
			p_planet[index].ring_vertex_list[s * 2 + 1].ny = 0.0;
			p_planet[index].ring_vertex_list[s * 2 + 1].nz = 1.0;
			p_planet[index].rings_indices[s * 2 + 1] = s * 2 + 1;

			if (a < (radian_correction * 180.0f / 3.0)) {
				p_planet[index].ring_vertex_list[s * 2 + 1].r =
					p_planet[index].ring_vertex_list[s * 2 + 1].g =
					p_planet[index].ring_vertex_list[s * 2 + 1].b = 0.2;
				p_planet[index].ring_vertex_list[s * 2 + 1].a = 1.0;
			} else {
				p_planet[index].ring_vertex_list[s * 2 + 1].r = 255;
				p_planet[index].ring_vertex_list[s * 2 + 1].g = 255;
				p_planet[index].ring_vertex_list[s * 2 + 1].b = 255;
				p_planet[index].ring_vertex_list[s * 2 + 1].a = 1.0;
			}
		}
		p_planet[index].rings_initialized = 1;

		glInterleavedArrays(GL_T2F_C4F_N3F_V3F, 0, p_planet[index].ring_vertex_list);
		glDrawElements(GL_QUAD_STRIP, (slices + 1) * 2, GL_UNSIGNED_INT,
					   p_planet[index].rings_indices);


	} else {
		da = 2.0 * radian_correction * 180.0f / slices;
		dr = (outerRadius - innerRadius);
		r2 = r1 + dr;
		for (s = 0; s <= slices; s++) {
			if (s == slices)
				a = 0.0;
			else
				a = s * da;
			sa = sin(a);
			ca = cos(a);
			b =

				atan2(p_planet[index].ze * scale_factor, p_planet[index].xe * scale_factor) + radian_correction * 180.0f / 3.0;
			while (b > (2 * radian_correction * 180.0f))
				b -= (2 * radian_correction * 180.0f);
			while (b < 0.0)
				b += (2 * radian_correction * 180.0f);
			if (in_range(a, b, b + radian_correction * 180.0f / 3.0)) {

				p_planet[index].ring_vertex_list[s * 2].r =
					p_planet[index].ring_vertex_list[s * 2].g =
					p_planet[index].ring_vertex_list[s * 2].b = 0.2;
				p_planet[index].ring_vertex_list[s * 2].a = 1.0;
				p_planet[index].ring_vertex_list[s * 2 + 1].r =
					p_planet[index].ring_vertex_list[s * 2 + 1].g =
					p_planet[index].ring_vertex_list[s * 2 + 1].b = 0.2;
				p_planet[index].ring_vertex_list[s * 2 + 1].a = 1.0;
			} else {
				p_planet[index].ring_vertex_list[s * 2].r = 255;
				p_planet[index].ring_vertex_list[s * 2].g = 255;
				p_planet[index].ring_vertex_list[s * 2].b = 255;
				p_planet[index].ring_vertex_list[s * 2].a = 1.0;
				p_planet[index].ring_vertex_list[s * 2 + 1].r = 255;
				p_planet[index].ring_vertex_list[s * 2 + 1].g = 255;
				p_planet[index].ring_vertex_list[s * 2 + 1].b = 255;
				p_planet[index].ring_vertex_list[s * 2 + 1].a = 1.0;
			}
		}

		glInterleavedArrays(GL_T2F_C4F_N3F_V3F, 0, p_planet[index].ring_vertex_list);
		glDrawElements(GL_QUAD_STRIP, (slices + 1) * 2, GL_UNSIGNED_INT,
					   p_planet[index].rings_indices);
	}
}

static int in_range ( double a, double b, double c )
{
	int i = 0;
	double tmp;

	if (c > (2 * radian_correction * 180.0f))
		tmp = a + 2 * radian_correction * 180.0f;
	else
		tmp = a;
	if ((tmp >= b) && (tmp <= c))
		i = 1;
	if ((a >= b) && (a <= c))
		i = 1;
	return i;
}
