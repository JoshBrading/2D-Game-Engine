#include "c_intel.h"
#include "gf2d_draw.h"

void intel_think( Entity* self );
void intel_debug( Entity *self );

Entity* intel_new()
{
    Entity *intel;
    intel = entity_new();
    intel->think = intel_think;
    intel->drawDebug = intel_debug;

    intel->health = 999;
    intel->position.x = rand() % 1200;
    intel->position.y = rand() % 720;
    intel->sprite = gf2d_sprite_load_image( "images/intel.png" );

    intel->scale.x = 0.125;
    intel->scale.y = 0.125;
    intel->tag = "intel";
    intel->offset.x = 16;
    intel->offset.y = 16;

    intel->bounds.w = 32;
    intel->bounds.h = 32;
    intel->bounds.x = intel->position.x - intel->offset.x;
    intel->bounds.y = intel->position.y - intel->offset.y;

    intel->collision_enabled = false;

    return intel;
}

void intel_think( Entity *self )
{
    Entity *player = entity_manager_get_by_id( 1 );
    const Uint8 *keys;
    keys = SDL_GetKeyboardState( NULL ); // get the keyboard state for this frame

    SDL_Event e;
    SDL_PollEvent( &e );

    if (keys[SDL_SCANCODE_E] && vector2d_distance_between_less_than( self->position, player->position, 32 ))
    {
        entity_free( self );
    }
}

void intel_debug( Entity *self )
{
    gf2d_draw_circle( self->position, 32, vector4d( 255, 255, 0, 255, 255 ) );
}