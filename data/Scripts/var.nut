//
// var.nut
// グローバル変数を記述する
// 変数名の先頭に"g_"を付ける
//

RED <- GetColor(255,0,0);
GREEN <- GetColor(0,255,0);
BLUE <- GetColor(0,0,255);

// ノーツの数(g_notesTableの番号に使う)
// 1プレイ中に出てきた累積ノーツ数
g_NotesNum <- 0;

// 全ノーツを格納するテーブル(Noteクラス)
// 現在画面にあるノーツのインスタンスのみ格納
g_NotesTable <- {};

// 楽譜データを格納する配列
g_ScoreTable <- {};

g_LaneIndex <- [56, 51, 52, 53, 14, 15, 18, 19];

// 楽曲一覧のデータ
//g_SongData <- {};

// 楽曲一覧の要素数
g_songNum <- 0;

// 各レーンに対応しているキー
g_LaneKey <- [
    INPUT_TYPE.ONE,
    INPUT_TYPE.TWO,
    INPUT_TYPE.THREE,
    INPUT_TYPE.FOUR,
    INPUT_TYPE.FIVE,
    INPUT_TYPE.SIX,
    INPUT_TYPE.SEVEN,
    INPUT_TYPE.EIGHT,
    ];

// 再生する楽曲のタイトル
g_SongTitle <- "kotonone_the_hard.bme";

g_bms <- KotononeBMS();

// 難易度表示の星の位置
g_DIFFSTAR_Y <- [DIFFSTAR_Y1,DIFFSTAR_Y2,DIFFSTAR_Y3];

// ノーツのハイスピード
//g_highSpeed <- 1.0;

// 難易度の画像
//g_levelImage = ["levelE.png", "levelN.png", "levelH.png"];

//選択された難易度
g_selectedLevel <- 0;

//読み込んだ譜面の全ノーツ数
g_numAllNote <- 0;

//各レーンの全ノーツ数
g_noteSize <- array(8,0);

// リザルト画面の表示
g_resultNum <- [0, 0, 0, 0];

// リザルト画像
g_resultStill <- ["result1.png", "result2.png", "result3.png"];
g_nasubiStill <- ["nasubi1.png", "nasubi2.png"];
