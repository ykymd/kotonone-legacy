#include "CSquirrel.h"
#include "SqBMS.h"
#include "BindFunc.h"
#include "DxLib.h"
#include "SceneController.h"

#include <tchar.h>
#include <sqstdmath.h>
#include <iostream>
#include <stdarg.h>

#ifdef UNICODE
	#define tvsnprintf vswprintf
#else
	#define tvsnprintf vsnprintf
#endif

using namespace Kotone;

// バッファサイズ
const int PRINT_BUFFER_SIZE = 512;

// Squirrelの文字列出力関数
static void StandardPrintFunc(HSQUIRRELVM vm, const SQChar* format, ...)
{
	// 出力結果を書き込むためのバッファを定義
	SQChar buf[PRINT_BUFFER_SIZE];

	// 可変長引数を格納するリストを定義
	va_list args;

	// 可変長引数を格納する
	va_start(args, format);

	// 可変長引数のデータをformatにしたがってバッファに書き込み
	tvsnprintf(buf, PRINT_BUFFER_SIZE, format, args);

	// 事後処理
	va_end(args);

#ifdef _UNICODE
	// 変換処理に必要なバッファサイズを算出する
	int size = WideCharToMultiByte(932, 0, buf, -1, NULL, 0, NULL, NULL);

	// 変換後の文字列を格納するバッファを作成する
	char* tmp = (char*)malloc(size + 1);
	memset(tmp, 0, size + 1);

	// Unicode文字列(UTF-16)をマルチバイト文字列(Shift-JIS)に変換する
	WideCharToMultiByte(932, 0, buf, -1, tmp, size, NULL, NULL);

	// 文字列を出力する
	printf("Standard Output: %s\n", tmp);

	// mallocした領域をfree
	free(tmp);
#else
	// 文字列を出力する
	printf("Standard Output: %s\n", buf);
#endif
}

static void ErrorPrintfFunc(HSQUIRRELVM vm, const SQChar* str, ...)
{
	//結果を書き込むためのバッファ
	SQChar buf[PRINT_BUFFER_SIZE];

	// 可変長引数を格納するリスト
	va_list arglist;

	// リストの初期化
	va_start(arglist, str);

	// 可変長引数のデータを、strが指す書式文字列に従って処理し、bufに書き込む
	tvsnprintf(buf, PRINT_BUFFER_SIZE, str, arglist);

	// 可変長引数にアクセスした後の事後処理を行う
	va_end(arglist);

#ifdef _UNICODE
	// 変換処理に必要なバッファサイズを算出する
	int size = WideCharToMultiByte(932, 0, buf, -1, NULL, 0, NULL, NULL);
	// 変換後の文字列を格納するバッファを作成する
	char* tmp = (char*)malloc(size + 1);
	memset(tmp, 0, size + 1);
	// Unicode文字列(UTF-16)をマルチバイト文字列(Shift-JIS)に変換する
	WideCharToMultiByte(932, 0, buf, -1, tmp, size, NULL, NULL);
	// 文字列を出力する
	printf("Standard Error Output: %s¥n", tmp);
	// mallocした領域をfree
	free(tmp);
#else
	// 文字列を出力する
	printf("Standard Error Output: %s¥n", buf);
#endif
}

