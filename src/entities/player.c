#include "../IDG_Common.h"

#include "../system/IDG_AnimationHandler.h"
#include "../system/IDG_Atlas.h"
#include "../system/IDG_Draw.h"
#include "../system/IDG_Hitbox.h"
#include "../system/IDG_Util.h"
#include "../game/bullets.h"

#include "player.h"

static void tick         (entity_t *self);
static void draw         (entity_t *self);
static void move         (entity_t *self, animation_handler_t *ah);
static void die          (entity_t *self);
static void shoot        (entity_t *self, gunner_t *g);
static void fire_bullet  (entity_t *self);
static void do_levels    (entity_t *self, gunner_t *g);
static int  is_control   (int type);

static int            facing;

// directional idle textures
static atlas_image_t *p_idle_n;
static atlas_image_t *p_idle_s;
static atlas_image_t *p_idle_ew;

// directional walk textures
static atlas_image_t *p_walk_n  [P_WALK_ANIM_FRAMES];
static atlas_image_t *p_walk_s  [P_WALK_ANIM_FRAMES];
static atlas_image_t *p_walk_ew [P_WALK_ANIM_FRAMES];

static atlas_image_t *p_die     [P_DIE_ANIM_FRAMES];

// directional hurt/damage textures
static atlas_image_t *p_hurt_n;
static atlas_image_t *p_hurt_s;
static atlas_image_t *p_hurt_ew;

// projectile textures
static atlas_image_t *bullet;

extern app_t   app;
extern game_t  game;
extern stage_t stage;

void init_player(entity_t *e)
{
    app.dev.entity_count++;

    /* Set textures */
    if(p_idle_n == NULL)
    {
        // load idle textures
        // p_idle_n  = IDG_GetAtlasImage ("data/gfx/player/p_idle_n/tile1.png",  1);
        // p_idle_s  = IDG_GetAtlasImage ("data/gfx/player/p_idle_s/tile1.png",  1);
        // p_idle_ew = IDG_GetAtlasImage ("data/gfx/player/p_idle_ew/tile1.png", 1);
        p_idle_n  = IDG_GetAtlasImage ("data/gfx/frin_test/f_idle_n/tile1.png",  1);
        p_idle_s  = IDG_GetAtlasImage ("data/gfx/frin_test/f_idle_s/tile1.png",  1);
        p_idle_ew = IDG_GetAtlasImage ("data/gfx/frin_test/f_idle_ew/tile1.png", 1);
    
        // load hurt textures
        p_hurt_n  = IDG_GetAtlasImage ("data/gfx/player/p_hurt_n/tile1.png",  1);
        p_hurt_s  = IDG_GetAtlasImage ("data/gfx/player/p_hurt_s/tile1.png",  1);
        p_hurt_ew = IDG_GetAtlasImage ("data/gfx/player/p_hurt_ew/tile1.png", 1);

        // projectile textures
        bullet    = IDG_GetAtlasImage ("data/gfx/effects/plasma/tile1.png",   1);

        // load walk textures
        char filename[MAX_FILENAME_LENGTH];
        for(int i=0; i<P_WALK_ANIM_FRAMES; i++)
        {
            // // walk north
            // sprintf(filename, "data/gfx/player/p_walk_n/tile%d.png", (i+1));
            // p_walk_n[i]  = IDG_GetAtlasImage(filename, 1);
            
            // // walk south
            // sprintf(filename, "data/gfx/player/p_walk_s/tile%d.png", (i+1));
            // p_walk_s[i]  = IDG_GetAtlasImage(filename, 1);
            
            // // walk east/west
            // sprintf(filename, "data/gfx/player/p_walk_ew/tile%d.png", (i+1));
            // p_walk_ew[i] = IDG_GetAtlasImage(filename, 1);
            // walk north
            sprintf(filename, "data/gfx/frin_test/f_walk_n/tile%d.png", (i+1));
            p_walk_n[i]  = IDG_GetAtlasImage(filename, 1);
            
            // walk south
            sprintf(filename, "data/gfx/frin_test/f_walk_s/tile%d.png", (i+1));
            p_walk_s[i]  = IDG_GetAtlasImage(filename, 1);
            
            // walk east/west
            sprintf(filename, "data/gfx/frin_test/f_walk_ew/tile%d.png", (i+1));
            p_walk_ew[i] = IDG_GetAtlasImage(filename, 1);
        }

        // load die textures
        for(int i=0; i<P_DIE_ANIM_FRAMES; i++)
        {
            sprintf(filename, "data/gfx/player/p_die/tile%d.png", (i+1));
            p_die[i]     = IDG_GetAtlasImage(filename, 1);
        }
    }

    // set mallocs
    // animation_handler_t *ah;
    // ah = malloc(sizeof(animation_handler_t));
    // memset(ah, 0, sizeof(animation_handler_t));

    gunner_t *g;
    g = malloc(sizeof(gunner_t));
    memset(g, 0, sizeof(gunner_t));
    
    // set defaults
    // ah->timer            = P_ANIM_TIME;

    g->max_life          = P_BASE_LIFE;    // base health, before modifiers
    g->curr_life         = P_BASE_LIFE;    // current health
    g->rest              = P_BASE_REST;    // base lives, before modifiers
    g->target_xp         = P_BASE_TRGT_XP; // base xp required to levelup    
    g->curr_xp           = P_BASE_XP;      // base xp - 0
    g->level             = P_BASE_LEVEL;   // base level - 1
    e->texture           = p_idle_s;
    e->flags             = EF_WEIGHTLESS;
    e->friction          = P_FRICTION;
    e->radius            = 64;
    
    // set inherited structs
    // e->animation_handler = ah;
    e->data              = g;
    
    // set callbacks
    e->tick              = tick;
    e->draw              = draw;
    e->die               = die;
    
    IDG_CreateAnimationHandler (e, P_ANIM_TIME);
    IDG_CreateHitbox           (e, HB_SPH);
    stage.player = e;
}

