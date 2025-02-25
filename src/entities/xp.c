#include "../IDG_Common.h"

#include "../system/IDG_Atlas.h"
#include "../system/IDG_Draw.h"
#include "../system/IDG_Sound.h"

#include "xp.h"

extern app_t   app;
extern stage_t stage;

static void tick_xp_sm  (entity_t *self);
static void tick_xp_md  (entity_t *self);
static void tick_xp_lg  (entity_t *self);
static void draw        (entity_t *self);
static void xp_sm_touch (entity_t *self, entity_t *other);
static void xp_md_touch (entity_t *self, entity_t *other);
static void xp_lg_touch (entity_t *self, entity_t *other);

static int            xp_sm_init = 0;
static int            xp_md_init = 0;
static int            xp_lg_init = 0;
static atlas_image_t *xp_sm_texture[XP_MD_ANIM_FRAMES];
static atlas_image_t *xp_md_texture[XP_MD_ANIM_FRAMES];
static atlas_image_t *xp_lg_texture[XP_LG_ANIM_FRAMES];

void init_xp_sm(entity_t *e)
{
    app.dev.entity_count++;

    if(!xp_sm_init)
    {
        char filename[MAX_FILENAME_LENGTH];
        for(int i=0; i<XP_SM_ANIM_FRAMES; i++)
        {
            sprintf(filename, "data/gfx/pickups/xp_sm/tile%d.png", (i+1));
            xp_sm_texture[i] = IDG_GetAtlasImage(filename, 1);
        }
        xp_sm_init = 1;
    }

    animation_handler_t *ah;
    ah = malloc(sizeof(animation_handler_t));
    memset(ah, 0, sizeof(animation_handler_t));
    ah->timer            = XP_SM_ANIM_TIME;
    ah->bob_value        = XP_SM_BOB;
    e->animation_handler = ah;

    e->texture = xp_sm_texture[0];
    e->flags   = EF_INTERACTIVE;
    e->radius  = (int)e->texture->rect.w/2;
    
    e->tick    = tick_xp_sm;
    e->draw    = draw;
    e->touch   = xp_sm_touch;
}

void init_xp_md(entity_t *e)
{
    app.dev.entity_count++;

    if(!xp_md_init)
    {
        char filename[MAX_FILENAME_LENGTH];
        for(int i=0; i<XP_MD_ANIM_FRAMES; i++)
        {
            sprintf(filename, "data/gfx/pickups/xp_md/tile%d.png", (i+1));
            xp_md_texture[i] = IDG_GetAtlasImage(filename, 1);
        }
        xp_md_init = 1;
    }

    animation_handler_t *ah;
    ah = malloc(sizeof(animation_handler_t));
    memset(ah, 0, sizeof(animation_handler_t));
    ah->timer            = XP_MD_ANIM_TIME;
    ah->bob_value        = XP_MD_BOB;
    e->animation_handler = ah;

    e->texture = xp_md_texture[0];
    e->flags   = EF_INTERACTIVE;
    e->radius  = (int)e->texture->rect.w/2;

    e->tick    = tick_xp_md;
    e->draw    = draw;
    e->touch   = xp_md_touch;
}

void init_xp_lg(entity_t *e)
{
    app.dev.entity_count++;

    if(!xp_lg_init)
    {
        char filename[MAX_FILENAME_LENGTH];
        for(int i=0; i<XP_LG_ANIM_FRAMES; i++)
        {
            sprintf(filename, "data/gfx/pickups/xp_lg/tile%d.png", (i+1));
            xp_lg_texture[i] = IDG_GetAtlasImage(filename, 1);
        }
        xp_lg_init = 1;
    }

    animation_handler_t *ah;
    ah = malloc(sizeof(animation_handler_t));
    memset(ah, 0, sizeof(animation_handler_t));
    ah->timer            = XP_LG_ANIM_TIME;
    ah->bob_value        = XP_LG_BOB;
    e->animation_handler = ah;

    e->texture = xp_lg_texture[0];
    e->flags   = EF_INTERACTIVE;
    e->radius  = (int)e->texture->rect.w/2;

    e->tick    = tick_xp_lg;
    e->draw    = draw;
    e->touch   = xp_lg_touch;
}

