#ifndef _ENTITY_H
#define _ENTITY_H

#include <SDL.h>

typedef struct ENTITY_S
{
	Uint8 _inuse; /**<This flag keeps track if the entity is active or free to reassign*/
	Sprite *sprite;
	Vector2D position;
	Vector2D velocity; /**<*/
	Vector2D mins, maxs; /**<Describe the bounding bow around the entity*/
}Entity;

#endif