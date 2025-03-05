#include "../IDG_Common.h"

#include "../system/IDG_AnimationHandler.h"
#include "../system/IDG_Atlas.h"
#include "../system/IDG_Draw.h"
#include "../system/IDG_Hitbox.h"
#include "../system/IDG_Util.h"
#include "../system/IDG_Map2D.h"
#include "../system/IDG_Quadtree.h"
#include "../system/IDG_Effect.h"
#include "../system/IDG_Sound.h"
#include "../system/IDG_Util.h"

#include "bullets.h"

extern app_t   app;
extern stage_t stage;

static void check_collisions        (bullet_t *b);
static void check_world_collisions  (bullet_t *b);
static void check_entity_collisions (bullet_t *b);
static void check_aoe_collisions    (bullet_t *b);

static bullet_t *spawn_bullet       (entity_t *owner, int type);

static void fire_pistol             (entity_t *owner, int type);
static void fire_shotgun            (entity_t *owner, int type);
static void fire_rocket             (entity_t *owner, int type);
static void fire_bfg                (entity_t *owner, int type);

// pistol
static atlas_image_t *wpn_pistol_prj;

// bfg
static atlas_image_t *wpn_bfg_prj[WPN_BFG_PRJ_ANIM_FRAMES];

void init_bullets(void)
{
    memset(&stage.bullet_head, 0, sizeof(bullet_t));
    stage.bullet_tail = &stage.bullet_head;

    if(wpn_pistol_prj == NULL)
    {
        char filename[MAX_FILENAME_LENGTH];

        // pistol
        wpn_pistol_prj = IDG_GetAtlasImage ("data/gfx/effects/plasma/tile1.png",   1);
        
        // bfg
        for(int i=0; i<WPN_BFG_PRJ_ANIM_FRAMES; i++)
        {
            sprintf(filename, "data/gfx/effects/bfg_prj/tile%d.png", (i+1));
            wpn_bfg_prj[i] = IDG_GetAtlasImage(filename, 1);
        }
    }
}

static bullet_t *spawn_bullet(entity_t *owner, int type)
{
    app.dev.entity_count++;

    bullet_t *b;
    b = malloc(sizeof(bullet_t));
    memset(b, 0, sizeof(bullet_t));
    stage.bullet_tail->next = b;
    stage.bullet_tail       = b;
    b->owner                = owner;
    b->type                 = type;
    b->x                    = owner->x;
    b->y                    = owner->y;
    return b;
}

void fire_weapon(entity_t *owner, int type)
{
    // TODO - this will cause issues later, when enemies fire projectiles.
    //        enemies (currently) dont have a gunner dependency.
    gunner_t *g;
    g = (gunner_t *)owner->data;

    // TODO - something could prob go wrong here w/ empty default block
    //        make sure all mem gets cleaned up and edgecases are handled
    switch(type)
    {
    case WPN_PISTOL:
        fire_pistol(owner, type);
        g->reload = WPN_PISTOL_BASE_RELOAD_SPD;
        break;
    case WPN_SHOTGUN:
        fire_shotgun(owner, type);
        break;
    // case WPN_ROCKET:
    //     fire_rocket(owner, type);
    //     break;
    case WPN_BFG:
        fire_bfg(owner, type);
        g->reload = WPN_BFG_BASE_RELOAD_SPD;
        break;
    // default:
    //     break;
    }
}

void do_bullets(void)
{
    bullet_t *b, *prev;
    prev = &stage.bullet_head;
    
    for(b=stage.bullet_head.next; b!=NULL; b=b->next)
    {
        b->x       += (b->dx*app.delta_time);
        b->y       += (b->dy*app.delta_time);
        b->life    -= app.delta_time;
        // animation_handler_t *ah = IDG_GetAnimationHandler(b);
        // ah->timer  -= app.delta_time;
        
        // IDG_UpdateHitbox(b);
        check_collisions(b);
        
        if(b->life <= 0)
        {
            if(b->type == WPN_BFG) { check_aoe_collisions(b); }

            prev->next = b->next;
            if(b == stage.bullet_tail)
                stage.bullet_tail = prev;
            app.dev.entity_count--;
            free(b);
            b = prev;
        }
        prev = b;
    }
}

