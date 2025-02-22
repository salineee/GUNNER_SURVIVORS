#ifndef __XP__
#define __XP__

#define XP_SM_BOB        (rand()%FPS);
#define XP_MD_BOB        (rand()%FPS);
#define XP_LG_BOB        (rand()%FPS);

#define XP_SM_AMT         1
#define XP_MD_AMT         10
#define XP_LG_AMT         50

#define XP_SM_ANIM_FRAMES 3
#define XP_MD_ANIM_FRAMES 3
#define XP_LG_ANIM_FRAMES 3

#define XP_SM_ANIM_TIME   15
#define XP_MD_ANIM_TIME   15
#define XP_LG_ANIM_TIME   15

void init_xp_sm (entity_t *e);
void init_xp_md (entity_t *e);
void init_xp_lg (entity_t *e);

#endif // __XP__