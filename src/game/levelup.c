#include "../IDG_Common.h"

#include "../system/IDG_Draw.h"
#include "../system/IDG_Text.h"

#include "levelup.h"

extern app_t   app;
extern stage_t stage;

// handles incremental XP increase
void do_levels(void)
{
    gunner_t *g = (gunner_t *)stage.player->data;
    if(g->curr_xp >= g->target_xp)
    {
        double div  = (double)g->curr_xp/g->target_xp;
        int    lvls = (int)div;
        int    rem  = (int)(10*(div-lvls));
        
        if(lvls > 0)
        {
            g->curr_xp    = rem;
            g->target_xp  = ((P_BASE_TRGT_XP*g->level)*P_XP_MODIFIER); // TODO - proof of concept. test this and see if its too dramatic.
            g->level     += lvls;
            // stage.levelup = 1;
        }
    }
}

// handlers PU selection
void do_levelup(void)
{
}

void draw_levelup_hud(void)
{
    IDG_DrawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0x00, 0x00, 0x00, 160);
    app.font_scale = 1.5;
    IDG_DrawText("LEVEL UP!", (SCREEN_WIDTH/2), (SCREEN_HEIGHT/4), 0xFF, 0xFF, 0xFF, TEXT_ALIGN_CENTER, 0);
    app.font_scale = 1;
    IDG_DrawRect(400, ((SCREEN_HEIGHT/4)+64), (SCREEN_WIDTH-800), 500, 0xFF, 0xFF, 0xFF, 0xFF);
}