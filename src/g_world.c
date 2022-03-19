#include "simple_json.h"
#include "simple_logger.h"
#include "g_world.h"

#include "g_entity.h"
#include "g_static_entity.h"


void world_load( char *filename )
{
    SJson *json, *wjson;

    sj_enable_debug();

    json = sj_load( filename );

    if (!json)
    {
        slog( "Failed to load json file (%s)", filename );
        return NULL;
    }

    wjson = sj_object_get_value( json, "asset_list" );

    if (!wjson)
    {
        slog( "Failed to find asset_list in (%s)", filename );
        sj_free( json );
        return NULL;
    }

   // 

    Uint32 asset_count = sj_array_get_count( wjson );

    for (int i = 0; i < asset_count; i++)
    {
        SJson *data;
        char *filename;

        Uint32 height = -1;
        Uint32 width = -1;

        data = sj_array_get_nth( wjson, i );

        sj_get_integer_value( sj_object_get_value( data, "height" ), &height );
        sj_get_integer_value( sj_object_get_value( data, "width" ), &width );
      

        filename = sj_get_string_value( sj_object_get_value( data, "sprite" ) );

        slog( filename );

        StaticEntity *sEnt;
        sEnt = static_entity_new();
        sEnt->sprite = gf2d_sprite_load_image( filename );

        sEnt->position.x = rand() % 1050;
        sEnt->position.y = rand() % 550;

        sEnt->collision_type = sj_get_string_value( sj_object_get_value( data, "collisionType" ) );
        slog( sEnt->collision_type );
        if (strcmp(sEnt->collision_type, "box") == 0)
        {
            if (height == -1 || width == -1)
            {
                sEnt->bounds.w = sEnt->sprite->frame_w;
                sEnt->bounds.h = sEnt->sprite->frame_h;
                sEnt->bounds.x = sEnt->position.x;
                sEnt->bounds.y = sEnt->position.y;
            }
            else
            {
                sEnt->bounds.w = width;
                sEnt->bounds.h = height;
                sEnt->bounds.x = sEnt->position.x;
                sEnt->bounds.y = sEnt->position.y;
            }
        }
        else if (strcmp( sEnt->collision_type, "polygon" ) == 0)
        {
            sEnt->poly_bounds.vert1 = vector2d( sEnt->position.x, sEnt->position.y + 70.5f);
            sEnt->poly_bounds.vert2 = vector2d( sEnt->position.x + 70.5f, sEnt->position.y);
            sEnt->poly_bounds.vert3 = vector2d( sEnt->position.x + 87, sEnt->position.y + 17.5f );
            sEnt->poly_bounds.vert4 = vector2d( sEnt->position.x + 17.5f, sEnt->position.y + 88 );
        }
        else
        {
            slog( "Failed to git collision type" );
        }




    }


}
