#include "simple_logger.h"
#include "gf2d_sprite.h"
#include "gf2d_draw.h"
#include "g_func.h"
#include "g_player.h"

void think_fixed(Entity* self);
void think(Entity* self);
Vector2D target;

Entity *player_new()
{
	Entity *ent = entity_new();
	if (!ent) return;

	ent->think = think;
	ent->thinkFixed = think_fixed;

	ent->sprite = gf2d_sprite_load_image( "images/player.png" );
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

	target = vector2d( 0, 0 );

	return ent;
}

void think(Entity* self)
{
	const Uint8 *keys;
	keys = SDL_GetKeyboardState( NULL ); // get the keyboard state for this frame

	int mx, my;
	SDL_GetMouseState( &mx, &my );
	
	target.x = mx;
	target.y = my;
	
	//if (keys[SDL_SCANCODE_W]) target.y -= 1;
	//if (keys[SDL_SCANCODE_A]) target.x -= 1;
	//if (keys[SDL_SCANCODE_S]) target.y += 1;
	//if (keys[SDL_SCANCODE_D]) target.x += 1;

	
	
	//self->rotation.z = look_at_angle( self->position, target );
	//slog("Test: %f", look_at_angle(self->position, target));

	Line a;
	a.a.x = 128;
	a.a.y = 600;
	a.b.x = 64;
	a.b.y = 128; 

	Line b;
	b.a = self->position;
	b.b = vector2d( mx, my );

	Rect r;

	r.x = 128;
	r.y = 128;

	r.w = 32;
	r.h = 32;

	SDL_Rect rect;

	rect.x = r.x;
	rect.y = r.y;
	rect.w = r.w;
	rect.h = r.h;

	//gf2d_draw_fill_rect( rect, vector4d( 255, 0, 0, 255 ));

	//collision_line_rect_test( r, b );

	//gf2d_draw_line( a.a, a.b, vector4d( 255, 255, 0, 255 ) );
	//gf2d_draw_line( b.a, b.b, vector4d( 255, 255, 0, 255 ) );

	//collision_line_line_test( a, b );
	Vector2D m = look_at_angle_slope( self->position, target );
	raycast( self->position, m, 1024, NULL, NULL);

	//gf2d_draw_circle( target, 6, vector4d( 255, 0, 0, 255 ) );
	//gf2d_draw_line( self->position, target, vector4d(255, 255, 0, 255) );
	//self->position.x = (float)mx;
	//self->position.y = (float)my;

}

void think_fixed(Entity* self)
{

}