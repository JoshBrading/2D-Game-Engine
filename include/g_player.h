#ifndef __G_PLAYER_H__
#define __G_PLAYER_H__

#include "g_entity.h"
/**
 * @brief Create a new player entity
 * @return NULL on error, or the player entity pointer on success
 */
Entity *player_new( );

void shoot ( Entity* self );

#endif
