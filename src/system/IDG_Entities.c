#include "../IDG_Common.h"

#include "../system/IDG_Draw.h"
#include "../system/IDG_Hitbox.h"
#include "../system/IDG_Map2D.h"
#include "../system/IDG_Util.h"
#include "../system/IDG_Quadtree.h"

#include "IDG_Entities.h"

extern app_t   app;
extern stage_t stage;

static entity_t dead_head, *dead_tail;

static void move               (entity_t *e);
static void move_to_world_x    (entity_t *e); // for maptile collision
static void move_to_world_y    (entity_t *e); // for maptile collision
static void move_to_sector2D_x (entity_t *e); // can these be consolidated?
static void move_to_sector2D_y (entity_t *e); // ^
static void move_to_entities   (entity_t *e, double dx, double dy);
static void update_hitbox      (entity_t *e, hitbox_t *hb);

void IDG_InitEntities(void)
{
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Initializing Entities ...");

	memset(&stage.entity_head, 0, sizeof(entity_t));
	stage.entity_tail = &stage.entity_head;

	memset(&dead_head, 0, sizeof(entity_t));
	dead_tail = &dead_head;
}

void IDG_DoEntities(void)
{
	entity_t *e, *prev;
	prev = &stage.entity_head;

	for (e=stage.entity_head.next; e!=NULL; e=e->next)
	{
		// remove the node first-
		// this ensures that, if the entity moves,
		// it is re-added to the tree in the correct node
		IDG_RemoveFromQuadtree(e, &stage.quadtree);
		e->is_on_ground = 0;

		if(!(e->flags & EF_WEIGHTLESS))
			e->dy = MIN(e->dy+(GRAVITY*app.delta_time), MAX_FALL_SPEED);
		
		move          (e);
		// update_hitbox (e, hb);
		e->tick       (e);
		
		if(!e->dead)
		{
			IDG_AddToQuadtree(e, &stage.quadtree);
		}
		else
		{
			prev->next = e->next;
			e->next    = NULL;

			if(e == stage.entity_tail)
				stage.entity_tail = prev;
			
			if(e != stage.player)
			{
				dead_tail->next = e;
				dead_tail       = e;
			}
			e = prev;
		}
		prev = e;
	}
}

static void move(entity_t *e)
{
	e->dx = (e->dx*e->friction); // friction prevents idle animations from playing
	e->dy = (e->dy*e->friction); // friction prevents idle animations from playing

	if (e->dx != 0)
	{
		e->x += (e->dx*app.delta_time);

		move_to_sector2D_x(e);
		move_to_entities(e, e->dx, 0);
	}
	
	if (e->dy != 0)
	{
		e->y += (e->dy*app.delta_time);
		
		move_to_sector2D_y(e);
		move_to_entities(e, 0, e->dy);
	}

	e->x = MIN(MAX(e->x, 0), (MAP_WIDTH*MAP_TILE_SIZE)-e->texture->rect.w);
	e->y = MIN(MAX(e->y, -e->texture->rect.h), (MAP_WIDTH*MAP_TILE_SIZE)-e->texture->rect.h);
}

static void move_to_world_x(entity_t *e)
{
	int mx, my, adj;

	mx  = e->dx > 0 ? (e->x+e->texture->rect.w) : e->x;
	mx /= MAP_TILE_SIZE;

	for(my=(e->y/MAP_TILE_SIZE); my<=(e->y+e->texture->rect.h-1)/MAP_TILE_SIZE; my++)
	{
		if (IDG_IsInsideMap2D(mx, my) && stage.map[mx][my] == 0) // != 0 for sidescrollers
		{
			adj  = e->dx > 0 ? -(e->texture->rect.w) : MAP_TILE_SIZE; 
			e->x = (mx * MAP_TILE_SIZE) + adj;

			if (e->flags & EF_BOUNCES) { e->dx = -(e->dx * 0.75); } 
			else     				   { e->dx = 0; }

			return;
		}
	}
}

static void move_to_world_y(entity_t *e)
{
	int mx, my, adj;

	my  = e->dy > 0 ? (e->y + e->texture->rect.h) : e->y;
	my /= MAP_TILE_SIZE;

	for(mx=e->x/MAP_TILE_SIZE; mx<=(e->x+e->texture->rect.w-1)/MAP_TILE_SIZE; mx++)
	{
		if (IDG_IsInsideMap2D(mx, my) && stage.map[mx][my] == 0) // != 0 for sidescrollers
		{
			adj = e->dy > 0 ? -(e->texture->rect.h) : MAP_TILE_SIZE;
			e->y = (my * MAP_TILE_SIZE) + adj;
			e->is_on_ground = e->dy > 0;

			if (e->flags & EF_BOUNCES) { e->dy = -(e->dy * 0.75); }
			else 					   { e->dy = 0; }

			return;
		}
	}
}

static void move_to_sector2D_x(entity_t *e)
{
	sector_2D_t *s;
	int mx, my, adj;

	mx  = e->dx > 0 ? (e->x+e->texture->rect.w) : e->x;
	mx /= MAP_TILE_SIZE;

	for(my=(e->y/MAP_TILE_SIZE); my<=(e->y+e->texture->rect.h-1)/MAP_TILE_SIZE; my++)
	{
		for(s=stage.sector2D_head.next; s!=NULL; s=s->next)
		{
			if(s->solid && IDG_Collision((e->x-stage.camera.pos.x), (e->y-stage.camera.pos.y), e->texture->rect.w, e->texture->rect.h, (s->x-stage.camera.pos.x), (s->y-stage.camera.pos.y), s->w, s->h))
			{
				app.dev.collision_checks += 1;
				adj  = e->dx > 0 ? -(e->texture->rect.w) : MAP_TILE_SIZE; 
				e->x = (mx * MAP_TILE_SIZE) + adj;
	
				if (e->flags & EF_BOUNCES) { e->dx = -(e->dx * 0.75); } 
				else     				   { e->dx = 0; }
	
				return;
			}
		}
	}
}

