#include "../IDG_Common.h"

#include "../system/IDG_AnimationHandler.h"
#include "../system/IDG_Atlas.h"
#include "../system/IDG_Draw.h"
#include "../system/IDG_Entities.h"
#include "../system/IDG_Hitbox.h"
#include "../system/IDG_Util.h"
#include "../game/bullets.h"
#include "../game/levelup.h"

#include "player.h"

static void tick         (entity_t *self);
static void draw         (entity_t *self);
static void move         (entity_t *self, animation_handler_t *ah);
static void die          (entity_t *self);
static void shoot        (entity_t *self, gunner_t *g);
static void fire_pistol  (entity_t *self);
static void fire_bfg     (entity_t *self); // TODO - refactor pickup fire funcs
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
static atlas_image_t *pu_bfg_prj   [PU_BFG_PRJ_ANIM_FRAMES];

extern app_t   app;
extern game_t  game;
extern stage_t stage;

void init_player(entity_t *e)
{
    app.dev.entity_count++;
    
    // set textures
    if(p_idle_n == NULL)
    {
        char filename[MAX_FILENAME_LENGTH];

        // load idle textures
        p_idle_n  = IDG_GetAtlasImage ("data/gfx/player/p_idle_n/tile1.png",  1);
        p_idle_s  = IDG_GetAtlasImage ("data/gfx/player/p_idle_s/tile1.png",  1);
        p_idle_ew = IDG_GetAtlasImage ("data/gfx/player/p_idle_ew/tile1.png", 1);
        // p_idle_n  = IDG_GetAtlasImage ("data/gfx/frin_test/f_idle_n/tile1.png",  1);
        // p_idle_s  = IDG_GetAtlasImage ("data/gfx/frin_test/f_idle_s/tile1.png",  1);
        // p_idle_ew = IDG_GetAtlasImage ("data/gfx/frin_test/f_idle_ew/tile1.png", 1);
    
        // load hurt textures
        p_hurt_n  = IDG_GetAtlasImage ("data/gfx/player/p_hurt_n/tile1.png",  1);
        p_hurt_s  = IDG_GetAtlasImage ("data/gfx/player/p_hurt_s/tile1.png",  1);
        p_hurt_ew = IDG_GetAtlasImage ("data/gfx/player/p_hurt_ew/tile1.png", 1);

        // projectile textures
        bullet    = IDG_GetAtlasImage ("data/gfx/effects/plasma/tile1.png",   1);
        for(int i=0; i<PU_BFG_PRJ_ANIM_FRAMES; i++)
        {
            sprintf(filename, "data/gfx/effects/bfg_prj/tile%d.png", (i+1));
            pu_bfg_prj[i] = IDG_GetAtlasImage(filename, 1);
        }

        // load walk textures
        for(int i=0; i<P_WALK_ANIM_FRAMES; i++)
        {
            // walk north
            sprintf(filename, "data/gfx/player/p_walk_n/tile%d.png", (i+1));
            p_walk_n[i]  = IDG_GetAtlasImage(filename, 1);
            
            // walk south
            sprintf(filename, "data/gfx/player/p_walk_s/tile%d.png", (i+1));
            p_walk_s[i]  = IDG_GetAtlasImage(filename, 1);
            
            // walk east/west
            sprintf(filename, "data/gfx/player/p_walk_ew/tile%d.png", (i+1));
            p_walk_ew[i] = IDG_GetAtlasImage(filename, 1);
            // walk north
            // sprintf(filename, "data/gfx/frin_test/f_walk_n/tile%d.png", (i+1));
            // p_walk_n[i]  = IDG_GetAtlasImage(filename, 1);
            
            // // walk south
            // sprintf(filename, "data/gfx/frin_test/f_walk_s/tile%d.png", (i+1));
            // p_walk_s[i]  = IDG_GetAtlasImage(filename, 1);
            
            // // walk east/west
            // sprintf(filename, "data/gfx/frin_test/f_walk_ew/tile%d.png", (i+1));
            // p_walk_ew[i] = IDG_GetAtlasImage(filename, 1);
        }

        // load die textures
        for(int i=0; i<P_DIE_ANIM_FRAMES; i++)
        {
            sprintf(filename, "data/gfx/player/p_die/tile%d.png", (i+1));
            p_die[i]     = IDG_GetAtlasImage(filename, 1);
        }
    }

    // set mallocs
    gunner_t *g;
    g = malloc(sizeof(gunner_t));
    memset(g, 0, sizeof(gunner_t));
    
    g->max_life          = P_BASE_LIFE;    // base health, before modifiers
    g->curr_life         = P_BASE_LIFE;    // current health
    g->rest              = P_BASE_REST;    // base lives, before modifiers
    g->target_xp         = P_BASE_TRGT_XP; // base xp required to levelup    
    g->curr_xp           = P_BASE_XP;      // base xp - 0
    g->level             = P_BASE_LEVEL;   // base level - 1
    // g->pickups           = PU_PISTOL;
    g->pickups           = PU_BFG;
    e->texture           = p_idle_s;
    e->flags             = EF_WEIGHTLESS;
    e->friction          = P_FRICTION;
    e->radius            = 64;
    
    // set inherited structs
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
    animation_handler_t *ah = IDG_GetAnimationHandler(self);
    gunner_t *g;
    g = (gunner_t *)self->data;

    move             (self, ah);
    shoot            (self, g);
    // do_levelup       (self, g);
    IDG_UpdateHitbox (self);
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
    // TODO - refactor this into animationhandler.h
    if(IDG_EntityIsMoving(self))
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
        // ah->frame = 0;
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
        if(g->pickups & PU_PISTOL)
        {
            fire_pistol(self);
            g->reload = PU_PISTOL_BASE_RELOAD_SPD;
        }
        if(g->pickups & PU_BFG)
        {
            fire_bfg(self);
            g->reload = PU_BFG_BASE_RELOAD_SPD;
        }
    }
}

static void fire_pistol(entity_t *self)
{
    bullet_t *b;
    int base_angle;
    
    b            = spawn_bullet(self);
    b->type_flag = PU_PISTOL;
    b->texture   = bullet;
    b->damage    = PU_PISTOL_BASE_DMG;
    b->life      = (FPS*PU_PISTOL_BASE_LIFE);
    IDG_CreateBulletHitbox(b, HB_RECT);

    b->x   = (self->x+(self->texture->rect.w/2));
    b->y   = (self->y+(self->texture->rect.h/2));

    IDG_GetSlope(app.mouse.x, app.mouse.y, (b->x-stage.camera.pos.x), (b->y-stage.camera.pos.y), &b->dx, &b->dy);
    
    b->dx *= PU_PISTOL_BASE_PRJ_SPD;
    b->dy *= PU_PISTOL_BASE_PRJ_SPD;
}

static void fire_bfg(entity_t *self)
{
    bullet_t *b;
    int base_angle;

    b            = spawn_bullet(b);
    b->type_flag = PU_BFG;
    b->texture   = pu_bfg_prj[0];
    b->damage    = PU_BFG_BASE_DMG;
    b->life      = (FPS*PU_BFG_BASE_LIFE);
    IDG_CreateBulletHitbox(b, HB_SPH);

    b->x = self->x;
    b->y = self->y;

    IDG_GetSlope(app.mouse.x, app.mouse.y, (b->x-stage.camera.pos.x), (b->y-stage.camera.pos.y), &b->dx, &b->dy);

    b->dx *= PU_BFG_BASE_PRJ_SPD;
    b->dy *= PU_BFG_BASE_PRJ_SPD;
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