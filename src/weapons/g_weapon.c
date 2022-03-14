#include "../../include/g_weapon.h"
#include "simple_logger.h"

typedef struct
{
	Weapon* weapon_list;
	Uint32	weapon_count;
}WeaponManager;

static WeaponManager weapon_manager = { 0 };

void weapon_manager_init ( Uint32 maxWeapons )
{
	weapon_manager.weapon_list = gfc_allocate_array ( sizeof ( Weapon ), maxWeapons ); // This is probably wrong...
	if ( weapon_manager.weapon_list == NULL )
	{
		slog ( "WeaponManagerInit: Failed to allocate array of size: %i", maxWeapons );
		return;
	}
	weapon_manager.weapon_count = maxWeapons;
	//atexit( entity_manager_close );
	slog ( "WeaponManager: Initialized" );
}