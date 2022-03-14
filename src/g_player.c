#include "simple_logger.h"
#include "gf2d_sprite.h"
#include "gf2d_draw.h"
#include "g_func.h"
#include "g_player.h"
#include "g_particle.h"

void think_fixed ( Entity* self );
void think ( Entity* self );
Vector2D target;
Uint8 leftMouseButton = 0;

Entity* player_new ()
{
	Entity* ent = entity_new ();
	if ( !ent ) return NULL;

	ent->think = think;
	ent->thinkFixed = think_fixed;

	ent->sprite = gf2d_sprite_load_image ( "images/player.png" );
	ent->scale.x = 0.125;
	ent->scale.y = 0.125;

	ent->offset.x = 16;
	ent->offset.y = 16;

	ent->bounds.w = 32;
	ent->bounds.h = 32;
	ent->bounds.x = ent->position.x - ent->offset.x;
	ent->bounds.y = ent->position.y - ent->offset.y;

	ent->rotation.x = 0;
	ent->rotation.y = 0;
	ent->rotation.z = 0;

	ent->health = 1000;

	target = vector2d ( 0, 0 );

	return ent;
}

void think ( Entity* self )
{
	const Uint8* keys;
	keys = SDL_GetKeyboardState ( NULL ); // get the keyboard state for this frame

	SDL_Event e;
	SDL_PollEvent ( &e );

	int mx, my;
	SDL_GetMouseState ( &mx, &my );

	target.x = mx;
	target.y = my;

	if ( keys[SDL_SCANCODE_W] ) self->position.y -= 1;
	if ( keys[SDL_SCANCODE_A] ) self->position.x -= 1;
	if ( keys[SDL_SCANCODE_S] ) self->position.y += 1;
	if ( keys[SDL_SCANCODE_D] ) self->position.x += 1;

	if ( e.type == SDL_MOUSEBUTTONDOWN )
	{
		leftMouseButton = 1;
		slog ( "Mouse clicked" );
	}

	if ( e.type == SDL_MOUSEBUTTONUP )
	{
		leftMouseButton = 0;
	}

	Vector2D hit_point;
	Vector2D m = look_at_angle_slope ( self->position, target );
	Entity* ent;
	Vector2D mouse = vector2d ( mx, my );

	raycast ( self->position, look_at_angle_slope ( self->position, mouse ), 64, self->_id );
	gf2d_draw_circle ( self->position, 64, vector4d ( 255, 255, 255, 100 ) );
	//self->position.x = (float)mx;
	//self->position.y = (float)my;



}

int fireRate = 75;  // time * 10ms between shots
int time = 0;
void think_fixed ( Entity* self )
{
	if ( time <= SDL_GetTicks () )
	{
		if ( leftMouseButton )
		{
			shoot ( self );
		}
		time = SDL_GetTicks () + fireRate;
	}

}

void shoot ( Entity* self )
{
	HitObj hit;
	hit = raycast ( self->position, look_at_angle_slope ( self->position, target ), 1024, self->_id );
	if ( hit.entity == NULL ) return;
	if ( hit.entity->_inuse )
	{
		//slog ( "Hit entity with ID: %i", hit.entity->_id );
		//gf2d_draw_line( self->position, hit.position, vector4d( 255, 255, 0, 255 ) );

		Sprite* vfx;
		vfx = gf2d_sprite_load_all ( "images/Bullet_Impact.png", 256, 256, 32 );
		vfx->frame_count = 32;
		//gf2d_sprite_draw ( test, hit.position, NULL, NULL, NULL, NULL, NULL, 0 );

		Particle* particle = particle_new ();
		particle->position = hit.position;
		particle->scale.x = 0.1;
		particle->scale.y = 0.1;
		particle->sprite = vfx;
		particle->life_time = 32;
		particle->timescale = 0.5;
		particle->offset.x = 10;
		particle->offset.y = 10;
		particle->frame = 1;
		//hit.entity->damage ( hit.entity, 1.0f, self );
		set_health ( hit.entity, 1, self);
	}
}