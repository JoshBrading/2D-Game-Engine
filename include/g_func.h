#ifndef __G_FUNC_H__
#define __G_FUNC_H__

#include "gfc_vector.h"

/**
 * @brief Lerp between two floats
 * @param a first float
 * @param b second float
 * @param t time of lerp 0-1
 */
float lerp( float a, float b, float t );

/**
 * @brief Lerp between two Vector2D's
 * @param a first Vector2D
 * @param b second Vector2D
 * @param t time of lerp 0-1
 */
Vector2D lerp_vector_2d( Vector2D a, Vector2D b, float t );

float look_at_angle_degree( Vector2D a, Vector2D b );

Vector2D look_at_angle_slope( Vector2D a, Vector2D b );

#endif