#pragma once

#include <prism/actorhandler.h>
#include <prism/mugenanimationhandler.h>

ActorBlueprint getBars();

void resetBarValues();

MugenSpriteFile* getUISprites();
MugenAnimations* getUIAnimations();
void setEnergyDepleting();
int getEnergyBarValue();
int updateEnergyBarTowardsTargetValueAndReturnIfOver(int tTargetValue);
int getRomanceBarValue();
int updateRomanceBarTowardsTargetValueAndReturnIfOver(int tTargetValue);