#include "Scene.h"
#include "ScriptLoader.h"

Scene::Scene(const char* sceneName, SceneType type)
{
	name = sceneName;
	sceneType = type;
}

Scene::~Scene()
{
}

Function Scene::Initialize()
{
	auto functionName = name + delimiter + SceneMethod::Initialize;
	return ScriptLoader::getFunction(functionName.c_str());
}

Function Scene::Update()
{
	auto functionName = name + delimiter + SceneMethod::Update;
	return ScriptLoader::getFunction(functionName.c_str());
}

Function Scene::Draw()
{
	auto functionName = name + delimiter + SceneMethod::Draw;
	return ScriptLoader::getFunction(functionName.c_str());
}

Function Scene::Finalize()
{
	auto functionName = name + delimiter + SceneMethod::Finalize;
	return ScriptLoader::getFunction(functionName.c_str());
}

SceneType Scene::getType()
{
	return sceneType;
}

void Scene::setType(SceneType type)
{
	sceneType = type;
}
