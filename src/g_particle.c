#include "g_particle.h"
#include "simple_logger.h"
#include "gf2d_draw.h"
typedef struct
{
	Particle*	particle_list;
	Uint32		particle_count;

	Sprite*		bullet_impact;
}ParticleManager;

static ParticleManager particle_manager = { 0 };

void particle_manager_init ( Uint32 maxParticles )
{
	particle_manager.particle_list = gfc_allocate_array ( sizeof ( Particle ), maxParticles );
	if ( particle_manager.particle_list == NULL )
	{
		slog ( "ParticleManagerInit: Failed to allocate array of size %i", maxParticles );
		return;
	}
	particle_manager.particle_count = maxParticles;
	//atexit ( particle_manager_close );
	slog ( "ParticleManager: Initialized" );

	particle_manager.bullet_impact = gf2d_sprite_load_all ( "images/Bullet_Impact.png", 256, 256, 32 );
}

void particle_manager_draw_all ()
{
	for ( int i = 0; i < particle_manager.particle_count; i++ )
	{
		if ( particle_manager.particle_list[i]._inuse )
		{
			particle_draw ( &particle_manager.particle_list[i] );
		}
	}
}

void particle_manager_clear ()
{
	for ( int i = 0; i < particle_manager.particle_count; i++ )
	{
		if ( particle_manager.particle_list[i]._inuse )
		{
			particle_free ( &particle_manager.particle_list[i] );
		}
	}
}

Particle* particle_new ()
{
	int i;
	for ( i = 0; i < particle_manager.particle_count; i++ )
	{
		if ( !particle_manager.particle_list[i]._inuse )
		{
			particle_manager.particle_list[i]._inuse = 1;
			particle_manager.particle_list[i].id = i;
			particle_manager.particle_list[i].scale.x = 1;
			particle_manager.particle_list[i].scale.y = 1;
			particle_manager.particle_list[i].offset.x = 0;
			particle_manager.particle_list[i].offset.y = 0;
			particle_manager.particle_list[i].timescale = 1;
			//particle_manager.particle_list[i].sprite = particle_manager.bullet_impact;
			
			slog ( "spawn particle" );
			return (&particle_manager.particle_list[i]);
		}
	}
	slog ( "particleNew: No free space" );
	return NULL;
}

void particle_draw ( Particle* self )
{
	if ( !self ) return;
	if ( !self->sprite ) return;
	gf2d_sprite_draw ( self->sprite, vector2d ( self->position.x - self->offset.x, self->position.y - self->offset.y ), &self->scale, NULL, NULL, NULL, NULL, self->frame );
}

void particle_free ( Particle* self )
{
	if ( !self )
	{
		slog ( "particleFree: particle does not exist" );
		return;
	}
	if ( self->sprite )
	{
		gf2d_sprite_free ( self->sprite );
	}
	memset ( self, 0, sizeof ( self ) );
}


void particle_update ( Particle* self )
{
	if ( !self )return;
	//gf2d_draw_circle ( self->position, 8, vector4d ( 255, 255, 0, 255 ) );
}

void particle_update_fixed ( Particle* self )
{
	if ( !self )return;

	if ( self->frame >= self->sprite->frame_count )
	{
		self->frame = 0; // Figure out why this is needed
		particle_free ( self );
	}
	self->frame += self->timescale;
	//slog ( "Frame: %f", self->frame );
}

void particle_manager_update_all ()
{
	int i;
	for ( i = 0; i < particle_manager.particle_count; i++ )
	{
		if ( !particle_manager.particle_list[i]._inuse )// not used yet
		{
			continue;// skip this iteration of the loop
		}
		particle_update ( &particle_manager.particle_list[i] );
	}
}

void particle_manager_update_fixed_all ()
{
	int i;
	for ( i = 0; i < particle_manager.particle_count; i++ )
	{
		if ( !particle_manager.particle_list[i]._inuse )// not used yet
		{
			continue;// skip this iteration of the loop
		}
		particle_update_fixed ( &particle_manager.particle_list[i] );
	}
}
