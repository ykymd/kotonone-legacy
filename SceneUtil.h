#pragma once
#include <string>

const std::string delimiter = ".";

class SceneUtil
{
public:
	SceneUtil();
	~SceneUtil();
};

class SceneMethod
{
public:
	static const std::string Initialize;
	static const std::string Update;
	static const std::string Draw;
	static const std::string Finalize;
};