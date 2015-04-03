#include "SqBMS.h"
#include "DxLib.h"
#include <sqrat.h>

#include "Music.h"

#include <iostream>
#include <sstream>

// タイトル、コンポーザー、ジャンル、ベースBPM

// 参照インデックスリスト(レーンL1~レーンR4まで順番に調べられるように)
const int index[8] = {56, 51, 52, 53, 14, 15, 18, 19};

using namespace Kotone;

SqBMS::SqBMS()
{
	// BMS管理クラスの初期化
	bar = NULL;

	// 選曲可能曲データの初期化
	ZeroMemory(availableSongs, sizeof(availableSongs));

	//chart = nullptr;

	// マシンの周波数を取得
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	llGlobalFreq = freq.QuadPart;
}

SqBMS::~SqBMS()
{
}

void SqBMS::SQ_Play()
{
	// BGMの再生
	Music::getInstance()->loadBGM(bms.GetWavFile());

	// 読みこんだ音をノーマル再生します(『PlaySoundMem』関数使用)
	Music::getInstance()->playBackBGM();

	Score = 0.0f;
	isStart = true;

	// 現在の時間を記録
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	llStartTime = li.QuadPart;
}

void SqBMS::SQ_Stop()
{
	// BGMの再生
	Music::getInstance()->stopBGM();
}

bool SqBMS::SQ_Load(char* file_name)
{
	bms.Load(_SC(file_name));

	isStart = false;
	return true;
}

std::vector<std::string> SqBMS::readDir(std::string folder)
{
	// ファイル名一覧のリスト
	std::vector<std::string> fileList;

	//ファイル名検索のためにワイルドカード追加
	std::stringstream ss;
	ss << folder;
	std::string::iterator itr = folder.end();
	itr--;
	if (*itr != '\\') ss << '\\';
	ss << "*.*";

	// ファイル検索
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(ss.str().c_str(), &fd);

	// 検索失敗
	if (hFind == INVALID_HANDLE_VALUE)
	{
		std::cout << "ファイル一覧を取得できませんでした" << std::endl;
		return fileList;
	}

	// ファイル名をリストに格納する
	do{
		//フォルダは除く
		if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
		{
			// ファイル名をリストに格納
			char *file = fd.cFileName;
			std::string str = file;
			fileList.push_back(str);
		}
	} while (FindNextFile(hFind, &fd));	//次のファイルを検索

	// hFindのクローズ
	FindClose(hFind);

	return fileList;
}

bool SqBMS::SQ_GetSelectSceneData()
{
	// ファイル入力
	std::string path = ".\\data\\Song\\";	//検索対象のパス
	std::vector<std::string> backfilelist = readDir(path);
	std::vector<std::string> multiDifficulty;	//複数難易度を持つ曲のファイル名
	for (unsigned int i = 0; i < backfilelist.size(); i++){

		// 拡張子を確認して譜面データかを調べる
		if (backfilelist[i].substr(backfilelist[i].size() - 3, 3) == "bme")
		{
			std::cout << backfilelist[i] << std::endl;

			std::string fileName = backfilelist[i].substr(0, backfilelist[i].size() - 4);

			// 複数難易度を持つ曲だったとき
			std::string tmp = backfilelist[i].substr(backfilelist[i].size() - 8, 4);
			if (tmp == "easy" || tmp == "norm" || tmp == "hard")
			{
				std::string tmp2 = backfilelist[i].substr(0, backfilelist[i].size() - 9);

				// 検索
				int i;
				for (i = 0; i < songNum; i++){
					if (availableSongs[i].fileName == tmp2){
						// 見つかった時、既存のものを更新する
						bms.UpdateSelectSceneData(path + backfilelist[i], &availableSongs[i]);

						if (songNum >= BMSMAXBUFFER) break;
						break;
					}
				}

				if (i != songNum) continue;

				// 見つからなかった時、 新しく作成する
				fileName = tmp2;
			}

			// 譜面データならデータの格納を行う
			SelectSceneData data;
			data.levels[0] = 0;
			data.levels[1] = 0;
			data.levels[2] = 0;
			bms.GetSelectSceneData(path + backfilelist[i], &data);
			data.fileName = fileName;
			availableSongs[songNum++] = data;

			if (songNum >= BMSMAXBUFFER) break;
		}
	}

	return true;
}