static void tick(entity_t *self) {
    animation_handler_t *ah;
    ah = (animation_handler_t *)self->animation_handler;

    gunner_t *g;
    g = (gunner_t *)self->data;

    move         (self, ah);
    shoot        (self, g);
    do_levels    (self, g);
    IDG_UpdateHitbox(self);
}

static void draw(entity_t *self) 
{
    IDG_BlitAtlasImage(
        self->texture,
        (self->x-stage.camera.pos.x), (self->y-stage.camera.pos.y),
        0,
        self->facing == FACING_RIGHT ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL
    );
}

static void move(entity_t *self, animation_handler_t *ah) 
{
    self->dx = self->dy = 0; // friction prevents idle animations from playing
    // TODO - why doesnt is_control(...) work?

    if(app.keyboard[SDL_SCANCODE_A])
    {
        self->dx      = -P_WALK_SPEED;
        self->facing  =  FACING_LEFT;
    }
    
    if(app.keyboard[SDL_SCANCODE_D])
    {
        self->dx      =  P_WALK_SPEED;
        self->facing  =  FACING_RIGHT; 
    }

    if(app.keyboard[SDL_SCANCODE_W])
    {
        self->dy      = -P_WALK_SPEED;
        self->facing  =  FACING_UP;
    }

    if(app.keyboard[SDL_SCANCODE_S])
    {
        self->dy      =  P_WALK_SPEED;
        self->facing  =  FACING_DOWN;
    }

    // TODO - consolidate these switch cases FFS
    if(self->dx != 0 || self->dy != 0)
    {
        ah->timer -= app.delta_time;
        if(ah->timer <= 0)
        {
            ah->frame = (ah->frame+1)%P_WALK_ANIM_FRAMES;
            ah->timer = P_ANIM_TIME; 
        }

        switch(self->facing)
        {
        case FACING_LEFT:
            self->texture = p_walk_ew[ah->frame];
            break;
        case FACING_RIGHT:
            self->texture = p_walk_ew[ah->frame];
            break;
        case FACING_UP:
            self->texture = p_walk_n[ah->frame];
            break;
        case FACING_DOWN:
            self->texture = p_walk_s[ah->frame];
            break;
        }
    }
    else
    {
        ah->frame = 0;
        switch(self->facing)
        {
        case FACING_LEFT:
            self->texture = p_idle_ew;
            break;
        case FACING_RIGHT:
            self->texture = p_idle_ew;
            break;
        case FACING_UP:
            self->texture = p_idle_n;
            break;
        case FACING_DOWN:
            self->texture = p_idle_s;
            break;
        }
    }
}

static void die(entity_t *self) {}

static void shoot(entity_t *self, gunner_t *g)
{
    g->reload = MAX((g->reload-app.delta_time), 0);
    if(app.mouse.buttons[SDL_BUTTON_LEFT] && g->reload == 0)
    {
        fire_bullet(self);
        g->reload = P_BASE_RELOAD_SPEED;
    }
}

static void fire_bullet(entity_t *self)
{
    bullet_t *b;
    int base_angle;
    
    b           = spawn_bullet(self);
    b->texture  = bullet;
    b->damage   = 1;
    b->life     = (FPS*2);
    IDG_CreateHitbox(b, HB_RECT);

    b->x   = (self->x+(self->texture->rect.w/2));
    b->y   = (self->y+(self->texture->rect.h/2));

    IDG_GetSlope(app.mouse.x, app.mouse.y, (b->x-stage.camera.pos.x), (b->y-stage.camera.pos.y), &b->dx, &b->dy);
    
    b->dx *= P_BASE_BULLET_SPEED;
    b->dy *= P_BASE_BULLET_SPEED;
}

static void do_levels(entity_t *self, gunner_t *g)
{
    if(g->curr_xp >= g->target_xp)
    {
        double div  = (double)g->curr_xp/g->target_xp;
        int    lvls = (int)div;
        int    rem  = (int)(10*(div-lvls));
        
        g->curr_xp   = rem;
        g->target_xp = ((P_BASE_TRGT_XP*g->level)*P_XP_MODIFIER); // TODO - proof of concept. test this and see if its too dramatic.
        g->level    += lvls;
    }
}

static int is_control(int type)
{
    int key, btn;

    /* include player control defs below this line */
    // if(type == CTRL_LEFT && app.joypad_axis[JOYPAD_AXIS_X] < -game.deadzone)
    //     return 1;

    if(type == CTRL_LEFT && app.joypad_axis[JAXIS_X] < -game.deadzone)
        return 1;
    if(type == CTRL_RIGHT && app.joypad_axis[JAXIS_X] > game.deadzone)
        return 1;

    key = game.keyboard_controls[type];
    btn = game.joypad_controls[type];

    return ((key != 0 && app.keyboard[key]) || (btn != -1 && app.joypad_button[btn]));
}