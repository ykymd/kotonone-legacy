#ifndef _CBMSPRO_H
#define _CBMSPRO_H
///////////////////////////////////////////////////////////////////////////////////
// CBmsPro : BMSクラス チュートリアルバージョン v3.0                             //
//                                                                               //
// このソースコードは自由に改変して使用可能です。                                //
// また商用利用も可能ですが、すべての環境で正しく動作する保障はありません。      //
//                          http://www.charatsoft.com/                           //
///////////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <stdio.h>
#include <unordered_map>
#include <vector>

#include "BMSHeader.h"

const LONG BMSDATA_RESOLUTION = 9600;		// １小節のカウント値
const LONG BMSMAXBUFFER = 256;				// 00～FFまでのバッファ数

// BMSチャンネル定義
enum class CHANNEL
{
	BACKMUSIC	= 0x01,		// その位置にきたら、自動的に再生されるWAVを指定します
	STRETCH		= 0x02,		// その小節の長さを定義したデータ倍します（１０進数、小数ともに使用可）
	TEMPO		= 0x03,		// 再生テンポ（ＢＰＭ / １分間の四分音符数）の途中変更（１６進数）
	BACKANIME	= 0x04,		// バックグラウンドアニメーション機能
	EXTENEDOBJ	= 0x05,		// 落下してくるオブジェを別のキャラクターナンバーのものにすり替える機能
	CHGPOORANI	= 0x06,		// POORを出したときに表示される画像を変更
	LAYERANIME	= 0x07,		// Ch.04で指定したBGAの上にかぶせるBMPを指定できます
	BPMINDEX	= 0x08,		// BPMのインデックス指定(新)
};

// BMSコマンドの一覧
enum class Command {
	PLAYER,
	GENRE,
	TITLE,
	ARTIST,
	BPM,
	MIDIFILE,
	PLAYLEVEL,
	RANK,
	VOLWAV,
	TOTAL,
	STAGEFILE,
	WAV,
	BMP,
	SONG_GUID,
	MOVIE,
	OBJECT,
	UNKNOWN,
};

// BMSデータ情報
typedef struct _BMSDATA {
	BOOL		bFlag;					// 存在しているか
	LONGLONG	llTime;					// 鳴らすタイミングデータ
	LONG		lData;					// 鳴らすデータ（0～255）
	float		fData;					// 小数点データ（テンポなど）
} BMSDATA,*LPBMSDATA;

// 小節情報
typedef struct _BMSBAR {
	BOOL		bFlag;					// 存在するか（無視）
	LONGLONG	llTime;					// 表示する時間
	LONG		lData;					// 小節についての追加情報（無視）
} BMSBAR,*LPBMSBAR;

// BPMインデックス
typedef struct _BPMINDEX {
	float		fBpm;					// BPM
} BPMINDEX,*LPBPMINDEX;

// 小節の長さ管理
typedef struct _BARMAGNI {
	long	iLines;						// 小節番号
	float	fMagni;						// その倍率
} BARMAGNI,*LPBARMAGNI;

// BMSファイルヘッダ情報
/*
typedef struct _BMSHEADER {
	long		Player;					// プレイモード
	string		Genre;					// データのジャンル
	string		Title;					// データのタイトル
	string		Artist;					// データの製作者
	float		Bpm;					// データのテンポ（初期値は130）
	char		Midifile[MAX_PATH];		// バックグラウンドで流すMIDIファイル
	long		Playlevel;				// データの難易度
	long		Rank;					// データの判定ランク
	long		WavVol;					// 音量を元の何％にするか
	long		Total;					// ゲージの増量を設定
	char		StagePic[MAX_PATH];		// 曲開始字に表示する画像
	char		Movie[MAX_PATH];		// ムービーファイル名
	GUID		guid;					// 曲のGUID(特殊仕様)
	long		EndBar;					// 終了小節
	LONGLONG	MaxCount;				// 最大のカウント数
	BPMINDEX	mBpmIndex[BMSMAXBUFFER];// テンポインデックス
} BMSHEADER,*LPBMSHEADER;
*/




////////////////////////////////////////////////////////////////////////////////////
// BMSクラス
////////////////////////////////////////////////////////////////////////////////////
class CBmsPro {
protected:
	BMSHeader*			bmsHeader;						// ヘッダデータ

	LPBMSDATA			mBmsData[BMSMAXBUFFER];			// 実データ
	int					iBmsData[BMSMAXBUFFER];			// 実データのそれぞれの数

