#include "storyhandler.h"

#include <assert.h>
#include <prism/blitz.h>
#include "datingscreen.h"
#include "gamescreen.h"
#include "bars.h"

static struct {
	std::string mStoryPath;
} gStoryHandlerData;

static void loadSingleStoryStepCB(void* tCaller, void* tData);

static void gotoGameCB(void*) {
	setNewScreen(getGameScreen());
}

class StoryHandler {
public:
	class TextBox {
	public:
		MugenAnimationHandlerElement* mBG;
		MugenAnimationHandlerElement* mFG;
		int mNameID;
		int mTextID;

		TextBox() {
			mBG = addMugenAnimation(getMugenAnimation(getDatingAnimations(), 1000), getDatingSprites(), makePosition(160, 160, 20));
			setMugenAnimationVisibility(mBG, 0);
			setMugenAnimationBaseDrawScale(mBG, 3);
			mFG = addMugenAnimation(getMugenAnimation(getDatingAnimations(), 1001), getDatingSprites(), makePosition(160, 160, 19));
			setMugenAnimationVisibility(mFG, 0);
			setMugenAnimationBaseDrawScale(mFG, 3);
			setMugenAnimationBlendType(mFG, BLEND_TYPE_SUBTRACTION);
			mNameID = addMugenTextMugenStyle("", makePosition(90, 185, 21), makeVector3DI(1, 0, 1));
			mTextID = addMugenTextMugenStyle("", makePosition(75, 205, 21), makeVector3DI(1, 0, 1));
			setMugenTextTextBoxWidth(mTextID, 170);
		}

		int updateAndReturnIfOver(StoryHandler& tHandler) {
			if (!getMugenAnimationVisibility(mBG)) return 0;
			if (tHandler.mSelection->mIsActive) return 0;

			if (hasPressedAFlank()) {
				if (!isMugenTextBuiltUp(mTextID)) {
					setMugenTextBuiltUp(mTextID);
				}
				else {
					setMugenAnimationVisibility(mBG, 0);
					setMugenAnimationVisibility(mFG, 0);
					changeMugenText(mNameID, "");
					changeMugenText(mTextID, "");
					return 1;
				}
			}
			return 0;
		}

		void setTextAndName(const std::string& tName, const std::string& tText) {
			setMugenAnimationVisibility(mBG, 1);
			setMugenAnimationVisibility(mFG, 1);
			std::string name;
			if (tName.size() >= 2) {
				auto firstLetter = tName.substr(0, 1);
				turnStringUppercase(firstLetter);
				name = firstLetter + tName.substr(1, std::string::npos);
			}
			else {
				name = tName;
			}
			changeMugenText(mNameID, name.c_str());
			changeMugenText(mTextID, tText.c_str());
			setMugenTextBuildup(mTextID, 1);
		}
	};

	class Selection {
	public:
		class SingleSelection {
		public:
			MugenAnimationHandlerElement* mFG;
			MugenAnimationHandlerElement* mBG;
			int mTextID;
			std::function<void()> mFunc;

			SingleSelection(int i) {
				mFG = addMugenAnimation(getMugenAnimation(getDatingAnimations(), 1101), getDatingSprites(), makePosition(160, 10 + i * 40, 49));
				setMugenAnimationDrawScale(mFG, makePosition(4, 2, 1));
				setMugenAnimationVisibility(mFG, 0);
				setMugenAnimationBlendType(mFG, BLEND_TYPE_SUBTRACTION);
				mBG = addMugenAnimation(getMugenAnimation(getDatingAnimations(), 1100), getDatingSprites(), makePosition(160, 10 + i * 40, 50));
				setMugenAnimationDrawScale(mBG, makePosition(4, 2, 1));
				setMugenAnimationVisibility(mBG, 0);
				mTextID = addMugenTextMugenStyle("", makePosition(44, 36 + i * 40, 51), makeVector3DI(1, 0, 1));
				setMugenTextTextBoxWidth(mTextID, 220);
			}

			void setSelection(const std::string& tText, std::function<void()> tFunc) {
				changeMugenText(mTextID, tText.c_str());
				mFunc = tFunc;
				setMugenAnimationVisibility(mBG, 1);
				setMugenAnimationVisibility(mFG, 1);
			}

