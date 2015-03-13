#include "SceneController.h"

// ジェネレータ
SceneController* SceneController::getInstance(void)
{
	static SceneController Instance;
	return &Instance;
}

SceneController::SceneController(void)
{
	currentScene = new Scene("root");
}


SceneController::~SceneController()
{
}

// 現在のシーン
Scene& SceneController::getCurrentScene(void)
{
	return *currentScene;
}

// シーン遷移
void SceneController::pushScene(Scene* nextScene)
{
	currentScene = nextScene;
}
