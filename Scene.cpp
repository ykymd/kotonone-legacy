#include "Scene.h"
#include "ScriptLoader.h"
#include "SceneUtil.h"

Scene::Scene(const char* sceneName)
{
	name = sceneName;
}

Scene::~Scene()
{
}

tstring Scene::getName()
{
	return name;
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
