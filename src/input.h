#ifndef INPUT_H
#define INPUT_H

#include "entity.h"

bool GetJumpInput(void);
float GetHorizontalInput(void);
void ChangeVelocity(Entity* entity, float velocityX, float velocityY);
void ChangeVelocityY(Entity* entity, float velocityY);
void ChangeVelocityX(Entity* entity, float velocityX);

#endif // INPUT_H