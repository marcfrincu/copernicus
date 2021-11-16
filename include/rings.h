#pragma once
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

typedef struct {
	float u, v;
	float r, g, b, a;
	float nx, ny, nz;
	float x, y, z;
} vertex_data;

void draw_ring ( float inner_radius, float outer_radius, int index, int tex_index, int slices, float scale_factor );
