#pragma once
#include <sqrat.h>

class ScriptLoader
{
public:
	ScriptLoader();
	~ScriptLoader();

	static Sqrat::Function getFunction(const SQChar* callback_func);
};