			void unsetSelection() {
				changeMugenText(mTextID, "");
				setMugenAnimationVisibility(mBG, 0);
				setMugenAnimationVisibility(mFG, 0);
			}
		};

		int mIsActive;
		std::vector<SingleSelection> mSelections;
		int mCurrentSelection;
		int mActiveSelectionAmount;

		Selection() {
			for (int i = 0; i < 5; i++) {
				mSelections.push_back(SingleSelection(i));
			}
			mIsActive = 0;
		}

		void setSelectionActive(StoryHandler& tHandler, const std::string& tName, const std::string& tText, const std::vector<std::string>& tQuestions, const std::vector<std::function<void()>>& tFuncs) {
			assert(tQuestions.size() == tFuncs.size());
			for (size_t i = 0; i < tQuestions.size(); i++) {
				mSelections[i].setSelection(tQuestions[i], tFuncs[i]);
			}
			mActiveSelectionAmount = tQuestions.size();
			mCurrentSelection = 0;
			setCurrentSelectionActive();

			mIsActive = 1;
			tHandler.mTextBox->setTextAndName(tName, tText);
		}

		void setSelectionInactive() {
			for (int i = 0; i < mActiveSelectionAmount; i++) {
				mSelections[i].unsetSelection();
			}
			mIsActive = 0;
		}

		void callSelectedFunction() {
			mSelections[mCurrentSelection].mFunc();
		}

		void setCurrentSelectionActive() {
			setMugenAnimationColor(mSelections[mCurrentSelection].mBG, 1.0, 0.0, 0.0);
			setMugenTextColor(mSelections[mCurrentSelection].mTextID, COLOR_CYAN);
		}

		void setCurrentSelectionInactive() {
			setMugenAnimationColor(mSelections[mCurrentSelection].mBG, 1.0, 1.0, 1.0);
			setMugenTextColor(mSelections[mCurrentSelection].mTextID, COLOR_WHITE);
		}

		void updateCurrentSelection(int tDelta) {
			setCurrentSelectionInactive();
			mCurrentSelection = (mCurrentSelection + mActiveSelectionAmount + tDelta) % mActiveSelectionAmount;
			setCurrentSelectionActive();
		}

		int updateAndReturnIfOver() {
			if (!mIsActive) return 0;

			if (hasPressedAFlank()) {
				return 1;
			}

			if (hasPressedUpFlank()) {
				updateCurrentSelection(-1);
			} else if (hasPressedDownFlank()) {
				updateCurrentSelection(1);
			}

			return 0;
		}
	};

	std::unique_ptr<TextBox> mTextBox = nullptr;
	std::unique_ptr<Selection> mSelection = nullptr;

	class StoryStep {
	public:
		virtual int loadAndReturnIfOver(StoryHandler& tStoryHandler) = 0;
	};

	class StoryStepText : public StoryStep {
	public:
		std::string mName;
		std::string mText;

		StoryStepText(const std::string& tName, const std::string& tText) 
			: mName(tName)
			, mText(tText)
		{	
		}

		virtual int loadAndReturnIfOver(StoryHandler& tStoryHandler) override {
			tStoryHandler.mTextBox->setTextAndName(mName, mText);
			return 1;
		}
	};

	class StoryStepBar : public StoryStep {
	public:
		std::function<int(int)> mDeltaFunction;
		std::function<int(void)> mGetFunction;
		int mTargetValue;

		StoryStepBar(const std::function<int(int)>& tDeltaFunction, const std::function<int(void)>& tGetFunction, int tTargetValue)
			: mDeltaFunction(tDeltaFunction)
			, mGetFunction(tGetFunction)
			, mTargetValue(tTargetValue)
		{
		}

		virtual int loadAndReturnIfOver(StoryHandler& tHandler) override {
			tHandler.setBarUpdate(mDeltaFunction, mGetFunction, mTargetValue);
			return 1;
		}
	};

