#ifndef _CGAME_H
#define _CGAME_H
////////////////////////////////////////////////////////////////////////////
// CGame : ゲームメインルーチン v1.00
////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include "./libfiles/CDDPro90.h"
#include "./libfiles/CDSPro81.h"
#include "./libfiles/CDIPro81.h"
#include "./libfiles/CWindow.h"
#include "./libfiles/CBmsPro.h"
#include "./libfiles/CTimer.h"



// ステート
enum STATE {
	G_INIT=0,		// 初期化モード
	G_MAININIT,		// メインルーチン初期化
	G_MAIN,			// メインループ
	G_END			// 全てのゲーム終了処理
};



class CGame {

	// ライブラリなど
	CWindow			win;				// ウインドウ管理
	CDDPro90		dd;					// Direct3D管理クラス
	CDSPro81		ds;					// DirectSound管理
	CDIPro81		di;					// DirectInput全般
	CTimer			tm;					// 60FPS処理
	CBmsPro			bms;				// BMSファイル管理用

	// ゲームのメイン処理で使用する変数
	STATE			dwState;			// ゲームのステート状況
	float			fScrMulti;			// 小節間の幅の倍率
	LONGLONG		llStartTime;		// ゲーム開始時の時間(高解像度タイマー)
	LONGLONG		llGlobalFreq;		// マシンの１秒間のカウント数(高解像度タイマー)
	int				iImageID;			// 現在表示されているアニメーションインデックス番号
	int				mBmsNum[256];		// BMS演奏で計算開始する配列番号(処理を軽くするため)

	// 演出など
	BOOL			bOnKey[6];			// キーを押したままか
	int				iScratchStatus;		// スクラッチの回転向き(-1=左,0=停止,1=右)
	int				mFlashIndex[6];		// 次に使用されるフラッシュカウンタのインデックス
	int				mFlashCount[6][3];	// フラッシュ６ｘ３個分のカウンタ
	int				mBackKeyCount[6];	// キーを離した時の後ろのバックライトの演出用カウンタ

private:
	// 初期化
	BOOL Init( HINSTANCE hinst );		// 初期化＆ゲーム形成
	BOOL Flash( void );					// ロード済みデータの開放

	// ゲーム処理
	BOOL GameInit( void );				// メインゲーム初期化
	HRESULT GameRun( BOOL demo=FALSE );	// ゲーム実行ループ

public:
	// 公開関数
	CGame();
	virtual ~CGame();
	BOOL Run( HINSTANCE hinst );		// ゲームメインルーチン
};

#endif
