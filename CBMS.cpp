#include "CBMS.h"
#include <iostream>
#include <fstream>
#include <tchar.h>
#include <string>
#include "DxLib.h"

// メモ(チャンネル番号)
// レーン1~4はロングノーツオンリーなのでチャンネル番号は50番代
// レーン1:56
// レーン2:51
// レーン3:52
// レーン4:53
//
// レーン5:14
// レーン6:15
// レーン7:18
// レーン8:19

#define FREE(x)		{ if(x) {free(x); x=NULL;} }

using namespace Kotone;

CBMS::CBMS()
{
	bHeader.Player = 1;
	bHeader.bpm = 120;

	bmsBar = NULL;
	numBmsBar = 0;

	barMagni = NULL;
	numBarMagni = 0;

	ZeroMemory(&bHeader, sizeof(bHeader));
	ZeroMemory(&bmsData, sizeof(bmsData));
	ZeroMemory(&numBmsData, sizeof(numBmsData));
	ZeroMemory(&bmsBar, sizeof(bmsBar));
	ZeroMemory(&barMagni, sizeof(barMagni));
}


CBMS::~CBMS()
{
	for (int i = 0; i<BMSMAXBUFFER; i++) {
		FREE(bmsData[i]);		// BMSデータ領域をクリア
		numBmsData[i] = 0;			// データの数をクリア
	}
}

Command CBMS::GetCommand(const std::string cmd)
{
	const char *command[15] = {
		"PLAYER\0",
		"GENRE\0",
		"TITLE\0",
		"ARTIST\0",
		"BPM\0",
		"MIDIFILE\0",
		"PLAYLEVEL\0",
		"RANK\0",
		"VOLWAV\0",
		"TOTAL\0",
		"StageFile\0",
		"WAV\0",
		"BMP\0",
		"GUID\0",
		"MOVIE",
	};

	// 検索ルーチン
	int i;
	for (i = 0; i<15; i++) {
		if (_strnicmp(cmd.data()+1, command[i], strlen(command[i])) == 0)
			return Command(i);	// コマンドならその番号を返す
	}

	// 先頭が'#iiiii'か
	BOOL obj = TRUE;
	for (i = 0; i<5; i++) {
		if (cmd[i+1]<'0' || cmd[i+1]>'9') {
			obj = FALSE;
			break;
		}
	}

	// オブジェ配置か判定
	if (obj) return OBJECT;

	// 判別不可能文字なら
	return UNKNOWN;
}

// ファイルアドレスからデータをロードする
bool CBMS::Load( LPSTR file )
{
	if (!GetHeader(file)){
		std::cout << "ヘッダ読み取りエラー" << std::endl;
		return false;
	}
	std::cout << "ヘッダ読み取り成功！" << std::endl;

	if (!GetData(file)){
		std::cout << "データ読み取りエラー" << std::endl;
		return false;
	}
	std::cout << "データ読み取り成功！" << std::endl;

	return true;
}

bool CBMS::GetCommandString(std::string src, std::string* dst)
{
	int i = 0;

	// データ部分までのポインタを取得
	while (true){
		if (src[i] == ' ' || src[i] == '　' || src[i] == ':'){
			i++;
			break;
		}
		else if (src[i] == '\n' || src[i] == NULL){
			return false;
		}
		i++;
	}

	*dst = src.substr(i).c_str();

	return true;
}

