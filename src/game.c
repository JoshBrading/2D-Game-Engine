#include <SDL.h>
#include <SDL_ttf.h>
#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "simple_logger.h"
#include "simple_json.h"
#include "g_globals.h"

#include "g_world.h"
#include "g_entity.h"
#include "g_static_entity.h"
#include "g_particle.h"
#include "g_collision.h"
#include "g_test_bounce_ball.h"

#include "g_weapon.h"
#include "g_player.h"
#include "c_intel.h"
#include "c_hostage.h"

#include "g_hud.h"

//#include "../include/g_weapon.h"

#include "g_weapon.h"

Uint8 g_debug;
Uint32 g_screen_width;
Uint32 g_screen_height;
Uint32 g_time;

int main ( int argc, char* argv[] )
{
    g_debug = false;
    if ( argc > 1 )
        if ( 0 == strcmp ( argv[1], "debug" ) )
            g_debug = true;

    /*variable declarations*/

    g_screen_width = 1200;
    g_screen_height = 720;
    g_time = 0;

    int done = 0;
    const Uint8* keys;
    Sprite* sprite;

    int mx, my;
    float mf = 0;
    Sprite* mouse;
    Vector4D mouseColor = { 255,100,255,200 };

    /*program initializtion*/
    init_logger ( "gf2d.log" );
    slog ( "---==== BEGIN ====---" );
    gf2d_graphics_initialize (
        "gf2d",
        1200,
        700,
        1200,
        700,
        vector4d ( 0, 0, 0, 255 ),
        0 );
    gf2d_graphics_set_frame_delay ( 0 ); // Changed from 16
    gf2d_sprite_init ( 128 );
    SDL_ShowCursor ( SDL_ENABLE );

    collision_system_init ( vector2d ( 32, 20 ) ); // DO NOT TOUCH
    entity_manager_init ( 128 );
    static_entity_manager_init( 128 );
    particle_manager_init ( 1024 );
    weapon_manager_init ( 128 );

    HUD_init();

    /*demo setup*/
    sprite = gf2d_sprite_load_image ( "images/backgrounds/bg_flat.png" );
    mouse = gf2d_sprite_load_all ( "images/pointer.png", 32, 32, 16 );

    //Entity *q;
    //q = entity_new();
    //q->sprite = mouse;
    //q->bounds.w = q->sprite->frame_w;
    //q->bounds.h = q->sprite->frame_h;
    //q->bounds.x = 0;
    //q->bounds.y = 0;

    Entity* player = player_new ();
    player->team = TEAM_FRIEND;
    player->position = vector2d ( 10, 10 );

    Entity* demo;
    demo = friendly_new ();
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

    for (int i = 0; i < 3; i++)
    {
        Entity *q3;
        q3 = enemy_new();
        q3->sprite = mouse;
        q3->bounds.w = q3->sprite->frame_w;
        q3->bounds.h = q3->sprite->frame_h;
        q3->bounds.x = q3->sprite->frame_w;
        q3->bounds.y = q3->sprite->frame_h;

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
        q3->sprite = mouse;
        q3->bounds.w = q3->sprite->frame_w;
        q3->bounds.h = q3->sprite->frame_h;
        q3->bounds.x = q3->sprite->frame_w;
        q3->bounds.y = q3->sprite->frame_h;

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
        q3->sprite = mouse;
        q3->bounds.w = q3->sprite->frame_w;
        q3->bounds.h = q3->sprite->frame_h;
        q3->bounds.x = q3->sprite->frame_w;
        q3->bounds.y = q3->sprite->frame_h;

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
   
   // Entity* hostage = hostage_new();
   // hostage->sprite = gf2d_sprite_load_image( "images/hostage.png" );
   // hostage->team = TEAM_FRIEND;
   // hostage->state = ENT_IDLE;

    world_load( "config/asset_list.json" );


    Uint32 time = 0;
    Uint32 time2 = 0;
    char text_line_1[32];
    /*main game loop*/
    while ( !done )
    {
        SDL_PumpEvents ();   // update SDL's internal event structures
        keys = SDL_GetKeyboardState ( NULL ); // get the keyboard state for this frame
        /*update things here*/
        g_time = SDL_GetTicks();
        SDL_GetMouseState ( &mx, &my );
        mf += 0.1;
        if ( mf >= 16.0 )mf = 0;


        gf2d_graphics_clear_screen ();// clears drawing buffers
        // all drawing should happen betweem clear_screen and next_frame
            //backgrounds drawn first
           // gf2d_sprite_draw_image(sprite,vector2d(0,0));

            //q->position = vector2d( (float)mx, (float)my );

        static_entity_draw_all();

        entity_manager_draw_all ();
        entity_manager_update_all ();
        entity_manager_think_all ();

        weapon_manager_draw_all();
        weapon_manager_update_all();
        weapon_manager_think_all();

        particle_manager_draw_all ();
        particle_manager_update_all ();

        collision_system_update_all ();
        if ( g_time > time + 20 ) // Run every 10ms
        {
            //collision_system_clear();
            entity_manager_update_fixed_all ();
            entity_manager_think_fixed_all ();
            particle_manager_update_fixed_all ();

            weapon_manager_think_fixed_all();


            time = g_time;
        }
        if ( g_debug )
        {
            collision_system_draw_debug ();
            static_entity_manager_draw_debug();
            entity_manager_draw_debug ();
        }
       
        HUD_draw();
        //UI elements last
        //gf2d_sprite_draw(
        //    mouse,
        //    vector2d(mx,my),
        //    NULL,
        //    NULL,
        //    NULL,
        //    NULL,
        //    &mouseColor,
        //    (int)mf);
        gf2d_grahics_next_frame ();// render current draw frame and skip to the next frame

        if ( keys[SDL_SCANCODE_Q] ) g_debug = true;
        if ( keys[SDL_SCANCODE_E] ) g_debug = false;

        if ( keys[SDL_SCANCODE_ESCAPE] )done = 1; // exit condition
        //slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
    }
    slog ( "---==== END ====---" );
    return 0;
}
/*eol@eof*/


