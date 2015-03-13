#include "Scene.h"

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

std::string Scene::Initialize()
{
	return name + ".initialize";
}

std::string Scene::Update()
{
	return name + ".update";
}

std::string Scene::Draw()
{
	return name + ".draw";
}

std::string Scene::Finalize()
{
	return name + ".finalize";
}
