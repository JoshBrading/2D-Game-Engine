#include "w_rifle.h"
#include "g_func.h"
#include "g_particle.h"
#include "g_globals.h"

#include "simple_logger.h"

void rifle_think_fixed( Weapon *self );

Weapon *rifle_new()
{
	Weapon *wep = NULL;

	wep = weapon_new();

	wep->ammo = 30;
	wep->max_ammo = 30;
	wep->reserve_ammo = 120;

	wep->reload_time = 300;
	wep->fire_rate = 150;
	wep->damage = 10;

	wep->impact = gf2d_sprite_load_all( "images/Bullet_Impact.png", 256, 256, 32 );

	wep->impact->frame_count = 32;

	wep->timer = g_time;

	wep->thinkFixed = rifle_think_fixed;
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
