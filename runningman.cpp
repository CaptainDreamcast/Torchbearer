#include "runningman.h"

#include <prism/blitz.h>

#include "gamescreen.h"
#include "collision.h"
#include "scrollingbackground.h"

static void playerHitCB(void* tCaller, void* tCollisionData);

class RunningMan {
public:
	int mEntity;
	static constexpr auto mFloorY = 230.0;
	int mDeadNow = 0;
	int mIsDead = 0;

	RunningMan()
	{
		mEntity = addBlitzEntity(makePosition(80, mFloorY, 10));
		addBlitzMugenAnimationComponent(mEntity, getRunningSprites(), getRunningAnimations(), 120);
		setBlitzMugenAnimationBaseDrawScale(mEntity, 4);
		int collisionID = addBlitzCollisionPassiveMugen(mEntity, getPlayerCollisionList());
		addBlitzCollisionCB(mEntity, collisionID, playerHitCB, NULL);
		addBlitzPhysicsComponent(mEntity);
		setBlitzPhysicsGravity(mEntity, makePosition(0, 0, 0));
	}


	int mIsJumping = 0;
	void flap() {
		if (!mIsJumping) return;

		addBlitzPhysicsImpulse(mEntity, makePosition(0, -0.3, 0));
	}
	
	void jump()
	{
		if (mIsJumping) return;

		addBlitzPhysicsImpulse(mEntity, makePosition(0, -5, 0));
		changeBlitzMugenAnimation(mEntity, 140);
		setBlitzPhysicsGravity(mEntity, makePosition(0, 0.1, 0));
		mIsJumping = 1;
	}

	void updateInput() {
		if (mIsDead) return;

		if (hasPressedAFlank()) {
			flap();
			jump();
		}
	}

	void updateLanding() {
		if (!mIsJumping) return;

		auto pos = getBlitzEntityPositionReference(mEntity);
		if (pos->y > mFloorY) {
			pos->y = mFloorY;
			if (!mIsDead) {
				changeBlitzMugenAnimation(mEntity, 120);
			}
			setBlitzPhysicsVelocity(mEntity, makePosition(0, 0, 0));
			setBlitzPhysicsGravity(mEntity, makePosition(0, 0, 0));
			mIsJumping = 0;
		}
	}

	void updateDeath() {
		if (!mIsDead) return;
		mDeadNow++;
		if (mDeadNow >= 120) {
			changeBlitzMugenAnimation(mEntity, 120);
			mIsDead = 0;
		}
	}

	void update() {
		updateInput();
		updateLanding();
		updateDeath();
	}

};

EXPORT_ACTOR_CLASS(RunningMan);

double getRunningManVelocityX()
{
	return gRunningMan->mIsDead ? 0.0 : 10.0;
}

static void playerHitCB(void*, void*) {
	changeBlitzMugenAnimationIfDifferent(gRunningMan->mEntity, 150);
	gRunningMan->mDeadNow = 0;
	gRunningMan->mIsDead = 1;
}