int SqBMS::SQ_GetBarLine_Y(int num)
{
	// 現在の時間を記録
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);

	LPBMSBAR bar = bms.GetBar(num);
	return (int)((double)(bar->time - bms.GetCountFromTime((double)(li.QuadPart - llStartTime) / llGlobalFreq)) / BMSDATA_RESOLUTION * (int)(bms.ScrMulti * 192));
}

long SqBMS::SQ_GetBarNum()
{
	return bms.GetBarNum();
}

int SqBMS::SQ_GetObject_X(int ch, int num)
{
	// 現在の時間を記録
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	LPBMSDATA obj = NULL;
	obj = bms.GetObj(ch, num);
	if (obj != NULL)
		return (int)((double)(obj->time - bms.GetCountFromTime((double)(li.QuadPart - llStartTime) / llGlobalFreq)) / BMSDATA_RESOLUTION * (int)(bms.ScrMulti * 192));
	return 0;
}

long SqBMS::SQ_GetObjeNum(int ch)
{
	return bms.GetObjeNum(ch);
}

int SqBMS::SQ_Judge(int ch, int num)
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	LONGLONG now_count = bms.GetCountFromTime((double)(li.QuadPart - llStartTime) / llGlobalFreq);
	LPBMSDATA bm = (LPBMSDATA)bms.GetObj(ch, num);

	// オブジェクトが存在しているか
	if (bm->flag){
		// ほぼピッタリのとき
		if ((now_count - GREAT_JUDGE_COUNT / bms.ScrMulti) < (bm->time) && (bm->time) < (now_count + GREAT_JUDGE_COUNT / bms.ScrMulti))
		{
			bm->flag = false;
			std::cout << "[" << ch << "]GREAT:" << bm->time << std::endl;
			bm->judge = Critical;
			return 4;
		}

		// ややずれているとき(2倍の許容時間)
		if ((now_count - NEAR_JUDGE_COUNT / bms.ScrMulti) <= (bm->time) && (bm->time) <= (now_count + NEAR_JUDGE_COUNT / bms.ScrMulti))
		{
			bm->flag = false;
			std::cout << "[" << ch << "]NEAR:" << bm->time << std::endl;
			bm->judge = Near;
			return 3;
		}

		// 早すぎるとき(2倍以上早いとき)s
		if ((now_count + NEAR_JUDGE_COUNT / bms.ScrMulti) < (bm->time) && (now_count + 2500) > (bm->time))
		{
			bm->flag = false;
			std::cout << "[" << ch << "]FAILED(too early):" << bm->time << std::endl;
			bm->judge = Failed;
			return 2;
		}
		
		// それ以外(あまりにも早すぎる時)であれば無視
		bm->flag = true;
		bm->judge = None;
		return 1;
	}

	return 0;
}

bool SqBMS::SQ_JudgeOver(int ch, int num)
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	LONGLONG now_count = bms.GetCountFromTime((double)(li.QuadPart - llStartTime) / llGlobalFreq);

	LPBMSDATA bm = (LPBMSDATA)bms.GetObj(ch, num);
	if (bm != NULL && bm->flag){
		// オブジェクトが存在しているか
		if (now_count  > bm->time + NEAR_JUDGE_COUNT / bms.ScrMulti)
		{
			// 範囲外であれば
			bm->flag = false;
			std::cout << "[" << ch << "]FAILED(too late):" << bm->time << std::endl;
			bm->judge = Failed;
			return false;
		}
		return true;
	}

	return false;
}

