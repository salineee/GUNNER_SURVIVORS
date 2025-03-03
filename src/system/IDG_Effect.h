#ifndef __IDG_EFFECT__
#define __IDG_EFFECT__

void IDG_InitEffects        (void);
void IDG_DoEffects          (void);
void IDG_DrawEffects        (void);
void IDG_ClearEffects       (void);

// add custom effects below this line
void add_attack_hit_effect  (int start_x, int start_y, int amt);
void add_bfg_impact_effect  (int x, int y);

#endif // __IDG_EFFECT__