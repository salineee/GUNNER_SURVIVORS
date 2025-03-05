#ifndef __BULLETS__
#define __BULLETS__

void init_bullets  (void);
void do_bullets    (void);
void draw_bullets  (void);
void clear_bullets (void);
void fire_weapon   (entity_t *owner, int type);
// bullet_t *spawn_bullet  (entity_t *owner, int type);

#endif // __BULLETS__