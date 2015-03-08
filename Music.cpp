#include "Music.h"
#include "DxLib.h"

using namespace Kotone;
using namespace DxLib;

Music::Music()
	: BGMHandle(0)
{
}

Music::~Music()
{
}

void Music::loadBGM(std::string filename)
{
	// BGMハンドルを取得
	auto fileURL = ".\\data\\snd\\" + filename;
	this->BGMHandle = LoadSoundMem(fileURL.c_str());
}

int Music::playBackBGM()
{
	// 読みこんだ音をノーマル再生します(『PlaySoundMem』関数使用)
	return PlaySoundMem(this->BGMHandle, DX_PLAYTYPE_BACK);
}

int Music::stopBGM()
{
	// BGMを停止
	return StopSoundMem(this->BGMHandle);
}