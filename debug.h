#ifndef _DEBUG_H
#define _DEBUG_H
///////////////////////////////////////////////////////////////////////////////////
// DEBUG : デバッグテキスト出力ライブラリ v1.00                                  //
//   デバッグモードをＯＮにするにはこのファイルをインクルードする前に、          //
//   「#define DEBUGMODE」を定義してください。                                   //
//   また、この定義をコメント化することで、出力を無効にすることが可能です。      //  
//                                                                               //
// このソースコードは自由に改変して使用可能です。                                //
// また商用利用も可能ですが、すべての環境で正しく動作する保障はありません。      //
//                          http://www.charatsoft.com/                           //
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
// 下記の#undefを有効にすることで、各cppで定義されたDEBUGMODEを、                //
// すべて無効化することが出来ます。                                              //
// 最終リリース版を作成する時は、デバッグ出力を無効化してください。              //
///////////////////////////////////////////////////////////////////////////////////

//#undef DEBUGMODE
#define DEBUGMODE

// デバッグモードなら
#ifdef	DEBUGMODE
#include <stdio.h>
#include <time.h>
#define INITDEBUG {\
time_t aclock;\
FILE *_fp;\
_fp=fopen("DEBUG.TXT","w");\
if(_fp) {\
time(&aclock);\
fputs(ctime(&aclock) ,_fp);\
fclose(_fp);\
}\
}

// デバッグ情報を追加する
#define DEBUG(x,y) {\
FILE *_fp;\
_fp=fopen("DEBUG.TXT","a");\
if(_fp){\
fprintf(_fp,x,y);\
fclose(_fp);\
}\
}

// デバッグ情報を追加する
#define DEBUG2(x,y,z) {\
FILE *_fp;\
_fp=fopen("DEBUG.TXT","a");\
if(_fp){\
fprintf(_fp,x,y,z);\
fclose(_fp);\
}\
}

#else

// デバッグモードでなければ何もしない
#define INITDEBUG		{;}
#define DEBUG(x,y)		{;}
#define DEBUG2(x,y,z)	{;}

#endif

#endif

