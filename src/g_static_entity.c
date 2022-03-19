#include <stdlib.h>
#include <string.h>

#include "gf2d_draw.h"
#include "simple_logger.h"
#include "g_static_entity.h"

StaticEntityManager static_entity_manager = { 0 };

StaticEntity *static_entity_new( )
{
    int i;
    for (i = 0; i < static_entity_manager.static_entity_count; i++)
    {
        if (!static_entity_manager.static_entity_list[i]._inuse)// not used yet, so we can!
        {
            static_entity_manager.static_entity_list[i]._inuse = 1;
            static_entity_manager.static_entity_list[i].scale.x = 1;
            static_entity_manager.static_entity_list[i].scale.y = 1;
            static_entity_manager.static_entity_list[i].offset.x = 0;
            static_entity_manager.static_entity_list[i].offset.y = 0;
            static_entity_manager.static_entity_list[i]._id = i; //+ 1; // FIXME

            return &static_entity_manager.static_entity_list[i];
        }
    }
    slog( "StaticEntityNew: no free space in the static_entity list" );
    return NULL;
}

void static_entity_manager_close()
{
    int i;
    for (i = 0; i < static_entity_manager.static_entity_count; i++)
    {
        static_entity_free( &static_entity_manager.static_entity_list[i] );
    }
    free( static_entity_manager.static_entity_list );
    memset( &static_entity_manager, 0, sizeof( StaticEntityManager ) );
    slog( "StaticEntityManager: closed" );
}

void static_entity_manager_init( Uint32 maxEntities )
{
    static_entity_manager.static_entity_list = gfc_allocate_array( sizeof( StaticEntity ), maxEntities );
    if (static_entity_manager.static_entity_list == NULL)
    {
        slog( "StaticEntityManager: failed to allocate entity list, cannot allocate ZERO entities" );
        return;
    }
    static_entity_manager.static_entity_count = maxEntities;
    atexit( static_entity_manager_close );
    slog( "StaticEntityManager: initialized" );
}

void static_entity_free( StaticEntity *self )
{
    if (!self)
    {
        slog( "StaticEntityFree: Entity does not exist" );
        return;
    }
    if (self->sprite)
    {
        gf2d_sprite_free( self->sprite );
    }
    memset( self, 0, sizeof( self ) );
}

void static_entity_draw( StaticEntity *self )
{
    if (!self)return;
    if (!self->sprite) return;
    gf2d_sprite_draw( self->sprite, vector2d( self->position.x - self->offset.x, self->position.y - self->offset.y ), &self->scale, NULL, &self->rotation, NULL, NULL, self->frame );
}

void static_entity_draw_all()
{
    int i;
    for (i = 0; i < static_entity_manager.static_entity_count; i++)
    {
        if (!static_entity_manager.static_entity_list[i]._inuse)// not used yet
        {
            continue;// skip this iteration of the loop
        }
        static_entity_draw( &static_entity_manager.static_entity_list[i] );
    }
}

void static_entity_manager_draw_debug()
{
    for (int i = 0; i < static_entity_manager.static_entity_count; i++)
    {
        if (static_entity_manager.static_entity_list[i]._inuse)
        {
            if (strcmp( static_entity_manager.static_entity_list[i].collision_type, "box" ) == 0)
            {
                SDL_Rect rectToDraw = { static_entity_manager.static_entity_list[i].bounds.x, static_entity_manager.static_entity_list[i].bounds.y, static_entity_manager.static_entity_list[i].bounds.w, static_entity_manager.static_entity_list[i].bounds.h };
                gf2d_draw_rect( rectToDraw, vector4d( 255, 255, 0, 255 ) );
            }
            else if (strcmp( static_entity_manager.static_entity_list[i].collision_type, "polygon" ) == 0)
            {
                Polygon poly = static_entity_manager.static_entity_list[i].poly_bounds;

                gf2d_draw_line( poly.vert1, poly.vert2, vector4d( 255, 255, 0, 255 ) );
                gf2d_draw_line( poly.vert2, poly.vert3, vector4d( 255, 255, 0, 255 ) );
                gf2d_draw_line( poly.vert3, poly.vert4, vector4d( 255, 255, 0, 255 ) );
                gf2d_draw_line( poly.vert4, poly.vert1, vector4d( 255, 255, 0, 255 ) );
            }

        }
    }
}