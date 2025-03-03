#include "../IDG_Common.h"

#include "../system/IDG_Draw.h"
#include "../system/IDG_Hitbox.h"
#include "../system/IDG_Util.h"
#include "../system/IDG_Map2D.h"
#include "../system/IDG_Quadtree.h"
#include "../system/IDG_Effect.h"
#include "../system/IDG_Sound.h"

#include "bullets.h"

extern app_t   app;
extern stage_t stage;

static void check_collisions        (bullet_t *b);
static void check_world_collisions  (bullet_t *b);
static void check_entity_collisions (bullet_t *b);

void init_bullets(void)
{
    memset(&stage.bullet_head, 0, sizeof(bullet_t));
    stage.bullet_tail = &stage.bullet_head;
}

bullet_t *spawn_bullet(entity_t *owner)
{
    app.dev.entity_count++;

    bullet_t *b;
    b = malloc(sizeof(bullet_t));
    memset(b, 0, sizeof(bullet_t));
    stage.bullet_tail->next = b;
    stage.bullet_tail       = b;
    b->owner                = owner;
    return b;
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

        // IDG_UpdateHitbox(b);
        check_collisions(b);

        if(b->life <= 0)
        {
            prev->next = b->next;
            if(b == stage.bullet_tail)
            {
                stage.bullet_tail = prev;
            }
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
        if(b->type_flag & PU_BFG)
        {
            for(e=stage.entity_head.next; e!=NULL; e=e->next)
            {
                if(e!=stage.player)
                {
                    int x1 = ((b->x-stage.camera.pos.x)+(b->texture->rect.w/2));
                    int y1 = ((b->y-stage.camera.pos.y)+(b->texture->rect.h/2));
                    int x2 = ((e->x-stage.camera.pos.x)+(e->texture->rect.w/2));
                    int y2 = ((e->y-stage.camera.pos.y)+(e->texture->rect.h/2));
                    SDL_SetRenderDrawColor(app.renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                    SDL_RenderDrawLine(app.renderer, x1, y1, x2, y2);
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
    if(b->type_flag == PU_BFG) { return; } // don't check entity collisions on BFG projectiles

    entity_t *e, *candidates[MAX_QT_CANDIDATES];
    SDL_Rect  r;
    int       i;

    r.x = b->x;
    r.y = b->y;
    r.w = b->texture->rect.w;
    r.h = b->texture->rect.h;
    // printf("BX: %d, Y: %d, W: %d, H: %d, DAMAGE: %d\n", b->hitbox.x, b->hitbox.y, b->hitbox.w, b->hitbox.h, b->damage);
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
                e->take_damage(e, b->damage, b->owner);
            }
            b->life = 0;
            return;
        }
    }
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