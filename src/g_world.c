#include "simple_json.h"
#include "simple_logger.h"
#include "g_world.h"

#include "g_door.h"
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
        Uint32 x = -1;
        Uint32 y = -1;

        Uint32 height = -1;
        Uint32 width = -1;

        data = sj_array_get_nth( wjson, i );

        sj_get_integer_value( sj_object_get_value( data, "height" ), &height );
        sj_get_integer_value( sj_object_get_value( data, "width" ), &width );
      
        sj_get_integer_value(sj_object_get_value( data, "position_x" ), &x);
        sj_get_integer_value(sj_object_get_value( data, "position_y" ), &y);

        filename = sj_get_string_value( sj_object_get_value( data, "sprite" ) );


        short isStatic;
        sj_get_bool_value( sj_object_get_value( data, "isStatic" ), &isStatic );

        if (isStatic)
        {
            StaticEntity *sEnt;
            sEnt = static_entity_new();
            sEnt->sprite = gf2d_sprite_load_image( filename );

            if (x == -1 && y == -1)
            {
                sEnt->position.x = rand() % 1050;
                sEnt->position.y = rand() % 550;
            }
            else
            {
                sEnt->position.x = x;
                sEnt->position.y = y;
            }

            sEnt->collision_type = sj_get_string_value( sj_object_get_value( data, "collisionType" ) );
          //  slog( sEnt->collision_type );
            if (strcmp( sEnt->collision_type, "box" ) == 0)
            {
                if (height == -1 || width == -1)
                {
                    sEnt->bounds.w = sEnt->sprite->frame_w;
                    sEnt->bounds.h = sEnt->sprite->frame_h;
                }
                else
                {
                    sEnt->bounds.w = width;
                    sEnt->bounds.h = height;
                }
            }
            else if (strcmp( sEnt->collision_type, "quad" ) == 0)
            {
                sEnt->quad_bounds.vert1 = vector2d( sEnt->position.x, sEnt->position.y + 70.5f );
                sEnt->quad_bounds.vert2 = vector2d( sEnt->position.x + 70.5f, sEnt->position.y );
                sEnt->quad_bounds.vert3 = vector2d( sEnt->position.x + 87, sEnt->position.y + 17.5f );
                sEnt->quad_bounds.vert4 = vector2d( sEnt->position.x + 17.5f, sEnt->position.y + 88 );
            }
            else
            {
                slog( "Failed to get collision type" );
            }


            if (strcmp( sj_get_string_value( sj_object_get_value( data, "type" ) ), "background" ) == 0)
            {
                sEnt->position.x = 0;
                sEnt->position.y = 0;
            }
            else
            {
                sEnt->bounds.x = sEnt->position.x;
                sEnt->bounds.y = sEnt->position.y;
            }

        }
        else
        {
            for (int i = 0; i < 3; i++)
            {


                Uint32 frame_h, frame_w, frames_per_line, height, width;
                float speed, range, health;

                Entity *door;
                door = door_new();

                sj_get_integer_value( sj_object_get_value( data, "height" ), &height );
                sj_get_integer_value( sj_object_get_value( data, "width" ), &width );

                sj_get_integer_value( sj_object_get_value( data, "frame_h" ), &frame_h );
                sj_get_integer_value( sj_object_get_value( data, "frame_w" ), &frame_w );
                sj_get_integer_value( sj_object_get_value( data, "frames_per_line" ), &frames_per_line );

                sj_get_float_value( sj_object_get_value( data, "speed" ), &speed );
                sj_get_float_value( sj_object_get_value( data, "range" ), &range );
                sj_get_float_value( sj_object_get_value( data, "health" ), &health );

                door->sprite = gf2d_sprite_load_all( filename, frame_w, frame_h, frames_per_line );
                door->position.x = rand() % 1050;
                door->position.y = rand() % 550;
                door->health = 1;
                door->interact_radius = range;
                door->speed.x = speed;
                door->state = DOOR_CLOSED;

                if (height == -1 || width == -1)
                {
                    door->bounds.w = door->sprite->frame_w;
                    door->bounds.h = door->sprite->frame_h;
                    door->bounds.x = door->position.x;
                    door->bounds.y = door->position.y;
                }
                else
                {
                    door->bounds.w = width;
                    door->bounds.h = height;
                    door->bounds.x = door->position.x;
                    door->bounds.y = door->position.y;
                }
            }

        }

        slog( sj_get_string_value( sj_object_get_value( data, "isStatic" ) ) );

    }


}