// ノーツのデータをキューに格納する
bool CBMS::AddNoteData(unsigned int ch_num, long long startpos, long dat)
{
	// チャンネル番号が0~255以内かチェック
	if (ch_num & ~255) {
		std::cout << "チャンネル数:" << ch_num << "はサポート外です(0~255)" << std::endl;
		return false;
	}

	if (ch_num == BMS_STRETCH) return false;

	if (dat == 0) return true;

	switch (ch_num)
	{
	case BMS_BPMINDEX:
		numBmsData[BMS_TEMPO]++;
		bmsData[BMS_TEMPO] = (LPBMSDATA)realloc(bmsData[BMS_TEMPO], numBmsData[BMS_TEMPO] * sizeof(BMSDATA));
		ZeroMemory(&bmsData[BMS_TEMPO][numBmsData[BMS_TEMPO] - 1], sizeof(BMSDATA));
		//bmsData[BMS_TEMPO][numBmsData[BMS_TEMPO] - 1] = 0;
		bmsData[BMS_TEMPO][numBmsData[BMS_TEMPO] - 1].flag = true;
		bmsData[BMS_TEMPO][numBmsData[BMS_TEMPO] - 1].time = startpos;
		bmsData[BMS_TEMPO][numBmsData[BMS_TEMPO] - 1].data = (long)bHeader.BPMIndex[dat].Bpm;
		bmsData[BMS_TEMPO][numBmsData[BMS_TEMPO] - 1].fData = bHeader.BPMIndex[dat].Bpm;
		bmsData[BMS_TEMPO][numBmsData[BMS_TEMPO] - 1].judge = None;		//判定は無視判定をデフォとする
		break;
	default:
		// データを追加
		numBmsData[ch_num]++;
		bmsData[ch_num] = (LPBMSDATA)realloc(bmsData[ch_num], numBmsData[ch_num] * sizeof(BMSDATA));
		ZeroMemory(&bmsData[ch_num][numBmsData[ch_num] - 1], sizeof(BMSDATA));
		bmsData[ch_num][numBmsData[ch_num] - 1].flag = true;
		bmsData[ch_num][numBmsData[ch_num] - 1].time = startpos;
		bmsData[ch_num][numBmsData[ch_num] - 1].data = dat;
		bmsData[ch_num][numBmsData[ch_num] - 1].fData = (float)dat;
		break;
	}

	return true;
}

// 難易度をアップデートする
bool CBMS::UpdateSelectSceneData(std::string file, SelectSceneData* header)
{
	std::ifstream ifs(file);
	std::string str;

	if (ifs.fail() || header == NULL)
	{
		std::cerr << "失敗！" << std::endl;
		return false;
	}

	// 1行ずつ読み込む
	while (getline(ifs, str))
	{
		// コマンド以外は読み飛ばす
		if (str[0] != '#') continue;

		// 改行文字を消去
		if (str[str.size() - 1] == '\n')
			str[str.size() - 1] = NULL;

		// 命令列からコマンド部分を取得
		Command cmd = GetCommand(str);
		if (cmd == UNKNOWN)  {
			std::cout << "判別不可能文字:" << str << std::endl;
			continue;
		}

		// 命令列からデータ部分を取得
		std::string data;
		GetCommandString(str, &data);

		if (cmd == PLAYLEVEL){
			std::string tmp = file.substr(file.size() - 8, 4);
			if (tmp == "easy") header->levels[0] = std::stoi(data);
			else if (tmp == "norm") header->levels[1] = std::stoi(data);
			else if (tmp == "hard") header->levels[2] = std::stoi(data);
			header->levelNum++;
			break;
		}
	}

	return true;
}

