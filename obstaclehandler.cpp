#include "obstaclehandler.h"

#include <queue>
#include <prism/blitz.h>

#include "runningman.h"
#include "gamescreen.h"
#include "collision.h"
#include "scrollingbackground.h"

static void obstacleCollisionCB(void* tCaller, void* tCollisionData, int tCollisionList);

class ObstacleHandler {
public:
	std::list<MugenAnimationHandlerElement*> mObstacles;

	ObstacleHandler() {
		for (int i = 0; i < 10; i++)
		{
			auto e = addMugenAnimation(getMugenAnimation(getRunningAnimations(), 1000), getRunningSprites(), makePosition(360 + i * 300, 240, 5));
			setMugenAnimationBaseDrawScale(e, 4);
			setMugenAnimationAttackCollisionActive(e, getObstacleCollisionList(), obstacleCollisionCB, e, NULL);
			mObstacles.push_back(e);
		}
	}

	void updateObstaclePositions() {
		for (auto& e : mObstacles)
		{
			const auto pos = getMugenAnimationPosition(e);
			setMugenAnimationPosition(e, pos - makePosition(getRunningManVelocityX() * 0.2, 0, 0));
		}
	}

	void removeOldObstacles() {
		while (getMugenAnimationPosition(mObstacles.front()).x < -100) {
			removeMugenAnimation(mObstacles.front());
			mObstacles.pop_front();
		}
	}

	void addNewObstacles() {
		for (int i = 0; i < 100; i++) {
			if (mObstacles.size() >= 10) break;
			const auto pos = makePosition(getMugenAnimationPosition(mObstacles.back()).x + randfrom(200, 400), 240, 5);
			auto e = addMugenAnimation(getMugenAnimation(getRunningAnimations(), 1000), getRunningSprites(), pos);
			setMugenAnimationBaseDrawScale(e, 4);
			setMugenAnimationAttackCollisionActive(e, getObstacleCollisionList(), obstacleCollisionCB, e, NULL);
			mObstacles.push_back(e);
		}
	}

	void update() {
		updateObstaclePositions();
		removeOldObstacles();
		addNewObstacles();
	}

};

EXPORT_ACTOR_CLASS(ObstacleHandler);

static void obstacleCollisionCB(void* tCaller, void*, int) {
	MugenAnimationHandlerElement* e = (MugenAnimationHandlerElement*)tCaller;
	changeMugenAnimation(e, getMugenAnimation(getRunningAnimations(), -1));
}