	std::unordered_map<int, std::string> wavData;		// WAVのファイル名
	std::unordered_map<int, std::string> bmpData;		// BMPのファイル名

	std::vector<LONGLONG> barData;	// 小節のデータ
	int					iBmsBar;						// 小節の個数

	std::vector<std::tuple<long, float>> barMagni;		// 小節の倍率
	int					iBarMagni;						// 小節倍率の個数

	char				mLastError[1024];				// エラーが発生した場合の文字列

private:
	int		atoi1610( char *s );													// 16進数文字列を数字に変換
	Command	getCommandPart(const std::string line);										// １行分のデータ中のコマンド番号を返す	
	std::string getCommandString(std::string line);									// コマンドの文字列部分を返す（'\n'は削除 ':'も区切りとして処理）
	BOOL	LoadBmsData( LPSTR file );												// BMSデータの読み込み
	GUID	GetGuidFromString( const char* mGuid );										// GUID文字列を解析する
	BOOL	NumToStr36( int num,char *dst,int keta=-1 );							// 10進数値を桁付きの36進数文字へ変換
	BOOL	LineCompact( char *src,char *dst );										// データをコンパクトにする

public:
	CBmsPro();																		// コンストラクタ
	virtual ~CBmsPro();																// デストラクタ

	// 基本メソッド
	BOOL	Clear( void );															// データの初期化
	BOOL	GetHeader( LPSTR file );												// BMSヘッダ情報だけを取り出す
	BOOL	Load( LPSTR file );														// BMSファイルのロード
	BOOL	AddNoteData( CHANNEL ch,LONGLONG startpos,LONG dat );							// １つのデータを追加（ソートはされない）
	BOOL	Sort( int ch );															// 指定チャンネルをソートする
	BOOL	Save( LPSTR file,BOOL same=TRUE );										// データをセーブする
	BOOL	Restart( void );														// リスタート用にオブジェのフラグを元に戻す
	LONGLONG GetCountFromTime( double sec );										// 時間から内部カウント値を計算

public:
	inline char	*GetLastError( void ) { return mLastError; }

	/*
	// ゲーム内必須メソッド
	inline LONGLONG		GetMaxCount( void ) { return mBH.llMaxCount; }				// ゲーム内の最大のカウント値
	inline LONG			GetObjeNum( int ch ) { return iBmsData[ch]; }				// 指定チャネルのデータ数を返す
	inline LPBMSDATA	GetObje( int ch,int num ) { return &mBmsData[ch][num]; }	// チャネルと配列番号でデータを取得する
	inline LPBMSHEADER	GetHeaderInfo( void ) { return &mBH; }						// ヘッダ情報構造体を返す

	// ヘッダ情報を個別に取得
	inline LPSTR		GetArtist( void ) { return mBH.mArtist; }					// アーティスト名
	inline LONG			GetBarNum( void ) { return iBmsBar; }						// 小節バーの個数
	inline LPBMSBAR		GetBar( int num ) { return &mBmsBar[num]; }					// 小節バーのデータ
	inline LPSTR		GetBmpFile( int num ) { return mBmp[num]; }					// 使用しているBMPファイル名
	inline float		GetBpm( void ) { return mBH.fBpm; }							// 初期時のテンポ数
	inline LONG			GetGageIncrease( void ) { return mBH.lTotal; }				// ゲージの増量
	inline LPSTR		GetGenre( void ) { return mBH.mGenre; }						// ジャンル名
	inline GUID*		GetGuid( void ) { return &mBH.guid; }						// GUIDの取得
	inline LONG			GetRank( void ) { return mBH.lRank; }						// ランク
	inline LPSTR		GetMidiFile( void ) { return mBH.mMidifile; }				// MIDIファイル名
	inline LPSTR		GetMovieFile( void ) { return mBH.mMovie; }					// MOVIEファイル名
	inline LONG			GetPlayer( void ) { return mBH.lPlayer; }					// プレイヤーナンバー
	inline LONG			GetPlayLevel( void ) { return mBH.lPlaylevel; }				// プレイレベル
	inline LPSTR		GetStagePic( void ) { return mBH.mStagePic; }				// ステージ画像名
	inline LPSTR		GetTitle( void ) { return mBH.mTitle; }						// タイトル名
	inline LONG			GetWavVol( void ) { return mBH.lWavVol; }					// 音量を元の何％にするか
	inline LPSTR		GetWavFile( int num ) { return mWav[num]; }					// 使用しているWAVファイル名
	inline LONG			GetBarMagniNum( void ) { return iBarMagni; }				// 小節倍率変更データの数
	*/
};

#endif
