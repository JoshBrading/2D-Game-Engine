#include "simple_logger.h"
#include "g_test_bounce_ball.h"

void ball_think_fixed( Entity *self );

Entity *ball_new()
{
	Entity *ent = NULL;
	ent = entity_new();

	ent->thinkFixed = ball_think_fixed;
	ent->scale.x = 1;
	ent->scale.y = 1;
	ent->damage = rand() % 10 + 1;
	return ent;
}

int ballDir = 5;
int ballDirY = 5;
void ball_think_fixed( Entity *self )
{
	int speed = self->damage;
	if(self->position.x > 1100 ) self->scale.x = -1;
	if(self->position.x < 32 ) self->scale.x = 1;
	if(self->position.y > 600 ) self->scale.y = -1;
	if(self->position.y < 32 ) self->scale.y = 1;
	int directionX = self->scale.x;
	int directionY = self->scale.y;
	self->position.x += speed * directionX;
	self->position.y += speed * directionY;
}