// ロングノーツにおいて指を離した瞬間の末尾判定
int SqBMS::SQ_LongNoteJudge(int ch, int num)
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	LONGLONG now_count = bms.GetCountFromTime((double)(li.QuadPart - llStartTime) / llGlobalFreq);
	LPBMSDATA bm1 = (LPBMSDATA)bms.GetObj(ch, num);
	LPBMSDATA bm2 = (LPBMSDATA)bms.GetObj(ch, num+1);

	// オブジェクトが存在しているか
	if (bm2->flag){
		// 許容範囲よりも早く離した時
		if (now_count > (bm1->time) && (bm2->time) > (now_count + NEAR_JUDGE_COUNT / bms.ScrMulti))
		{
			bm2->flag = false;
			std::cout << "[" << ch << "]UPFAILED:" << bm2->time << std::endl;
			bm2->judge = Failed;
			return 1;
		}

		// 押したまま判定ラインをすぎると自動的にCriticalになる
		// 実装は他関数内
	}

	return 0;
}

bool SqBMS::SQ_LongNoteJudgeOver(int ch, int num)
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	LONGLONG now_count = bms.GetCountFromTime((double)(li.QuadPart - llStartTime) / llGlobalFreq);

	if (num % 2 == 1) {
		std::cout << "奇数番目の参照" << std::endl;
		return false;
	}

	LPBMSDATA bm1 = (LPBMSDATA)bms.GetObj(ch, num);
	LPBMSDATA bm2 = (LPBMSDATA)bms.GetObj(ch, num+1);
	if (bm1 != NULL && bm1->flag){
		// オブジェクトが存在しているか
		if (now_count  > bm1->time + NEAR_JUDGE_COUNT / bms.ScrMulti)
		{
			// 範囲外であれば
			bm1->flag = false;
			std::cout << "[" << ch << "]FAILED(too early):" << bm1->time << std::endl;
			bm1->judge = Failed;
		}
	}

	if (bm2 != NULL && bm2->flag) {
		// オブジェクトが存在しているか
		if (now_count  > bm2->time + NEAR_JUDGE_COUNT / bms.ScrMulti)
		{
			// 範囲外であれば
			bm2->flag = false;
			bm2->judge = Critical;
			return false;
		}
		return true;
	}

	return false;
}

// スコアを取得
float SqBMS::SQ_GetScore(int ch, int num)
{
	LPBMSDATA bm = (LPBMSDATA)bms.GetObj(ch, num);
	switch (bm->judge)
	{
	case Critical:
		return 1.0f;
	case Near:
		return 0.6f;
	case Failed:
		return 0.0f;
	}
	return 0.0f;
}

int SqBMS::SQ_GetAvailableSongDifficulty(int i)
{
	int level = 0;
	for (int j = 0; j < 3; j++){
		level += availableSongs[i].levels[j];
		level *= 10;
	}

	return level;
}

bool SqBMS::SQ_isStart()
{
	return isStart;
}

// これで終了するか
bool SqBMS::SQ_isFinished()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	LONGLONG now_count = bms.GetCountFromTime((double)(li.QuadPart - llStartTime) / llGlobalFreq);

	// BMSカウンタが曲の最大カウント値(+1小節分)を超えたら終了
	if (bms.GetMaxCount() + BMSDATA_RESOLUTION*2 <= now_count)
		return true;

	return false;
}

bool SqBMS::SQ_Clear(void)
{
	// 曲名をリセット
	bms.wavData = "";
	ZeroMemory(&bms.bmsData, sizeof(LPBMSDATA));
	for (int i = 0; i<BMSMAXBUFFER; i++) {
		bms.bHeader.BPMIndex[i].Bpm = 120.0f;
		bms.bmsData[i] = NULL;		// BMSデータ領域をクリア
		bms.numBmsData[i] = 0;			// データの数をクリア
	}

	bms.barMagni = NULL;
	bms.numBarMagni = 0;

	bms.bmsBar = NULL;
	bms.numBmsBar = 0;

	ZeroMemory(&bms.bHeader, sizeof(BMSHEADER));
	// デフォルトセット
	bms.bHeader.Player = 1;
	bms.bHeader.bpm = 130;

	return TRUE;
}