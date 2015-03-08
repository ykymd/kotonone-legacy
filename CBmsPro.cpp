///////////////////////////////////////////////////////////////////////////////////
//                    BMSクラス チュートリアルバージョン v3.0                    //
//                          http://www.charatsoft.com/                           //
//                             THIS CODE IS FREE.                                //
///////////////////////////////////////////////////////////////////////////////////
#pragma warning( disable : 4996 )

#include "CBmsPro.h"
#include <stdio.h>

//#define DEBUGMODE
//#include "debug.h"

#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>

#define FREE(x)		{ if(x) {free(x); x=NULL;} }
#define BINCLOSE(x)	{ if(x) {fclose(x); x=NULL;} pBinSrc=NULL; }

using namespace std;

////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////
CBmsPro::CBmsPro( void )
{
	//ZeroMemory( &mBH,sizeof(mBH) );
	ZeroMemory( &mBmsData,sizeof(mBmsData) );
	ZeroMemory( &iBmsData,sizeof(iBmsData) );
	//ZeroMemory( &mWav,sizeof(mWav) );
	//ZeroMemory( &mBmp,sizeof(mBmp) );

	ZeroMemory( &mLastError,sizeof(mLastError) );

	// デフォルトセット
	/*
	mBH.lPlayer = 1;
	mBH.fBpm = 130;

	iBarMagni = 0;
	mBarMagni = NULL;
	iBmsBar = 0;
	mBmsBar = NULL;
	*/
}

////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////
CBmsPro::~CBmsPro()
{
	Clear();
}

////////////////////////////////////////////
// 16進数文字列を数字に変換(0～F)
////////////////////////////////////////////
int CBmsPro::atoi1610(char *s)
{
	int i = 0;	// ポインタ
	int x = 0;	// 10進数に変換した数
	while(1)
	{
		if(s[i] == NULL)
			break;
		if( !(s[i]>='0' && s[i]<='9') &&
			!(s[i]>='A' && s[i]<='Z') &&
			!(s[i]>='a' && s[i]<='z') )
			return 0;

		x *= 16;	// 16倍
		int n = s[i] - '0';
		if(n>9)
			n -= 7;
		if(n>15)
			n -= 0x20;
		x += n;
		i++;
	}
	return x;
}

////////////////////////////////////////////
// データのクリア
////////////////////////////////////////////
BOOL CBmsPro::Clear( void )
{
	int i;
	for( i=0;i<BMSMAXBUFFER;i++ ) {
		//FREE( mWav[i] );
		//FREE( mBmp[i] );
		//mBH.mBpmIndex[i].fBpm = 120.0f;
	}
	for( i=0;i<BMSMAXBUFFER;i++ ) {
		//FREE( mBmsData[i] );		// BMSデータ領域をクリア
		//iBmsData[i] = 0;			// データの数をクリア
	}

	//FREE( mBarMagni );
	iBarMagni = 0;

	//FREE( mBmsBar );
	iBmsBar = 0;

	//ZeroMemory( &mBH,sizeof(BMSHEADER) );
	// デフォルトセット
	//mBH.lPlayer = 1;
	//mBH.fBpm = 130;

	return TRUE;
}

////////////////////////////////////////////
// データロード
////////////////////////////////////////////
BOOL CBmsPro::Load( LPSTR file )
{
	// ヘッダ＆小節倍率の読み込み
	if( !GetHeader( file ) ) {
		cout << "ヘッダ読み取りエラー" << endl;
		return FALSE;
	}
	cout << "HEADER OK" << endl;

	// 実データの読み込み
	if( !LoadBmsData( file ) ) {
		cout << "データ読み込みエラー" << endl;
		return FALSE;
	}
	cout << "DATA OK" << endl;

	return TRUE;
}

////////////////////////////////////////////
// １行分のデータ中のコマンド番号を返す	
////////////////////////////////////////////
Command CBmsPro::getCommandPart(const string line)
{
	const string command[] = {
		"PLAYER",
		"GENRE",
		"TITLE",
		"ARTIST",
		"BPM",
		"MIDIFILE",
		"PLAYLEVEL",
		"RANK",
		"VOLWAV",
		"TOTAL",
		"STAGEFILE",
		"WAV",
		"BMP",
		"GUID",
		"MOVIE",
	};

	// 文字列中のコマンド部分が、指定のコマンド群と一致しているか
	auto cnt = 0;
	for (auto pattern : command){
		// 検索でかなりオーバーヘッドが起きるかもしれない
		auto cmdpart = line.substr(1, pattern.size());
		if (cmdpart == pattern){
			return Command(cnt);
		}

		cnt++;
	}

	// 命令部分がオブジェ配置の形('#iiiii')か
	// 文字列中のコマンド部分を抽出
	auto cmd = line.substr(1, 5);

	// コマンド部分がすべて数値であるならオブジェ配置として返す
	if (all_of(cmd.cbegin(), cmd.cend(), isdigit)){
		return Command::OBJECT;
	}
	else {
		// それ以外の場合、判別不可能文字として返す
		return Command::UNKNOWN;
	}
}

