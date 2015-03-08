//
// const.nut
// それぞれの定数値や列挙値を記述する
//

// ウインドウのサイズ
const WINDOW_WIDTH = 640;
const WINDOW_HEIGHT = 480;

// プレイ領域のサイズ
enum GAME_AREA
{
    LEFT = 50,
    TOP = 80,
    RIGHT = 590,
    BOTTOM = 320,
};

// プレイ領域の幅
const GAME_AREA_WIDTH = 540;
const GAME_AREA_HEIGHT = 240;

// ゲーム画面の中心ライン
const HALF_LINE = 320;

// ゲーム画面の真ん中の横線のY座標
const HALF_HEIGHT = 200;

// 各レーンのID。LRはそれぞれ左右、数字は上から何番目のレーンかを表している
enum LANE_ID
{
    L1 = 0,
    L2 = 1,
    L3 = 2,
    L4 = 3,
    R1 = 4,
    R2 = 5,
    R3 = 6,
    R4 = 7,
    NONE = -1,
};

// レーンの数
const LANE_NUM = 4;

// 各レーンのY座標
const LANE1_Y = 80;
const LANE2_Y = 140;
const LANE3_Y = 200;
const LANE4_Y = 260;

// ノーツの画像幅
const NOTE_W = 15.0;
const NOTE_H = 60.0;

const NOTES_W = 30.0;
const NOTES_H = 80.0;

LANE_Y <- [110, 170, 230, 290];

// セレクト画面の曲表示ウインドウ
const WND_W=90;
const WND_H=286;

// 曲表示ウインドウの間隔
const WND_INTERVAL = 10;

// セレクト画面の星のサイズ
const STAR_W = 20;
const STAR_H = 20;

// 難易度表示の星の位置
const DIFFSTAR_X = 220;
const DIFFSTAR_Y1 = 200;
const DIFFSTAR_Y2 = 233;
const DIFFSTAR_Y3 = 265;

// ハイスピードの限界値
const HIGHSPEED_MAX = 9;
const HIGHSPEED_MIN = 1;