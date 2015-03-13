#include "Game.h"
#include "BindFunc.h"
#include "SceneController.h"
#include "ScriptLoader.h"

using namespace Kotone;

// キーボードのキーとジョイパッドのボタンの押下状態を初期化する
void Game::InitializeJoypadInputState()
{
	for (int i = 0; i<enableKeyNum; ++i)
		for (int j = 0; j<2; ++j)
			g_JoypadInputState[i][j] = false;
}

// キーボードのキーとジョイパッドのボタンの押下状態を初期化する
void Game::UpdateJoypadInputState()
{
	// g_FlipIndexが0なら1に、1なら0にする
	g_FlipIndex = g_FlipIndex ^ 1;

	// キーボードやジョイパッドの押下状態を調べてセットする(DXライブラリAPI)
	g_JoypadInputState[INPUT_TYPE::UP][g_FlipIndex] =
		(DxLib::GetJoypadInputState(DX_INPUT_KEY_PAD1) & PAD_INPUT_UP) != 0;

	g_JoypadInputState[INPUT_TYPE::DOWN][g_FlipIndex] =
		(DxLib::GetJoypadInputState(DX_INPUT_KEY_PAD1) & PAD_INPUT_DOWN) != 0;

	g_JoypadInputState[INPUT_TYPE::LEFT][g_FlipIndex] =
		(DxLib::GetJoypadInputState(DX_INPUT_KEY_PAD1) & PAD_INPUT_LEFT) != 0;

	g_JoypadInputState[INPUT_TYPE::RIGHT][g_FlipIndex] =
		(DxLib::GetJoypadInputState(DX_INPUT_KEY_PAD1) & PAD_INPUT_RIGHT) != 0;

	g_JoypadInputState[INPUT_TYPE::ONE][g_FlipIndex] = CheckHitKey(KEY_INPUT_3);

	g_JoypadInputState[INPUT_TYPE::TWO][g_FlipIndex] = CheckHitKey(KEY_INPUT_E);

	g_JoypadInputState[INPUT_TYPE::THREE][g_FlipIndex] = CheckHitKey(KEY_INPUT_D);

	g_JoypadInputState[INPUT_TYPE::FOUR][g_FlipIndex] = CheckHitKey(KEY_INPUT_C);

	g_JoypadInputState[INPUT_TYPE::FIVE][g_FlipIndex] = CheckHitKey(KEY_INPUT_0);

	g_JoypadInputState[INPUT_TYPE::SIX][g_FlipIndex] = CheckHitKey(KEY_INPUT_O);

	g_JoypadInputState[INPUT_TYPE::SEVEN][g_FlipIndex] = CheckHitKey(KEY_INPUT_K);

	g_JoypadInputState[INPUT_TYPE::EIGHT][g_FlipIndex] = CheckHitKey(KEY_INPUT_M);

	g_JoypadInputState[INPUT_TYPE::ESC][g_FlipIndex] = CheckHitKey(KEY_INPUT_ESCAPE);

	g_JoypadInputState[INPUT_TYPE::SPACE][g_FlipIndex] = CheckHitKey(KEY_INPUT_SPACE);
}

bool Game::Init()
{
	// ウインドウのタイトルを設定
	SetMainWindowText(_T("コトノネドライブ～KottON-pluckin'!～"));

	//ログの出力を行わない
	DxLib::SetOutApplicationLogValidFlag(false);

	// ウィンドウモードに変更する(DXライブラリAPI)
	DxLib::ChangeWindowMode(TRUE);

	// DXライブラリを初期化する(DXライブラリAPI)
	if (DxLib::DxLib_Init() == -1) {
		// エラーであれば終了する
		return false;
	}

	//アーカイブファイルの拡張子を指定する
	DxLib::SetDXArchiveExtension("dat");

	// DXライブラリの描画先領域を裏画面(バックバッファ)に設定する(DXライブラリAPI)
	DxLib::SetDrawScreen(DX_SCREEN_BACK);

	// 描画する文字列の太さをデフォルトより細くする(DXライブラリAPI)
	DxLib::SetFontThickness(3);

	// 文字列の描画処理をアンチエイリアスありに変更する(DXライブラリAPI)
	DxLib::ChangeFontType(DX_FONTTYPE_ANTIALIASING);

	return true;
}

