#ifndef __G_STATIC_ENTITY_H__
#define __G_STATIC_ENTITY_H__

#include "g_entity.h"

typedef struct StaticEntity_S
{
	Uint8					_inuse;		/**<Flag to check if static entity is being used*/
	Uint32					_id;		/**<ID of a static entity */
	Sprite					*sprite;	/**<a pointer to the sprite that is used by this static entity*/
	float					frame;		/**<Current frame of sprite*/

	char					*collision_type;
	Rect					bounds;
	Quad					quad_bounds;
	Vector2D				offset;		/**<Offset to the true center of the static entity*/
	Vector2D				position;	/**<where the static entity is in the world*/
	Vector3D				rotation;	/**<Rotation of the static entity*/
	Vector2D				scale;		/**<Vector2D Scale of static entity*/

}StaticEntity;

typedef struct
{
	StaticEntity	*static_entity_list;
	Uint32          static_entity_count;

}StaticEntityManager;


/**
 * @brief initializes the static entity subsystem
 * @param maxEntities the limit on number of entities that can exist at the same time
 */
void static_entity_manager_init( Uint32 maxEntities );

/**
 * @brief provide a pointer to a new static entity
 * @param filename of the model to load
 * @param position to spawn static entity
 * @param rotation to spawn static entity
 * @return NULL on error or a valid static entity pointer otherwise
 */
StaticEntity *static_entity_new( );

/**
 * @brief free a previously created static entity from memory
 * @param self the static entity in question
 */
void static_entity_free( StaticEntity *self );

/**
 * @brief Draw a static entity in the current frame
 * @param self the static entity in question
 */
void static_entity_draw( StaticEntity *self );

/**
 * @brief draw ALL active static entities
 */
void static_entity_draw_all();

void static_entity_manager_draw_debug();

StaticEntityManager *static_entity_manager_get();

//StaticEntity* static_entity_get_by_tag( char* tag );
#endif // !__STATIC_ENTITY_H__