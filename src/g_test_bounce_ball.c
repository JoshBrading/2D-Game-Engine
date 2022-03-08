#include "simple_logger.h"
#include "g_test_bounce_ball.h"
#include "g_func.h"

void ball_think_fixed( Entity *self );

Entity *ball_new()
{
	Entity *ent = NULL;
	ent = entity_new();

	ent->thinkFixed = ball_think_fixed;
	ent->offset.x = 16;
	ent->offset.x = 16;

	ent->speed.x = 1;
	ent->speed.y = 1;
	ent->damage = rand() % 10 + 1;
	ent->health = 0;
	return ent;
}

int ballDir = 5;
int ballDirY = 5;
void ball_think_fixed( Entity *self )
{
	
	//self->position = lerpVector2D( self->position, vector2d(256, 256), 0.1 );
	//slog( "Test: %f", self->health );
	int speed = self->damage;
	if(self->position.x > 1100 ) self->speed.x = -1;
	if(self->position.x < 32 ) self->speed.x = 1;
	if(self->position.y > 600 ) self->speed.y = -1;
	if(self->position.y < 32 ) self->speed.y = 1;
	int directionX = self->speed.x;
	int directionY = self->speed.y;
	self->position.x += speed * directionX;
	self->position.y += speed * directionY;
}
