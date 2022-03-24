#ifndef __G_WEAPON__
#define	__G_WEAPON__

#include "gf2d_sprite.h"
#include "g_entity.h"

typedef enum
{
	WEP_RELOAD,
	WEP_FIRE,
	WEP_OUT_OF_AMMO,
	WEP_IDLE
}WeaponState;

typedef enum
{
	WEP_SAFE,
	WEP_SEMI_AUTO,
	WEP_FULL_AUTO,
	WEP_BURST
}FireMode;

typedef struct Weapon_S
{
	Uint8				_inuse; /**<Flag to check if weapon is being used*/
	Uint8				held;	/**<Flag to check if weapon is held by an entity*/

	enum WeaponState	state; /**<Enum containingt the weapons current state*/
	enum FireMode		fire_mode; /**<Enum containing the weapons current fire mode*/

	Sprite*				sprite; /**<A pointer to the sprite of the weapon*/
	Sprite*				impact; /**<A pointer to the sprite of the impact particle*/

	Uint32				ammo; /**<Ammo left in the magazine*/
	Uint32				reserve_ammo; /**<Ammo left outside the magazine*/
	Uint32				max_ammo;

	float				fire_rate; /**<Time between bullets*/
	float				reload_time; /**<Time to reload the weapon*/
	float				damage; /**<Amount of damage a bullet does*/

	Uint32				timer;

	struct Entity_S*	owner; /**<Pointer to the entity currently using the weapon*/

	void				(*think)(struct Weapon_S* self); /**<A pointer to the think function*/
	void				(*thinkFixed)(struct Weapon_S* self); /**<A pointer to the fixed think function*/
	
	void				(*update)(struct Weapon_S* self); /**<A pointer to the update function*/
	void				(*updateFixed)(struct Weapon_S* self); /**<A pointer to the fixed update function*/

}Weapon;

Weapon* weapon_new ();

void weapon_manager_init ( Uint32 maxWeapons );

void weapon_manager_close();

void weapon_manager_draw_all();

void weapon_draw ( Weapon *self );

void weapon_think ( Weapon *self );
void weapon_think_fixed ( Weapon *self );

void weapon_update ( Weapon *self );
void weapon_update_fixed ( Weapon *self );

void weapon_manager_think_all ();
void weapon_manager_think_fixed_all ();

void weapon_manager_update_all ();
void weapon_manager_update_fixed_all ();

void weapon_free ( Weapon *self );

void weapon_shoot( Weapon* self );

void weapon_reload( Weapon *self );
#endif