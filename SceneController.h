#pragma once

#include "tstring.h"
#include "Scene.h"
#include <stack>

class SceneController
{
public:
	~SceneController();

	// ジェネレータ
	static SceneController* getInstance(void);

	// 現在のシーン
	Scene& getCurrentScene(void);

	// シーン遷移
	void pushScene(Scene& nextScene);

private:
	// コンストラクタ隠蔽
	SceneController(void);

	std::stack<Scene&> sceneStack;
};

