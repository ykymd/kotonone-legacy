#include "BindFunc.h"
#include <sqrat.h>
#include <vector>
#include <boost/algorithm/string.hpp>
#include "DxLib.h"

#include "SceneController.h"

int titleFontHandle = -1;
int subFontHandle = -1;

// 各効果音のハンドル
int EnterSEHandle = 0;
int CancelSEHandle = 0;
int CursorSEHandle = 0;
int DecideSEHandle = 0;

const std::string folderParentAddress = ".\\data\\";
const std::string scriptFolderName = "Scripts\\";

// SquirrelScriptを読み込んで実行する
bool OpenSquirrelFile(char* file_name)
{
	try {
		// フォルダパスと結合する
		std::string file_path(_SC(folderParentAddress + scriptFolderName));
		file_path += file_name;

		// スクリプトを扱うためのクラスインスタンスを作成する
		Sqrat::Script script;

		// スクリプトファイルを読み込んでコンパイルする
		script.CompileFile(file_path.c_str());

		// コンパイルしたスクリプトを実行する
		script.Run();

		return true;
	}
	catch (const Sqrat::Exception& ex){
		// 例外メッセージを出力
		printf("%s\n", ex.Message().data());

		return false;
	}
}

// initialize, finalize関数を呼び出すかどうかのフラグ
bool g_InitializeFlag = false;
bool g_FinalizeFlag = false;
bool g_FirstSceneFlag = true;

// シーン名(テーブル名)を表す文字列

// シーンを切り替える関数
void ChangeScene(const SQChar* scene_name)
{
	// initialize関数を呼び出すフラグを立てる
	g_InitializeFlag = true;

	// finalize関数は、2回目以降のシーン切り替え時にのみ呼び出されるようにする
	if (g_FirstSceneFlag)
	{
		g_FinalizeFlag = false;
		g_FirstSceneFlag = false;
	}
	else
	{
		g_FinalizeFlag = true;
	}

	// 遷移先シーンを生成
	auto nextScene = Scene(scene_name);

	// シーン名をセットする
	SceneController::getInstance()->pushScene(nextScene);
}

// 0と1を交互にセットする変数。g_JoypadInputStateのインデックスに指定し、
// 前回の状態と今回の状態を交互に保持するために利用される
int g_FlipIndex = 0;

// キーが押された状態を保持する変数。今回の状態と前回の状態を保持する
BOOL g_JoypadInputState[enableKeyNum][2];

// キーボードのキーとジョイパッド1のボタンの押下状態を調べて、引数に指定したキーやボタンが押された瞬間のみtrueを返す
bool GetJoypadInputStatePushed(int input_type)
{
	// キーボードのキーかジョイパッド1のボタンが押されており、かつ前回押されていないかどうか
	if (g_JoypadInputState[input_type][g_FlipIndex] &&
		!g_JoypadInputState[input_type][g_FlipIndex ^ 1])
	{
		//押された瞬間ならtrueを返す
		return true;
	}
	else
	{
		//押された瞬間でなければfalseを返す
		return false;
	}
}

// キーボードのキーとジョイパッド1のボタンの押下状態を調べて、引数に指定したキーやボタンが押されていればtrueを返す
bool GetJoypadInputStatePressed(int input_type)
{
	// キーボードのキーかジョイパッド1のボタンが押されているかどうか
	if (g_JoypadInputState[input_type][g_FlipIndex])
	{
		// 押されていたらtrueを返す
		return true;
	}
	else
	{
		// 押されていなければfalseを返す
		return false;
	}
}

// 画像ファイルを読み込むラッパー関数
int SQ_LoadGraph(const TCHAR *file_name)
{
	// 第2引数にデフォルト引数を持っているので、DxLib::LoadGraphを直接登録せずにラッパー関数をSqratで登録する
	return DxLib::LoadGraph(file_name);
}

// 読み込んだ画像ファイルを描画するラッパー関数
int SQ_DrawGraph(int x, int y, int handle)
{
	// 第4引数(透過フラグ)にTRUEを毎回指定するのが面倒なので、ラッパー関数をSqratで登録する
	return DxLib::DrawGraph(x, y, handle, TRUE);
}

// 読み込んだ画像ファイルを反転して描画するラッパー関数
int SQ_DrawTurnGraph(int x, int y, int GrHandle, int TransFlag)
{
	return DxLib::DrawTurnGraph(x, y, GrHandle, TransFlag);
}