void Game::Run()
{
	// キーボードとジョイパッドの入力状態を初期化する
	InitializeJoypadInputState();

	// Squirrel側の最初に呼ばれる関数を呼び出す
	auto initialFunction = ScriptLoader::getFunction(_SC("boot"));
	initialFunction.Execute();

	// 現在の時間(マイクロ秒)を取得し、OldTimeにセットする(DXライブラリAPI)
	OldTime = DxLib::GetNowHiPerformanceCount();

	// フォントハンドルをセット
	titleFontHandle = CreateFontToHandle("@ＭＳ 明朝", 18, -1, DX_FONTTYPE_ANTIALIASING);
	subFontHandle = CreateFontToHandle("@ＭＳ 明朝", 16, -1, DX_FONTTYPE_ANTIALIASING);

	auto sceneController = SceneController::getInstance();

	while (DxLib::ProcessMessage() == 0){
		// 現在のシーンを取得
		auto currentScene = sceneController->getCurrentScene();

		/* シーン切替時の処理 */

		// シーン名.finalize関数を呼び出す
		if (g_FinalizeFlag)
		{
			currentScene.Finalize().Execute();
			g_FinalizeFlag = false;
		}

		if (g_InitializeFlag)
		{
			// シーン名.initialize関数を呼び出す
			currentScene.Initialize().Execute();

			// 更新処理用の関数として、シーン名.update関数をセットする
			update_function = currentScene.Update();

			// 描画処理用の関数として、シーン名.draw関数をセットする
			draw_function = currentScene.Draw();
		}


		// 前回のF5キーフラグをセットする
		CheckHitF5keyPrev = CheckHitF5KeyNow;

		// F5キーが押されているかどうか調べる
		CheckHitF5KeyNow = CheckHitKey(KEY_INPUT_F5);

		//再読み込みを実行する
		/*
		if (CheckHitF5KeyNow && !CheckHitF5keyPrev){
		if (OpenSquirrelFile("title.nut"))
		{
		printf("Succes File Reloaded\n");
		}
		else{
		printf("Failed File Reloaded\n");
		}
		}
		*/

		// 差分時間を計測
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);

		// 現在の時間(マイクロ秒)を取得する(DXライブラリAPI)
		LONGLONG NowTime = DxLib::GetNowHiPerformanceCount();

		// 現在の時間と、前回実行時の時間との差分を計算する
		ElapsedTime = (int)((NowTime - OldTime) / 1000);

		// 次回計算用に、現在の時間を保持する
		OldTime = NowTime;


		/* Sleep処理と、Updateの事前処理 */

		// Updateする回数。1フレームぶんの回数で初期化する
		int UpdateCount = 1;

		// 経過時間には前回Sleepした分の時間も含まれているので、その分を引く
		ElapsedTime -= SleepTime;

		// 経過時間が、1フレームあたりの単位時間より小さければ
		if (ElapsedTime < FrameTime)
		{
			// 単位時間 - 経過時間 ぶんだけスリープする
			SleepTime = (int)FrameTime - ElapsedTime;
			Sleep(SleepTime);
		}
		// 1フレームあたりの単位時間より経過時間が大きければ、処理が遅れている
		else
		{
			// スリープ処理は行わないので、スリープ時間を0にする
			SleepTime = 0;

			// 1フレーム分の時間を引く
			ElapsedTime -= (int)FrameTime;

			// フレームスキップ(移動処理を全部一度に実行する)
			if (FrameSkipFlag)
			{
				// Updateする回数を加算する
				for (; ElapsedTime >= (int)FrameTime; ElapsedTime -= (int)FrameTime)
				{
					UpdateCount++;
				}
			}
			// FrameSkipFlagがfalseなら処理落ち(更新処理が1回だけ実行される)
			// UpdateCountは1のまま
		}

		// シーン切替時には、更新処理を1回だけ行う
		if (g_InitializeFlag)
		{
			g_InitializeFlag = false;
			UpdateCount = 1;
		}

		// 更新処理を必要な回数だけ行う
		for (int i = 0; i<UpdateCount; ++i)
		{
			// キーボードとジョイパッドの入力状態を更新する
			UpdateJoypadInputState();

			// update関数を呼び出す
			update_function.Execute();
		}

		// 画面に描画されている内容をクリアする(DXライブラリAPI)
		DxLib::ClearDrawScreen();

		// draw関数を呼び出す
		draw_function.Execute();

		//DrawVStringToHandle(0, 0, "♪琴古主との邂逅", GetColor(255, 255, 255), FontHandle);

		// 裏画面(バックバッファ)に描画されている中身を、表画面と入れ替える(DXライブラリAPI)
		DxLib::ScreenFlip();
	}
}

void Game::Finalize()
{
	// finalize関数を呼び出す
	if (g_FirstSceneFlag)
		SceneController::getInstance()->getCurrentScene().Finalize().Execute();

	// 関数オブジェクトを解放する
	// (Sqrat::Function型のデストラクタ内で仮想マシンにアクセスしているので、
	// sq_close関数で仮想マシンを解放する前に、Release関数で解放しなければエラーとなる)
	update_function.Release();
	draw_function.Release();

	DxLib::DxLib_End();				// ＤＸライブラリ使用の終了処理
}