// セレクト画面用のデータを取得
bool CBMS::GetSelectSceneData(std::string file, SelectSceneData* header)
{
	std::ifstream ifs(file);
	std::string str;

	if (ifs.fail() || header == NULL)
	{
		std::cerr << "失敗！" << std::endl;
		return false;
	}

	// 1行ずつ読み込む
	while (getline(ifs, str))
	{
		// コマンド以外は読み飛ばす
		if (str[0] != '#') continue;

		// 改行文字を消去
		if (str[str.size() - 1] == '\n')
			str[str.size() - 1] = NULL;

		// 命令列からコマンド部分を取得
		Command cmd = GetCommand(str);
		if (cmd == UNKNOWN)  {
			std::cout << "判別不可能文字:" << str << std::endl;
			continue;
		}

		// 命令列からデータ部分を取得
		std::string data;
		GetCommandString(str, &data);

		// パラメータの代入
		switch (cmd)
		{
		case GENRE:
			header->Genre = data;
			std::cout << "ジャンル     " << data << std::endl;
			break;
		case TITLE:
			header->Title = data;
			std::cout << "タイトル     " << data << std::endl;
			break;
		case ARTIST:
			header->Composer = data;
			std::cout << "アーティスト " << data << std::endl;
			break;
		case BPM:		// BPM
			if (str[4] == ' ' || str[4] == '　') {
				// ヘッダなら
				header->bpm = std::stoi(data);
				std::cout << "ベースBPM    " << data << std::endl;
			}
			break;
		case PLAYLEVEL:
			// 複数難易度を持つ曲だったとき
			std::string tmp = file.substr(file.size() - 8, 4);
			std::cout << "LEVEL:" << data << std::endl;
			if (tmp == "easy") header->levels[0] = std::stoi(data);
			else if (tmp == "norm") header->levels[1] = std::stoi(data);
			else if (tmp == "hard") header->levels[2] = std::stoi(data);
			header->levelNum++;
			break;
		}
	}
	return true;
}

// 譜面データのヘッダーを取得する
bool CBMS::GetHeader( LPSTR file )
{
	//std::ifstream ifs(file);
	std::string str;

	char tmp[BMSMAXBUFFER];

	int FileHandle = DxLib::FileRead_open(file);
	//FileRead_gets(str, BMSMAXBUFFER, FileHandle);

	while (DxLib::FileRead_eof(FileHandle) == 0)
	{
		FileRead_gets(tmp, BMSMAXBUFFER, FileHandle);
		str = std::string(tmp);

		// コマンド以外は読み飛ばす
		if (str[0] != '#') continue;

		// 改行文字を消去
		if (str[str.size() - 1] == '\n')
			str[str.size() - 1] = NULL;

		// 命令列からコマンド部分を取得
		Command cmd = GetCommand(str);
		if (cmd == UNKNOWN)  {
			std::cout << "判別不可能文字:" << str << std::endl;
			continue;
		}

		// 命令列からデータ部分を取得
		std::string data;
		GetCommandString(str, &data);

		// パラメータの代入
		switch (cmd)
		{
		case PLAYER:
			bHeader.Player = std::stoi(data);
			std::cout << "PLAYER       " << bHeader.Player << std::endl;
			break;
		case GENRE:
			bHeader.Genre = data;
			std::cout << "ジャンル     " << bHeader.Genre << std::endl;
			break;
		case TITLE:
			bHeader.Title = data;
			std::cout << "タイトル     " << bHeader.Title << std::endl;
			break;
		case ARTIST:
			bHeader.Artist = data;
			std::cout << "アーティスト " << bHeader.Artist << std::endl;
			break;
		case BPM:		// BPM
			if (str[4] == ' ' || str[4] == '　') {
				// ヘッダなら
				bHeader.bpm = (float)std::stof(data);
				//AddData(BMS_TEMPO, 0, (LONG)mBH.fBpm);
				AddNoteData(BMS_TEMPO, 0, (long)bHeader.bpm);
				std::cout << "ベースBPM    " << bHeader.bpm << std::endl;
			}
			else {
				// インデックスなら
				bHeader.BPMIndex[std::stoi(str.substr(4, 2))].Bpm = std::stof(data);
				std::cout << "インデックスBPM " << bHeader.BPMIndex[std::stoi(str.substr(4, 2))].Bpm << std::endl;
			}
			break;
		case MIDIFILE:
			bHeader.midifile = data;
			std::cout << "MIDIFILE      " << bHeader.midifile << std::endl;
			break;
		case PLAYLEVEL:
			bHeader.PlayLevel = std::stoi(data);
			break;
		case RANK:
			bHeader.Rank = std::stoi(data);
			break;
		case VOLWAV:
			bHeader.WavVol = std::stoi(data);
			break;
		case TOTAL:		// TOTAL
			bHeader.Total = std::stoi(data);
			break;
		case STAGEFILE:	// StageFile
			//DEBUG("StageImage-[%s]\n", mBH.mStagePic);
			bHeader.StagePic = data;
			break;
		case WAV:
			wavData = data;
			std::cout << "楽曲:" << data << std::endl;
			break;
		case BMP:
			break;
		case SONG_GUID:
			break;
		case MOVIE:	// MOVIE
			break;
		default:
			int iLines;
			int iChannel;
			// 小節番号の取得
			iLines = std::stoi(str.substr(1, 3));
			// チャンネル番号の取得
			iChannel = std::stoi(str.substr(4, 2));	// 数字化
			if (iChannel == BMS_STRETCH) {
				// 小節の倍率変更命令の場合
				//				DEBUG("小節倍率変換 [%s]倍\n",str);
				numBarMagni++;
				barMagni = (LPBARMAGNI)realloc(barMagni, sizeof(BARMAGNI)*numBarMagni);
				barMagni[numBarMagni - 1].lines = iLines;
				barMagni[numBarMagni - 1].magni = std::stof(data);
			}
			// 小節番号の最大値を記憶する
			if (bHeader.EndBar < iLines) bHeader.EndBar = iLines;

			break;
		}
	}

	FileRead_close(FileHandle);

	// 小節倍率データを元に小節バーを作成
	long long lastCount = 0;
	for (int i = 0; i <= bHeader.EndBar + 1; i++)
	{
		// 小節を追加
		numBmsBar++;
		bmsBar = (LPBMSBAR)realloc(bmsBar, sizeof(BMSBAR)*numBmsBar);
		bmsBar[numBmsBar - 1].flag = true;
		bmsBar[numBmsBar - 1].time = lastCount;
		long long add = BMSDATA_RESOLUTION;	//加算数を定義

		// 現在の小節で倍率を変換しているか
		for (int j = 0; j < numBarMagni; j++)
		{
			if (barMagni[j].lines == i)
			{
				// 倍率変更命令が存在する場合、1小節あたりのカウント加算値を変化させる
				add = (long long)((float)BMSDATA_RESOLUTION * barMagni[j].magni);
				break;
			}
		}

		lastCount += add;	//次小節の開始位置までカウント値を加算する

		// 最大の泊数を計算
		if (i < bHeader.EndBar && bHeader.MaxCount < lastCount)
			bHeader.MaxCount = lastCount;
	}

	std::cout << "NumBmsBar:" << numBmsBar << std::endl;

	return true;
}

