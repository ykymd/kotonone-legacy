#pragma once
#include "CBMS.h"

#include <vector>

namespace Kotone{
	// バインド用KotononeBMS管理クラス
	// なるべくSquirrelVMにC++クラスに依存したデータを渡さないようにする
	class SqBMS
	{
	public:
		float Score = 0.0f;

		SqBMS();
		~SqBMS();
		bool SQ_Load(char* file_name);	// 譜面データをロードする
		int SQ_GetBarLine_Y(int num);
		long SQ_GetBarNum();
		void SQ_Play();	//楽曲再生
		void SQ_Stop();
		int SQ_GetObject_X(int ch, int num);
		long SQ_GetObjeNum(int ch);
		bool SQ_GetSelectSceneData();
		std::string SQ_GetTitle() { return bms.bHeader.Title; };
		int SQ_GetBPM() { return (int)bms.bHeader.bpm; };
		int SQ_Judge(int ch, int num);
		bool SQ_JudgeOver(int ch, int num);
		int SQ_LongNoteJudge(int ch, int num);
		bool SQ_LongNoteJudgeOver(int ch, int num);
		int SQ_GetAvailableSoungNum() { return songNum; };	// 利用可能曲数を取得
		std::string SQ_GetAvailableSongTitle(int i){ return availableSongs[i].Title; }
		std::string SQ_GetAvailableSongComposer(int i){ return availableSongs[i].Composer; }
		std::string SQ_GetAvailableSongGenre(int i){ return availableSongs[i].Genre; }
		int SQ_GetAvailableSongBPM(int i){ return availableSongs[i].bpm; }
		std::string SQ_GetAvailableSongFilename(int i){ return availableSongs[i].fileName; }
		int SQ_GetAvailableSongDifficulty(int i);
		float SQ_GetHighSpeed() { return bms.ScrMulti; };	// ハイスピード値を取得
		void SQ_SetHighSpeed(float spd) { bms.ScrMulti = spd; };	// ハイスピード値を更新
		float SQ_GetScore(int ch, int num);
		bool SQ_isStart();
		bool SQ_isFinished();
		bool SQ_Clear(void);


	private:
		bool isStart;

		static const int GREAT_JUDGE_COUNT = 750;	//GREAT判定を出す範囲のカウント
		static const int NEAR_JUDGE_COUNT = 1250;	//NEAR判定を出す範囲のカウント
		static const int EARLY_JUDGE_COUNT = 2500;		//EARLY_FAILED判定を出す範囲のカウント

		LONGLONG		llStartTime;		// ゲーム開始時の時間(高解像度タイマー)
		LONGLONG		llGlobalFreq;		// マシンの１秒間のカウント数(高解像度タイマー)
		int songNum = 0;	//availableSongsの末尾のインデックス

		// 選択可能曲リスト
		Kotone::SelectSceneData availableSongs[BMSMAXBUFFER];
		Kotone::CBMS bms;
		Kotone::LPBMSBAR bar;

		static std::vector<std::string> readDir(std::string folder);
	};
}
