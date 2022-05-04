#include "simple_json.h"
#include "simple_logger.h"
#include "g_world.h"

#include "g_door.h"
#include "g_static_entity.h"


void world_load( char *filename )
{
    SJson *json, *wjson;

    //sj_enable_debug();

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
            if (strcmp( sj_get_string_value( sj_object_get_value( data, "type" ) ), "background" ) == 0) continue;
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


            Uint32 frame_h, frame_w, frames_per_line, height, width, hit_offset_x, hit_offset_y, x, y, interact_x, interact_y;
            float speed, range, health;

            Entity *door = NULL;
            door = door_new();

            sj_get_integer_value( sj_object_get_value( data, "height" ), &height );
            sj_get_integer_value( sj_object_get_value( data, "width" ), &width );

            sj_get_integer_value( sj_object_get_value( data, "hit_off_x" ), &hit_offset_x );
            sj_get_integer_value( sj_object_get_value( data, "hit_off_y" ), &hit_offset_y );

            sj_get_integer_value( sj_object_get_value( data, "position_x" ), &x );
            sj_get_integer_value( sj_object_get_value( data, "position_y" ), &y );

            sj_get_integer_value( sj_object_get_value( data, "interact_offset_x" ), &interact_x );
            sj_get_integer_value( sj_object_get_value( data, "interact_offset_y" ), &interact_y );

            sj_get_integer_value( sj_object_get_value( data, "frame_h" ), &frame_h );
            sj_get_integer_value( sj_object_get_value( data, "frame_w" ), &frame_w );
            sj_get_integer_value( sj_object_get_value( data, "frames_per_line" ), &frames_per_line );

            sj_get_float_value( sj_object_get_value( data, "speed" ), &speed );
            sj_get_float_value( sj_object_get_value( data, "range" ), &range );
            sj_get_float_value( sj_object_get_value( data, "health" ), &health );

            door->sprite = gf2d_sprite_load_all( filename, frame_w, frame_h, frames_per_line );
            door->position.x = x;
            door->position.y = y;
            door->health = 1;
            door->interact_radius = range;
            door->interact_offset = vector2d( interact_x, interact_y );
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
                door->bounds.x = door->position.x + hit_offset_x;
                door->bounds.y = door->position.y + hit_offset_y;
            }

            slog( "DoorP: %f, O: %i, B: %f", door->position.y, hit_offset_y, door->bounds.y );
            
        }
    }
}
