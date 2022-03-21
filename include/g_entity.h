#ifndef __G_ENTITY_H__
#define __G_ENTITY_H__

#include "gf2d_sprite.h"
#include "g_collision.h"

typedef enum
{
	ENT_IDLE,
	ENT_ATTACK,
	ENT_HEAL,
	DOOR_OPENED,
	DOOR_CLOSED,
	DOOR_DEAD,
	DOOR_SHOT,
	DOOR_HAND,
	DOOR_BLOWN

}EntityState;

typedef struct Entity_S
{
	Uint8					_inuse;		/**<Flag to check if entity is being used*/
	Uint8					visibility;	/**<Current visibility of entity*/
	Uint8					collision_enabled;

	Uint32					_id;		/**<ID of an entity, assume ID cannot be 0*/
	Sprite					*sprite;	/**<a pointer to the sprite that is used by this entity*/
	float					frame;		/**<Current frame of sprite*/

	Rect					bounds;

	Vector2D				offset;		/**<Offset to the true center of the entity*/
	Vector2D				position;	/**<where the entity is in the world*/
	Vector3D				rotation;	/**<Rotation of the entity*/
	Vector2D				scale;		/**<Vector2D Scale of entity*/
	Vector2D				speed;

	float					health;		/**<Current health of entity*/
	//Weapon					weapon;		/**<Currently held weapon by the entity*/

	EntityState					state;		/**<Current state of entity, waiting, attacking*/
	char					*tag;		/**<Tag for naming the entity*/
	Uint8					team;		/**<Team the entity is on*/
	struct CollisionCell_S*	cell;		/**<Current cell position of the entity, used for collision detection*/

	void               (*think)(struct Entity_S *self);			/* <pointer to the think function */
	void               (*thinkFixed)(struct Entity_S *self);	/* <pointer to the think fixed function */

	void               (*update)(struct Entity_S *self);		/* <pointer to the update function */
	void               (*updateFixed)(struct Entity_S *self);	/* <pointer to the update fixed function */

	void               (*damage)(struct Entity_S *self, float damage, struct Entity_S *inflictor);	/* <pointer to the damage function */
	void               (*onDeath)(struct Entity_S *self);											/* <pointer to a funciton to call when the entity dies */	


	// Collision specific variables
	void				(*onCollision)(struct Entity_S *self, CollisionInfo collision);	/* <pointer to a funciton to call when the entity collides */	

	Uint32				col_timer;	/* <Time of last call, use however needed */

	// Doors!
	float				interact_radius;
	Uint32				door_timer;
}Entity;


typedef struct
{

	Entity *entity_list;
	Uint32	entity_count;

}EntityManager;

/**
 * @brief initialize internal entity management system
 * @param max_entities how many concurrent entities to support
 */
void entity_manager_init( Uint32 maxEntities );

/**
 * @brief call draw on all entities
 */
void entity_manager_draw_all();

/**
 * @brief Draw colliders for all entities
 */
void entity_manager_draw_debug();

/**
 * @brief call free on all entities
 */
void entity_manager_clear();

/**
 * @brief allocate an entity, initialize it to zero and return a pointer to it
 * @return NULL on error (see logs) or a pointer to an initialized entity.
 */
Entity *entity_new();

/**
 * @brief frees provided entity
 * @param ent the entity to free
 */
void entity_free( Entity *self );

/**
 * @brief draws an entity to the current render frame
 * @param ent the entity to draw
 */
void entity_draw( Entity *self );

/**
 * @brief Returns the entity with the given ID
 * @param ID of the entity to return
 * @return entity with ID
 */
Entity* entity_manager_get_by_id( Uint32 id );

/**
 * @brief run the think functions for ALL active entities
 */
void entity_manager_think_all();

/**
 * @brief run the think functions for ALL active entities at a fixed rate
 */
void entity_manager_think_fixed_all();

/**
 * @brief run the update functions for ALL active entities
 */
void entity_manager_update_all();

/**
 * @brief Fixed update runs the update functions for all entities at a fixed rate
 */
void entity_manager_update_fixed_all();

EntityManager *entity_manager_get();

void set_health( Entity* self, float damage, Entity* inflictor );

void entity_die ( Entity* self );

/**
* @brief Runs once every frame an entity is colliding
* @param Entity with collision
* @param Details information about the collision
*/
void entity_on_collision( Entity *self, CollisionInfo collision );

#endif