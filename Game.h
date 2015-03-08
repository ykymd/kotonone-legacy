#pragma once

#include "DxLib.h"
#include <sqrat.h>
#include <vector>
#include <boost/algorithm/string.hpp>

namespace Kotone {
	class Game
	{
	public:
		// 更新処理と描画処理用のSquirrel関数
		Sqrat::Function update_function;
		Sqrat::Function draw_function;

		// F5ーが押されているかどうかのフラグ
		int CheckHitF5KeyNow = false;
		int CheckHitF5keyPrev = false;

		/* タイマー関連のパラメータ */

		// 経過時間(ミリ秒)を格納する変数
		int ElapsedTime = 0;

		// Sleepする時間を格納する変数
		int SleepTime = 0;

		// 1フレームが何ミリ秒か(60FPS)
		float FrameTime = 1000.0f / 60.0f;

		// 処理が追いつかなかった際に、フレームスキップを行うかどうか
		bool FrameSkipFlag = true;

		// 前回実行時の時間(マイクロ秒)を格納する変数
		LONGLONG OldTime = 0;

		bool Init();
		void Run();
		void Finalize();

		Sqrat::Function GetSquirrelFunction(const SQChar* callback_func);
		void InitializeJoypadInputState();
		void UpdateJoypadInputState();
	};
}