#include "simple_json.h"
#include "simple_logger.h"
#include "g_world.h"

#include "g_door.h"
#include "g_static_entity.h"

void world_spawn_friendly( char* filename, Vector2D pos );
void world_spawn_enemies();

void world_load( char *filename )
{
    SJson *json, *wjson;

    slog( filename );
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
        char *filename, *type;
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

        type = sj_get_string_value( sj_object_get_value( data, "type" ) );

        //if (strcmp( "friendly", type ) == 0)
        //{
        //
        //    world_spawn_friendly( "config/compantion_1.json", vector2d( x, y ) );
        //    world_spawn_friendly( "config/compantion_2.json", vector2d( x, y ) );
        //    world_spawn_friendly( "config/compantion_3.json", vector2d( x, y ) );
        //    world_spawn_friendly( "config/compantion_4.json", vector2d( x, y ) );
        //
        //    continue;
        //}

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

          //  slog( "DoorP: %f, O: %i, B: %f", door->position.y, hit_offset_y, door->bounds.y );
            
        }
    }

    world_spawn_friendly( "null", vector2d( 32, 32 ) );
    world_spawn_enemies();

}

void world_spawn_enemies( )
{
    for (int i = 0; i < 3; i++)
    {
        Entity *q3;
        q3 = enemy_new();

        q3->nav_zone.x = 629;
        q3->nav_zone.y = 0;
        q3->nav_zone.w = 550;
        q3->nav_zone.h = 350;

        q3->position.x = 800 + 80 * i;// rand() % 1050;
        q3->position.y = 20;// rand() % 550;

        q3->target_position = q3->position;
        q3->view_range = 100;

        q3->idle_time_max = 1000;
        q3->idle_time_min = 500;
        q3->state = ENT_WANDER;
    }
    for (int i = 0; i < 2; i++)
    {
        Entity *q3;
        q3 = enemy_new();

        q3->nav_zone.x = 629;
        q3->nav_zone.y = 400;
        q3->nav_zone.w = 550;
        q3->nav_zone.h = 250;

        q3->position.x = 650 + 80 * i;// rand() % 1050
        q3->position.y = 510;// rand() % 550;

        q3->target_position = q3->position;
        q3->view_range = 100;

        q3->idle_time_max = 1000;
        q3->idle_time_min = 500;
        q3->state = ENT_WANDER;
    }
    for (int i = 0; i < 2; i++)
    {
        Entity *q3;
        q3 = enemy_new();

        q3->nav_zone.x = 129;
        q3->nav_zone.y = 0;
        q3->nav_zone.w = 470;
        q3->nav_zone.h = 250;

        q3->position.x = 129 + 80 * i;// rand() % 1050
        q3->position.y = 50;// rand() % 550;

        q3->target_position = q3->position;
        q3->view_range = 100;

        q3->idle_time_max = 1000;
        q3->idle_time_min = 500;
        q3->state = ENT_WANDER;
    }

    for (int i = 0; i < 3; i++)
    {
        intel_new();
    }

    Entity *hostage = entity_new();
    hostage->sprite = gf2d_sprite_load_image( "images/hostage.png" );
    hostage->team = TEAM_FRIEND;
    hostage->state = ENT_IDLE;
    hostage->tag = "hostage";
    hostage->scale.x = 0.125;
    hostage->scale.y = 0.125;
    hostage->offset.x = 16;
    hostage->offset.y = 16;
    hostage->bounds.w = 32;
    hostage->bounds.h = 32;
    hostage->bounds.x = hostage->position.x - hostage->offset.x;
    hostage->bounds.y = hostage->position.y - hostage->offset.y;
    hostage->toggles.B = false;
    hostage->toggles.A = false;
    hostage->move_speed = 3;
    hostage->offset.x = 16;
    hostage->offset.x = 16;
    hostage->position = vector2d( rand() % 1000, rand() % 700 );
    hostage->health = 999;

}

void world_spawn_friendly( char *filename, Vector2D pos )
{
    if (entity_manager_get_player()) return;
    Entity *player = player_new();
    player->team = TEAM_FRIEND;
    player->position = vector2d( 64, 64 );
    player->tag = "player";

    Entity *demo;
    demo = friendly_new();
    demo->sprite = gf2d_sprite_load_image( "images/expo.png" );
    demo->scale.x = 0.125;
    demo->scale.y = 0.125;
    demo->tag = "demo";
    demo->offset.x = 16;
    demo->offset.y = 16;

    demo->bounds.w = 32;
    demo->bounds.h = 32;
    demo->bounds.x = demo->position.x - demo->offset.x;
    demo->bounds.y = demo->position.y - demo->offset.y;

    demo->nav_zone.x = 629;
    demo->nav_zone.y = 0;
    demo->nav_zone.w = 550;
    demo->nav_zone.h = 350;

    demo->position.y = 20 + 80;// rand() % 1050;
    demo->position.x = 20;// rand() % 550;

    demo->target_position = demo->position;
    demo->view_range = 100;

    demo->idle_time_max = 1000;
    demo->idle_time_min = 500;
    demo->state = ENT_WANDER;

    Entity *shotgun;
    shotgun = friendly_new();
    shotgun->sprite = gf2d_sprite_load_image( "images/shotgun.png" );
    shotgun->scale.x = 0.125;
    shotgun->scale.y = 0.125;
    shotgun->tag = "shotgun";
    shotgun->offset.x = 16;
    shotgun->offset.y = 16;

    shotgun->bounds.w = 32;
    shotgun->bounds.h = 32;
    shotgun->bounds.x = shotgun->position.x - shotgun->offset.x;
    shotgun->bounds.y = shotgun->position.y - shotgun->offset.y;

    shotgun->nav_zone.x = 629;
    shotgun->nav_zone.y = 0;
    shotgun->nav_zone.w = 550;
    shotgun->nav_zone.h = 350;

    shotgun->position.y = 20 + 80;// rand() % 1050;
    shotgun->position.x = 20;// rand() % 550;

    shotgun->target_position = shotgun->position;
    shotgun->view_range = 100;

    shotgun->idle_time_max = 1000;
    shotgun->idle_time_min = 500;
    shotgun->state = ENT_WANDER;
}