bool CBMS::GetData( LPSTR file )
{
	std::ifstream ifs(file);
	std::string str;

	char tmp[BMSMAXBUFFER];
	int FileHandle = DxLib::FileRead_open(file);

	// 1行ずつ読み込む
	while (DxLib::FileRead_eof(FileHandle) == 0)
	{
		FileRead_gets(tmp, BMSMAXBUFFER, FileHandle);
		str = std::string(tmp);

		// コマンド以外は読み飛ばす
		if (str[0] != '#') continue;

		// 改行文字を消去
		if (str[str.size() - 1] == '\n')
			str[str.size() - 1] = NULL;

		// 命令列からコマンド部分を取得
		Command cmd = GetCommand(str);
		if (cmd != OBJECT)  continue;

		std::string data;
		if (!GetCommandString(str, &data))  {
			std::cout << "パラメータ文字列取得エラー" << data << std::endl;
		}

		// データであれば意味解析を行う
		int barLines;		// 現在の小節番号
		int channel;		// 現在のチャンネル番号
		char num[4] = {0};	//汎用バッファ

		// チャンネル番号の取得
		channel = std::stoi(str.substr(4,2));

		// 小節の倍率変更命令の場合はキャンセル
		if (channel == BMS_STRETCH) continue;

		// 小節番号の取得
		barLines = std::stoi(str.substr(1, 3));

		// データが存在するか
		int size = data.size();
		if (size < 1) {
			std::cout << "データ部分が定義されていません:" << str << std::endl;
			continue;
		}

		// データ部分の文字数が偶数個か
		if (size%2 == 1) {
			std::cout << "データ部分の文字数が偶数個ではありません:" << str << std::endl;
			return false;
		}

		// データを追加する
		size = size / 2;

		// 小節区分からデータ配置場所を特定
		long long startCount = 0;	// その小節の最初のカウント値
		int chkLine = 0;					// 計算中の小節

		// 現在の小節の1つ前までを処理
		while (chkLine < barLines) {
			// 加算するポインタ数を定義
			long long add = BMSDATA_RESOLUTION;

			// 現在チェック中の小節に倍率変更がないか検索する
			// サポート外

			//  現在までの小節カウントを加算
			startCount += add;

			// 次の小節へ
			chkLine++;
		}

		// 現在の小節の幅を求める
		float currentBarResolution = (float)BMSDATA_RESOLUTION / (float)size;

		// その小節に倍率変更命令があるか

		// 実データを追加
		for (int i = 0; i < size; i++) {
			// 16進数表記の文字列を10進数の数値に直す。
			long hex = std::stoi(data.substr(i * 2, 2), nullptr, 16);
			if (hex > 0) {
				AddNoteData(channel, startCount + (long long)(currentBarResolution*(float)i),hex);
			}
		}
	}

	// ソート
	for (int i = 0; i < BMSMAXBUFFER; i++)
		sort(i);

	return true;
}

