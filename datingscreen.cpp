#include "datingscreen.h"

#include <prism/blitz.h>

#include "storyhandler.h"
#include "bars.h"

static struct {
	MugenSpriteFile mSprites;
	MugenAnimations mAnimations;
} gDatingScreenData;

class DatingScreen {
public:


	DatingScreen() {
		gDatingScreenData.mSprites = loadMugenSpriteFileWithoutPalette("dating/DATING.sff");
		gDatingScreenData.mAnimations = loadMugenAnimationFile("dating/DATING.air");

		instantiateActor(getBars());
		instantiateActor(getStoryHandler());		
		streamMusicFile("tracks/3.ogg");
	}

	void update() {}
};

EXPORT_SCREEN_CLASS(DatingScreen);

MugenSpriteFile * getDatingSprites()
{
	return &gDatingScreenData.mSprites;
}

MugenAnimations * getDatingAnimations()
{
	return &gDatingScreenData.mAnimations;
}


static int gCurrentFile = 0;

void startDating()
{
	gCurrentFile++;
	std::stringstream ss;
	ss << "story/STORY" << gCurrentFile << ".def";
	if (isFile(ss.str())) {
		setStoryHandlerPath(ss.str());
	}
	else {
		while (true) {
			int i = randfromInteger(1, 10);
			ss = std::stringstream();
			ss << "story/STORY" << i << ".def";
			if (isFile(ss.str())) {
				setStoryHandlerPath(ss.str());
				break;
			}
		}
	}

	setNewScreen(getDatingScreen());
}
