#ifndef __IDG_CAMERA__
#define __IDG_CAMERA__

void IDG_InitCamera    (void);
void IDG_DoCamera      (void);
void IDG_DoCameraShake (double life, int shake_amt); // NEW - IMPORT TO ENGINE
                                                  // TODO - prob belongs in effects, yea?

#endif // __IDG_CAMERA__