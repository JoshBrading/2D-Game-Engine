#ifndef __G_PARTICLE__
#define __G_PARTICLE__

#include "gf2d_sprite.h"

typedef struct Particle_S
{
	Uint8		_inuse;
	Uint32		id;
	Sprite*		sprite;
	Vector2D	offset;

	float		frame;
	float		life_time;
	float		timescale;
	
	Vector2D	position;
	Vector2D	scale;

}Particle;

void particle_manager_init ( Uint32 maxParticles );

void particle_manager_draw_all ();

void particle_manager_clear ();

Particle* particle_new ();

void particle_free ( Particle* self );

void particle_draw ( Particle* self );

void particle_manager_update_all ();

void particle_update ( Particle* self );

void particle_manager_update_fixed_all ();

void particle_update_fixed ( Particle* self );
#endif // !__G_PARTICLE__
