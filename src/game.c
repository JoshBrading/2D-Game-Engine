#include <SDL.h>
#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "simple_logger.h"

#include "g_entity.h"
#include "g_collision.h"
#include "g_test_bounce_ball.h"

int main(int argc, char * argv[])
{
    Uint8 debug = false;
    if( argc > 1)
        if (0 == strcmp(argv[1], "debug"))
            debug = true;

    /*variable declarations*/
    int done = 0;
    const Uint8 * keys;
    Sprite *sprite;
    
    int mx,my;
    float mf = 0;
    Sprite *mouse;
    Vector4D mouseColor = {255,100,255,200};

    /*program initializtion*/
    init_logger("gf2d.log");
    slog("---==== BEGIN ====---");
    gf2d_graphics_initialize(
        "gf2d",
        1200,
        720,
        1200,
        720,
        vector4d(0,0,0,255),
        0);
    gf2d_graphics_set_frame_delay(16);
    gf2d_sprite_init(1024);
    SDL_ShowCursor(SDL_DISABLE);
    
    collision_system_init( vector2d( 6, 6 ) );
    entity_manager_init( 128 );

    /*demo setup*/
    sprite = gf2d_sprite_load_image("images/backgrounds/bg_flat.png");
    mouse = gf2d_sprite_load_all("images/pointer.png",32,32,16);

    Entity *q;
    q = entity_new();
    q->sprite = mouse;
    q->bounds.w = q->sprite->frame_w;
    q->bounds.h = q->sprite->frame_h;
    q->bounds.x = 0;
    q->bounds.y = 0;
    
    Entity *q2;
    q2 = ball_new();
    q2->sprite = mouse;
    q2->bounds.w = q2->sprite->frame_w;
    q2->bounds.h = q2->sprite->frame_h;
    q2->bounds.x = q2->sprite->frame_w;
    q2->bounds.y = q2->sprite->frame_h;
    q2->position.x = 512;
    q2->position.y = 270;


    Uint32 time = 0;
    Uint32 time2 = 0;
    /*main game loop*/
    while(!done)
    {
        SDL_PumpEvents();   // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        /*update things here*/
        SDL_GetMouseState(&mx,&my);
        mf+=0.1;
        if (mf >= 16.0)mf = 0;
        
        
        gf2d_graphics_clear_screen();// clears drawing buffers
        // all drawing should happen betweem clear_screen and next_frame
            //backgrounds drawn first
           // gf2d_sprite_draw_image(sprite,vector2d(0,0));
            
            q->position = vector2d( mx, my );

            entity_manager_draw_all();
            entity_manager_update_all();
            collision_system_update_all();
            if (SDL_GetTicks() > time + 20)
            {
                //collision_system_clear();
                entity_manager_update_fixed_all();
                entity_manager_think_fixed_all();
                time = SDL_GetTicks();
            }
            if (SDL_GetTicks() > time2 + 100)
            {
                //collision_system_clear();
                time2 = SDL_GetTicks();
            }

            if (debug)
            {
                collision_system_draw_debug();
                entity_manager_draw_debug();
            }
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
        gf2d_grahics_next_frame();// render current draw frame and skip to the next frame
        
        if (keys[SDL_SCANCODE_ESCAPE])done = 1; // exit condition
        //slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
    }
    slog("---==== END ====---");
    return 0;
}
/*eol@eof*/
