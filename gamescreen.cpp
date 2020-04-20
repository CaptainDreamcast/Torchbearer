#include "gamescreen.h"

#include <memory>

#include <prism/blitz.h>

#include "bars.h"
#include "scrollingbackground.h"
#include "obstaclehandler.h"
#include "runningman.h"
#include "collision.h"

class GameScreen {
public:
	GameScreen()
	{
		initCollisions();
		instantiateActor(getScrollingBackground());
		instantiateActor(getRunningMan());
		instantiateActor(getObstacleHandler());
		instantiateActor(getBars());
		setEnergyDepleting();
		streamMusicFile("tracks/4.ogg");
	}

	void update() {}
};

EXPORT_SCREEN_CLASS(GameScreen);