bool CSquirrel::Init()
{
	// SquirrelVMをスタックサイズ1024で作成
	vm = sq_open(1024);

	// 標準のエラーハンドラをセットする
	sqstd_seterrorhandlers(vm);

	// 仮想マシンのセット
	Sqrat::DefaultVM::Set(vm);

	// 文字列出力関数を設定
	sq_setprintfunc(vm, StandardPrintFunc, ErrorPrintfFunc);

	// スタックトップ値を取得
	int top = sq_gettop(vm);

	// ルートテーブルをスタックに積む
	sq_pushroottable(vm);

	// 算術演算ライブラリを積む
	sqstd_register_mathlib(vm);

	// スタックトップ値を戻す
	sq_settop(vm, top);

	// 関数を登録する
	Sqrat::RootTable().
		Func(_SC("SQInclude"), OpenSquirrelFile).
		Func(_SC("LoadGraph"), SQ_LoadGraph).
		Func(_SC("DrawGraph"), SQ_DrawGraph).
		Func(_SC("DrawTurnGraph"), SQ_DrawTurnGraph).
		Func(_SC("DrawCircle"), SQ_DrawCircle).
		Func(_SC("DrawBox"), SQ_DrawBox).
		Func(_SC("DrawLine"), SQ_DrawLine).
		Func(_SC("GetColor"), SQ_GetColor).
		Func(_SC("ChangeScene"), ChangeScene).
		Func(_SC("BackScene"), BackScene).
		Func(_SC("GetJoypadInputStatePushed"), GetJoypadInputStatePushed).
		Func(_SC("GetJoypadInputStatePressed"), GetJoypadInputStatePressed).
		Func(_SC("SetDrawBlendMode"), DxLib::SetDrawBlendMode).
		Func(_SC("DrawString"), SQ_DrawString).
		Func(_SC("DrawVString"), SQ_DrawVString).
		Func(_SC("CreateFontToHandle"), SQ_CreateFontToHandle).
		Func(_SC("DrawSongTitleVString"), SQ_DrawTitleString).
		Func(_SC("DrawSongSubVString"), SQ_DrawSubString).
		Func(_SC("InitSoundMem"), SQ_InitSoundMem).
		Func(_SC("LoadSoundMem"), SQ_LoadSoundMem).
		Func(_SC("PlaySoundMem"), SQ_LoadSoundMem).
		Func(_SC("StopSoundMem"), SQ_LoadSoundMem).
		Func(_SC("DeleteSoundMem"), SQ_LoadSoundMem).
		Func(_SC("PlaySoundFile"), SQ_PlaySoundFile).
		Func(_SC("StopSoundFile"), SQ_PlaySoundFile).
		Func(_SC("SetDrawBlendMode"), SQ_SetDrawBlendMode).
		Func(_SC("EXIT"), SQ_Exit).
		Func(_SC("LoadSE"), SQ_LoadSE).
		Func(_SC("PlaySE"), SQ_PlaySE);

	// 定数と列挙値を登録
	Sqrat::ConstTable().
		Const(_SC("TRUE"), TRUE).
		Const(_SC("FALSE"), FALSE).
		Const(_SC("DX_BLENDMODE_ALPHA"), DX_BLENDMODE_ALPHA).
		Const(_SC("DX_PLAYTYPE_NORMAL"), DX_PLAYTYPE_NORMAL).
		Const(_SC("DX_PLAYTYPE_BACK"), DX_PLAYTYPE_BACK).
		Const(_SC("DX_PLAYTYPE_LOOP"), DX_PLAYTYPE_LOOP).
		Const(_SC("DX_BLENDMODE_NOBLEND"), DX_BLENDMODE_NOBLEND).
		Const(_SC("DX_BLENDMODE_NOBLEND"), DX_BLENDMODE_ALPHA).
		Const(_SC("DX_BLENDMODE_NOBLEND"), DX_BLENDMODE_ADD).
		Const(_SC("DX_BLENDMODE_NOBLEND"), DX_BLENDMODE_SUB).
		Const(_SC("DX_BLENDMODE_NOBLEND"), DX_BLENDMODE_MULA).
		Const(_SC("DX_BLENDMODE_NOBLEND"), DX_BLENDMODE_INVSRC).
		Enum(_SC("INPUT_TYPE"),
		Sqrat::Enumeration().
		Const(_SC("UP"), INPUT_TYPE::UP).
		Const(_SC("DOWN"), INPUT_TYPE::DOWN).
		Const(_SC("LEFT"), INPUT_TYPE::LEFT).
		Const(_SC("RIGHT"), INPUT_TYPE::RIGHT).
		Const(_SC("ONE"), INPUT_TYPE::ONE).
		Const(_SC("TWO"), INPUT_TYPE::TWO).
		Const(_SC("THREE"), INPUT_TYPE::THREE).
		Const(_SC("FOUR"), INPUT_TYPE::FOUR).
		Const(_SC("FIVE"), INPUT_TYPE::FIVE).
		Const(_SC("SIX"), INPUT_TYPE::SIX).
		Const(_SC("SEVEN"), INPUT_TYPE::SEVEN).
		Const(_SC("EIGHT"), INPUT_TYPE::EIGHT).
		Const(_SC("SPACE"), INPUT_TYPE::SPACE).
		Const(_SC("ESC"), INPUT_TYPE::ESC));

	// BMS管理クラスを公開
	Sqrat::RootTable().Bind(
		_SC("KotononeBMS"),
		Sqrat::Class<SqBMS>().
		Var(_SC("Score"), &SqBMS::Score).
		Func(_SC("Load"), &SqBMS::SQ_Load).
		Func(_SC("Play"), &SqBMS::SQ_Play).
		Func(_SC("Stop"), &SqBMS::SQ_Stop).
		Func(_SC("GetBarLine_X"), &SqBMS::SQ_GetBarLine_Y).
		Func(_SC("GetBarNum"), &SqBMS::SQ_GetBarNum).
		Func(_SC("GetObject_X"), &SqBMS::SQ_GetObject_X).
		Func(_SC("GetObjNum"), &SqBMS::SQ_GetObjeNum).
		Func(_SC("GetSelectSceneData"), &SqBMS::SQ_GetSelectSceneData).
		Func(_SC("GetTitle"), &SqBMS::SQ_GetTitle).
		Func(_SC("GetBPM"), &SqBMS::SQ_GetBPM).
		Func(_SC("NoteJudge"), &SqBMS::SQ_Judge).
		Func(_SC("NoteJudgeOver"), &SqBMS::SQ_JudgeOver).
		Func(_SC("LongNoteJudge"), &SqBMS::SQ_LongNoteJudge).
		Func(_SC("LongNoteJudgeOver"), &SqBMS::SQ_LongNoteJudgeOver).
		Func(_SC("GetAvailableSongNum"), &SqBMS::SQ_GetAvailableSoungNum).
		Func(_SC("GetAvailableSongTitle"), &SqBMS::SQ_GetAvailableSongTitle).
		Func(_SC("GetAvailableSongComposer"), &SqBMS::SQ_GetAvailableSongComposer).
		Func(_SC("GetAvailableSongGenre"), &SqBMS::SQ_GetAvailableSongGenre).
		Func(_SC("GetAvailableSongBPM"), &SqBMS::SQ_GetAvailableSongBPM).
		Func(_SC("GetAvailableSongFilename"), &SqBMS::SQ_GetAvailableSongFilename).
		Func(_SC("GetAvailableSongLevels"), &SqBMS::SQ_GetAvailableSongDifficulty).
		Func(_SC("GetHighSpeed"), &SqBMS::SQ_GetHighSpeed).
		Func(_SC("SetHighSpeed"), &SqBMS::SQ_SetHighSpeed).
		Func(_SC("GetScore"), &SqBMS::SQ_GetScore).
		Func(_SC("isStart"), &SqBMS::SQ_isStart).
		Func(_SC("isFinished"), &SqBMS::SQ_isFinished).
		Func(_SC("Clear"), &SqBMS::SQ_Clear));

	// シーンクラスを追加
	/*
	Sqrat::RootTable().Bind(
		_SC("Scene"),
		Sqrat::Class<Scene>());
	//*/

	// シーン管理クラスを追加
	/*
	Sqrat::RootTable().Bind(
		_SC("SceneController"),
		Sqrat::Class<SceneController>().
		StaticFunc(_SC("getInstance"), &SceneController::getInstance).
		Func(_SC("getCurrentScene"), &SceneController::getCurrentScene).
		Func(_SC("pushScene"), &SceneController::pushScene).
		Func(_SC("popScene"), &SceneController::popScene));
	//*/

	// Squirrelコード読み込み
	//*
	OpenSquirrelFile("const.nut");
	OpenSquirrelFile("var.nut");
	OpenSquirrelFile("boot.nut");
	//OpenSquirrelFile("Script.dat");
	//OpenSquirrelFile("Script.cnut");
	//*/

	//compileFile("Script.nut", "Script.cnut");

	return true;
}

void CSquirrel::compileFile(char* srcfilename, char* dstfilename)
{
	std::string file_path(_SC(".\\data\\Scripts\\"));
	std::string src, dst;
	src = file_path + srcfilename;
	dst = file_path + dstfilename;

	Sqrat::Script script(vm);
	try
	{
		script.CompileFile(src.c_str());
		script.WriteCompiledFile(dst.c_str());
	}
	catch (const Sqrat::Exception& ex)
	{
		// 例外メッセージを出力
		StandardPrintFunc(vm, _SC("%s\n"), ex.Message().data());
	}
}

bool CSquirrel::Finalize()
{
	// SquirrelVMの解放
	sq_close(vm);

	return true;
}