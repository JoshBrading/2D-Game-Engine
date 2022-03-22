#include "simple_logger.h"
#include "gf2d_sprite.h"
#include "gf2d_draw.h"
#include "g_player.h"
#include "g_particle.h"
#include "g_globals.h"
//#include "g_func.h"

void think_fixed ( Entity* self );
void think ( Entity* self );
void player_collision( Entity *self, CollisionInfo info );
void move_to_target( Entity *self, Uint32 index );
void draw_path( Entity *self );

Vector2D target;
Uint8 leftMouseButton = 0;

Vector2D mouse;


Vector2D look_at_angle_slope( Vector2D a, Vector2D b );
Entity* player_new ()
{
	Entity *ent = NULL;
	ent = entity_new();

	if ( !ent ) return NULL;

	Vector2D pos = vector2d( -1, -1 );


	ent->team = 0;

	ent->think = think;
	ent->thinkFixed = think_fixed;
	ent->onCollision = player_collision;

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

	ent->path.positions[0] = pos;
	ent->path.positions[1] = pos;
	ent->path.positions[2] = pos;
	ent->path.positions[3] = pos;

	ent->state = ENT_WANDER;
	ent->move_speed = 4;

	ent->timers.A = g_time;

	ent->path.pos_max = 4;
	ent->path.pos_count = 0;
	ent->path.color = vector4d( 0, 255, 0, 255 );

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
		//slog ( "Mouse clicked" );
	}

	if ( e.type == SDL_MOUSEBUTTONUP )
	{
		leftMouseButton = 0;
	}

	Vector2D hit_point;
	Vector2D m = look_at_angle_slope ( self->position, target );
	Entity* ent;
	mouse = vector2d ( mx, my );

	raycast ( self->position, look_at_angle_slope ( self->position, mouse ), 64, self->_id );
	gf2d_draw_circle ( self->position, 64, vector4d ( 255, 255, 255, 100 ) );

	draw_path( self );
	//self->position.x = (float)mx;
	//self->position.y = (float)my;

}

int fireRate = 75;  // time * 10ms between shots
int time = 0;
void think_fixed ( Entity* self )
{
	//if ( time <= SDL_GetTicks () )
	//{
	//	if ( leftMouseButton )
	//	{
	//		shoot ( self );
	//	}
	//	time = SDL_GetTicks () + fireRate;
	//}
		
	if (leftMouseButton && self->path.pos_count != self->path.pos_max ) // why doesnt < work here?
	{
		if (self->timers.A + 500 < g_time)
		{
			slog( "Writing pos: %i", self->path.pos_count );
			self->path.positions[self->path.pos_count] = mouse;
			self->path.pos_count += 1;

			self->timers.A = g_time;
		}
	}

	if (self->path.pos_count != 0 && !leftMouseButton)
	{
		move_to_target( self, self->path.pos_max - self->path.pos_count );
	}
}

void move_to_target( Entity *self, Uint32 index )
{
	//slog( "Index: %i", index );

	if (!vector2d_distance_between_less_than( self->position, self->path.positions[index], self->move_speed ))
	{
		Vector2D newPos;
		vector2d_move_towards( &newPos, self->position, self->path.positions[index], self->move_speed );

		CollisionInfo col = self->col_info;

		if (col.time + 100 > g_time)
		{
			if (col.top || col.bottom)
			{
				newPos.y = self->position.y;
			}

			if (col.left || col.right)
			{
				newPos.x = self->position.x;
			}
		}

		self->position = newPos;
	}
	else
	{
		self->path.positions[self->path.pos_max - self->path.pos_count] = vector2d( -1, -1 );
		self->path.pos_count -= 1;
	}
}

void shoot ( Entity* self )
{
	HitObj hit;
	hit = raycast ( self->position, look_at_angle_slope ( self->position, target ), 1024, self->_id );
	
	Sprite *vfx;
	vfx = gf2d_sprite_load_all( "images/Bullet_Impact.png", 256, 256, 32 );
	vfx->frame_count = 32;

	Particle *particle = particle_new();
	particle->position = hit.position;
	particle->scale.x = 0.1;
	particle->scale.y = 0.1;
	particle->sprite = vfx;
	particle->life_time = 32;
	particle->timescale = 0.5;
	particle->offset.x = 10;
	particle->offset.y = 10;
	particle->frame = 1;
	
	if ( hit.entity == NULL ) return;
	if ( hit.entity->_inuse )
	{
		//slog ( "Hit entity with ID: %i", hit.entity->_id );
		//gf2d_draw_line( self->position, hit.position, vector4d( 255, 255, 0, 255 ) );
		//hit.entity->damage ( hit.entity, 1.0f, self );
		set_health ( hit.entity, 1, self);
	}



}

void player_collision( Entity *self, CollisionInfo info )
{
	self->col_info = info;

	// Wiggle around :)
	if (info.side == COL_LEFT)		self->position.x += 0.01;
	if (info.side == COL_RIGHT)		self->position.x -= 0.01;
	if (info.side == COL_TOP)		self->position.y += 0.01;
	if (info.side == COL_BOTTOM)	self->position.y -= 0.01;
}

void draw_path( Entity *self )
{
	if (!self) return;

	//int i = self->path.pos_max - self->path.pos_count;

	//slog( "%i", self->path.positions[i] );

	//if( self->path.pos_count != 0) gf2d_draw_line( self->position, self->path.positions[i], vector4d( 0, 0, 255, 255 ) );


	//gf2d_draw_line( self->path.positions[0], self->path.positions[1], vector4d( 0, 0, 255, 255 ) );
	//gf2d_draw_line( self->path.positions[1], self->path.positions[2], vector4d( 0, 0, 255, 255 ) );
	//gf2d_draw_line( self->path.positions[2], self->path.positions[3], vector4d( 0, 0, 255, 255 ) );

	gf2d_draw_circle( self->path.positions[0], 4, self->path.color );
	gf2d_draw_circle( self->path.positions[1], 4, self->path.color );
	gf2d_draw_circle( self->path.positions[2], 4, self->path.color );
	gf2d_draw_circle( self->path.positions[3], 4, self->path.color );

}