#pragma once

#include "tstring.h"
#include "Scene.h"
#include <stack>

class SceneController
{
public:
	// ジェネレータ
	static SceneController* getInstance(void);

	// 現在のシーン
	Scene& getCurrentScene(void);

	// シーン遷移
	void pushScene(Scene& nextScene);

	// 前のシーンに戻る
	void popScene();

	void setInitializeFlag(bool flag){ initializeFlag = flag; }
	void setFinalizeFlag(bool flag){ finalizeFlag = flag; }
	void setFirstSceneFlag(bool flag){ firstSceneFlag = flag; }

	bool getInitializeFlag(){ return initializeFlag; }
	bool getFinalizeFlag(){ return finalizeFlag; }
	bool getFirstSceneFlag(){ return firstSceneFlag; }

private:
	// initialize, finalizeを呼び出すかのフラグ
	bool initializeFlag;
	bool finalizeFlag;
	bool firstSceneFlag;

	std::stack<Scene> sceneStack;

	void changeScene();
};