// 指定チャンネルのデータを昇順ソートする
bool CBMS::sort(int ch)
{
	if (ch < 0 || ch > BMSMAXBUFFER - 1)
		return false;

	// 並び替える
	for (int i = 0; i < numBmsData[ch] - 1; i++)
	{
		for (int j = i + 1; j < numBmsData[ch]; j++)
		{
			if (bmsData[ch][i].time > bmsData[ch][j].time)
			{
				BMSDATA tmp;
				memcpy(&tmp, &bmsData[ch][i], sizeof(BMSDATA));		//tmp = i
				memcpy(&bmsData[ch][i], &bmsData[ch][j], sizeof(BMSDATA));	// i = j
				memcpy(&bmsData[ch][i], &tmp, sizeof(BMSDATA));		// j = tmp
			}
		}
	}

	return true;
}

long long CBMS::GetCountFromTime(double sec)
{
	long long bmscnt = 0;	//BMSカウント
	double t = 0;				//BMS上の時間
	double bpm = bHeader.bpm;

	if (numBmsData[BMS_TEMPO] > 0)
		bpm = bmsData[BMS_TEMPO][0].data;	//初期BPM

	//std::cout << "bpm:" << bpm << std::endl;

	if (sec < 0) return 0;

	// 指定時間を超えるまでタイムを加算
	for (int i = 0; i < numBmsData[BMS_TEMPO]; i++)
	{
		// 1つ前の時間と新しい時間との経過時間から秒を算出
		double add = (double)(bmsData[BMS_TEMPO][i].time - bmscnt) / (bpm / 60) / (BMSDATA_RESOLUTION / 4);

		// 現在のテンポ値で時間が過ぎたら抜ける
		if (t + add>sec) break;

		t += add;
		bpm = (double)bmsData[BMS_TEMPO][i].data;	//次のBPMをセット
		bmscnt = bmsData[BMS_TEMPO][i].time;				//計算済みのカウントをセット
	}

	// 指定時間と1つ前までの時間の差分
	double sub = sec - t;

	// 差分からBMSカウント数を算出
	long long cnt = (long long)(sub * (BMSDATA_RESOLUTION / 4) * (bpm / 60));

	// BMSカウント値に加算
	bmscnt += cnt;

	return cnt;
}
