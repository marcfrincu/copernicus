#pragma once

#define NUM_PARTICLES 500

typedef struct {
   float life;
   float fade;
   double p[3];
   float v[3];
   float a[3];
   float c[3];
} particle;

extern particle particles[NUM_PARTICLES];
extern particle *p_particles;

void initialize_particle ( particle *p_particle, double object_pos[], double vector[] );
void move_particle ( particle *p_particle );
void update_comet_particle_acceleration ( particle *p_particle, float vector[] );