static void move_to_sector2D_y(entity_t *e)
{
	sector_2D_t *s;
	int mx, my, adj;

	my  = e->dy > 0 ? (e->y + e->texture->rect.h) : e->y;
	my /= MAP_TILE_SIZE;

	for(mx=(e->x/MAP_TILE_SIZE); mx<=(e->x+e->texture->rect.w-1)/MAP_TILE_SIZE; mx++)
	{
		for(s=stage.sector2D_head.next; s!=NULL; s=s->next)
		{
			if(s->solid && IDG_Collision((e->x-stage.camera.pos.x), (e->y-stage.camera.pos.y), e->texture->rect.w, e->texture->rect.h, (s->x-stage.camera.pos.x), (s->y-stage.camera.pos.y), s->w, s->h))
			{
				app.dev.collision_checks += 1;
				adj = e->dy > 0 ? -(e->texture->rect.h) : MAP_TILE_SIZE;
				e->y = (my * MAP_TILE_SIZE) + adj;
				e->is_on_ground = e->dy > 0;

				if (e->flags & EF_BOUNCES) { e->dy = -(e->dy * 0.75); }
				else 					   { e->dy = 0; }

				return;
			}
		}
	}
}

static void move_to_entities(entity_t *e, double dx, double dy)
{
	entity_t *other, *candidates[MAX_QT_CANDIDATES];
	int i, adj, y;

	y = (dy > 0 && dy < 1) ? 1 : 0;

	IDG_GetAllEntsWithin(e->x, e->y, e->texture->rect.w, e->texture->rect.h, candidates, e);

	for(i=0, other=candidates[0]; i<MAX_QT_CANDIDATES && other!=NULL; other=candidates[++i])
	{
		app.dev.collision_checks++;

		hitbox_t *hb1 = IDG_GetHitbox(e);
		hitbox_t *hb2 = IDG_GetHitbox(other);
		int dist = IDG_GetDistance(hb1->pos.x, hb1->pos.y, hb2->pos.x, hb2->pos.y);
		// if(!e->dead && !other->dead && hb1!=NULL && hb2!=NULL && IDG_Collision((hb1->pos.x), (hb1->pos.y+y), hb1->pos.w, hb1->pos.h, hb2->pos.x, hb2->pos.y, hb2->pos.w, hb2->pos.h))
		if(!e->dead && !other->dead && hb1!=NULL && hb2!=NULL && IDG_SphCollide(dist, hb1->radius, hb2->radius))
		{
			if(other->flags & EF_SOLID)
			{
				if(dy != 0)
				{
					adj = dy > 0 ? -e->texture->rect.h : other->texture->rect.h;
					e->y = (other->y+adj);

					if(e->dy > 0)
						e->is_on_ground = 1;
					e->dy = 0;
				}

				if(dx != 0)
				{
					adj = dx > 0 ? -e->texture->rect.w : other->texture->rect.w;
					e->x = (other->x+adj);
					e->dx = 0;
				}
			}

			if(e->touch != NULL)
				e->touch(e, other);
			if(other->flags & EF_INTERACTIVE && other->touch != NULL)
				other->touch(other, e);
		}
	}
}

void IDG_DrawEntities(int layer)
{
	entity_t *e, *candidates[MAX_QT_CANDIDATES];
	int i;

	IDG_GetAllEntsWithin(stage.camera.pos.x, stage.camera.pos.y, SCREEN_WIDTH, SCREEN_HEIGHT, candidates, NULL);

	for(i=0, e=candidates[0]; i<MAX_QT_CANDIDATES && e!=NULL; e=candidates[++i])
	{
		if(e->layer == layer)     { e->draw(e); }
		if(app.dev.show_hitboxes) { IDG_DrawHitbox(e); }
	}
}

void IDG_ClearEntities(void)
{
	entity_t *e;

	while(stage.entity_head.next != NULL)
	{
		e = stage.entity_head.next;
		stage.entity_head.next = e->next;

		if(e != stage.player)
		{
			if(e->data != NULL)
				free(e->data);
			if(e->hitbox != NULL) // NEW - IMPORT TO ENGINE
				free(e->hitbox);
			if(e->animation_handler != NULL) // NEW - IMPORT TO ENGINE
				free(e->animation_handler);
			free(e);
		}
	}

	while(dead_head.next != NULL)
	{
		e = dead_head.next;
		dead_head.next = e->next;

		if(e->data != NULL)
			free(e->data);
		if(e->hitbox != NULL) // NEW - IMPORT TO ENGINE
			free(e->hitbox);
		if(e->animation_handler != NULL) // NEW - IMPORT TO ENGINE
			free(e->animation_handler);
		free(e);
	}
}

// NEW - IMPORT TO ENGINE
// TODO - maybe better off in util.c
int IDG_EntityIsMoving(entity_t *self)
{
	return (self->dx != 0 || self->dy != 0) ? 1 : 0;
}