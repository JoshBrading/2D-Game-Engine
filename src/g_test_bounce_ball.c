#include "simple_logger.h"
#include "g_test_bounce_ball.h"

void ball_think_fixed( Entity *self );

Entity *ball_new()
{
	Entity *ent = NULL;
	ent = entity_new();

	ent->thinkFixed = ball_think_fixed;

	return ent;
}

int ballDir = 3;
int ballDirY = 5;
void ball_think_fixed( Entity *self )
{
	if(self->position.x > 512 ) ballDir *= -1;
	if(self->position.x < 32 ) ballDir *= -1;
	if(self->position.y > 256 ) ballDirY *= -1;
	if(self->position.y < 32 ) ballDirY *= -1;
	//self->position.x += ballDir;
	//self->position.y += ballDirY;
}
