#include "SceneController.h"

// ジェネレータ
SceneController* SceneController::getInstance(void)
{
	static SceneController Instance;
	return &Instance;
}

SceneController::SceneController(void)
{
	sceneStack.push(Scene("root"));
}


SceneController::~SceneController()
{
}

// 現在のシーン
Scene& SceneController::getCurrentScene(void)
{
	if (!sceneStack.empty())
	{
		return sceneStack.top();
	}
	else{
		return Scene("root");
	}
}

// シーン遷移
void SceneController::pushScene(Scene& nextScene)
{
	sceneStack.push(nextScene);
}
