#pragma once
#include <Windows.h>

namespace Kotone {
	class DebugConsole
	{
	public:
		void setConsole();
		void Finalize();

		// コンソールを割り当てる
		BOOL console_enable;
	};
}