int SQ_DrawCircle(int x, int y, int r, int Color)
{
	return DxLib::DrawCircle(x, y, r, Color, TRUE);
}

int SQ_DrawBox(int x1, int y1, int x2, int y2, int Color, int FillFlag)
{
	return DxLib::DrawBox(x1, y1, x2, y2, Color, FillFlag);
}

int SQ_DrawLine(int x1, int y1, int x2, int y2, int Color)
{
	return DxLib::DrawLine(x1, y1, x2, y2, Color);
}

int SQ_GetColor(int Red, int Green, int Blue)
{
	return DxLib::GetColor(Red, Green, Blue);
}

void SQ_DrawString(int x, int y, const SQChar* str, int Color, int fontsize, int line_space_size)
{
	// フォントサイズをセット
	DxLib::SetFontSize(fontsize);

	// 文字の高さを取得
	int height = fontsize + line_space_size;

	// 改行区切りの文字列を格納する
	std::vector<tstring> vec;
	boost::split(vec, str, boost::is_any_of(_T("\n")));

	// 改行ごとに1段ずらす
	for (int i = 0, n = vec.size(); i < n; i++)
	{
		// 文字列を描画
		DxLib::DrawString(x, y + height + i, vec[i].c_str(), Color);
	}
}

int SQ_DrawVString(int x, int y, const SQChar* str, int Color, int FontHandle)
{
	return DxLib::DrawVStringToHandle(x, y, str, Color, FontHandle);
}

int SQ_CreateFontToHandle(const SQChar *FontName, int Size, int Thick, int FontType)
{
	return DxLib::CreateFontToHandle(FontName, Size, Thick, FontType);
}

int SQ_DrawTitleString(int x, int y, const SQChar *str, int Color)
{
	std::string tmp = str;
	return DxLib::DrawVStringToHandle(x, y, ("♪" + tmp).c_str(), Color, titleFontHandle);
}

int SQ_DrawSubString(int x, int y, const SQChar *str, int Color)
{
	return DxLib::DrawVStringToHandle(x, y, str, Color, subFontHandle);
}


void SQ_InitSoundMem()
{
	DxLib::InitSoundMem();
}

int SQ_LoadSoundMem(const SQChar* file)
{
	return DxLib::LoadSoundMem(file);
}

int SQ_PlaySoundMem(int SoundHandle, int PlayType, int TopPositionFlag)
{
	return DxLib::PlaySoundMem(SoundHandle, PlayType, TopPositionFlag);
}

int SQ_StopSoundMem(int SoundHandle)
{
	return DxLib::StopSoundMem(SoundHandle);
}

int SQ_DeleteSoundMem(int SoundHandle)
{
	return DxLib::DeleteSoundMem(SoundHandle);
}

int SQ_PlaySoundFile(char *FileName, int PlayType)
{
	return DxLib::PlaySoundFile(FileName, PlayType);
}

int SQ_SetDrawBlendMode(int BlendMode, int Pal)
{
	return DxLib::SetDrawBlendMode(BlendMode, Pal);
}

int SQ_StopSoundFile()
{
	return DxLib::StopSoundFile();
}

// ゲームを終了する
int SQ_Exit()
{
	exit(0);
}

// 効果音をメモリ上に読み込む
void SQ_LoadSE(int n)
{
	switch (n){
	case 0:
		if (EnterSEHandle == 0) EnterSEHandle = DxLib::LoadSoundMem(".\\data\\se\\kettei.ogg");
	case 1:
		if (CancelSEHandle == 0) CancelSEHandle = DxLib::LoadSoundMem(".\\data\\se\\back.ogg");
	case 2:
		if (CursorSEHandle == 0) CursorSEHandle = DxLib::LoadSoundMem(".\\data\\se\\cursor.mp3");
	case 3:
		if (DecideSEHandle == 0) DecideSEHandle = DxLib::LoadSoundMem(".\\data\\se\\musicdecide.ogg");
	}
}

// 効果音を再生する
int SQ_PlaySE(int n)
{
	switch (n){
	case 0:
		return DxLib::PlaySoundMem(EnterSEHandle, DX_PLAYTYPE_BACK);
	case 1:
		return DxLib::PlaySoundMem(CancelSEHandle, DX_PLAYTYPE_NORMAL);
	case 2:
		return DxLib::PlaySoundMem(CursorSEHandle, DX_PLAYTYPE_BACK);
	case 3:
		return DxLib::PlaySoundMem(DecideSEHandle, DX_PLAYTYPE_BACK);
	}

	return -1;
}
