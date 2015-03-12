#pragma once

#include "tstring.h"

class SceneController
{
public:
	~SceneController();

	// ジェネレータ
	static SceneController* getInstance(void);

	// 現在のシーン
	const tstring SceneController::currentScene(void);

	// シーン遷移
	void pushScene(const char* scene_name);

private:
	// コンストラクタ隠蔽
	SceneController(void);

	// シーン名(テーブル名)を表す文字列
	tstring g_SceneName;
};

