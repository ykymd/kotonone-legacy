#pragma once

#include <Windows.h>
#include <string>
#include <unordered_map>

class BMSHeader
{
public:
	BMSHeader();
	~BMSHeader();

	long		player;			// プレイモード
	std::string	genre;			// データのジャンル
	std::string	title;			// データのタイトル
	std::string	artist;			// データの製作者
	float		bpm;			// データのテンポ（初期値は130）
	std::string	midiFile;		// バックグラウンドで流すMIDIファイル
	long		playLevel;		// データの難易度
	long		rank;			// データの判定ランク
	long		wavVol;			// 音量を元の何％にするか
	long		total;			// ゲージの増量を設定
	std::string	stagePic;		// 曲開始字に表示する画像
	std::string	movie;			// ムービーファイル名
	GUID		guid;			// 曲のGUID(特殊仕様)
	long		endBar;			// 終了小節
	LONGLONG	maxCount;		// 最大のカウント数
	std::unordered_map<int,float> bpmIndex;	// テンポインデックス
};
