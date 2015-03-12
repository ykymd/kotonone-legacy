#include "SceneController.h"

#include <squirrel.h>

// ジェネレータ
SceneController* SceneController::getInstance(void)
{
	static SceneController Instance;
	return &Instance;
}

SceneController::SceneController(void)
	:g_SceneName(_SC(""))
{
}


SceneController::~SceneController()
{
}

// 現在のシーン
const tstring SceneController::currentScene(void)
{
	return g_SceneName;
}

// シーン遷移
void SceneController::pushScene(const char* scene_name)
{
	g_SceneName = scene_name;
}
