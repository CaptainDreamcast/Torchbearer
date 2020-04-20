#include "collision.h"

#include <prism/blitz.h>

static struct {
	CollisionListData* mPlayerCollisionList;
	CollisionListData* mObstacleCollisionList;
} gCollisionData;

void initCollisions()
{
	gCollisionData.mPlayerCollisionList = addCollisionListToHandler();
	gCollisionData.mObstacleCollisionList = addCollisionListToHandler();
	addCollisionHandlerCheck(gCollisionData.mPlayerCollisionList, gCollisionData.mObstacleCollisionList);
}

CollisionListData* getPlayerCollisionList()
{
	return gCollisionData.mPlayerCollisionList;
}

CollisionListData* getObstacleCollisionList()
{
	return gCollisionData.mObstacleCollisionList;
}