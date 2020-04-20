#pragma once

#include <prism/actorhandler.h>
#include <prism/mugenanimationhandler.h>

ActorBlueprint getScrollingBackground();

void setScrollingBackground(double tX);
void moveScrollingBackground(double tDelta);

MugenSpriteFile* getRunningSprites();
MugenAnimations* getRunningAnimations();

void resetBGPosition();