// コマンドの文字列部分を返す（'\n'は削除 ':'も区切りとして処理）
string	CBmsPro::getCommandString(std::string line)
{
	auto i = 1;

	// スペースか:の文字が来るまで読み取り、それ以降の文字列を返す
	for (auto let : line){
		if (let == ' ' || let == '　' || let == ':'){
			return line.substr(i);
		}
		else if (let == NULL){
			break;
		}

		i++;
	}

	return NULL;
}


//////////////////////////////////////////////////////////////////
// GUID文字列を解析する(汎用関数)
// 形式="{5BCECBC6-6C0F-452c-AF8D-C6CE6BC0BDA0}"
//////////////////////////////////////////////////////////////////
GUID CBmsPro::GetGuidFromString( const char* mGuid )
{
	static GUID guid={0};
	
	// まず始めに'{'と'}'をチェック
	if( mGuid[0]!='{' || mGuid[37]!='}' ) {
		ZeroMemory( &guid,sizeof(GUID) );
		return guid;
	}
	int i,j;
	BYTE c;
	// Data1 
	for( i=0;i<8;i++ ) {
		c = mGuid[i+1]-'0';
		if( c>9 )
			c-=7;
		if( c>15 )
			c-=0x20;
		guid.Data1 <<= 4;
		guid.Data1 |= c;
	}
	// Data2
	for( i=0;i<4;i++ ) {
		c = mGuid[i+10]-'0';
		if( c>9 )
			c-=7;
		if( c>15 )
			c-=0x20;
		guid.Data2 <<= 4;
		guid.Data2 |= c;
	}
	// Data3
	for( i=0;i<4;i++ ) {
		c = mGuid[i+15]-'0';
		if( c>9 )
			c-=7;
		if( c>15 )
			c-=0x20;
		guid.Data3 <<= 4;
		guid.Data3 |= c;
	}
	// BYTE 2
	for( j=0;j<2;j++ ) {
		for( i=0;i<2;i++ ) {
			c = mGuid[i+20+j*2]-'0';
			if( c>9 )
				c-=7;
			if( c>15 )
				c-=0x20;
			guid.Data4[j] <<= 4;
			guid.Data4[j] |= c;
		}
	}
	// BYTE 6
	for( j=0;j<6;j++ ) {
		for( i=0;i<2;i++ ) {
			c = mGuid[i+25+j*2]-'0';
			if( c>9 )
				c-=7;
			if( c>15 )
				c-=0x20;
			guid.Data4[j+2] <<= 4;
			guid.Data4[j+2] |= c;
		}
	}
	return guid;
}

BOOL CBmsPro::NumToStr36( int num,char *dst,int keta )
{
	if( num<0 )
		return FALSE;

	itoa( num,dst,36 );

	// 小文字チェック
	int i;
	for( i=0;i<(int)strlen(dst);i++ ) {
		if( dst[i]>='a' && dst[i]<='z' ) {
			dst[i] -= 0x20;
		}
	}


	if( keta>=0 ) {
		// 桁数指定の場合
//		DEBUG( "桁指定\n","" );
		if( (int)strlen(dst)<keta ) {
			// 桁数が足りない場合
//			DEBUG( "桁数が足りない場合 %d\n",keta-strlen(dst) );
			char dmy[1024];
			ZeroMemory( dmy,sizeof(dmy) );
			for( i=0;i<keta-(int)strlen(dst);i++ ) {
				dmy[i] = '0';			// 先頭に足りない分の0を追加
			}
			strcpy( &dmy[i],dst );		// その次に算出された文字列をつなげる
			strcpy( dst,dmy );			// 出力結果にダミー文字列をコピー		
		}
	}

	return TRUE;
}

