#include "w_rifle.h"
#include "g_func.h"
#include "g_particle.h"
#include "g_globals.h"

#include "simple_logger.h"

void rifle_think_fixed( Weapon *self );

Weapon *rifle_new()
{
	Weapon *weapon = NULL;

	weapon = weapon_new();

	weapon->ammo = 30;
	weapon->max_ammo = 30;
	weapon->reserve_ammo = 120;

	weapon->reload_time = 300;
	weapon->fire_rate = 150;
	weapon->damage = 10;

	weapon->impact = gf2d_sprite_load_all( "images/Bullet_Impact.png", 256, 256, 32 );

	weapon->impact->frame_count = 32;

	weapon->timer = g_time;

	weapon->thinkFixed = rifle_think_fixed;

	return weapon;
}

void rifle_think_fixed( Weapon *self )
{
	if (self->state == WEP_FIRE && self->ammo > 0 && self->timer + self->fire_rate < g_time)
	{
		weapon_shoot( self );
		self->timer = g_time;
	}
	else if (self->state == WEP_RELOAD && self->ammo != self->max_ammo)
	{
		weapon_reload( self );
	}

}
