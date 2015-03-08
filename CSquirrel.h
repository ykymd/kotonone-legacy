#pragma once
#include <sqrat.h>

namespace Kotone {
	class CSquirrel
	{
	public:
		bool Init();
		bool Finalize();
	private:
		HSQUIRRELVM vm;
		void compileFile(char* srcfilename, char*dstfilename);
	};
}