BOOL CBmsPro::LineCompact( char *src,char *dst )
{
	int i;
	char buf[1024];		// バッファ
	strcpy( buf,src );

	while(1) {
		// データの数分ループ
		int l = strlen( buf ) / 2;	// データ数に変換
		if( l==0 ) {
//			DEBUG( "データなし\n","" );
			return FALSE;
		}


		if( (l%2)==1 ) {
			//DEBUG( "奇数 [%d]\n",l );
			BOOL ok = FALSE;
			// データが１つでも存在するか
			for( i=0;i<l;i++ ) {
				if( buf[i*2]!='0' || buf[i*2+1]!='0' ) {
					ok = TRUE;
					break;
				}
			}
			if( !ok )
				return FALSE;

			break;					// 奇数なら変換なし
		}

		// １つおきでデータがあるかチェック
		BOOL ok = FALSE;
		for( i=0;i<l;i+=2 ) {
			if( buf[i*2+2]!='0' || buf[i*2+3]!='0' ) {
				ok = TRUE;
				break;
			}
		}

		// 分解終了なら
		if( ok )
			break;

		// バッファを２分割する
		char tmp[1024];
		strcpy( tmp,buf );
		ZeroMemory( buf,sizeof(buf) );
		for( i=0;i<l/2;i++ ) {
			buf[i*2  ] = tmp[i*4  ];
			buf[i*2+1] = tmp[i*4+1];
		}
		//DEBUG( "CONVERT [%s]\n",buf );
	}

	strcpy( dst,buf );
	//DEBUG( "dst=[%s]\n",dst );

	return TRUE;
}