static void tick_xp_sm(entity_t *self)
{
    animation_handler_t *ah;
    ah = (animation_handler_t *)self->animation_handler;
    ah->bob_value += (0.05*app.delta_time);
    ah->timer     -= app.delta_time;

    if(ah->timer <= 0)
    {
        ah->frame = (ah->frame+1)%XP_SM_ANIM_FRAMES;
        ah->timer = XP_SM_ANIM_TIME;
    }

    self->texture  = xp_sm_texture[ah->frame];
    // self->hitbox.x = self->x;
    // self->hitbox.y = self->y;
    // self->hitbox.w = self->texture->rect.w;
    // self->hitbox.h = self->texture->rect.h;
}

static void tick_xp_md(entity_t *self)
{
    animation_handler_t *ah;
    ah = (animation_handler_t *)self->animation_handler;
    ah->bob_value += (0.05*app.delta_time);
    ah->timer     -= app.delta_time;

    if(ah->timer <= 0)
    {
        ah->frame = (ah->frame+1)%XP_SM_ANIM_FRAMES;
        ah->timer = XP_SM_ANIM_TIME;
    }

    self->texture  = xp_md_texture[ah->frame];
    // self->hitbox.x = self->x;
    // self->hitbox.y = self->y;
    // self->hitbox.w = self->texture->rect.w;
    // self->hitbox.h = self->texture->rect.h;
}

static void tick_xp_lg(entity_t *self)
{
    animation_handler_t *ah;
    ah = (animation_handler_t *)self->animation_handler;
    ah->bob_value += (0.05*app.delta_time);
    ah->timer     -= app.delta_time;

    if(ah->timer <= 0)
    {
        ah->frame = (ah->frame+1)%XP_SM_ANIM_FRAMES;
        ah->timer = XP_SM_ANIM_TIME;
    }

    self->texture  = xp_lg_texture[ah->frame];
    // self->hitbox.x = self->x;
    // self->hitbox.y = self->y;
    // self->hitbox.w = self->texture->rect.w;
    // self->hitbox.h = self->texture->rect.h;
}

static void draw(entity_t *self)
{
    double y = (self->y-stage.camera.pos.y)+sin(((animation_handler_t *)self->animation_handler)->bob_value)*2;
    IDG_BlitAtlasImage(
        self->texture,
        (self->x-stage.camera.pos.x), y,
        0,
        SDL_FLIP_NONE
    );

    // SDL_Point xp_center = { ((self->x+(self->texture->rect.w/2))-stage.camera.pos.x), ((self->y+(self->texture->rect.h/2))-stage.camera.pos.y) };
    // IDG_BlitCircle(xp_center, self->radius);
}

// TODO - can the below functions be consolidated?

static void xp_sm_touch(entity_t *self, entity_t *other)
{
    gunner_t *g;
    if(other == stage.player)
    {
        g = (gunner_t *)other->data;
        g->curr_xp += XP_SM_AMT;
        self->dead = 1;
        app.dev.entity_count--;
    }
}

static void xp_md_touch(entity_t *self, entity_t *other)
{
    gunner_t *g;
    if(other == stage.player)
    {
        g = (gunner_t *)other->data;
        g->curr_xp += XP_MD_AMT;
        self->dead = 1;
        app.dev.entity_count--;
    }
}

static void xp_lg_touch(entity_t *self, entity_t *other)
{
    gunner_t *g;
    if(other == stage.player)
    {
        g = (gunner_t *)other->data;
        g->curr_xp += XP_LG_AMT;
        self->dead = 1;
        app.dev.entity_count--;
    }
}