#pragma once

#include <string>
#include <sqrat.h>
#include <Windows.h>

#ifdef UNICODE
#define tstring ::std::wstring
#define tvsnprintf  vswprintf
#else
#define tstring ::std::string
#define tvsnprintf  vsnprintf
#endif

enum INPUT_TYPE
{
	UP,     // 上ボタン/キー
	DOWN,   // 下ボタン/キー
	LEFT,   // 左ボタン/キー
	RIGHT,  // 右ボタン/キー
	ONE,    // 1ボタンとZキー
	TWO,    // 2ボタンとXキー
	THREE,  // 3ボタンとCキー
	FOUR,	// 4ボタンとVキー
	FIVE,	// 5ボタン
	SIX,    // 6ボタン
	SEVEN,	// 7ボタン
	EIGHT,	// 8ボタン
	ESC,		// ESCキー
	SPACE,	 //スペースキー
};

enum SYSTEM_SE {
	ENTER,	//決定
	CANCEL,	//キャンセル
	CURSOR,	//カーソル
	DECIDE,	//楽曲決定
};

extern int titleFontHandle;
extern int subFontHandle;

// SquirrelScriptを読み込んで実行する
extern bool OpenSquirrelFile(char* file_name);

// initialize, finalize関数を呼び出すかどうかのフラグ
extern bool g_InitializeFlag;
extern bool g_FinalizeFlag;
extern bool g_FirstSceneFlag;

// シーン名(テーブル名)を表す文字列
//extern tstring g_SceneName;

// シーンを切り替える関数
extern void ChangeScene(const SQChar* scene_name);

// 0と1を交互にセットする変数。g_JoypadInputStateのインデックスに指定し、
// 前回の状態と今回の状態を交互に保持するために利用される
extern int g_FlipIndex;

// 使用できるキーの種類の数
static const int enableKeyNum = 14;

// キーが押された状態を保持する変数。今回の状態と前回の状態を保持する
extern BOOL g_JoypadInputState[enableKeyNum][2];

// キーボードのキーとジョイパッド1のボタンの押下状態を調べて、引数に指定したキーやボタンが押された瞬間のみtrueを返す
extern bool GetJoypadInputStatePushed(int input_type);

// キーボードのキーとジョイパッド1のボタンの押下状態を調べて、引数に指定したキーやボタンが押されていればtrueを返す
extern bool GetJoypadInputStatePressed(int input_type);

// 画像ファイルを読み込むラッパー関数
extern int SQ_LoadGraph(const TCHAR *file_name);

// 読み込んだ画像ファイルを描画するラッパー関数
extern int SQ_DrawGraph(int x, int y, int handle);

extern int SQ_DrawTurnGraph(int x, int y, int GrHandle, int TransFlag);

extern int SQ_DrawCircle(int x, int y, int r, int Color);

extern int SQ_DrawBox(int x1, int y1, int x2, int y2, int Color, int FillFlag);

extern int SQ_DrawLine(int x1, int y1, int x2, int y2, int Color);

extern int SQ_GetColor(int Red, int Green, int Blue);

extern void SQ_DrawString(int x, int y, const SQChar* str, int Color, int fontsize, int line_space_size);
extern int SQ_DrawVString(int x, int y, const SQChar* str, int Color, int FontHandle);

extern int SQ_CreateFontToHandle(const SQChar *FontName, int Size, int Thick, int FontType);

extern int SQ_DrawTitleString(int x, int y, const SQChar *str, int Color);
extern int SQ_DrawSubString(int x, int y, const SQChar *str, int Color);

extern int SQ_SetDrawBlendMode(int BlendMode, int Pal);


extern void SQ_InitSoundMem();
extern int SQ_LoadSoundMem(const SQChar* file);
extern int SQ_PlaySoundMem(int SoundHandle, int PlayType, int TopPositionFlag);
extern int SQ_StopSoundMem(int SoundHandle);
extern int SQ_DeleteSoundMem(int SoundHandle);
extern int SQ_PlaySoundFile(char *FileName, int PlayType);
extern int SQ_StopSoundFile();

extern int SQ_Exit();

// 効果音のハンドル
extern int EnterSEHandle;
extern int CancelSEHandle;

// 効果音を読み込む
extern void SQ_LoadSE(int n);

// 効果音を再生する
extern int SQ_PlaySE(int n);