////////////////////////////////////////////
// ヘッダ情報だけを取り出す
////////////////////////////////////////////
BOOL CBmsPro::GetHeader( LPSTR file )
{
	Clear();

	auto bh = new BMSHeader();

	// 読み込んだファイルの1行分のデータを格納する文字列
	string str;

#ifndef __DXLIB
	// 指定されたアドレスからファイルを読み込む
	ifstream ifs(file);

	// 失敗時の処理
	if (ifs.fail())
	{
		cerr << "失敗" << endl;
		//return EXIT_FAILURE;
		return FALSE;
	}
#endif

	// 指定のファイルから1行ずつ文を読みだす
#ifdef __DXLIB
	char tmp[BMSMAXBUFFER];
	int FileHandle = DxLib::FileRead_open(file);

	// DXライブラリを利用している際は、DXアーカイバに対応した関数に切り替える
	while (DxLib::FileRead_eof(FileHandle) == 0){
		DxLib::FileRead_gets(tmp, BMSMAXBUFFER, FileHandle);
		str = std::string(tmp);
#else
	while (getline(ifs, str)) {
#endif
		// コマンド以外の部分は読み飛ばす
		if (str[0] != '#') continue;

		cout << "出力:" << str << endl;

		// 改行文字を消去
		if (str.back() == '\n')
			str.back() = '\0';

		// 1行分のデータからコマンドに相当する部分を取得
		auto command = getCommandPart(str);

		// 判別不可能文字なら警告
		if (command == Command::UNKNOWN)  {
			std::cout << "判別不可能文字:" << str << std::endl;
			continue;
		}

		// 1行分のデータからデータ部分を取得
		auto dataString = getCommandString(str);
		int num = 0;

		// コマンドの種類に応じたパラメータに文字列を代入
		switch (command)
		{
		case Command::PLAYER:
			bh->player = stoi(dataString);
			cout << "PLAYER       " << bh->player << endl;
			break;
		case Command::GENRE:
			bh->genre = dataString;
			cout << "ジャンル     " << bh->genre << endl;
			break;
		case Command::TITLE:
			bh->title = dataString;
			cout << "タイトル     " << bh->title << endl;
			break;
		case Command::ARTIST:
			bh->artist = dataString;
			cout << "アーティスト " << bh->artist << endl;
			break;
		case Command::BPM:
			if (str[4] == ' ' || str[4] == '　') {
				// ヘッダなら
				bh->bpm = stof(dataString);
				AddData(CHANNEL::TEMPO, 0, (LONG)bh->bpm);
				cout << "ベースBPM    " << bh->bpm << endl;
			}
			else {
				// インデックスなら
				auto index = stoi(str.substr(4, 2));
				bh->bpmIndex[index] = stof(dataString);
				cout << "インデックスBPM " << bh->bpmIndex.at(index) << endl;
			}
			break;
		case Command::MIDIFILE:
			bh->midiFile = dataString;
			cout << "MIDIFILE      " << bh->midiFile << endl;
			break;
		case Command::PLAYLEVEL:
			bh->playLevel = stoi(dataString);
			cout << "PLAYLEVEL     " << bh->playLevel << endl;
			break;
		case Command::RANK:
			bh->rank = stoi(dataString);
			cout << "RANK          " << bh->rank << endl;
			break;
		case Command::VOLWAV:
			bh->wavVol = stoi(dataString);
			cout << "WAVVOL        " << bh->wavVol << endl;
			break;
		case Command::TOTAL:
			bh->total = stoi(dataString);
			cout << "TOTAL         " << bh->total << endl;
			break;
		case Command::STAGEFILE:
			cout << "StageImage-" << bh->stagePic << endl;
			bh->stagePic = dataString;
			break;
		case Command::WAV:
			num = stoi(str.substr(4, 2), nullptr, 16);
			wavData[num] = dataString;

			cout << "WAV%04d-" << num << endl;
			cout << wavData.at(num) << endl;
			break;
		case Command::BMP:
			num = stoi(str.substr(4, 2), nullptr, 16);
			bmpData[num] = dataString;

			cout << "BMP%04d-" << num << endl;
			cout << bmpData.at(num) << endl;
			break;
		case Command::SONG_GUID:
			bh->guid = GetGuidFromString(dataString.c_str());
			cout << "BMS : GUID=" << bh->guid.Data1 << endl;
			break;
		case Command::MOVIE:
			bh->movie = dataString;
			cout << "MOVIE=" << bh->movie << endl;
			break;
		default:
			// 小節番号の取得
			auto iLines = stoi(str.substr(1, 3));
			// チャンネル番号の取得
			auto iChannel = stoi(str.substr(4, 2));	// 数字化
			auto channel = static_cast<CHANNEL>(iChannel);

			if (channel == CHANNEL::STRETCH) {
				// 小節の倍率変更命令の場合
				//				DEBUG("小節倍率変換 [%s]倍\n",str);
				iBarMagni++;

				auto magni = make_tuple(iLines, stof(dataString));
				barMagni.push_back(magni);
			}

			// 小節番号の最大値を記憶する
			if (bh->endBar < iLines)
				bh->endBar = iLines;

			break;
		}
	}

#ifdef __DXLIB
	FileRead_close(FileHandle);
#endif


	// 小節分割倍率データを元に小節バーを生成
	LONGLONG lastCount = 0;
	for(auto i=0; i <= bh->endBar+1; i++) {				// 最後の小節までを処理
		// 小節を追加
		iBmsBar++;
		
		barData.push_back(lastCount);
		auto add = BMSDATA_RESOLUTION;			// 加算するポインタ数を定義

		// 現在の小節で倍率を変換しているか
		for (auto magni : barMagni) {
			if( get<0>(magni) == i ) {
				// 倍率変更命令が存在する場合
				add = (float)BMSDATA_RESOLUTION * get<1>(magni);	// 加算値をn倍する
				break;	// この小節の倍率変換は終わり
			}
		}
		lastCount += add;	// ポジションを追加

		// 最大の拍数をついでに換算
		if (i < bh->endBar && bh->maxCount < lastCount)
			bh->maxCount = lastCount;
	}

	cout << "NumBmsBar:" << barData.size() << endl;

	this->bmsHeader = bh;

	return TRUE;
}

////////////////////////////////////////////
// １つのデータを追加
////////////////////////////////////////////
BOOL CBmsPro::AddData( CHANNEL ch,LONGLONG startpos,LONG dat )
{
	/*
	// チャンネル番号をチェック
	//if( ch<0 || ch>255 )
	//	return FALSE;

	if( ch==CHANNEL::STRETCH )
		return FALSE;

	if( dat==0 )
		return TRUE;

	switch( ch )
	{
	case CHANNEL::BPMINDEX:
		auto tempo = static_cast<int>(CHANNEL::TEMPO);
		iBmsData[tempo]++;
		mBmsData[tempo] = (LPBMSDATA)realloc( mBmsData[tempo],iBmsData[tempo]*sizeof(BMSDATA) );
		ZeroMemory( &mBmsData[tempo][ iBmsData[tempo]-1 ],sizeof(BMSDATA) );	// きれいに
		mBmsData[tempo][iBmsData[tempo]-1].bFlag = TRUE;
		mBmsData[tempo][iBmsData[tempo]-1].llTime = startpos;
		mBmsData[tempo][iBmsData[tempo]-1].lData = (long)mBH.mBpmIndex[dat].fBpm;
		mBmsData[tempo][iBmsData[tempo]-1].fData = mBH.mBpmIndex[dat].fBpm;
		break;
	default:
		// データを追加
		auto channel = static_cast<int>(ch);
		iBmsData[channel]++;
		mBmsData[channel] = (LPBMSDATA)realloc( mBmsData[channel],iBmsData[channel]*sizeof(BMSDATA) );
		ZeroMemory( &mBmsData[channel][ iBmsData[channel]-1 ],sizeof(BMSDATA) );	// きれいに
		mBmsData[channel][iBmsData[channel]-1].bFlag = TRUE;
		mBmsData[channel][iBmsData[channel]-1].llTime = startpos;
		mBmsData[channel][iBmsData[channel]-1].lData = dat;
		mBmsData[channel][iBmsData[channel]-1].fData = (float)dat;		// float型も同一の数値に
		break;
	}
	*/

	return TRUE;
}

// ノーツのデータをキューに格納する
bool CBmsPro::AddNoteData(unsigned int ch_num, long long startpos, long dat)
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

////////////////////////////////////////////
// BMSデータの読み込み
////////////////////////////////////////////
BOOL CBmsPro::LoadBmsData( LPSTR file )
{
	// 読み込んだファイルの1行分のデータを格納する文字列
	string str;

#ifndef __DXLIB
	// 指定されたアドレスからファイルを読み込む
	ifstream ifs(file);

	// 失敗時の処理
	if (ifs.fail())
	{
		cerr << "失敗" << endl;
		//return EXIT_FAILURE;
		return FALSE;
	}
#endif

	// 指定のファイルから1行ずつ文を読みだす
#ifdef __DXLIB
	char tmp[BMSMAXBUFFER];
	int FileHandle = DxLib::FileRead_open(file);

	// DXライブラリを利用している際は、DXアーカイバに対応した関数に切り替える
	while (DxLib::FileRead_eof(FileHandle) == 0){
		DxLib::FileRead_gets(tmp, BMSMAXBUFFER, FileHandle);
		str = std::string(tmp);
#else
	while (getline(ifs, str)) {
#endif
		// コマンド以外は読み飛ばす
		if (str[0] != '#') continue;

		// 改行文字を消去
		if (str[str.size() - 1] == '\n')
			str[str.size() - 1] = NULL;

		// 命令列からコマンド部分を取得
		Command cmd = getCommandPart(str);
		if (cmd != Command::OBJECT)  continue;

		// 
		auto dataString = getCommandString(str);

		// データであれば意味解析を行う
		int barLines;		// 現在の小節番号
		//char num[4] = { 0 };	//汎用バッファ

		// チャンネル番号の取得
		auto numChannel = std::stoi(str.substr(4, 2));
		// 列挙値に変換
		auto channel = static_cast<CHANNEL>(numChannel);

		// 小節の倍率変更命令の場合はキャンセル
		if (channel == CHANNEL::STRETCH) continue;

		// 小節番号の取得
		barLines = std::stoi(str.substr(1, 3));

		// データが存在するか
		int size = dataString.size();
		if (size < 1) {
			std::cout << "データ部分が定義されていません:" << str << std::endl;
			continue;
		}

		// データ部分の文字数が偶数個か
		if (size % 2 == 1) {
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
			long hex = std::stoi(dataString.substr(i * 2, 2), nullptr, 16);
			if (hex > 0) {
				AddNoteData(channel, startCount + (long long)(currentBarResolution*(float)i), hex);
			}
		}
	}
#ifdef __DXLIB
	FileRead_close(FileHandle);
#endif




	int i;
	char data[1024];

	//FILE *fp;
	auto fp = fopen( file,"r" );
	if( !fp ) {
		sprintf( mLastError,"[%s] ファイルオープンエラー",file );
		return FALSE;
	}

	char buf[1024];
	while(1) {
		// １行を読みこむ
		ZeroMemory( buf,1024 );
		fgets( buf,1024,fp );
		if( buf[0]==NULL && feof(fp) )	// ファイルの終端なら検索終わり
			break;

//		DEBUG( "%s",buf );

		// コマンド以外なら次の行へ
		if( buf[0]!='#' )
			continue;

		// 最後の改行を消去
		if( buf[strlen(buf)-1]=='\n' )
			buf[strlen(buf)-1] = NULL;

		// コマンドの種類を取得
		int com = GetCommand( buf );

		// オブジェ以外は全て無視
		if( com!=-1 )
			continue;

		// パラメータ文字列を取得
		ZeroMemory(data,1024);
		if( !GetCommandString(buf,data) ) {
			DEBUG( "パラメータ文字列取得エラー\n","" );
			fclose(fp);
			sprintf( mLastError,"[%s] パラメータ文字列取得エラー",buf );
			return FALSE;
		}
		
		// データであれば解析
		int iLines;					// 現在の小節番号
		int iChannel;				// 現在のチャンネル番号
		char mNum[4];				// 汎用バッファ

		// チャンネル番号の取得
		ZeroMemory( &mNum,sizeof(mNum) );
		mNum[0] = buf[4];			// チャンネル番号
		mNum[1] = buf[5];
		iChannel = atoi( mNum );	// 数字化

		auto channel = static_cast<CHANNEL>(iChannel);
		// 小節の倍率変更命令の場合はキャンセル
		if( channel==CHANNEL::STRETCH )	
			continue;

		// 小節番号を取得
		ZeroMemory( &mNum,sizeof(mNum) );
		memcpy( mNum,buf+1,3 );		// 小節部分を取得
		iLines = atoi( mNum );		// 数字化

		// データが存在するかチェック
		if( strlen(data)<1 ) {
			DEBUG( "データが定義されていない箇所が存在\n%s",buf );
			continue;
		}

		// データが偶数かチェック
		if( strlen(data)%2==1 ) {
			DEBUG( "データが偶数個ではない [%s]\n",buf );
			fclose( fp );
			sprintf( mLastError,"[%s] データが偶数個ではない",buf );
			return FALSE;
		}

		// データを追加する
		int len = strlen(data) / 2;			// 数値の数
	
		// 小節区分からデータ配置場所を特定
		LONGLONG llStartCount = 0;					// その小節の最初のカウント値
		int chkline = 0;							// 計算中の小節（0小節目から現在の1つ前までを加算していく）

		while( chkline<iLines ) {					// 現在の小節の１つ前までを処理

			LONGLONG add = BMSDATA_RESOLUTION;		// 加算するポインタ数を定義

			// 現在チェック中の小節に倍率変更が存在しないか検索する
			for( i=0;i<iBarMagni;i++ ) {
				if( mBarMagni[i].iLines==chkline ) {									// 倍率変更命令が存在する場合
					add = (LONGLONG)((float)BMSDATA_RESOLUTION * mBarMagni[i].fMagni);	// 加算値をf倍する(つまり0.5なら半分になる)
					break;																// 見つかったので終わり
				}
			}

			// 現在までの小節カウントを加算
			llStartCount += add;

			// 次の小節へ
			chkline++;		
		}

		// 現在の小節の幅を求める
		float fNowBarReso = (float)BMSDATA_RESOLUTION / (float)len;	// デフォルトの１音符分のカウント値

		for( i=0;i<iBarMagni;i++ ) {
			// その小節に倍率変更命令が存在するか
			if( mBarMagni[i].iLines==chkline ) {
				// 存在した場合は分割数を再計算
				fNowBarReso = ((float)BMSDATA_RESOLUTION * mBarMagni[i].fMagni) / len;
				break;					// 見つかったので終わり
			}
		}

		// 実データを追加
		ZeroMemory( &mNum,sizeof(mNum) );
		for( i=0;i<len;i++ ) {
			mNum[0] = data[i*2];
			mNum[1] = data[i*2+1];
			long hex = atoi1610( mNum );			// １６進数を数値へ変換
			if( hex>0 ) {
				// データが存在する場合
				AddData( channel, llStartCount + (LONGLONG)(fNowBarReso*(float)i),hex );
			}
		}
	}


//	DEBUG( "データロードOK1\n","") ;
	// ソート
	for( i=0;i<BMSMAXBUFFER;i++ )
		Sort( i );
//	DEBUG( "データロードOK2\n","") ;

	fclose( fp );

/*	for( i=0;i<iBmsData[BMS_TEMPO];i++ ) {
		char s[256];
		sprintf( s,"[%08d] %f",(int)mBmsData[BMS_TEMPO][i].llTime,mBmsData[BMS_TEMPO][i].fData );
		DEBUG( "%s\n",s )
	}/**/

	return TRUE;
}

////////////////////////////////////////////
// 指定チャンネルのデータを早い順にソートする
////////////////////////////////////////////
BOOL CBmsPro::Sort( int ch )
{
	if( ch<0 || ch>BMSMAXBUFFER-1 )
		return FALSE;

//	DEBUG( "SORT [%d]-",ch );
//	DEBUG( "[%d]\n",iBmsData[ch] );

	int i,j;
	// 昇順に並び替える
	for( i=0;i<iBmsData[ch]-1;i++ ) {
		for( j=i+1;j<iBmsData[ch];j++ ) {
			if( mBmsData[ch][i].llTime > mBmsData[ch][j].llTime ) {
				BMSDATA dmy;
				memcpy( &dmy,&mBmsData[ch][i],sizeof(BMSDATA) );					// ダミーに保存
				memcpy( &mBmsData[ch][i],&mBmsData[ch][j],sizeof(BMSDATA) );		// iにjを入れる
				memcpy( &mBmsData[ch][j],&dmy,sizeof(BMSDATA) );					// jにダミーを入れる
			}
		}
	}
	return TRUE;
}


////////////////////////////////////////////
// 指定チャンネルのデータを早い順にソートする
////////////////////////////////////////////
BOOL CBmsPro::Save( LPSTR file,BOOL same )
{
	int i,j;
	FILE *fp;
	fp = fopen( file,"wb" );
	if( !fp ) {
		sprintf( mLastError,"[%s] ファイル書き込みエラー",file );
		return FALSE;
	}


	// BGMチャンネル以外の同一場所に存在するデータを自動的にずらす
	if( same ) {
		//DEBUG( "同一チャンネル変更処理\n","" );
		const int start[2] = { 11,21 };
		const int end[2] = { 19,29 };
		const int not[2] = { 17,27 };
		int lp;
		for( lp=0;lp<2;lp++ ) {
			for( i=start[lp];i<=end[lp];i++ ) {
				//DEBUG( "\nチェック [%02d]\n",i );
				LONGLONG old = 0;
				j = 0;
				while( j<iBmsData[i] ) {
					LONGLONG tm = mBmsData[i][j].llTime;
					//DEBUG( "check %d",tm );
					if( old==tm ) {
						// １つ前と同一の時間だった場合はあいているチャンネルを検索
						//DEBUG( " > １つ前と同一時間","" );
						BOOL ok = FALSE;
						int k = i + 1;		// 次のチャンネルから検索
						if( k==not[lp] )
							k++;
						if( k>end[lp] )
							k = start[lp];

						while(1) {
							// チャンネル内検索
							int l = 0;
							while( l<iBmsData[k] ) {
								if( mBmsData[k][l].llTime==tm ) {
									//DEBUG( " > %02dNG",k );
									break;
								}
								l++;
							}

							// 空きチャンネルなら
							if( l>=iBmsData[k] ) {
								//DEBUG( " > %02d OK",k );
								ok = TRUE;
								break;
							}

							// 次のチャンネルのチェック
							k++;
							if( k==not[lp] )
								k++;
							if( k>end[lp] )
								k = start[lp];
							if( i==k ) {
								//DEBUG( " > ALL ERROR(16部音符UP)","" );
								mBmsData[i][j].llTime += BMSDATA_RESOLUTION / 16;		// 16部音符ずらす
								break;
							}
						}

						// チャンネルを変更
						if( ok ) {
							AddData( static_cast<CHANNEL>(k),mBmsData[i][j].llTime,mBmsData[i][j].lData );
							memmove( &mBmsData[i][j],&mBmsData[i][j+1],sizeof(BMSDATA)*(iBmsData[i]-j-1) );
							iBmsData[i]--;
							// 次のポインタには行かない
							//DEBUG( "\n","" );
							continue;
						}

						//DEBUG( "\n","" );

					} else {
						old = tm;
					}
					//DEBUG( "\n","" );
					j++;
				}
			}

		}

		// 最終ソート
		for( i=0;i<BMSMAXBUFFER;i++ )
			Sort( i );

		for( j=0;j<5;j++ ) {
			//DEBUG( "\n[%d]\n",j+11 );
			for( i=0;i<iBmsData[11+j];i++ ) {
				//DEBUG( "%d:",mBmsData[11+j][i].llTime );
				//DEBUG( "%d\n",mBmsData[11+j][i].lData );
			}
		}

	}

	/*
	// ヘッダの書き出し
	fprintf( fp,"\n" );
	fprintf( fp,"#PLAYER %d\n",mBH.lPlayer );
	fprintf( fp,"#GENRE %s\n",mBH.mGenre );
	fprintf( fp,"#TITLE %s\n",mBH.mTitle );
	fprintf( fp,"#ARTIST %s\n",mBH.mArtist );
	fprintf( fp,"#BPM %5.2f\n",mBH.fBpm );
	fprintf( fp,"#PLAYLEVEL %d\n",mBH.lPlaylevel );
	fprintf( fp,"#STAGEFILE %s\n",mBH.mStagePic );
	fprintf( fp,"#MOVIE %s\n",mBH.mMovie );
	fprintf( fp,"#MIDIFILE %s\n",mBH.mMidifile );
	fprintf( fp,"\n" );

	char buf[1024];

	// WAVファイルの書き出し
	for( i=0;i<BMSMAXBUFFER;i++ ) {
		if( mWav[i] ) {
			NumToStr36( i,buf,2 );
			fprintf( fp,"#WAV%s %s\n",buf,mWav[i] );
		}
	}
	fprintf( fp,"\n" );

	// BMPファイルの書き出し
	for( i=0;i<BMSMAXBUFFER;i++ ) {
		if( mBmp[i] ) {
			NumToStr36( i,buf,2 );
			fprintf( fp,"#BMP%s %s\n",buf,mBmp[i] );
		}
	}
	fprintf( fp,"\n" );

	// データの書き出し
	const int ch[] = {
		static_cast<int>(CHANNEL::BACKMUSIC),
//		BMS_TEMPO,
//		BMS_BACKANIME,
//		BMS_BPMINDEX,
		11,12,13,14,15,16,18,19,
		21,22,23,24,25,26,28,29,
		-1,
	};


	int n = 0;
	int index;
	int line;
	char s[1024];
	char dst[1024];

	while( ch[n]!=-1 ) {

		DEBUG( "ch [%02d]\n",ch[n] );

		index = 0;
		line = 0;

		// 全小節の処理
		while( index<iBmsData[ch[n]] ) {
			DEBUG( "LINE %d\n",line );
			strcpy( s,"00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" );

			// 現在の小節内のデータを処理
			while( index<iBmsData[ch[n]] ) {
				if( (line+1)*BMSDATA_RESOLUTION<=mBmsData[ch[n]][index].llTime )
					break;
//				DEBUG( "%d-",mBmsData[ch[n]][index].llTime );

				// 時間から書き込み先アドレスを算出
				int p = (int)(mBmsData[ch[n]][index].llTime - line*BMSDATA_RESOLUTION);
				DEBUG( "%d:",p );
				p = (int)((float)p / (BMSDATA_RESOLUTION/64)) * 2;
				DEBUG( "%d\n",p );

				// 既に１つ前の情報があるか
				if( s[p]!='0' || s[p+1]!='0' ) {
					// 既にある場合は現在までのデータを書き込んでおく
					if( LineCompact( s,dst ) )
						fprintf( fp,"#%03d%02d:%s\n",line,ch[n],dst );
					strcpy( s,"00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" );
				}

				// データをセット
				NumToStr36( mBmsData[ch[n]][index].lData,buf,2 );
//				DEBUG( "%s\n",buf );
				memcpy( &s[p],buf,2 );

				index++;
			}

			// 残りを書き出す
			if( strcmp(s,"00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000")!=0 ) {
				if( LineCompact( s,dst ) ) {
					DEBUG( "書き出し [%s]\n",dst );
					fprintf( fp,"#%03d%02d:%s\n",line,ch[n],dst );
				}
			}


			line++;
		}

		fprintf( fp,"\n" );

		n++;
	}

	fclose( fp );

	*/
	return TRUE;
}

BOOL CBmsPro::Restart( void )
{
	int i,j;
	for( j=0;j<BMSMAXBUFFER;j++ ) {
		for( i=0;i<iBmsData[j];i++ ) {
			mBmsData[j][i].bFlag = TRUE;
		}
	}
	return TRUE;
}

LONGLONG CBmsPro::GetCountFromTime( double sec )
{
	LONGLONG cnt = 0;			// BMSカウント
	double t = 0;				// BMS上の時間
	double bpm = 130;
	
	auto channel = static_cast<int>(CHANNEL::TEMPO);
	if (iBmsData[channel] >0) {
		bpm = mBmsData[channel][0].fData;		// 初期BPM
	}

	if( sec<0 )
		return 0;

//	DEBUG( "■指定時間 (%.2f秒)\n",sec );

	// 指定時間を越えるまでタイムを加算
	int i;
	auto tempo = static_cast<int>(CHANNEL::TEMPO);
	for( i=0;i<iBmsData[tempo];i++ ) {

		// １つ前の時間と新しい時間との経過時間から秒を算出
		double add = (double)( mBmsData[tempo][i].llTime - cnt ) / ( bpm / 60 ) / (BMSDATA_RESOLUTION / 4);
//		DEBUG2( "  [%d] 経過時間 %f秒\n",i,t+add );

		// 現在のテンポ値で時間が過ぎたら抜ける
		if( t+add>sec ) {
//			DEBUG( "                  └時間が過ぎた\n","" );
			break;
		}

		t += add;										// 経過時間を加算
		bpm = (double)mBmsData[tempo][i].fData;		// 次のBPMをセット
		cnt = mBmsData[tempo][i].llTime;			// 計算済みのカウントをセット
	}

//	DEBUG( "  BPM %f\n",bpm );
//	DEBUG( "  CNT %I64d\n",cnt );

	// 指定時間と1つ前までの時間の差分
	double sub = sec - t;
//	DEBUG( "  差分 %f秒\n",sub );

	// 差分からBMSカウント数を算出
	LONGLONG cnt2 = (LONGLONG)(sub * (BMSDATA_RESOLUTION / 4) * ( bpm / 60 ));

	// BMSカウント値に加算
	cnt += cnt2;
//	DEBUG( "  結果のカウント値 %I64d\n",cnt );

	return cnt;
}
