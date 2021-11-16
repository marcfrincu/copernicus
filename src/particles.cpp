#include <malloc.h>
#include <string.h>

#include "include/particles.h"

particle particles[NUM_PARTICLES];
particle *p_particle = particles;

void initialize_particle ( particle *p, double object_pos[], double vector[] )
{
    // The particle must be alive when initialized
    if ( p->life <= 0.f )
    {
        p->life = 1.f;
        p->fade = (float) ( rand ( ) % 100 ) / 100.0f + 0.15;
    }

    p->c[0] = 1; // Red
    p->c[1] = 0.2f; // Green
    p->c[2] = 0.2f; // Blue

    p->p[0] = object_pos[0]; // position X
    p->p[1] = object_pos[1]; // position Y
    p->p[2] = object_pos[2]; // position Z
    //p->p[0] = 0.f;
    //p->p[1] = 0.f;
    //p->p[2] = 0.f;

    int scale = 10;

    p->a[0] = vector[0] / scale; // acceleration X
    p->a[1] = vector[1] / scale; // acceleration Y
    p->a[2] = vector[2] / scale; // acceleration Z
}

void move_particle ( particle *p )
{
    if ( p->life > 0.f )
    {
        // position = position + speed * time. However we assume time = 1 thus:
        p->p[0] += p->v[0] ;
        p->p[1] += p->v[1];
        p->p[2] += p->v[2];

        // speed = speed + acceleration * time. However we assume time = 1 thus:
        p->v[0] += p->a[0];
        p->v[1] += p->a[1];
        p->v[2] += p->a[2];

        // Decrease particle life
        p->life -= p->fade;
    }
}

void update_comet_particle_acceleration ( particle *p, float vector[] )
{
    int scale = 10;
    p->a[0] = vector[0] / scale; // acceleration X
    p->a[1] = vector[1] / scale; // acceleration Y
    p->a[2] = vector[2] / scale; // acceleration Z
}
