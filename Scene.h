#pragma once
#include "tstring.h"
#include <sqrat.h>
#include "SceneUtil.h"

using namespace Sqrat;

class Scene
{
public:
	Scene(const char* sceneName, SceneType type = SceneType::VIEW);
	~Scene();

	Function Initialize();
	Function Update();
	Function Draw();
	Function Finalize();

	SceneType getType();
	void setType(SceneType type);

private:
	// シーン名(テーブル名)を表す文字列
	tstring name;

	// シーンの種類
	SceneType sceneType;
};