void draw_bullets(void)
{
    bullet_t *b;
    entity_t *e;

    for(b=stage.bullet_head.next; b!=NULL; b=b->next)
    {
        IDG_BlitAtlasImage(
            b->texture,
            (b->x-stage.camera.pos.x), (b->y-stage.camera.pos.y),
            0,
            b->dx > 0 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL
        );
        
        // TODO - this belongs elsewhere
        if(b->type == WPN_BFG)
        {
            for(e=stage.entity_head.next; e!=NULL; e=e->next)
            {
                if(e!=stage.player)
                {
                    int x1 = ((b->x-stage.camera.pos.x)+(b->texture->rect.w/2));
                    int y1 = ((b->y-stage.camera.pos.y)+(b->texture->rect.h/2));
                    int x2 = ((e->x-stage.camera.pos.x)+(e->texture->rect.w/2));
                    int y2 = ((e->y-stage.camera.pos.y)+(e->texture->rect.h/2));
                    
                    if((IDG_GetDistance(x1, y1, x2, y2) < WPN_BFG_TRACER_RANGE))
                    {
                        SDL_SetRenderDrawColor(app.renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                        SDL_RenderDrawLine(app.renderer, x1, y1, x2, y2);
                    }
                }
            }
        }
    }
}

static void check_collisions(bullet_t *b)
{
    // check_world_collisions(b);
    if(b->life != 0)
    {
        check_entity_collisions(b);
    }
}

static void check_world_collisions(bullet_t *b)
{

}

static void check_entity_collisions(bullet_t *b)
{
    if(b->type == WPN_BFG) { return; } // don't check entity collisions on BFG projectiles

    entity_t *e, *candidates[MAX_QT_CANDIDATES];
    SDL_Rect  r;
    int       i;

    r.x = b->x;
    r.y = b->y;
    r.w = b->texture->rect.w;
    r.h = b->texture->rect.h;
    IDG_GetAllEntsWithin(b->x, b->y, b->texture->rect.w, b->texture->rect.h, candidates, b->owner);

    // for(i=0; i<MAX_QT_CANDIDATES && e!=NULL; e=candidates[++i])
    // {
    // TODO - FIX THIS!!! FIX THE GODDAMN QUADTREE!!
    for(e=stage.entity_head.next; e!=NULL; e=e->next) {
        app.dev.collision_checks++;
        hitbox_t *hb = IDG_GetHitbox(e);
        if(((e->flags & EF_SOLID) || e->take_damage != NULL) && IDG_RectCollide(&r, &hb->pos))
        {
            if(!e->dead && e->take_damage)
            {
                e->take_damage(e, (b->damage == -1 ? 9999 : b->damage), b->owner);
            }
            b->life = 0;
            return;
        }
    }
}

static void check_aoe_collisions(bullet_t *b)
{
    // TODO - fix and implement quadtree here
    entity_t *e;
    for(e=stage.entity_head.next; e!=NULL; e=e->next)
    {
        app.dev.collision_checks++;
        hitbox_t *hb = IDG_GetHitbox(e);

        int x1   = ((b->x-stage.camera.pos.x)+(b->texture->rect.w/2));
        int y1   = ((b->y-stage.camera.pos.y)+(b->texture->rect.h/2));
        int x2   = ((e->x-stage.camera.pos.x)+(e->texture->rect.w/2));
        int y2   = ((e->y-stage.camera.pos.y)+(e->texture->rect.h/2));
        int dist = IDG_GetDistance(x1, y1, x2, y2);
        
        if(((e->flags & EF_SOLID) || e->take_damage != NULL) && IDG_SphCollide(dist, WPN_BFG_TRACER_RANGE, hb->radius))
        {
            if(!e->dead && e->take_damage)
            {
                e->take_damage(e, (b->damage == -1 ? 9999 : b->damage), b->owner);
            }
        }
    }
    b->life = 0;
    return;
}

void clear_bullets(void)
{
    bullet_t *b;
    while(stage.bullet_head.next != NULL)
    {
        app.dev.entity_count--;
        b = stage.bullet_head.next;
        stage.bullet_head.next = b->next;

        if(b->hitbox != NULL)
            free(b->hitbox);
        free(b);
    }
}

static void fire_pistol(entity_t *owner, int type)
{
    bullet_t *b = spawn_bullet(owner, type);

    b->texture = wpn_pistol_prj;
    b->life    = (FPS*WPN_PISTOL_BASE_LIFE);
    b->damage  = WPN_PISTOL_BASE_DMG;

    b->x       = (b->owner->x+(b->owner->texture->rect.w/2));
    b->y       = (b->owner->y+(b->owner->texture->rect.h/2));

    IDG_GetSlope(app.mouse.x, app.mouse.y, (b->x-stage.camera.pos.x), (b->y-stage.camera.pos.y), &b->dx, &b->dy);
    
    b->dx *= WPN_PISTOL_BASE_PRJ_SPD;
    b->dy *= WPN_PISTOL_BASE_PRJ_SPD;

    IDG_CreateBulletHitbox(b, HB_RECT);
}

static void fire_shotgun(entity_t *owner, int type)
{
    bullet_t *b = spawn_bullet(owner, type);

    b->texture = wpn_pistol_prj;
    b->life    = (FPS*WPN_SHOTGUN_BASE_LIFE);
    b->damage  = WPN_SHOTGUN_BASE_DMG;

    IDG_CreateBulletHitbox(b, HB_RECT);
}

// static void fire_rocket(entity_t *owner, int type)
// {
//     // IDG_CreateBulletHitbox(b, HB_RECT);
// }

static void fire_bfg(entity_t *owner, int type)
{
    bullet_t *b = spawn_bullet(owner, type);

    b->texture = wpn_bfg_prj[0];
    b->life    = (FPS*WPN_BFG_BASE_LIFE);
    b->damage  = WPN_BFG_BASE_DMG;

    b->x       = (b->owner->x+(b->owner->texture->rect.w/2));
    b->y       = (b->owner->y+(b->owner->texture->rect.h/2));

    IDG_GetSlope(app.mouse.x, app.mouse.y, (b->x-stage.camera.pos.x), (b->y-stage.camera.pos.y), &b->dx, &b->dy);
    
    b->dx *= WPN_BFG_BASE_PRJ_SPD;
    b->dy *= WPN_BFG_BASE_PRJ_SPD;

    IDG_CreateBulletHitbox(b, HB_RECT);
}