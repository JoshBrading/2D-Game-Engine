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

#include "g_hostage.h"

#include "g_weapon.h"
#include "g_player.h"
#include "c_intel.h"

#include "g_hud.h"
#include "g_menu.h"
#include "g_weapon.h"

#include "gfc_list.h"

Uint8 g_debug;
Uint32 g_screen_width;
Uint32 g_screen_height;
Uint32 g_time;
GameState g_state;

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
    g_state = G_PAUSE;

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

    menu_manager_init( 16 );

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
   
    Entity* hostage = entity_new();
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
    hostage->position = vector2d( 1130, 325 );
    hostage->health = 999;

    world_load( "config/asset_list.json" );

    Menu *mNew = menu_new();
    mNew->tag = "test";
    Menu *menu = menu_new();
    menu->tag = "main";
    menu->enabled = true;
    menu->background = gf2d_sprite_load_image( "images/gui/background.png" );
   
    menu->selector_sprite = gf2d_sprite_load_image( "images/gui/selected.png" );
   
    menu->selector_target_pos = vector2d(0, 0);
    menu->selector_position = vector2d(0, 0);

    mNew->background = gf2d_sprite_load_image( "images/gui/background.png" );
    mNew->selector_sprite = gf2d_sprite_load_image( "images/gui/selected.png" );
    mNew->selector_target_pos = vector2d(0, 0);
    mNew->selector_position = vector2d(0, 0);
   
    MenuText title;
    title.text = "PAUSED";
    title.font = TTF_OpenFont( "fonts/FRADMCN.ttf", 36 );
    title.position = vector2d( 323.6, 270 );
    gfc_list_append( menu->labels, &title );
   
    MenuText title2;
    title2.text = "OBJECTIVES";
    title2.font = TTF_OpenFont( "fonts/FRADMCN.ttf", 36 );
    title2.position = vector2d( 743.8, 270 );
    gfc_list_append( menu->labels, &title2 );
   
   
    MenuImage image;
    image.sprite = gf2d_sprite_load_image( "images/gui/obj_grad.png" );
    image.position = vector2d( 696, 314 );
    gfc_list_append( menu->images, &image );
   
    MenuText obj;
    obj.text = "Kill all enemies.";
    obj.font = TTF_OpenFont( "fonts/arial.ttf", 18 );
    obj.position = vector2d( 710, 320 );
    gfc_list_append( menu->labels, &obj );
    
   
    MenuText obj2;
    obj2.text = "Locate all intel.";
    obj2.font = TTF_OpenFont( "fonts/arial.ttf", 18 );
    obj2.position = vector2d( 710, 340 );
    gfc_list_append( menu->labels, &obj2 );
    
   
    MenuText obj3;
    obj3.text = "Rescue the hostage.";
    obj3.font = TTF_OpenFont( "fonts/arial.ttf", 18 );
    obj3.position = vector2d( 710, 360 );
    gfc_list_append( menu->labels, &obj3 );
   
    MenuButton button;
    button.action = menu_close;
    button.selected = true;
    button.label.text = "RESUME";
    button.label.font = TTF_OpenFont( "fonts/arial.ttf", 18 );
    button.background = gf2d_sprite_load_image( "images/gui/button.png" );
    button.icon_offset = vector2d( 5, 2 );
    button.position = vector2d( 279, 317 );
    button.label.position = vector2d( 30, 0 );
    gfc_list_append( menu->buttons, &button );
    menu->current_button = &button;
   
    MenuButton button2;
    button2.action = menu_go_to;
    button2.data = mNew;
    button2.selected = false;
    button2.label.text = "EDITOR";
    button2.label.font = TTF_OpenFont( "fonts/arial.ttf", 18 );
    button2.background = gf2d_sprite_load_image( "images/gui/button.png" );
    button2.icon_offset = vector2d( 5, 2 );
    button2.position = vector2d( 279, 343 );
    button2.label.position = vector2d( 30, 0);
    gfc_list_append( menu->buttons, &button2 );
   
    MenuButton button3;
    button3.selected = false;
    button3.label.text = "SETTINGS";
    button3.label.font = TTF_OpenFont( "fonts/arial.ttf", 18 );
    button3.background = gf2d_sprite_load_image( "images/gui/button.png" );
    button3.icon_offset = vector2d( 5, 2 );
    button3.position = vector2d( 279, 369 );
    button3.label.position = vector2d( 30, 0 );
    gfc_list_append( menu->buttons, &button3 );
   
    MenuButton button4;
    button4.action = menu_quit;
    button4.selected = false;
    button4.label.text = "QUIT";
    button4.label.font = TTF_OpenFont( "fonts/arial.ttf", 18 );
    button4.background = gf2d_sprite_load_image( "images/gui/button.png" );
    button4.icon_offset = vector2d( 5, 2 );
    button4.position = vector2d( 279, 395 );
    button4.label.position = vector2d( 30, 0 );
    gfc_list_append( menu->buttons, &button4 );
   
    MenuButton button6;
    button6.selected = false;
    button6.label.text = "BUTTON";
    button6.label.font = TTF_OpenFont( "fonts/arial.ttf", 18 );
    button6.background = gf2d_sprite_load_image( "images/gui/button.png" );
    button6.icon_offset = vector2d( 5, 2 );
    button6.position = vector2d( 279, 317 );
    button6.label.position = vector2d( 30, 0 );
    gfc_list_append( mNew->buttons, &button6 );
   
    MenuButton button5;
    button5.action = menu_go_back;
    button5.selected = true;
    button5.label.text = "BACK";
    button5.label.font = TTF_OpenFont( "fonts/arial.ttf", 18 );
    button5.background = gf2d_sprite_load_image( "images/gui/button.png" );
    button5.icon_offset = vector2d( 5, 2 );
    button5.position = vector2d( 279, 343 );
    button5.label.position = vector2d( 30, 0 );
    gfc_list_append( mNew->buttons, &button5 );
    mNew->current_button = &button5;

    MenuText title6;
    title6.text = "MENU 2";
    title6.font = TTF_OpenFont( "fonts/FRADMCN.ttf", 36 );
    title6.position = vector2d( 128, 64 );
    gfc_list_append( mNew->labels, &title6 );

    MenuText title5;
    title5.text = "MENU 1";
    title5.font = TTF_OpenFont( "fonts/FRADMCN.ttf", 36 );
    title5.position = vector2d( 128, 64 );
    gfc_list_append( menu->labels, &title5 );

