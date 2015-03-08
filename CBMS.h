#include <Windows.h>
#include <string>

#pragma once

#define BMSMAXBUFFER 256

// BMSチャンネル定義
#define BMS_BACKMUSIC		0x01		// その位置にきたら、自動的に再生されるWAVを指定します
#define BMS_STRETCH			0x02		// その小節の長さを定義したデータ倍します（１０進数、小数ともに使用可）
#define BMS_TEMPO			0x03		// 再生テンポ（ＢＰＭ / １分間の四分音符数）の途中変更（１６進数）
#define BMS_BACKANIME		0x04		// バックグラウンドアニメーション機能
#define BMS_EXTENEDOBJ		0x05		// 落下してくるオブジェを別のキャラクターナンバーのものにすり替える機能
#define BMS_CHGPOORANI		0x06		// POORを出したときに表示される画像を変更
#define BMS_LAYERANIME		0x07		// Ch.04で指定したBGAの上にかぶせるBMPを指定できます
#define BMS_BPMINDEX		0x08		// BPMのインデックス指定(新)

const long long BMSDATA_RESOLUTION = 9600;	//1小節のカウント数

namespace Kotone {

	enum Command {
		PLAYER = 0,
		GENRE = 1,
		TITLE = 2,
		ARTIST = 3,
		BPM = 4,
		MIDIFILE = 5,
		PLAYLEVEL = 6,
		RANK = 7,
		VOLWAV = 8,
		TOTAL = 9,
		STAGEFILE = 10,
		WAV = 11,
		BMP = 12,
		SONG_GUID = 13,
		MOVIE = 14,
		OBJECT = -1,
		UNKNOWN = -2,
	};

	enum JUDGE_TYPE
	{
		Critical,       // ピッタリ
		Near,           // ややずれてる
		Failed,         // 失敗
		None,           //　無視
	};

	// BMSデータ情報
	typedef struct _BMSDATA {
		bool flag;				//存在しているか
		long long time;	//鳴らすタイミング
		long data;			//鳴らすデータ
		float fData;			//小節点データ
		JUDGE_TYPE judge;	//判定
	} BMSDATA, *LPBMSDATA;

	// 小節データ
	typedef struct _BMSBAR
	{
		bool flag;				//存在するか
		long long time;	//表示する時間
		long data;			//小節についての追加情報
	}BMSBAR, *LPBMSBAR;

	// BPMインデックス
	typedef struct _BPMINDEX {
		float		Bpm;					// BPM
	} BPMINDEX;

	// 小節の長さ管理
	typedef struct _BARMAGNI {
		long lines;		//小節番号
		float magni;		//倍率
	}BARMAGNI, *LPBARMAGNI;

	typedef struct _BMSHEADER
	{
		long Player;
		std::string Genre;
		std::string Title;
		std::string Artist;
		float bpm;
		std::string midifile;
		long PlayLevel;
		long Rank;
		long WavVol;
		long Total;
		std::string StagePic;
		std::string Movie;
		GUID guid;
		long EndBar;
		LONGLONG MaxCount;
		BPMINDEX BPMIndex[BMSMAXBUFFER];

	} BMSHEADER;

	// セレクト画面に必要なデータ
	// セレクト画面でデモ音源は流さない
	typedef struct _SelectSceneData
	{
		std::string Title;			//タイトル
		std::string Composer;	//コンポーザー
		std::string Genre;		//ジャンル
		int bpm = 0;				//BPM
		std::string fileName;	// ファイル名
		int levels[3];				//難易度
		int levelNum = 0;
	} SelectSceneData;

	class CBMS
	{
	public:
		BMSHEADER bHeader;	//ヘッダデータ

		LPBMSDATA bmsData[BMSMAXBUFFER];	//実データ
		int numBmsData[BMSMAXBUFFER];			//実データのチャンネルそれぞれの数

		//int numPastNote[8];	//既に判定済みのノーツの数

		LPBMSBAR bmsBar;
		int numBmsBar;	//小節の個数

		LPBARMAGNI barMagni;	//小節の倍率データ
		int numBarMagni;	//小節倍率の個数

		std::string wavData;
		float ScrMulti = 1.0f;	//スクロール速度

		CBMS();
		~CBMS();
		bool Load(LPSTR file);
		bool UpdateSelectSceneData(std::string file, SelectSceneData* header);
		bool GetSelectSceneData(std::string file, SelectSceneData* dat);
		bool GetHeader(LPSTR file);
		bool GetData(LPSTR file);
		Command GetCommand(std::string cmd);
		bool GetCommandString(std::string src, std::string* dst);
		bool AddNoteData(unsigned int ch_num, long long startpos, long dat);
		bool sort(int ch);
		long long GetCountFromTime(double sec);

		inline long long GetMaxCount(void) { return bHeader.MaxCount; }
		inline LPBMSBAR GetBar(int num) { return &bmsBar[num]; }
		inline long GetBarNum(void) { return numBmsBar; }
		inline LPBMSDATA GetObj(int ch, int num) { return &bmsData[ch][num]; }
		inline long GetObjeNum(int ch) { return numBmsData[ch]; }
		inline std::string GetWavFile(void) { return wavData; }
	};
}
