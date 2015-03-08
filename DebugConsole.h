#pragma once
#include <Windows.h>

namespace Kotone {
	class DebugConsole
	{
	public:
		void setConsole();
		void Finalize();

		// ƒRƒ“ƒ\[ƒ‹‚ğŠ„‚è“–‚Ä‚é
		BOOL console_enable;
	};
}