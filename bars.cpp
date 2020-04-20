#include "bars.h"

#include <prism/blitz.h>

#include "datingscreen.h"
#include "storyscreen.h"

static struct {
	int mRomanceValue;
	int mEnergyValue;

} gBarsData;

static void datingCB(void*) {
	startDating();
}

class Bars {
public:
	class SingleBar {
	public:
		Bars& mBars;
		MugenAnimationHandlerElement* mBG;
		MugenAnimationHandlerElement* mFG;
		int mBGAnimationNo;
		int& mValue;

		void setBarPercentage() {
			const auto t = mValue / 1000.0;
			const auto spriteWidth = getAnimationFirstElementSpriteSize(getMugenAnimation(&mBars.mAnimations, mBGAnimationNo), &mBars.mSprites);
			setMugenAnimationRectangleWidth(mBG, int(spriteWidth.x * t));
		}

		void changeBar(int tValue) {
			mValue += tValue;
			mValue = std::max(0, mValue);
			mValue = std::min(1000, mValue);
			setBarPercentage();
		}

		SingleBar(Bars& tBars, int tBGAnimationNo, int tFGAnimationNo, int& tValue, const Position& tPosition) 
			: mBars(tBars)
			, mValue(tValue)
		{
			mBG = addMugenAnimation(getMugenAnimation(&mBars.mAnimations, tBGAnimationNo), &mBars.mSprites, tPosition);
			setMugenAnimationBaseDrawScale(mBG, 4);
			mFG = addMugenAnimation(getMugenAnimation(&mBars.mAnimations, tFGAnimationNo), &mBars.mSprites, tPosition + makePosition(0, 0, 1));
			setMugenAnimationBaseDrawScale(mFG, 4);
			mBGAnimationNo = tBGAnimationNo;
			setBarPercentage();
		}

	};

	MugenSpriteFile mSprites;
	MugenAnimations mAnimations;
	std::unique_ptr<SingleBar> mRomanceBar = nullptr;
	std::unique_ptr<SingleBar> mEnergyBar = nullptr;


	Bars() {
		mSprites = loadMugenSpriteFileWithoutPalette("ui/UI.sff");
		mAnimations = loadMugenAnimationFile("ui/UI.air");
		mRomanceBar = std::make_unique<SingleBar>(*this, 2000, 2001, gBarsData.mRomanceValue, makePosition(40, 15, 20));
		mEnergyBar = std::make_unique<SingleBar>(*this, 2010, 2011, gBarsData.mEnergyValue, makePosition(120, 15, 20));
	}
	
	int mIsEnergyDepleting = 0;

	void updateEnergyDepletion() {
		if (!mIsEnergyDepleting) return;
		mEnergyBar->changeBar(-1);
		if (!mEnergyBar->mValue) {
			addFadeOut(30, datingCB, NULL);
			mIsEnergyDepleting = 0;
		}
	}
	
	void update() {
		updateEnergyDepletion();
	}

};

EXPORT_ACTOR_CLASS(Bars);

void resetBarValues()
{
	gBarsData.mRomanceValue = 1000;
	gBarsData.mEnergyValue = 1000;
}

MugenSpriteFile * getUISprites()
{
	return &gBars->mSprites;
}

MugenAnimations * getUIAnimations()
{
	return &gBars->mAnimations;
}

void setEnergyDepleting()
{
	gBars->mIsEnergyDepleting = 1;
}

int getEnergyBarValue()
{
	return gBars->mEnergyBar->mValue;
}

int updateEnergyBarTowardsTargetValueAndReturnIfOver(int tTargetValue)
{
	const auto trueTarget = std::min(std::max(0, tTargetValue), 1000);
	const auto current = getEnergyBarValue();
	const auto delta = trueTarget - current;
	if (!delta) return 1;
	const auto sign = delta / std::abs(delta);
	gBars->mEnergyBar->changeBar(sign * 5);
	return getEnergyBarValue() == trueTarget;
}

int getRomanceBarValue()
{
	return gBars->mRomanceBar->mValue;
}

static void gameOverCB(void*) {
	setCurrentStoryDefinitionFile("GAMEOVER");
	setNewScreen(getStoryScreen());
}

int updateRomanceBarTowardsTargetValueAndReturnIfOver(int tTargetValue)
{
	const auto trueTarget = std::min(std::max(0, tTargetValue), 1000);
	const auto current = getRomanceBarValue();
	const auto delta = trueTarget - current;
	const auto sign = delta / std::abs(delta);
	gBars->mRomanceBar->changeBar(sign * 5);
	if (!gBars->mRomanceBar->mValue) {
		gameOverCB(NULL);
	}
	return getRomanceBarValue() == trueTarget;
}
