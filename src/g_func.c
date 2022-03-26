#include "simple_logger.h"
#include "g_func.h"

float lerp( float a, float b, float t )
{
	return a + t * (b - a);
}

Vector2D lerp_vector_2d( Vector2D a, Vector2D b, float t )
{
	float x, y;

	x = a.x + t * (b.x - a.x);
	y = a.y + t * (b.y - a.y);

	return vector2d( x, y );
}

float look_at_angle_degree( Vector2D a, Vector2D b )
{
	float deltaY = ( b.y - a.y );
	float deltaX = ( b.x - a.x );
	return atan2( deltaY, deltaX) * (180.0 / M_PI);
}

Vector2D look_at_angle_slope( Vector2D a, Vector2D b )
{
	return vector2d( (b.y - a.y), (b.x - a.x) );
}