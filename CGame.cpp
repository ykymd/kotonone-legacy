#include "CGame.h"
#include <stdio.h>
#include "resource.h"

#define DEBUGMODE
#include "libfiles/debug.h"

#define FREE(x) { if(x) { free(x); x=NULL; } }

/////////////////////////////////////////////////////////////////////////
// コンストラクタ
/////////////////////////////////////////////////////////////////////////
CGame::CGame()
{
	dwState			= G_INIT;
	fScrMulti		= 1.0f;
	llStartTime		= 0;
	llGlobalFreq	= 0;
	iImageID		= 0;
	ZeroMemory( &mBmsNum,sizeof(mBmsNum) );

	ZeroMemory( &bOnKey,sizeof(bOnKey) );
	iScratchStatus	= 0;
	ZeroMemory( &mFlashIndex,sizeof(mFlashIndex) );
	ZeroMemory( &mFlashCount,sizeof(mFlashCount) );
	ZeroMemory( &mBackKeyCount,sizeof(mBackKeyCount) );
}

/////////////////////////////////////////////////////////////////////////
// デストラクタ
/////////////////////////////////////////////////////////////////////////
CGame::~CGame()
{
	Flash();
}

///////////////////////////////////////////////////////
// 初期化＆ゲーム形成
///////////////////////////////////////////////////////
BOOL CGame::Init( HINSTANCE hinst )
{
	// ウインドウ生成
	win.SetWindowStyle( WS_POPUP );				// 枠無しウインドウ()
	win.SetIcon( MAKEINTRESOURCE(IDI_ICON1) );	// アイコン設定
	if( !win.Create(hinst,"BMEmu2005") ) {
		DEBUG("Window create error\n","");
		return FALSE;
	}

	// Direct3D生成
	// フルスクリーンの640x480の32bitカラーにセットする。
	// ※2つ目の引数をFALSEにするとウインドウモードに出来る
	if( !dd.Create(win.hWnd,FALSE,640,480,32,0,FALSE) ) {
		DEBUG( "Direct3D create error\n","" );
		return FALSE;
	}

	// DirectSound生成
	if( !ds.Create(win.hWnd) ) {
		DEBUG( "DirectAudio create error\n","" );
		return FALSE;
	}

	// DirectInput生成
	if( !di.Create(win.hWnd,win.hInstance) ){
		DEBUG( "DirectInput生成失敗\n","" );
		return FALSE;
	}

	// キーボードを使う
	if( !di.CreateKeyboard() ) {
		DEBUG( "キーボード使用不可\n","" );
//		return FALSE;				// キーボードが使用できなくても起動可能とする
	}

	// ジョイスティックを使う
	if( !di.CreateJoystick() ) {
		DEBUG( "ジョイスティック使用不可\n","" );
//		return FALSE;				// ジョイスティックが使用できなくても起動可能とする
	}

	// マウスカーソルOFF
	win.ShowCursor( FALSE );

	return TRUE;
}

