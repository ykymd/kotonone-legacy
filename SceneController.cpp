#include "SceneController.h"

// ジェネレータ
SceneController* SceneController::getInstance(void)
{
	static SceneController* Instance = nullptr;
	if (Instance == nullptr)
	{
		Instance = new SceneController();
		Instance->setInitializeFlag(false);
		Instance->setFinalizeFlag(false);
		Instance->setFirstSceneFlag(true);
		Instance->sceneStack.push(Scene("root", SceneType::ROOT));
	}
	return Instance;
}

// 現在のシーン
Scene& SceneController::getCurrentScene(void)
{
	if (sceneStack.empty())
	{
		sceneStack.push(Scene("root", SceneType::ROOT));
	}

	return sceneStack.top();
}

void SceneController::changeScene()
{
	// initialize関数を呼び出すフラグを立てる
	initializeFlag = true;

	// finalize関数は、2回目以降のシーン切り替え時にのみ呼び出されるようにする
	if (firstSceneFlag)
	{
		finalizeFlag = false;
		firstSceneFlag = false;
	}
	else
	{
		finalizeFlag = true;
	}
}

// シーン遷移
void SceneController::pushScene(Scene& nextScene)
{
	changeScene();
	sceneStack.push(nextScene);
}

// 前のシーンに戻る
void SceneController::popScene()
{
	changeScene();
	sceneStack.pop();
}