	std::vector<std::unique_ptr<StoryStep>> mStorySteps;
	std::vector<std::unique_ptr<StoryStep>> mActionStorySteps;
	std::vector<std::unique_ptr<StoryStep>> mCorrectStorySteps;
	std::vector<std::unique_ptr<StoryStep>> mWrongStorySteps;
	std::vector<std::unique_ptr<StoryStep>>* mActiveSteps;

	int mCurrentStoryStep = -1;

	void loadTextStep(MugenDefScriptGroupElement* e, std::vector<std::unique_ptr<StoryStep>>* tSteps) {
		const auto text = getSTLMugenDefStringVariableAsElement(e);
		tSteps->push_back(std::make_unique<StoryStepText>(e->mName, text));
	}

	void loadBarStep(MugenDefScriptGroupElement* e, const std::function<int()> tGetFunc, const std::function<int(int)> tDeltaFunc, std::vector<std::unique_ptr<StoryStep>>* tSteps) {
		const auto delta = getMugenDefNumberVariableAsElement(e);
		tSteps->push_back(std::make_unique<StoryStepBar>(tDeltaFunc, tGetFunc, delta));
	}

	void loadSingleStoryStepInternalCB(void* tCaller, void* tData) {
		std::vector<std::unique_ptr<StoryStep>>* steps = (std::vector<std::unique_ptr<StoryStep>>*)tCaller;
		MugenDefScriptGroupElement* e = (MugenDefScriptGroupElement*)tData;
		if (e->mName == "romancebar") {
			loadBarStep(e, getRomanceBarValue, updateRomanceBarTowardsTargetValueAndReturnIfOver, steps);
		}
		else if (e->mName == "energybar") {
			loadBarStep(e, getEnergyBarValue, updateEnergyBarTowardsTargetValueAndReturnIfOver, steps);
		}
		else {
			loadTextStep(e, steps);
		}
	}

	void loadStorySteps(MugenDefScript* tScript, const std::string& tGroupName, std::vector<std::unique_ptr<StoryStep>>* tSteps) {
		auto stepsGroup = tScript->mGroups[tGroupName];
		list_map(&stepsGroup.mOrderedElementList, loadSingleStoryStepCB, tSteps);
	}

	class Question {
	public:
		std::string mName;
		std::string mText;
		std::vector<std::string> mAnswers;
		int mCorrectAnswer;

		Question(const std::string& tName, const std::string& tText, const std::vector<std::string>& tAnswers, int tCorrectAnswer)
			: mName(tName)
			, mText(tText)
			, mAnswers(tAnswers)
			, mCorrectAnswer(tCorrectAnswer)
		{
		}
	};

	std::unique_ptr<Question> mPersonalQuestion;
	std::unique_ptr<Question> mRandomQuestion;
	std::unique_ptr<Question> mSelectionQuestion;

	void loadStoryQuestion(MugenDefScript* tScript, std::unique_ptr<Question>& tQuestion, const std::string& tGroupName) {
		auto& group = tScript->mGroups[tGroupName];
		const auto name = getSTLMugenDefStringOrDefaultAsGroup(&group, "name", "DUMMY");
		const auto text = getSTLMugenDefStringOrDefaultAsGroup(&group, "q", "question");
		std::vector<std::string> answers;
		for (int i = 1; i < 5; i++) {
			std::stringstream ss;
			ss << "a" << i;
			if (isMugenDefStringVariableAsGroup(&group, ss.str().c_str())) {
				const auto answer = getSTLMugenDefStringOrDefaultAsGroup(&group, ss.str().c_str(), "answer");
				answers.push_back(answer);
			}
		}
		const auto correctAnswer = getMugenDefIntegerOrDefaultAsGroup(&group, "correct", -1);

		tQuestion = std::make_unique<Question>(name, text, answers, correctAnswer);
	}

	void loadStory() {
		MugenDefScript script;
		loadMugenDefScript(&script, gStoryHandlerData.mStoryPath);
		loadStorySteps(&script, "Steps", &mStorySteps);
		loadStorySteps(&script, "Activity", &mActionStorySteps);
		loadStorySteps(&script, "Correct", &mCorrectStorySteps);
		loadStorySteps(&script, "Wrong", &mWrongStorySteps);
		loadStoryQuestion(&script, mPersonalQuestion, "Relationship");
		loadStoryQuestion(&script, mRandomQuestion, "Random");
		loadStoryQuestion(&script, mSelectionQuestion, "Select");
		unloadMugenDefScript(script);
	}

