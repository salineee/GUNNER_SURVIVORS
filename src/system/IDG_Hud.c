#include "../IDG_Common.h"

#include "../system/IDG_Atlas.h"
#include "../system/IDG_Draw.h"
#include "../system/IDG_Text.h"
#include "../system/IDG_Util.h"

#include "IDG_Hud.h"

extern app_t   app;
extern stage_t stage;

static void draw_player_hud  (void);
static void IDG_DrawDebugHUD (void);

static atlas_image_t *xp_bar_texture = NULL;
static atlas_image_t *xp_lvl_texture = NULL;

void IDG_InitHUD(void) {
    if(xp_bar_texture == NULL)
    {
        xp_bar_texture = IDG_GetAtlasImage("data/gfx/gui/xp_bar.png", 1);
        xp_lvl_texture = IDG_GetAtlasImage("data/gfx/gui/xp_lvl.png", 1);
    }
}

void IDG_DoHUD(void) {}

void IDG_DrawHUD(void)
{
    /* draw player's hud below this line */
    draw_player_hud();
    if(app.dev.show_debug) { IDG_DrawDebugHUD(); }
}

static void draw_player_hud(void)
{
    gunner_t *g;
    g = (gunner_t *)stage.player->data;

    char   text[64];
    int    base_hpbar_w,    // base hpbar width
           base_xpbar_w;    // base xpbar width
    double hpbar_w, hp_per, // current hpbar width, hp percentage
           xpbar_w, xp_per; // current xpbar width, xp percentage

    // #272d30 - light blueish grey
    // #98dced - light dusty blue

    // health bar
    // base_hpbar_w = 300;
    // hp_per       = ((100.0f*g->curr_life)/g->max_life);
    // hpbar_w      = MAX(((hp_per/100)*base_hpbar_w), 0);
    // IDG_DrawRect(10, (SCREEN_HEIGHT-60), base_hpbar_w, 20, 0x27, 0x2d, 0x30, 0xFF);
    // IDG_DrawRect(10, (SCREEN_HEIGHT-60), (int)hpbar_w, 20, 0xFF, 0x00, 0x00, 0xFF);
    // IDG_DrawOutlineRect(10, (SCREEN_HEIGHT-60), 300, 20, 0xFF, 0xFF, 0xFF, 0xFF);
    
    // experience bar
    base_xpbar_w = 166;
    xp_per       = ((100.0f*g->curr_xp)/g->target_xp);
    xpbar_w      = MAX(((xp_per/100)*base_xpbar_w), 0);
    IDG_DrawRect(40, (SCREEN_HEIGHT-34), base_xpbar_w, 20, 0x00, 0x00, 0x00, 0xFF);
    IDG_DrawRect(40, (SCREEN_HEIGHT-34), (int)xpbar_w, 20, 0x98, 0xdc, 0xed, 0xFF);
    IDG_BlitAtlasImage(xp_bar_texture, 10, (SCREEN_HEIGHT-37), 0, SDL_FLIP_NONE);
    IDG_BlitAtlasImage(xp_lvl_texture, (10+200+10), (SCREEN_HEIGHT-37), 0, SDL_FLIP_NONE);
    
    app.font_scale = 0.7;
    sprintf(text, "lvl. %d", g->level);
    IDG_DrawText(text, 269, (SCREEN_HEIGHT-38), 0xFF, 0xFF, 0xFF, TEXT_ALIGN_CENTER, 0);
    app.font_scale = 1.0;
}

static void IDG_DrawDebugHUD(void)
{
    // NEW - IMPORT TO ENGINE

    char text[64];
    app.font_scale = 0.5;

    sprintf(text, "%s", IDG_PRJ_TITLE);
    IDG_DrawText(text, 10, 10, 0xFF, 0xFF, 0xFF, TEXT_ALIGN_LEFT, 0);

    sprintf(text, "FPS: %d", app.dev.fps);
    IDG_DrawText(text, 10, 30, 0xFF, 0xFF, 0xFF, TEXT_ALIGN_LEFT, 0);

    if(stage.player != NULL)
    {
        gunner_t *g;
        g = (gunner_t *)stage.player->data;

        // hitbox_t *hb;
        // hb = (hitbox_t *)stage.player->hitbox;
        
        sprintf(text, "x: %d, y: %d", (int)stage.player->x, (int)stage.player->y);
        IDG_DrawText(text, 10, 50, 0xFF, 0xFF, 0xFF, TEXT_ALIGN_LEFT, 0);

        sprintf(text, "friction: %.3lf, dx: %.3lf, dy: %.3lf", stage.player->friction, stage.player->dx, stage.player->dy);
        IDG_DrawText(text, 10, 70, 0xFF, 0xFF, 0xFF, TEXT_ALIGN_LEFT, 0);
        
        sprintf(text, "Curr. HP: %d, Max HP: %d", g->curr_life, g->max_life);
        IDG_DrawText(text, 10, 90, 0xFF, 0xFF, 0xFF, TEXT_ALIGN_LEFT, 0);

        sprintf(text, "Curr. Lvl: %d, Curr. XP: %d, Tgt XP: %d", g->level, g->curr_xp, g->target_xp);
        IDG_DrawText(text, 10, 110, 0xFF, 0xFF, 0xFF, TEXT_ALIGN_LEFT, 0);
    }

    sprintf(text, "Mouse X: %d, Mouse Y: %d", app.mouse.x, app.mouse.y);
    IDG_DrawText(text, 10, 130, 0xFF, 0xFF, 0xFF, TEXT_ALIGN_LEFT, 0);

    sprintf(text, "Collisions: %d", app.dev.collision_checks);
    IDG_DrawText(text, 10, 150, 0xFF, 0xFF, 0xFF, TEXT_ALIGN_LEFT, 0);

    sprintf(text, "Entities: %d", app.dev.entity_count);
    IDG_DrawText(text, 10, 170, 0xFF, 0xFF, 0xFF, TEXT_ALIGN_LEFT, 0);

    sprintf(text, "Display Hitboxes: %s", (app.dev.show_hitboxes ? "TRUE" : "FALSE"));
    IDG_DrawText(text, 10, 190, 0xFF, 0xFF, 0xFF, TEXT_ALIGN_LEFT, 0);

    sprintf(text, "Display Sector2D: %s", (app.dev.show_sectors ? "TRUE" : "FALSE"));
    IDG_DrawText(text, 10, 210, 0xFF, 0xFF, 0xFF, TEXT_ALIGN_LEFT, 0);

    app.font_scale = 1.0;
}