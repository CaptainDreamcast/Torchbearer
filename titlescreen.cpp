#include "titlescreen.h"

#include <prism/blitz.h>

#include "gamescreen.h"
#include "storyscreen.h"
#include "bars.h"
#include "scrollingbackground.h"

static struct {
	MugenSpriteFile mSprites;
	MugenSounds mSounds;
} gTitleScreenData;

static void loadTitleScreen() {
	gTitleScreenData.mSprites = loadMugenSpriteFileWithoutPalette("title/TITLE.sff");
	auto id = addMugenAnimation(createOneFrameMugenAnimationForSprite(1, 0), &gTitleScreenData.mSprites, makePosition(0, 0, 1));
	setMugenAnimationBaseDrawScale(id, 2);
	addFadeIn(30, NULL, NULL);
	resetBarValues();
	resetBGPosition();
	setWrapperTitleScreen(getTitleScreen());
	streamMusicFile("tracks/3.ogg");
}


static void gotoGameScreen(void* tCaller) {
	(void)tCaller;
	setNewScreen(getGameScreen());
}

static void updateTitleScreen() {

	if (hasPressedAFlank() || hasPressedStartFlank()) {
		addFadeOut(30, gotoGameScreen, NULL);
	}
}

static Screen gTitleScreen;

Screen* getTitleScreen() {
	gTitleScreen = makeScreen(loadTitleScreen, updateTitleScreen);
	return &gTitleScreen;
};