///////////////////////////////////////////////////////
// ロード済みデータの開放
///////////////////////////////////////////////////////
BOOL CGame::Flash( void )
{
	ds.Flash();
	dd.Clear();
	bms.Clear();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////
// ゲームメインルーチン
/////////////////////////////////////////////////////////////////////////
BOOL CGame::Run( HINSTANCE hinst )
{
	// ゲームメインループ
	MSG msg;
	BOOL bLoop=TRUE;
	while( bLoop ) {
		if( PeekMessage(&msg,NULL,0,0,PM_REMOVE) ) {
			if( msg.message==WM_QUIT ) {
				bLoop = FALSE;
				DEBUG( "WM_QUIT\n","" );
				break;
			}
			TranslateMessage( &msg );
        	DispatchMessage( &msg );
		}

		// メインゲーム処理分け
		switch( dwState )
		{
		case G_INIT:
			// 初期化
			if( !Init(hinst) ) {
				dwState = G_END;
				break;
			}
			dwState = G_MAININIT;
			break;

		case G_MAININIT:
			// メインゲーム初期化
			if( !GameInit() ) {
				dwState = G_END;
				break;
			}
			dwState = G_MAIN;
			break;

		case G_MAIN:
			// メインゲーム
			switch( GameRun(FALSE) )
			{
			case -1:
				// ESCAPE
				dwState = G_END;
				break;
			case 1:
				// ゲーム終了
				dwState = G_END;
				break;
			}
			break;

		case G_END:
			// 終了処理
			Flash();
			bLoop = FALSE;
			break;

		default:
			// 未定義のステート
			DEBUG( "異常終了\n","" );
			return FALSE;
		}


	}

	// マウスカーソルON
	win.ShowCursor( TRUE );
	win.Delete();

	// プログラム正常終了
	return TRUE;
}

///////////////////////////////////////////////////////
// メインゲーム初期化
///////////////////////////////////////////////////////
BOOL CGame::GameInit( void )
{
	// 画像ロード
	if( !dd.AddTexture(0,"FILES\\BM.TGA") ) {
		DEBUG( "BM.TGA load error\n","" );
		return FALSE;
	}

	// 背景
	dd.SetPutRange( 0,0,0,0,128,480 );		// 背景
	dd.SetPutRange( 1,0,224,0,256,256 );	// センターアニメーション

	// 小節ライン
	dd.SetPutRange( 2,0,292,258,126,2 );
 
	// キーを押したときの後ろの光
	dd.SetPutRange( 10,0,130,0,28,419 );	// 白
	dd.SetPutRange( 11,0,160,0,24,419 );	// 青
	dd.SetPutRange( 12,0,186,0,35,419 );	// 赤

	// オブジェ
	dd.SetPutRange( 15,0,130,422,28,6 );	// 白
	dd.SetPutRange( 16,0,160,422,24,6 );	// 青
	dd.SetPutRange( 17,0,186,422,35,6 );	// 赤

	// フラッシュ
	dd.SetPutRange( 20,0,224,258,60,50 );	// 白
	dd.SetPutRange( 21,0,224,308,52,50 );	// 青
	dd.SetPutRange( 22,0,224,358,68,50 );	// 赤

	// BMSロード
	if( !bms.Load("SAMPLE\\SAMPLE.BMS") ) {
		DEBUG( "SAMPLE\\SAMPLE.BMS error\n","" );
		return FALSE;
	}

	// サウンドデータのロード
	int i,j=-1;
	char buf[MAX_PATH];
	for( i=0;i<BMSMAXBUFFER;i++ ) {
		if( bms.GetWavFile(i) ) {
			if( j==-1 )
				j = i;
			sprintf( buf,"SAMPLE\\%s",bms.GetWavFile(i) );
			ds.AddSound( i,buf );
		}
	}
	if( j!=-1 ) {
		// キャッシング
		ds.SetVolume( j,0 );
		ds.Play( j );
		ds.Stop( j );
		ds.SetVolume( j,1 );
	}

	// 画像データのロード
	for( i=0;i<256;i++ ) {
		if( bms.GetBmpFile(i) ) {
			sprintf( buf,"SAMPLE\\%s",bms.GetBmpFile(i) );
			dd.AddTexture( i+256,buf );
			dd.SetPutRange( i+256,i+256,0,0,256,256 );
		}
	}

	// 変数の初期化
	iImageID		= 0;
	fScrMulti		= 1.0f;
	ZeroMemory( &mBmsNum,sizeof(mBmsNum) );

	ZeroMemory( &bOnKey,sizeof(bOnKey) );
	iScratchStatus = 0;
	ZeroMemory( &mFlashIndex,sizeof(mFlashIndex) );
	ZeroMemory( &mFlashCount,sizeof(mFlashCount) );
	ZeroMemory( &mBackKeyCount,sizeof(mBackKeyCount) );

	// マシンの周波数を取得
	LARGE_INTEGER freq;
	QueryPerformanceFrequency( &freq );
	llGlobalFreq = freq.QuadPart;

	// 現在の時間を記録
	LARGE_INTEGER li;
	QueryPerformanceCounter( &li );
	llStartTime = li.QuadPart;

	// タイマー初期化
	tm.Start( 60 );

	return TRUE;
}

///////////////////////////////////////////////////////
// ゲーム実行ループ
///////////////////////////////////////////////////////
HRESULT CGame::GameRun( BOOL demo )
{
	const int index[6]		= { 0,2,4,1,3,5 };				// 参照インデックスリスト
	const int obj_kind[6]	= { 0,1,0,1,0,2 };				// オブジェの種類
	const float obj_x[6]	= { 1,18,30,46,59,92 };			// オブジェ表示X座標
	const float obj_fx[6]	= { 14,29,44,58,72,109 };		// フラッシュ表示X座標

	int i,j,k;
	BYTE key[256];
	di.GetKeyboard( key );
	if( key[DIK_ESCAPE]&0x80 )
		return -1;

	// １つ目のジョイスティックの情報を取得
	DIJOYSTATE2 js;
	ZeroMemory( &js,sizeof(js) );
	di.GetJoystick( &js,0 );

	// 60FPSでのデータ操作
	for( k=0;k<tm.Run();k++ ) {
		// フラッシュ部
		for( j=0;j<6;j++ ) {
			for( i=0;i<3;i++ ) {
				if( mFlashCount[j][i]>0 )
					mFlashCount[j][i] -= 2;
			}
		}

		// 後ろのバックライト演出
		for( i=0;i<6;i++ ) {
			if( mBackKeyCount[i]>0 )
				mBackKeyCount[i]--;
		}

		// スクロール幅変更
		if( key[DIK_ADD]&0x80 ) {
			// テンキーの＋
			fScrMulti += 0.05f;
		}
		if( key[DIK_SUBTRACT]&0x80 ) {
			// テンキーの－
			fScrMulti -= 0.05f;
			if( fScrMulti<0.05f )
				fScrMulti = 0.05f;
		}
	}


	// 経過した時間から進んだBMSカウント値を算出
	LARGE_INTEGER li;
	QueryPerformanceCounter( &li );
	LONGLONG now_count = bms.GetCountFromTime( (double)(li.QuadPart-llStartTime)/llGlobalFreq );

	// BMSカウンタが曲の最大カウント値(+1小節分)を超えたら終了
	if( bms.GetMaxCount()+BMSDATA_RESOLUTION<=now_count )
		return 1;

	// BGMが１つでも設定されている場合はタイミングにあわせて再生
	for( i=mBmsNum[BMS_BACKMUSIC];i<bms.GetObjeNum(BMS_BACKMUSIC);i++ ) {
		LPBMSDATA bf = bms.GetObje( BMS_BACKMUSIC,i );
		if( now_count<bf->llTime )
			break;
		if( bf->bFlag ) {
			if( now_count>=bf->llTime ) {
				bf->bFlag = FALSE;
				ds.Reset( bf->lData );
				ds.Play( bf->lData );
				mBmsNum[BMS_BACKMUSIC] = i+1;
			}
		}
	}

	// アニメーション画像のセット
	for( i=mBmsNum[BMS_BACKANIME];i<bms.GetObjeNum(BMS_BACKANIME);i++ ) {
		LPBMSDATA bf = bms.GetObje( BMS_BACKANIME,i );
		if( bf->bFlag ) {
			if( now_count>=bf->llTime ) {
				bf->bFlag = FALSE;
				iImageID = bf->lData;
				mBmsNum[BMS_BACKANIME] = i+1;
			}
		}
	}

	if( demo ) {

		/////////////////////////////////////////////////////////////////////////////////////
		// コンピュータプレイ
		/////////////////////////////////////////////////////////////////////////////////////
		for( j=0;j<6;j++ ) {
			for( i=mBmsNum[index[j]+11+20];i<bms.GetObjeNum(index[j]+11);i++ ) {
				LPBMSDATA bf = bms.GetObje( index[j]+11,i );
				if( now_count<bf->llTime )
					break;
				if( bf->bFlag ) {
					if( now_count>=bf->llTime ) {
						// 当たり
						bf->bFlag = FALSE;				// フラグを消す
						ds.Reset( bf->lData );
						ds.Play( bf->lData );
						mBmsNum[index[j]+11+20] = i+1;
						// フラッシュ開始
						mFlashCount[index[j]][ mFlashIndex[index[j]] ] = 45;
						// 次のインデックスへ
						mFlashIndex[index[j]]++;
						if( mFlashIndex[index[j]]>2 )
							mFlashIndex[index[j]] = 0;
						mBackKeyCount[index[j]] = 30;
					}
				}
			}
		}

	} else {

		/////////////////////////////////////////////////////////////////////////////////////
		// 人間プレイ
		/////////////////////////////////////////////////////////////////////////////////////
		const int keynum[6] = { DIK_Z,DIK_S,DIK_X,DIK_D,DIK_C,DIK_SPACE };

		// 全ての入力機器から１つのボタン情報を算出
		BOOL in[7];
		ZeroMemory( &in,sizeof(in) );

		// キーボード入力チェック
		if( key[DIK_Z]&0x80 )			in[0] = TRUE;		// Z
		if( key[DIK_S]&0x80 )			in[1] = TRUE;		// S
		if( key[DIK_X]&0x80 )			in[2] = TRUE;		// X
		if( key[DIK_D]&0x80 )			in[3] = TRUE;		// D
		if( key[DIK_C]&0x80 )			in[4] = TRUE;		// C
		if( key[DIK_SPACE]&0x80 )		in[5] = TRUE;		// SPACE
		if( key[DIK_LSHIFT]&0x80 )		in[6] = TRUE;		// 左SHIFT

		// ジョイスティック入力チェック
		if( js.rgbButtons[3-1]&0x80 )	in[0] = TRUE;		// 白鍵(左)
		if( js.rgbButtons[5-1]&0x80 )	in[1] = TRUE;		// 黒鍵(左)
		if( js.rgbButtons[1-1]&0x80 )	in[2] = TRUE;		// 白鍵(中)
		if( js.rgbButtons[7-1]&0x80 )	in[3] = TRUE;		// 黒鍵(右)
		if( js.rgbButtons[2-1]&0x80 )	in[4] = TRUE;		// 白鍵(右)
		if( js.lY>500 )					in[5] = TRUE;		// 右回り
		if( js.lY<-500 )				in[6] = TRUE;		// 左回り

		// 仮想入力ハードウェア
		BOOL bButton[6];
		ZeroMemory( &bButton,sizeof(bButton) );
		bButton[0] = in[0];									// 白鍵(左)
		bButton[1] = in[1];									// 黒鍵(左)
		bButton[2] = in[2];									// 白鍵(中)
		bButton[3] = in[3];									// 黒鍵(右)
		bButton[4] = in[4];									// 白鍵(右)
		// スクラッチの処理
		if( in[5] ) {
			// 右回転
			if( iScratchStatus!=1 ) {
				// 以前が停止か左回りならスクラッチON
				bButton[5] = TRUE;
				iScratchStatus = 1;
			}
		} else if( in[6] ) {
			// 左回転
			if( iScratchStatus!=-1 ) {
				// 以前が停止か右回りならスクラッチON
				bButton[5] = TRUE;
				iScratchStatus = -1;
			}
		} else {
			// 回転停止
			iScratchStatus = 0;
		}

		// 入力判定
		for( j=0;j<6;j++ ) {
			// キーが押されている場合
			if( bButton[j] ) {
				// キー押し背景のフェードアウト用
				mBackKeyCount[j] = 30;
				// すでにキーが押されていないか
				if( !bOnKey[j] ) {
					// 押されていなかった場合は押されたことにする
					bOnKey[j] = TRUE;
					// 当たり判定
					for( i=mBmsNum[j+11+20];i<bms.GetObjeNum(j+11);i++ ) {
						LPBMSDATA bm = (LPBMSDATA)bms.GetObje( j+11,i );
						if( bm->bFlag ) {
							// オブジェが存在していて
							if( (now_count-625)<bm->llTime && bm->llTime<(now_count+625) ) {
								// 範囲内で弾いたなら
								bm->bFlag = FALSE;						// オブジェを消す
								ds.Reset( bm->lData );
								ds.Play( bm->lData );					// そのオブジェの音を再生
								// フラッシュ画像の定義
								mFlashCount[j][mFlashIndex[j]] = 45;
								mFlashIndex[j]++;
								if( mFlashIndex[j]>2 )
									mFlashIndex[j] = 0;
								mBmsNum[j+11+20] = i+1;
								break;
							}
						}
					}
				} else {
					// すでに押されていたら何もしない
				}
			} else {
				// キー長押しクリア
				bOnKey[j] = FALSE;
			}

			// 見逃し判定
			for( i=mBmsNum[j+11+20];i<bms.GetObjeNum(j+11);i++ ) {
				LPBMSDATA bm = (LPBMSDATA)bms.GetObje( j+11,i );
				if( bm->bFlag ) {
					// オブジェが存在していて
					if( bm->llTime<(now_count-625) ) {
						// 見逃した場合はここでPOOR処理を行う

						// オブジェを消す
						bm->bFlag = FALSE;
						// 判定オブジェをその次からに変更
						mBmsNum[j+11+20] = i+1;
						break;
					}
				}
			}
		}
	}




	////////////////////////////////////////////////////////////////////////////////////
	// 描画処理
	////////////////////////////////////////////////////////////////////////////////////
	dd.DrawBegin();

	// アニメーション表示
	dd.SetRenderState( D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA );	// 通常合成
	if( iImageID>0 ) {
		// アニメーション画像が存在する場合
		dd.SetPutStatus( iImageID+256,1,480.0f/256.0f );		// その画像を画面いっぱいに拡大
		dd.Put2( iImageID+256,320,240 );						// 真ん中に表示
	} else {
		// 無ければダミーを表示
		dd.SetPutStatus( 1,1,480.0f/256.0f );					// 画面いっぱいに拡大
		dd.Put2( 1,320,240 );									// 真ん中に表示
	}

	// 背景
	dd.Put( 0,0,0 );											// 左側のレーン背景
	dd.Put( 0,512,0 );											// 右側のレーン背景

	// キーのバックライト演出
	dd.SetRenderState( D3DRS_DESTBLEND,D3DBLEND_ONE );			// 加算合成
	for( i=0;i<6;i++ ) {
		if( mBackKeyCount[i]>0 ) {
			dd.SetPutStatus( obj_kind[i]+10,(float)mBackKeyCount[i]/30.0f,1.0f,0 );		// 徐々にフェードアウト
			dd.Put( obj_kind[i]+10,obj_x[i],0 );										// レーンの種類ごとにバックライト画像を表示
		}
	}

	// 小節
	dd.SetRenderState( D3DRS_DESTBLEND,D3DBLEND_ONE );			// 加算合成
	for( i=mBmsNum[0];i<bms.GetBarNum();i++ ) {
		LPBMSBAR bar = bms.GetBar(i);
		int yy = (int)((double)( bar->llTime - now_count ) / BMSDATA_RESOLUTION * (int)(fScrMulti * 192));
		// 判定ラインより下なら次回からその次の小節から参照する
		if( yy<0 )
			mBmsNum[0] = i + 1;
		// 画面の上より外ならばその先は全て描画スキップ
		if( yy>412+2 )
			break;
		// 画面内なら描画
		dd.Put( 2,1,(float)(413-yy+5) );
	}

	// オブジェクト
	dd.SetRenderState( D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA );	// 通常合成
	for( j=0;j<6;j++ ) {
		int size = bms.GetObjeNum( index[j]+11 );
		for( i=mBmsNum[index[j]+11];i<size;i++ ) {
			LPBMSDATA bm = bms.GetObje( index[j]+11,i );
			int yy = (int)((double)( bm->llTime - now_count ) / BMSDATA_RESOLUTION * (int)(fScrMulti * 192));
			// 判定ラインより下なら次回からその次のオブジェから参照する
			if( yy<0 )
				mBmsNum[index[j]+11] = i + 1;
			// 画面の上より外ならばその先は全て描画スキップ
			if( yy>412+6 )
				break;
			// 画面内なら描画
			dd.Put( 15+obj_kind[index[j]],obj_x[index[j]],(float)(413-yy) );
		}
	}

	// フラッシュ表示
	dd.SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );			// 加算合成
	for( j=0;j<6;j++ ) {
		for( i=0;i<3;i++ ) {
			if( mFlashCount[j][i]>0 ) {
				// 演出が存在する場合のみ表示
				dd.SetPutStatus(
					obj_kind[j]+20,
					1-(float)cos( (double)mFlashCount[j][i]*2*3.14159/180 ),
					1+(float)cos( (double)mFlashCount[j][i]*2*3.14159/180 ),
					0 );
				dd.Put2( obj_kind[j]+20,obj_fx[j],416 );
			}
		}
	}

	dd.DrawEnd();

	// 継続
	return 0;
}
