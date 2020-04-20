#include "scrollingbackground.h"

#include <prism/blitz.h>

#include "gamescreen.h"
#include "runningman.h"
#include "storyscreen.h"

static struct {
	double mPosX;

} gScrollingData;

static void outroCB(void*) {
	setCurrentStoryDefinitionFile("OUTRO");
	setNewScreen(getStoryScreen());
}

class ScrollingBackground {
public:
	MugenSpriteFile mRunningSprites;
	MugenAnimations mRunningAnimations;
	
	MugenAnimationHandlerElement* mBG;


	static const auto mZ = 1;

	std::vector<MugenAnimationHandlerElement*> mTrackTiles;
	void loadTracks() {
		static const auto TRACK_AMOUNT = 15;
		for (int i = 0; i < 15; i++) {
			auto e = addMugenAnimation(getMugenAnimation(&mRunningAnimations, 11), &mRunningSprites, makePosition(0 + 32 * 4 * i, 240, 2));
			setMugenAnimationBaseDrawScale(e, 4);
			mTrackTiles.push_back(e);
		}
	}

	ScrollingBackground() {
		mRunningSprites = loadMugenSpriteFileWithoutPalette("running/RUNNING.sff");
		mRunningAnimations = loadMugenAnimationFile("running/RUNNING.air");
		mBG = addMugenAnimation(getMugenAnimation(&mRunningAnimations, 10), &mRunningSprites, makePosition(gScrollingData.mPosX, 0, mZ));
		setMugenAnimationBaseDrawScale(mBG, 4);
		loadTracks();
	}

	void updateScrollingBGPosition() {
		auto pos = getMugenAnimationPosition(mBG);
		pos.x -= getRunningManVelocityX() * 0.1;
		setMugenAnimationPosition(mBG, pos);
		gScrollingData.mPosX = pos.x;

		if (gScrollingData.mPosX < -2800) {
			addFadeOut(30, outroCB);
		}
	}

	void updateTrack() {
		for (size_t i = 0; i < mTrackTiles.size(); i++) {
			auto pos = getMugenAnimationPosition(mTrackTiles[i]);
			pos.x -= getRunningManVelocityX() * 0.2;
			if (pos.x < -64 * 4) {
				pos.x += 32 * 4 * mTrackTiles.size();
			}
			setMugenAnimationPosition(mTrackTiles[i], pos);
		}
	}

	void update() {
		updateScrollingBGPosition();
		updateTrack();
	}
};

EXPORT_ACTOR_CLASS(ScrollingBackground);

void setScrollingBackground(double tX)
{
	setMugenAnimationPosition(gScrollingBackground->mBG, makePosition(tX, 0, gScrollingBackground->mZ));
}

void moveScrollingBackground(double tDelta)
{
	const auto pos = getMugenAnimationPosition(gScrollingBackground->mBG);
	setScrollingBackground(pos.x + tDelta);
}

MugenSpriteFile* getRunningSprites()
{
	return &gScrollingBackground->mRunningSprites;
}

MugenAnimations* getRunningAnimations()
{
	return &gScrollingBackground->mRunningAnimations;
}

void resetBGPosition() {
	gScrollingData.mPosX = 0;
}