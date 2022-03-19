#ifndef __G_WEAPON__
#define	__G_WEAPON__

#include "gf2d_sprite.h"
#include "g_entity.h"

typedef struct Weapon_S
{
	Uint8				_inuse; /**<Flag to check if weapon is being used*/
	Uint8				held;	/**<Flag to check if weapon is held by an entity*/

	enum State			state; /**<Enum containingt the weapons current state*/
	enum FireMode		fire_mode; /**<Enum containing the weapons current fire mode*/

	Sprite*				sprite; /**<A pointer to the sprite of the weapon*/
	Sprite*				impact; /**<A pointer to the sprite of the impact particle*/

	Uint32				ammo; /**<Ammo left in the magazine*/
	Uint32				reserve_ammo; /**<Ammo left outside the magazine*/
	float				fire_rate; /**<Time between bullets*/
	float				reload_time; /**<Time to reload the weapon*/
	float				damage; /**<Amount of damage a bullet does*/

	struct Entity_S*	owner; /**<Pointer to the entity currently using the weapon*/

	void				(*think)(struct Weapon_S* self); /**<A pointer to the think function*/
	void				(*thinkFixed)(struct Weapon_S* self); /**<A pointer to the fixed think function*/
	
	void				(*update)(struct Weapon_S* self); /**<A pointer to the update function*/
	void				(*updateFixed)(struct Weapon_S* self); /**<A pointer to the fixed update function*/

}Weapon;

typedef enum
{
	RELOADING,
	FIRING,
	OUT_OF_AMMO
}State;

typedef enum
{
	SAFE,
	SEMI_AUTO,
	FULL_AUTO
}FireMode;

Weapon* weapon_new ();

void weapon_manager_init ( Uint32 maxWeapons );

void weapon_manager_close();

void weapon_manager_draw_all();

void weapon_draw ();

void weapon_think ();
void weapon_think_fixed ();

void weapon_update ();
void weapon_update_fixed ();

void weapon_manager_think_all ();
void weapon_manager_think_fixed_all ();

void weapon_manager_update_all ();
void weapon_manager_update_fixed_all ();

void weapon_free ();

#endif