	void startStorySteps(std::vector<std::unique_ptr<StoryStep>>* tSteps) {
		mCurrentStoryStep = -1;
		mActiveSteps = tSteps;
		setNextStoryStepActive();
	}

	void setCorrectActive() {
		startStorySteps(&mCorrectStorySteps);
	}

	void setWrongActive() {
		startStorySteps(&mWrongStorySteps);
	}

	void setQuestionActive(Question& tQuestion) {
		std::vector<std::function<void()>> funcs;
		for (size_t i = 0; i < tQuestion.mAnswers.size(); i++) {
			funcs.push_back([this, &tQuestion, i]() { 
				if (tQuestion.mCorrectAnswer == i + 1) 
				{ 
					setCorrectActive(); 
				}
				else 
				{ 
					setWrongActive(); 
				} 
			});
		}
		mSelection->setSelectionActive(*this, tQuestion.mName, tQuestion.mText, tQuestion.mAnswers, funcs);
	}

	void setQuestionRelationShipActive() {
		setQuestionActive(*mPersonalQuestion);
	}

	void setQuestionRandomActive() {
		setQuestionActive(*mRandomQuestion);
	}

	void setTaskActive() {
		startStorySteps(&mActionStorySteps);
	}

	void setQuestionSelectionActive() {
		std::vector<std::function<void()>> funcs;
		funcs.push_back([this]() { setQuestionRelationShipActive(); });
		funcs.push_back([this]() { setQuestionRandomActive(); });
		funcs.push_back([this]() { setTaskActive(); });
		mSelection->setSelectionActive(*this, mSelectionQuestion->mName, mSelectionQuestion->mText, mSelectionQuestion->mAnswers, funcs);
	}

	void setNextStoryStepActive() {
		while (true) {
			mCurrentStoryStep++;
			if (mCurrentStoryStep >= int(mActiveSteps->size())) {
				if (mActiveSteps == &mStorySteps) {
					setQuestionSelectionActive();
				}
				else {
					addFadeOut(20, gotoGameCB);
				}
				return;
			}
			auto& currentStep = (*mActiveSteps)[mCurrentStoryStep];
			if (currentStep->loadAndReturnIfOver(*this)) return;
		}
	}

	MugenAnimationHandlerElement* mBG;

	StoryHandler() {
		mBG = addMugenAnimation(getMugenAnimation(getDatingAnimations(), 1), getDatingSprites(), makePosition(0, 0, 1));

		mTextBox = std::make_unique<TextBox>();
		mSelection = std::make_unique<Selection>();

		loadStory();
		mActiveSteps = &mStorySteps;
		setNextStoryStepActive();
	}

	int mIsBarUpdateActive = 0;
	std::function<int(int)> mBarDeltaFunction;
	int mBarTargetValue;
	void setBarUpdate(const std::function<int(int)>& tDeltaFunction, const std::function<int(void)>& tGetFunction, int tTargetValue) {
		mBarDeltaFunction = tDeltaFunction;
		mBarTargetValue = tTargetValue + tGetFunction();
		mIsBarUpdateActive = 1;
	}

	int updateBarAndReturnIfOver() {
		if (!mIsBarUpdateActive) return 0;
		if (mBarDeltaFunction(mBarTargetValue)) {
			mIsBarUpdateActive = 0;
			return 1;
		}
		return 0;
	}

	void update() {
		if (mTextBox->updateAndReturnIfOver(*this) || updateBarAndReturnIfOver()) {
			setNextStoryStepActive();
		}
		else if (mSelection->updateAndReturnIfOver()) {
			mSelection->setSelectionInactive();
			mSelection->callSelectedFunction();
		}
	}
};

EXPORT_ACTOR_CLASS(StoryHandler);

static void loadSingleStoryStepCB(void* tCaller, void* tData) {
	gStoryHandler->loadSingleStoryStepInternalCB(tCaller, tData);
}

void setStoryHandlerPath(const std::string & tPath)
{
	gStoryHandlerData.mStoryPath = tPath;
}
