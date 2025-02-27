#include "../IDG_Common.h"

#include "../system/IDG_Atlas.h"
#include "../system/IDG_Draw.h"
#include "../system/IDG_Effect.h"
#include "../system/IDG_EntityFactory.h"
#include "../system/IDG_Hitbox.h"
#include "../system/IDG_Sound.h"
#include "../system/IDG_Util.h"
#include "../entities/xp.h"
// #include "../game/ai.h"

#include "goblin.h"

extern app_t   app;
extern stage_t stage;

static int            goblin_init = 0;
static atlas_image_t *goblin_textures[G_ANIM_FRAMES];

static void tick        (entity_t *self);
static void touch       (entity_t *self, entity_t *other);
static void draw        (entity_t *self);
static void take_damage (entity_t *self, int amount, entity_t *attacker);

void init_goblin(entity_t *e)
{
    app.dev.entity_count++;

    if(!goblin_init)
    {
        char filename[MAX_FILENAME_LENGTH];
        for(int i=0; i<G_ANIM_FRAMES; i++)
        {
            sprintf(filename, "data/gfx/enemies/goblin/tile%d.png", (i+1));
            goblin_textures[i] = IDG_GetAtlasImage(filename, i);
        }
        goblin_init = 1;
    }

    animation_handler_t *ah;
    ah = malloc(sizeof(animation_handler_t));
    memset(ah, 0, sizeof(animation_handler_t));

    enemy_t *em;
    em = malloc(sizeof(enemy_t));
    memset(em, 0, sizeof(enemy_t));

    ah->timer            = G_ANIM_TIME;
    em->life             = G_BASE_LIFE;
    em->think_time       = G_THINKTIME;

    e->texture           = goblin_textures[0];
    e->friction          = G_FRICTION;
    // e->flags             = EF_WEIGHTLESS+EF_SOLID;
    e->flags = EF_WEIGHTLESS;
    
    e->animation_handler = ah;
    e->data              = em;
    
    e->tick              = tick;
    e->touch             = touch;
    e->draw              = draw;
    e->take_damage       = take_damage;

    IDG_CreateHitbox(e, HB_SPH);
}

static void tick(entity_t *self)
{
    animation_handler_t *ah;
    ah = (animation_handler_t *)self->animation_handler;

    enemy_t *em;
    em = (enemy_t *)self->data;

    em->damage_timer = MAX((em->damage_timer-app.delta_time), 0);
    em->think_time   = MAX((em->think_time-app.delta_time), 0);

    ah->timer -= app.delta_time;
    if(ah->timer <= 0)
    {
        ah->frame = (ah->frame+1)%G_ANIM_FRAMES;
        ah->timer = G_ANIM_TIME;
    }

    if(stage.player != NULL)
    {
        // IDG_GetSlope(stage.player->x, stage.player->y, self->x, self->y, &self->dx, &self->dy);
    }
    
    self->texture = goblin_textures[ah->frame];
    IDG_UpdateHitbox(self);
}

static void draw(entity_t *self)
{
    enemy_t *em;
    em = (enemy_t *)self->data;

    int x = (self->x-stage.camera.pos.x);
    int y = (self->y-stage.camera.pos.y);

    if(em->damage_timer == 0)
    {
        IDG_BlitAtlasImage(
            self->texture,
            x, y,
            0,
            SDL_FLIP_NONE
        );
    }
    else
    {
        SDL_SetTextureColorMod(self->texture->texture, 255, 32, 32);
        IDG_BlitAtlasImage(
            self->texture,
            x, y,
            0,
            SDL_FLIP_NONE
        );
        SDL_SetTextureColorMod(self->texture->texture, 0xFF, 0xFF, 0xFF);
    }
}

static void touch(entity_t *self, entity_t *other)
{
    if(other == stage.player)
    {
        printf("'%s' TOUCHING PLAYER!!!\n", self->type_name);
        // other->take_damage(other, 1, self);
    }
}

static void take_damage(entity_t *self, int amt, entity_t *attacker)
{
    enemy_t *em;
    
    if(attacker == stage.player)
    {
        em = (enemy_t *)self->data;
        em->life        -= amt;
        em->damage_timer = 4;
        em->think_time   = (FPS/4);

        if(em->life <= 0)
        {
            entity_t *e = IDG_SpawnEntity();
            e->x = self->x+(self->texture->rect.w/2);
            e->y = self->y+(self->texture->rect.h/2);
            init_xp_sm(e);
            self->dead = 1;
        }

        add_attack_hit_effect((self->x+self->texture->rect.w), self->y, amt);
    }
}