Uint8 m = true;

    Uint32 time = 0;
    Uint32 time2 = 0;
    char text_line_1[32];
    /*main game loop*/
    while ( g_state != G_STOP )
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
        entity_manager_draw_all();
        weapon_manager_draw_all();
        particle_manager_draw_all();

        if (g_state == G_RUN)
        {
            entity_manager_update_all();
            entity_manager_think_all();

            weapon_manager_update_all();
            weapon_manager_think_all();

            particle_manager_update_all();

            collision_system_update_all();

            if (g_time > time + 20) // Run every 10ms
            {
                //collision_system_clear();
                entity_manager_update_fixed_all();
                entity_manager_think_fixed_all();

                particle_manager_update_fixed_all();

                weapon_manager_think_fixed_all();


                time = g_time;
            }

            HUD_draw();

        }
        else if( g_state == G_PAUSE)
        {
            menu_manager_update_all();
            menu_manager_draw_all();

            if (g_time > time + 20)
            {
                menu_manager_update_fixed_all();
                time = g_time;
            }
        }


        if ( g_debug )
        {
            collision_system_draw_debug ();
            static_entity_manager_draw_debug();
            entity_manager_draw_debug ();
        }
       


        /// FROM WEB
       // SDL_Renderer *renderer = gf2d_graphics_get_renderer();
       //
       // SDL_Surface *SurfaceA = IMG_Load( "images/backgrounds/floor.png" ); 
       // SDL_Surface *SurfaceB = IMG_Load( "images/backgrounds/floor_dark.png" );
       //
       // SDL_Texture *TextureA = SDL_CreateTextureFromSurface( renderer, SurfaceA );
       // SDL_Texture *TextureB = SDL_CreateTextureFromSurface( renderer, SurfaceB );
       //
       // SDL_Texture *result = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 300, 300 );
       // SDL_SetTextureBlendMode( result, SDL_BLENDMODE_BLEND ); // blend - mod - none - none
       // SDL_SetRenderTarget( renderer, result );
       // SDL_SetTextureBlendMode( TextureA, SDL_BLENDMODE_MOD );
       // SDL_SetTextureBlendMode( TextureB, SDL_BLENDMODE_NONE );
       // SDL_SetRenderDrawColor( renderer, 0, 0, 0, 0 ); SDL_RenderClear( rende       rer );
       // SDL_RenderCopy( renderer, TextureB, NULL, NULL ); SDL_RenderCopy( renderer, TextureA, NULL, NULL );
       // SDL_SetRenderTarget( renderer, NULL ); SDL_SetRenderDrawColor( renderer, 0, 0, 255, 0 );
       // SDL_RenderClear( renderer );
       //
       // SDL_Surface *SurfaceD = IMG_Load( "images/backgrounds/floor_mask.png" );
       // SDL_Texture *TextureD = SDL_CreateTextureFromSurface( renderer, SurfaceD );
       // SDL_RenderCopy( renderer, TextureD, NULL, NULL ); 
       // SDL_RenderCopy( renderer, result, NULL, NULL );
       //
       // SDL_RenderPresent( renderer );
       //
       // SDL_FreeSurface( SurfaceB );
       // SDL_FreeSurface( SurfaceD );
       // SDL_DestroyTexture( TextureA );
       // SDL_DestroyTexture( TextureB );
       // SDL_DestroyTexture( TextureD );
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

        if ( keys[SDL_SCANCODE_T] && m)
        {
           // Menu *menu = menu_load( "config/menus/pause.json" );
           // if (menu) menu->enabled = true;
            m = false;
        }

        if ( keys[SDL_SCANCODE_ESCAPE] ) menu_open( menu ); // exit condition
        //slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
    }
    slog ( "---==== END ====---" );
    return 0;
}
/*eol@eof*/


