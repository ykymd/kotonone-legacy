//const.nut
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

/*
// 判定結果
enum JUDGE
{
    Critical,       // ピッタリ
    Near,           // ややずれてる
    Failed,         // 失敗
    None,           //　無視
}
*/

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


//var.nut
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


//FadeIO.nut
/* フェードイン/アウト処理 */

// 名前空間用のテーブルと変数を定義する
FadeIO <- {
    // フェードイン/アウト処理を実行中か否か
    FadeIn_Flag = false,
    FadeOut_Flag = false,

    // フェードイン/アウト用のカウンタ
    FadeIO_Counter = 0,

    // フェードイン/アウトの色
    FadeIO_Color = 0,

    // フェードイン/アウトの処理で毎回加減算する値
    FadeIO_Num = 0,
};

// フェードイン処理を開始する関数
function FadeIO::startFadeIn(frame, r, g, b)
{
    FadeIn_Flag = true;
    FadeOut_Flag = false;
    FadeIO_Counter = 0;
    FadeIO_Color = GetColor(r, g, b);
    FadeIO_Num = 255 / frame;
}

// フェードアウト処理を開始する関数
function FadeIO::startFadeOut(frame, r, g, b)
{
    FadeIn_Flag = false;
    FadeOut_Flag = true;
    FadeIO_Counter = 0;
    FadeIO_Color = GetColor(r, g, b);
    FadeIO_Num = 255 / frame;
}

// フェードイン/アウト処理を終了する関数
function FadeIO::stop()
{
    FadeIn_Flag = false;
    FadeOut_Flag = false;
    FadeIO_Counter = 0;
    FadeIO_Color = 0;
    FadeIO_Num = 0;
}

// フェードイン/アウト用の更新処理
function FadeIO::update()
{
    // フェードイン/アウト用のカウンタをインクリメントする
    if(FadeIn_Flag || FadeOut_Flag)
        FadeIO_Counter++;
}

// フェードイン/アウト用の描画処理
function FadeIO::draw()
{
    // フェードイン/アウト用の矩形で画面を塗りつぶす
    if(FadeIn_Flag)
    {
        local param = 255 - FadeIO_Num * FadeIO_Counter;
        if(param > 0)
        {
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, param);
            DrawBox(0, 0, 640, 480, FadeIO_Color, TRUE);

            // ブレンドモードのパラメータを元に戻す
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
        }
    }
    else if(FadeOut_Flag)
    {
        local param = FadeIO_Num * FadeIO_Counter;
        if(param > 255) param = 255;
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, param);
        DrawBox(0, 0, 640, 480, FadeIO_Color, TRUE);

        // ブレンドモードのパラメータを元に戻す
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
    }
}


//thread.nut
// スレッド用の名前空間
Thread <- {
    Thread_Object = null,
};

// スレッドを作成する
function Thread::createThread(func)
{
    // 中断状態であればスレッドを終了
    stopThread();

    // オブジェクトの作成
    Thread_Object = newthread(func);
}

// スレッドを終了する
function Thread::stopThread()
{
    if(Thread_Object != null)
    {
        if(Thread_Object.getstatus() != "idle")
        {
            Thread_Object.wakeup(true);
        }
    }
}

// スレッドを実行/再開する関数
function Thread::executeThread()
{
    // スレッドオブジェクトが作成されており、
    if(Thread_Object != null)
    {
        // 中断状態なら、再開する
        if( Thread_Object.getstatus() == "suspended" )
        {
            return Thread_Object.wakeup(false);
        }
            // アイドル状態なら、実行する
        else if( Thread_Object.getstatus() == "idle" )
        {
            return Thread_Object.call();
        }
    }
}

// 引数に指定した回数、中断処理を行う関数
function Thread::wait(frame)
{
    for(local i=0; i<frame; i++)
    {
        if( suspend(false) ) return true;
    }
    return false;
}

// キーが押されるまで中断処理を行う関数
function Thread::waitPushKey()
{
    do {
        if( suspend(false) ) return true;
    } while( !GetJoypadInputStatePushed(INPUT_TYPE.ONE) );

    return false;
}


//resource.nut
// テーブル内から指定画像のIDを取得する
function getImgID(file_name, index=0)
{
    return g_ResourceList[file_name][index].img_id;
}

// 画像情報テーブルを元に、画像ファイルを読み込む
function loadResource()
{
    // 全ての画像を読み込む
    foreach(file_name, img_chip_arr in g_ResourceList)
    {
        // 画像ファイルを読み込む
        local tmp_handle = LoadGraph(".\\data\\img\\" + file_name);

        // 画像が1枚だった場合はその画像のみをセットする
        if( img_chip_arr.len() == 1 )
        {
            img_chip_arr[0].img_id = tmp_handle;
        }
        // 画像が複数枚ある場合は一部の画像をimg_idにセットする
        else
        {
            foreach(val in img_chip_arr)
            {
                val.img_id = DerivationGraph(val.x, val.y, val.w, val.h, tmp_handle);
            }
        }
    }

    /*
    // ?A?j???[?V??????mg_id??????????
    foreach(key, anime_group in g_AnimeInfoList)
    {
        foreach(key, anime_info in anime_group)
        {
            foreach(val in anime_info.anime)
            {
                val.img_id = g_ResourceList[val.img_file][val.img_idx].img_id;
            }
        }
    }
    */
}

// 使用する素材のリスト
g_ResourceList <- {
    ["bg_gamescene.jpg"] = [
        { img_id=-1, },
    ],
    ["skin_game.png"] = [
        { img_id=-1, },
    ],
    ["note1.png"] = [
        { img_id=-1, },
    ],
    ["Logo.png"] = [
        { img_id=-1, },
    ],
    ["title.png"] = [
        { img_id=-1, },
    ],
    ["songbanner_selected.png"] = [
        { img_id=-1, },
    ],
    ["songbanner_unselected.png"] = [
        { img_id=-1, },
    ],
    ["infobanner1_select.png"] = [
        { img_id=-1, },
    ],
    ["levelselect2.png"] = [
        { img_id=-1, },
    ],
    ["infogauge_play.png"] = [
        { img_id=-1, },
    ],
    ["infobanner1_play.png"] = [
        { img_id=-1, },
    ],
    ["star_ep_w.png"] = [
        { img_id=-1, },
    ],
    ["star_ep_y.png"] = [
        { img_id=-1, },
    ],
    ["star_ch_w.png"] = [
        { img_id=-1, },
    ],
    ["star_ch_y.png"] = [
        { img_id=-1, },
    ],
    ["lane_test.png"] = [
        { img_id=-1, },
    ],
    ["infobanner2_mode_mu.png"] = [
        { img_id=-1, },
    ],
    ["infobanner2_mode_lv.png"] = [
        { img_id=-1, },
    ],
    ["levelE.png"] = [
        { img_id=-1, },
    ],
    ["levelN.png"] = [
        { img_id=-1, },
    ],
    ["levelH.png"] = [
        { img_id=-1, },
    ],
    ["gauge_ep_w.png"] = [
        { img_id=-1, },
    ],
    ["gauge_ep_r.png"] = [
        { img_id=-1, },
    ],
    ["gauge_ch_w.png"] = [
        { img_id=-1, },
    ],
    ["gauge_ch_r.png"] = [
        { img_id=-1, },
    ],
    ["nasubi1.png"] = [
        { img_id=-1, },
    ],
    ["nasubi2.png"] = [
        { img_id=-1, },
    ],
    ["result1.png"] = [
        { img_id=-1, },
    ],
    ["result2.png"] = [
        { img_id=-1, },
    ],
    ["result3.png"] = [
        { img_id=-1, },
    ],
    ["keybeam.png"] = [
        { img_id=-1, },
    ],
    ["judge_great.png"] = [
        { img_id=-1, },
    ],
    ["judge_miss.png"] = [
        { img_id=-1, },
    ],
    ["judge_near.png"] = [
        { img_id=-1, },
    ]
};


//SongList.nut
// 楽曲のリスト
g_SongData <- {
    [0] =
        {
            title = "7th Cruise",
            composer = "みるく",
            genre = "Ambient House",
            bpm = 171,
            filename = "7thCruise",
            level = [4, 8, 10],
        },

    [1] =
        {
            title = "Double Dagger",
            composer = "Trimally",
            genre = "Techno",
            bpm = 212,
            filename = "Double Dagger",
            level = [3, 8, 10],
        },
    [2] =
        {
            title = "RAYNs",
            composer = "みるく",
            genre = "Rock",
            bpm = 180,
            filename = "RAYNs",
            level = [3, 5, 9],
        },
    [3] =
        {
            title = "SOLID CITY",
            composer = "Rocky",
            genre = "Rock",
            bpm = 170,
            filename = "SOLID CITY",
            level = [1, 4, 7],
        },

    // SweetPanic
    [4] =
        {
            title = "Sweet×Panic!",
            composer = "mossari",
            genre = "Happy Hardcore",
            bpm = 190,
            filename = "SweetPanic",
            level = [2, 6, 8],
        },

    [5] =
        {
            title = "Torrid Zone",
            composer = "mossari",
            genre = "Tech Dance",
            bpm = 150,
            filename = "Torrid Zone",
            level = [2, 7, 10],
        },

    [6] =
        {
            title = "コトノネドライブ",
            composer = "Rocky",
            genre = "Theme",
            bpm = 180,
            filename = "kotonone_the",
            level = [1, 5, 9],
        },

    [7] =
        {
            title = "琴古主との邂逅",
            composer = "Yokohei",
            genre = "Hard Style",
            bpm = 180,
            filename = "kotohu king",
            level = [4, 8, 10],
        },
};


//songwnd.nut
class SongWnd {
    // 座標(左上)
    x = 0.0;
    y = 0.0;

    // 大きさ
    w = WND_W;
    h = WND_H;

    // ID (0が真ん中であり選択中状態。IDは正負ありで相対的な位置関係を示す)
    id = 0;

    fontHandle = 0;

    // 曲データ
    SongData = {};

    // コンストラクタ
    constructor(dat)
    {
        this.SongData = dat;
        fontHandle = CreateFontToHandle("@ＭＳ 明朝", -1, -1, -1);
    }

    function setID(i)
    {
        this.id = i;
        x = WINDOW_WIDTH/2-WND_W/2 + WND_INTERVAL*id + WND_W*id;
        y = WINDOW_HEIGHT/2-WND_H/2;

        if (i==0)
            g_SongTitle = SongData.title;
    }

    function getSongFileName()
    {
        return SongData.filename;
    }

    // 更新処理
    function onUpdate()
    {
    }

    //　描画処理
    function onDraw()
    {
        if (id < 4 && id > -4){
            DrawGraph(x, y, getImgID((id==0) ? "songbanner_selected.png" : "songbanner_unselected.png"));
            DrawString(x+10,y+WND_H-45,"BPM",GetColor(255,255,255),20,0);
            DrawString(x+WND_W-40,y+WND_H-45,SongData.bpm,GetColor(255,255,255),20,0);

            // タイトルを描画
            DrawSongTitleVString(x+WND_W/2+5, y+10, SongData.title, GetColor(255,255,255));

            // コンポーザーを描画
            DrawSongSubVString(x+30, y+60, SongData.composer, GetColor(255,255,255));

            // ジャンルを描画
            DrawSongSubVString(x+5, y+60, SongData.genre, GetColor(255,255,255));
        }
    }
}


//Logo.nut
//　ロゴ表示画面のクラス
Logo <- {};

function Logo::initialize()
{
    print("Logo init");

    // フェードイン/アウト用のパラメータの初期化
    FadeIO.stop();

    // タイトル画面用のスレッドを作成する
    Thread.createThread(threadFunction);
}

function Logo::finalize()
{
    print("Logo final");

    Thread.stopThread();
}

function Logo::threadFunction()
{
    //print("Title Thread");
    // フェードインを開始する
    FadeIO.startFadeIn(90,0,0,0);

    // 中断処理を行う
    if (Thread.wait(180)) return true;

    // フェードアウト処理を開始する
    FadeIO.startFadeOut(45, 255, 255, 255);

    // しばらく待つ
    if( Thread.wait(120) ) return true;

    // ゲームシーンへと切り替える
    ChangeScene("Title");

    return true;
}

function Logo::update()
{
    // スレッドを実行する
    Thread.executeThread();

    // フェードイン/アウト用の処理を行う
    FadeIO.update();
}

function Logo::draw()
{
    // ロゴの描画
    DrawGraph(0,0,getImgID("Logo.png"));

    // フェードイン/アウト用の処理を行う
    FadeIO.draw();
}


//Title.nut
//　ロゴ表示画面のクラス
Title <- {};

function Title::initialize()
{
    print("Title init");

    // フェードイン/アウト用のパラメータの初期化
    FadeIO.stop();

    // タイトル画面用のスレッドを作成する
    Thread.createThread(threadFunction);

    // SEを読み込む
    LoadSE(0);
    LoadSE(1);

    //BGMの再生
    PlaySoundFile(".\\data\\snd\\タイトルBGM.mp3", DX_PLAYTYPE_LOOP);
}

function Title::finalize()
{
    print("Title final");

    Thread.stopThread();
}

function Title::threadFunction()
{
    //print("Title Thread");
    // フェードインを開始する
    FadeIO.startFadeIn(30,0,0,0);

    // 中断処理を行う
    if (Thread.wait(30)) return true;

    while (true)
    {
        if (GetJoypadInputStatePushed(INPUT_TYPE.SPACE))
        {
            print("KEY Pushed");
            PlaySE(0);
            break;
        }

        if (GetJoypadInputStatePushed(INPUT_TYPE.ESC))
        {
            PlaySE(1);
            EXIT();
        }

        if (suspend(false)) return true;
    }

    // フェードアウト処理を開始する
    FadeIO.startFadeOut(45, 255, 255, 255);

    // しばらく待つ
    if( Thread.wait(90) ) return true;

    //StopSoundMem(soundHandle);
    //DeleteSoundMem(soundHandle);

    // ゲームシーンへと切り替える
    ChangeScene("Select");

    return true;
}

function Title::update()
{
    // スレッドを実行する
    Thread.executeThread();

    // フェードイン/アウト用の処理を行う
    FadeIO.update();
}

function Title::draw()
{
    // ロゴの描画
    DrawGraph(0,0,getImgID("title.png"));

    // フェードイン/アウト用の処理を行う
    FadeIO.draw();
}


//Select.nut
//　曲選択画面のクラス
Select <- {
    buttonClicked = FALSE,
    soundHandle = 0,    //音声用のハンドル
    speedType = 3,
    isDiffMode = false,
    // セレクト画面のウインドウを格納するテーブル
    SongWindows = {},
    centerWndKey = 0,   //中心の要素
    //stri = array("a",0),
};

function Select::SetAvailableSong()
{
    // 曲数を取得
    g_songNum = g_SongData.len();
}

function Select::SetSongWindow()
{
    foreach(key, val in g_SongData) {
        local tmp = SongWnd(val);
        tmp.setID(key);
        SongWindows[key] <- tmp;
    }

    centerWndKey = 0;
}

// 選択した難易度を元に、正しいファイル名を導き出す
function Select::setFileName()
{
    g_SongTitle = "";
    g_SongTitle = SongWindows[centerWndKey].getSongFileName();
    switch (g_selectedLevel)
    {
    case 0:
        //Easy
        g_SongTitle += "_easy";
        break;
    case 1:
        //Normal
        g_SongTitle += "_norm";
        break;
    case 2:
        //hard
        g_SongTitle += "_hard";
        break;
    }

    g_SongTitle += ".bme";
}

function Select::initialize()
{
    print("Select init");

    isDiffMode = false;

    // フェードイン/アウト用のパラメータの初期化
    FadeIO.stop();

    // タイトル画面用のスレッドを作成する
    Thread.createThread(threadFunction);

    //g_bms.GetSelectSceneData();   //利用可能曲を取得
    SetAvailableSong(); //テーブル内に格納する
    SetSongWindow();    //曲表示ウインドウを作成

    // SEを読み込む
    LoadSE(0);
    LoadSE(1);
    LoadSE(2);
    LoadSE(3);

    // BGMの再生
    PlaySoundFile(".\\data\\snd\\select.mp3", DX_PLAYTYPE_BACK);
}

function Select::finalize()
{
    print("Select final");

    Thread.stopThread();
}

function Select::threadFunction()
{
    // フェードインを開始する
    FadeIO.startFadeIn(30,0,0,0);

    // 中断処理を行う
    if (Thread.wait(30)) return true;

    while (true)
    {
        if (GetJoypadInputStatePushed(INPUT_TYPE.LEFT) && !Select.buttonClicked)
        {
            PlaySE(2);
            if (!Select.isDiffMode && Select.SongWindows[0].id < 0){
                foreach(key, val in Select.SongWindows) val.setID(val.id+1);
                Select.centerWndKey--;
            }else{
                // 難易度選択時は速度調整
                Select.speedType--;
                if (Select.speedType < HIGHSPEED_MIN) Select.speedType = HIGHSPEED_MIN;
            }
        }
        if (GetJoypadInputStatePushed(INPUT_TYPE.RIGHT) && !Select.buttonClicked)
        {
            PlaySE(2);
            if (!Select.isDiffMode && Select.SongWindows[g_songNum-1].id > 0){
                foreach(key, val in Select.SongWindows) val.setID(val.id-1);
                Select.centerWndKey++;
            }else{
                // 難易度選択時は速度調整
                Select.speedType++;
                if (Select.speedType > HIGHSPEED_MAX) Select.speedType = HIGHSPEED_MAX;
            }
        }
        if (GetJoypadInputStatePushed(INPUT_TYPE.UP) && !Select.buttonClicked)
        {
            PlaySE(2);
            if (!Select.isDiffMode){
                Select.speedType++;
                if (Select.speedType > HIGHSPEED_MAX) Select.speedType = HIGHSPEED_MAX;
            }else{
                g_selectedLevel--;
                if (g_selectedLevel < 0) g_selectedLevel=0;
            }
        }
        if (GetJoypadInputStatePushed(INPUT_TYPE.DOWN) && !Select.buttonClicked)
        {
            PlaySE(2);
            if (!Select.isDiffMode){
                Select.speedType--;
                if (Select.speedType < HIGHSPEED_MIN) Select.speedType = HIGHSPEED_MIN;
            }else{
                g_selectedLevel++;
                if (g_selectedLevel > 2) g_selectedLevel=2;
            }
        }
        if (GetJoypadInputStatePushed(INPUT_TYPE.SPACE) && !Select.buttonClicked)
        {
            if (!Select.isDiffMode){
                print("KEY SPACE Pushed");
                PlaySE(2);
                Select.isDiffMode = true;
            }else{
                //Select.GetFileName();
                PlaySE(3);
                break;
            }
        }
        if (GetJoypadInputStatePushed(INPUT_TYPE.ESC) && !Select.buttonClicked)
        {
            PlaySE(1);
            if (Select.isDiffMode) {
                Select.isDiffMode = false;
            }else{
                ChangeScene("Title");
            }
        }

        if (suspend(false)) return true;
    }

    Select.setFileName();

    // ハイスピード値をセット
    g_bms.SetHighSpeed(Select.speedType*0.5);

    // フェードアウト処理を開始する
    FadeIO.startFadeOut(45, 255, 255, 255);

    // しばらく待つ
    if( Thread.wait(120) ) return true;

    // ゲームシーンへと切り替える
    ChangeScene("GamePlay");

    StopSoundFile();

    return true;
}

function Select::update()
{
    //ウインドウの更新
    foreach(key, val in SongWindows)
        val.onUpdate();

    // スレッドを実行する
    Thread.executeThread();

    // フェードイン/アウト用の処理を行う
    FadeIO.update();
}

function Select::ShowSong()
{
    local str = "";
    local y = 0;
    foreach (key, val in g_SongData){
        str = "TITLE :"+val.title+"\n";
        DrawString(0,y,str,GetColor(255,255,255),12,0);

        y+=14;

        str = "GENRE :"+val.genre+"\n";
        DrawString(0,y,str,GetColor(255,255,255),12,0);
        y+=14;

        str = "ARTIST:"+val.composer+"\n";
        DrawString(0,y,str,GetColor(255,255,255),12,0);
        y+=14;

        str = "BPM   :"+val.bpm+"\n"+"\n";
        DrawString(0,y,str,GetColor(255,255,255),12,0);
        y+=20;
    }
}
function Select::draw()
{
    // 背景の描画
    DrawGraph(0,0,getImgID("bg_gamescene.jpg"));

    // ウインドウの描画
    foreach(key, val in SongWindows)
        val.onDraw();

    // 情報バー(下部)を表示
    DrawGraph(240-10,420-10,getImgID("infobanner1_select.png"));

    DrawString(240-10+140,420-10-15, speedType,GetColor(255,255,255),30,0);

    if (isDiffMode){
        DrawGraph(0,150,getImgID("levelselect2.png"));

        // 難易度を描画
        drawDiffStars();

        //DrawString(0,150+40, "TEST", GetColor(255,255,255), 30, 0);

    }

    // 上部バナーを表示
    if (!isDiffMode){
        DrawGraph(0,0,getImgID("infobanner2_mode_mu.png"));
    }else{
        DrawGraph(0,0,getImgID("infobanner2_mode_lv.png"));
    }

    //DrawVString(0,0,g_SongData[0].title, GREEN, fontHandle);

    //ShowSong();

    // フェードイン/アウト用の処理を行う
    FadeIO.draw();

}
//難易度の星の描画
function drawDiffStars()
{
    //難易度の星の描画
    for (local i=0; i<30; i++){
        local imdID = getImgID((i%10 < SongWindows[centerWndKey].SongData.level[i/10]) ? "star_ch_w.png" : "star_ep_w.png");
        DrawGraph(DIFFSTAR_X+STAR_W*(i%10),g_DIFFSTAR_Y[(i/10).tointeger()], imdID);
    }

    // 選択中ラインを発光
    for (local i=0; i<10; i++)
        DrawGraph(DIFFSTAR_X+STAR_W*(i%10),g_DIFFSTAR_Y[g_selectedLevel],getImgID((i%10 < SongWindows[centerWndKey].SongData.level[g_selectedLevel]) ? "star_ch_y.png" : "star_ep_y.png"));
}


//gameplay.nut
//　ゲーム画面のクラス
GamePlay <-{
    cnt = 0,
    bmsNum = 0,
    noteNum = array(8,0),
    judgeNum = array(8,0),
    numAllNote = 0,    //全ノーツ数
    beforePress = array(4,FALSE),   //レーンL1からL4までを直前フレームで押したか
    songTitle = "",
    bpmLabel = 0,
    highSpeed = 0,
    scoreFinished = false,
    operationLock = false,
    showJudge = array(2,0), //直近の判定を格納する(0:左、1:右)
};

function GamePlay::initialize()
{
    print("GamePlay init");

    // フェードイン/アウト用のパラメータの初期化
    FadeIO.stop();

    // スレッドを作成する
    Thread.createThread(threadFunction);

    g_bms.Load(".\\data\\snd\\"+g_SongTitle);

    bmsNum = 0;

    // 累積ノーツ数の初期化
    g_NotesNum = 0;

    g_numAllNote = 0;

    // 曲名のセット
    songTitle = g_bms.GetTitle();

    // BPMのセット
    bpmLabel = g_bms.GetBPM();

    // ハイスピード値を取得
    highSpeed = g_bms.GetHighSpeed();

    //操作ロックを解除
    operationLock = false;

    //noteSizeの初期化
    for (local i=0; i<8; i++){
        g_noteSize[i] = 0;
    }

    //　ノーツの格納テーブルの初期化
    //g_NotesTable = {};

    // タイトル画面用のスレッドを作成する
    //Thread.createThread(threadFunction);

    //newNote({lane=LANE_ID.L1, speed=1.0, frame=0});
    //newNote({lane=LANE_ID.R2, speed=3.0, frame=0});

    //foreach (key, val in g_NotesTable){
    //    val.initNote();
    //}

    //累積ノーツ数の初期化
    for (local i=0; i<8; i++){
        noteNum[i] = 0;
        judgeNum[i] = 0;
    }

    g_numAllNote = 0;

    // 全ノーツの最大数を記録
    for (local i=0; i<4; i++){
        g_noteSize[i] = g_bms.GetObjNum(g_LaneIndex[i]);
        g_numAllNote += g_noteSize[i]/2;
        print("noteSize["+i+"]="+g_noteSize[i]);
    }
    for (local i=4; i<8; i++){
        g_noteSize[i] = g_bms.GetObjNum(g_LaneIndex[i]);
        g_numAllNote += g_noteSize[i];
        print("noteSize["+i+"]="+g_noteSize[i]);
    }

    print("全ノーツ数:"+g_numAllNote);

    //スコアのリセット
    g_bms.Score = 0;

    //再生開始
    g_bms.Play();
}

function GamePlay::finalize()
{
    print("GamePlay final");
    g_bms.Stop();
    Thread.stopThread();
}

function GamePlay::threadFunction()
{
    //print("Title Thread");
    // フェードインを開始する
    FadeIO.startFadeIn(30,0,0,0);

    // 中断処理を行う
    if (Thread.wait(30)) return true;

    while (true)
    {
        if (GetJoypadInputStatePushed(INPUT_TYPE.ESC))
        {
            print("Song Cancelled");
            g_bms.Stop();
            break;
        }

        if (suspend(false)) return true;
    }

    // フェードアウト処理を開始する
    FadeIO.startFadeOut(45, 255, 255, 255);

    // しばらく待つ
    if( Thread.wait(90) ) return true;

    //StopSoundMem(soundHandle);
    //DeleteSoundMem(soundHandle);

    GamePlay.operationLock = true;    //操作ロックをかける

    // ゲームシーンへと切り替える
    ChangeScene("Result");

    return true;
}

// 矢印キーそれぞれが押されているか調べる
function GamePlay::getHitArrowKey()
{
    if (GetJoypadInputStatePushed(INPUT_TYPE.LEFT))
    {
        print("LEFT Pushed");
    }
    else if (GetJoypadInputStatePushed(INPUT_TYPE.UP))
    {
        print("UP Pushed");
    }
    else if (GetJoypadInputStatePushed(INPUT_TYPE.RIGHT))
    {
        print("RIGHT Pushed");
    }
    else if (GetJoypadInputStatePushed(INPUT_TYPE.DOWN))
    {
        print("DOWN Pushed");
    }
}

// 各パッドキーそれぞれが押されているか調べる
function GamePlay::getHitPadKey()
{
    for (local i=0; i<4; i++){
        if (GetJoypadInputStatePressed(g_LaneKey[i]))
        {
            // ボタンの押し始めの時
            if (!beforePress[i]){
                print("KEY "+(i+1)+" onPress");
                beforePress[i] = true;

                for (local j=judgeNum[i]; j<g_noteSize[i]; j+=2){
                    local ext = g_bms.NoteJudge(g_LaneIndex[i],j);

                    switch (ext)
                    {
                    case 1:
                        // 無視
                        showJudge[0] = 0;
                        break;
                    case 2:
                        // 早すぎ
                        /*
                        if (g_bms.Score >= 0.2) g_bms.Score -= 0.2;
                        print ("Score:-0.2");
                        */
                        showJudge[0] = 1;   //失敗
                        break;
                    case 3:
                        // ややずれ
                        g_bms.Score += 0.8;
                        print ("Score:+0.8");
                        showJudge[0] = 2;   //近い
                        break;
                    case 4:
                        // ピッタリ
                        g_bms.Score += 1.0;
                        print ("Score:+1.0");
                        showJudge[0] = 3;
                        break;
                    }

                    // 一番近いノーツが無視判定圏内ならループ中断
                    if (ext != 0) break;
                }
            }
        }
        // ボタンから指を離した時
        else if (beforePress[i]) {
            print("KEY "+(i+1)+" onUp");
            beforePress[i] = false;

            for (local j=judgeNum[i]; j<g_noteSize[i]; j++){
                local ext = g_bms.LongNoteJudge(g_LaneIndex[i],judgeNum[i]);

                // 一番近いノーツが無視判定圏内ならループ中断
                if (ext == 1) break;
            }
        }
    }

    for (local i=4; i<8; i++){
        if (GetJoypadInputStatePushed(g_LaneKey[i]))
        {
            //print("KEY "+(i+1)+" Pushed");
            for (local j=judgeNum[i]; j<g_noteSize[i]; j++){
                local ext = g_bms.NoteJudge(g_LaneIndex[i],j);

                switch (ext)
                {
                case 1:
                    // 無視
                    showJudge[1] = 0;
                    break;
                case 2:
                    // 早すぎ
                    /*
                    if (g_bms.Score >= 0.2) g_bms.Score -= 0.2;
                    print ("Score:-0.2");
                    */
                    showJudge[1] = 1;
                    break;
                case 3:
                    // ややずれ
                    g_bms.Score += 0.8;
                    showJudge[1] = 2;
                    break;
                case 4:
                    // ピッタリ
                    g_bms.Score += 1.0;
                    print ("Score:+1.0");
                    showJudge[1] = 3;
                    break;
                }

                // 一番近いノーツが無視判定圏内ならループ中断
                if (ext != 0) break;
            }
        }
    }
}

function GamePlay::getScoreData()
{
    if (cnt in g_ScoreTable)
    {
        foreach (val in g_ScoreTable[cnt]){
            newNote(val);
        }
    }
}

// 更新処理(毎フレーム呼ばれる)
function GamePlay::update()
{
    cnt++;

    if (!operationLock){
        // 見逃し判定
        for (local j=0; j<4; j++){
            for (local i=judgeNum[j]; i<g_noteSize[j]; i+=2){
                local tmp = g_bms.LongNoteJudgeOver(g_LaneIndex[j],i);
                if (tmp == 0) {
                    showJudge[0] = 0;
                    judgeNum[j] = i+2;
                }else{
                    break;
                }
            }
        }

        for (local j=4; j<8; j++){
            for (local i=judgeNum[j]; i<g_noteSize[j]; i++){
                local tmp = g_bms.NoteJudgeOver(g_LaneIndex[j],i);
                switch(tmp)
                {
                // オブジェクトが存在しない
                case 0:
                    break;
                // オブジェクトは存在してるが、まだ失敗判定が出る位置ではない
                case 1:
                    break;
                // オブジェクトが存在し、かつ失敗判定が出る位置である
                case 2:
                    showJudge[1] = 1;
                    break;
                }
                if (tmp == 0 || tmp == 2) judgeNum[j] = i+1;
                break;
            }
        }

        getScoreData();
        getHitArrowKey();
        getHitPadKey();
    }

    if (g_bms.isFinished()){
        //g_bms.Clear();
        ChangeScene("Result");
    }

    // スレッドを実行する
    Thread.executeThread();

    // フェードイン/アウト用の処理を行う
    FadeIO.update();
}

// ゲーム領域の描画を行う
function GamePlay::drawGameArea()
{
    // スキン画像を表示
    DrawGraph(0, HALF_HEIGHT-381/2+15, getImgID("skin_game.png"));

    DrawGraph(GAME_AREA.LEFT, GAME_AREA.TOP, getImgID("lane_test.png"));

    /*
    // レーン幅を描画する
    for (local i=0; i<=LANE_NUM; i++){
        DrawLine(GAME_AREA.LEFT, GAME_AREA.TOP+NOTE_H*i, GAME_AREA.RIGHT, GAME_AREA.TOP+NOTE_H*i, BLUE);
    }
    */

    /*
    // レーンを描画する
    for (local i=0; i<LANE_NUM; i++){
        DrawLine(GAME_AREA.LEFT, GAME_AREA.TOP+NOTE_H*i+NOTE_H/2, GAME_AREA.RIGHT, GAME_AREA.TOP+NOTE_H*i+NOTE_H/2, GREEN);
    }
    */

    /*
    // ノーツ出現ラインを描画する
    DrawBox(GAME_AREA.LEFT, GAME_AREA.TOP, GAME_AREA.LEFT+5, GAME_AREA.BOTTOM, GetColor(120,120,120), TRUE);
    DrawBox(GAME_AREA.RIGHT-4, GAME_AREA.TOP, GAME_AREA.RIGHT+1, GAME_AREA.BOTTOM, GetColor(120,120,120), TRUE);

    // レーンを描画する
    for (local i=0; i<LANE_NUM; i++){
        DrawLine(GAME_AREA.LEFT, GAME_AREA.TOP+NOTE_H*i+NOTE_H/2, GAME_AREA.RIGHT, GAME_AREA.TOP+NOTE_H*i+NOTE_H/2, GREEN);
    }

    // 判定ラインを描画する
    DrawBox(HALF_LINE - 3, GAME_AREA.TOP, HALF_LINE + 3, GAME_AREA.BOTTOM, RED, TRUE);
    */
}

function DrawNotes()
{
    local yy;//, index;

    // 左側(ロングノーツオンリー)
    for (local i=0; i<4;i++)
    {
        //index = g_LaneIndex[i];
        //size = bms.GetBarNum(index);

        yy = LANE_Y[i]-30;

        //local startPos = (noteNum[i] > judgeNum[i]) ? noteNum[i] : judgeNum[i];

        for (local j=noteNum[i]; j<g_noteSize[i]; j+=2)
        {
            local note1_x = g_bms.GetObject_X(g_LaneIndex[i], j);
            //if (j+1 >= size) break;
            local note2_x = g_bms.GetObject_X(g_LaneIndex[i], j+1);

            // 判定ラインを超えたなら次回からは次の小節から始める
            if (note2_x < 0)
            {
                noteNum[i] = j+2;
                continue;
            }

            // ゲーム画面外に居るなら、これ以降の描写はスキップ
            if (note1_x > GAME_AREA_WIDTH/2)
                break;

            // ロングノーツの先端が既に画面外の時は縁を先端にする
            if (note1_x < 0) note1_x = 0;

            // ロングノーツの末尾がまだ画面外の時は縁を末尾にする
            if (note2_x > GAME_AREA_WIDTH/2) note2_x = GAME_AREA_WIDTH/2;

            // 画面内なら描画する
            DrawBox(note1_x+GAME_AREA.LEFT, yy, note2_x+GAME_AREA.LEFT, yy+NOTE_H, RED,TRUE);
        }
    }

    // 右側
    for (local i=4; i<8; i++)
    {
        //index = g_LaneIndex[i];
        //size = bms.GetObjNum(index);

        yy = LANE_Y[i-4];

        local startPos = (noteNum[i] > judgeNum[i]) ? noteNum[i] : judgeNum[i];

        //print("ObjNum:"+(i+11)+"->"+bms.GetObjNum(index));
        for (local j=startPos; j<g_noteSize[i]; j++)
        {
            local note_x = g_bms.GetObject_X(g_LaneIndex[i], j);

            // 判定ラインを超えたなら次回からは次の小節から始める
            if (GAME_AREA.RIGHT-note_x > GAME_AREA.RIGHT)
            {
                noteNum[i] = j+1;
                continue;
            }

            // ゲーム画面外に居るなら、これ以降の描写はスキップ
            if (note_x > GAME_AREA_WIDTH/2)
                break;

            // 画面内なら描画する
            local xx = GAME_AREA.RIGHT-note_x;
            //DrawLine(GAME_AREA.RIGHT-note_x, yy, GAME_AREA.RIGHT-note_x, yy+NOTE_H, RED);
            //DrawBox(xx-NOTE_W/2, yy, xx+NOTE_W/2, yy+NOTE_H, RED, TRUE);
            DrawGraph(xx-NOTES_W/2, yy-NOTES_H/2, getImgID("note1.png"));
        }
    }
}

//小節線の描画を行う
function DrawBarLine()
{
    for (local i=bmsNum; i<g_bms.GetBarNum(); i++)
    {
        local xx = g_bms.GetBarLine_X(i);
        //print("xx="+xx);

        // 判定ラインを超えたなら次回からは次の小節から始める
        if (xx < 0) {
            bmsNum = i+1;
            continue;
        }

        // ゲーム画面外に居るなら、これ以降の描写はスキップ
        if (xx > GAME_AREA_WIDTH/2)
            break;

        // 画面内なら描画する
        // 左へ流れる
        DrawLine(xx+GAME_AREA.LEFT, GAME_AREA.TOP, xx+GAME_AREA.LEFT, GAME_AREA.BOTTOM, GetColor(255,255,255));
        // 右へ流れる
        DrawLine(GAME_AREA.RIGHT-xx, GAME_AREA.TOP, GAME_AREA.RIGHT-xx, GAME_AREA.BOTTOM, GetColor(255,255,255));
    }
}

function GamePlay::drawInfoBanner()
{
    // ゲージの描画
    DrawGraph(WINDOW_WIDTH-632, WINDOW_HEIGHT-120, getImgID("infogauge_play.png"));

    // ゲージ画像の描画
    for (local i=0; i<20; i++){
        DrawGraph(WINDOW_WIDTH-608+(25+4)*i, WINDOW_HEIGHT-48, getImgID((i<11) ? "gauge_ep_w.png" : "gauge_ep_r.png"));
    }

    // スコアの取得
    local sc = (g_bms.Score/g_numAllNote*100).tointeger();

    // 取得済みゲージ画像の描画
    for (local i=0; i<(sc/5).tointeger(); i++)
    {
        DrawGraph(WINDOW_WIDTH-608+(25+4)*i, WINDOW_HEIGHT-48, getImgID((i<11) ? "gauge_ch_w.png" : "gauge_ch_r.png"));
    }

    // 達成率の描画
    DrawString(WINDOW_WIDTH-82, WINDOW_HEIGHT-120, sc, GetColor(255,255,255), 30, 0);

    // 情報バナーの描画
    DrawGraph(WINDOW_WIDTH-570, WINDOW_HEIGHT-130, getImgID("infobanner1_play.png"));

    // 難易度画像の描画
    switch(g_selectedLevel){
        case 0:
            DrawGraph(WINDOW_WIDTH-520, WINDOW_HEIGHT-115, getImgID("levelE.png"));
            break;
        case 1:
            DrawGraph(WINDOW_WIDTH-520, WINDOW_HEIGHT-115, getImgID("levelN.png"));
            break;
        case 2:
            DrawGraph(WINDOW_WIDTH-520, WINDOW_HEIGHT-115, getImgID("levelH.png"));
            break;
    }

    //　速度の描画
    DrawString(380,WINDOW_HEIGHT-130+28,highSpeed/0.5,GetColor(255,255,255),20,0);

    //　BPMの描画
    DrawString(380,WINDOW_HEIGHT-130,bpmLabel,GetColor(255,255,255),20,0);
}

// 描画処理(毎フレーム呼ばれる)
function GamePlay::draw()
{
    //背景を描画する
    DrawGraph(0, 0, getImgID("bg_gamescene.jpg"));

    // プレイ領域を描画
    drawGameArea();

    // キービームの描画
    for (local i=0; i<8; i++){
        if (GetJoypadInputStatePressed(g_LaneKey[i])){
            // 左側か
            if (i < 4)
                DrawGraph(GAME_AREA.LEFT, GAME_AREA.TOP+NOTE_H*i, getImgID("keybeam.png"));
            else
                DrawTurnGraph(GAME_AREA.RIGHT-135, GAME_AREA.TOP+NOTE_H*(i-4), getImgID("keybeam.png"), TRUE);
        }
    }

    if(!operationLock){
        // 小節線を描画
        DrawBarLine();

        // ノーツの描画
        DrawNotes();
    }

    // 情報バーを描画
    drawInfoBanner();

    // 曲名の描画
    //DrawString(20,20,songTitle,GetColor(255,255,255),20,0);

    // 判定文字の描画
    switch (showJudge[0])
    {
        case 1:
            DrawGraph(10,10,getImgID("judge_miss.png"));
            break;
        case 2:
            DrawGraph(10,10,getImgID("judge_near.png"));
            break;
        case 3:
            DrawGraph(10,10,getImgID("judge_great.png"));
            break;
    }
    switch (showJudge[1])
    {
        case 1:
            DrawGraph(WINDOW_WIDTH-10-90,10,getImgID("judge_miss.png"));
            break;
        case 2:
            DrawGraph(WINDOW_WIDTH-10-90,10,getImgID("judge_near.png"));
            break;
        case 3:
            DrawGraph(WINDOW_WIDTH-10-90,10,getImgID("judge_great.png"));
            break;
    }

    // デバッグ用フレーム
    //DrawString(0,0,"Frame:"+cnt,GetColor(255,255,255),20,0);

    // フェードイン/アウト用の処理を行う
    FadeIO.draw();
}



//result.nut
//　ロゴ表示画面のクラス
Result <- {
    //soundHandle = 0,  //音声用のハンドル
    songTitle = "",
    score = 0,
};

function Result::initialize()
{
    print("Result init");

    // フェードイン/アウト用のパラメータの初期化
    FadeIO.stop();

    // タイトル画面用のスレッドを作成する
    Thread.createThread(threadFunction);

    songTitle = g_bms.GetTitle();
    score = (g_bms.Score/g_numAllNote*100).tointeger();
    //score = 1;
    PlaySoundFile(".\\data\\snd\\リザルトBGM.mp3", DX_PLAYTYPE_LOOP);
}

function Result::finalize()
{
    print("Result final");

    Thread.stopThread();
}

function Result::threadFunction()
{
    //print("Title Thread");
    // フェードインを開始する
    FadeIO.startFadeIn(30,0,0,0);

    // 中断処理を行う
    if (Thread.wait(30)) return true;

    while (true)
    {
        if (GetJoypadInputStatePushed(INPUT_TYPE.SPACE))
        {
            print("KEY Pushed");
            break;
        }

        if (suspend(false)) return true;
    }

    // フェードアウト処理を開始する
    FadeIO.startFadeOut(45, 255, 255, 255);

    // しばらく待つ
    if( Thread.wait(90) ) return true;

    //StopSoundMem(soundHandle);
    //DeleteSoundMem(soundHandle);

    // ゲームシーンへと切り替える
    // 譜面データをリセット
    g_bms.Clear();
    //EXIT();
    ChangeScene("Select");

    return true;
}

function Result::update()
{
    // スレッドを実行する
    Thread.executeThread();

    // フェードイン/アウト用の処理を行う
    FadeIO.update();
}

function Result::draw()
{
    // ロゴの描画
    //SetDrawBlendMode(DX_BLENDMODE_ALPHA,128);
    if (score < 60) {
        DrawGraph(0,0,getImgID(g_nasubiStill[score%2]));
    }else{
        DrawGraph(0,0,getImgID(g_resultStill[score%3]));
    }
    //SetDrawBlendMode(DX_BLENDMODE_NOBLEND,255);

    // 曲名の描画
    DrawString(20,0,songTitle,GetColor(0,0,0),40,0);

    //
    switch(g_selectedLevel){
        case 0:
            DrawString(20,40,"Easy",GetColor(0,0,0),40,0);
            break;
        case 1:
            DrawString(20,40,"Normal",GetColor(0,0,0),40,0);
            break;
        case 2:
            DrawString(20,40,"Hard",GetColor(0,0,0),40,0);
            break;
    }

    // 達成率の描画
    DrawString(20,80,"Score:"+score+"%",GetColor(0,0,0),40,0);

    // フェードイン/アウト用の処理を行う
    FadeIO.draw();
}



//boot.nut

// ゲーム起動時に呼び出される関数
function boot()
{
    // 画像ファイル群を読み込む
    loadResource();

    // 音声ファイルを初期化する
    InitSoundMem();

    // タイトルシーンへと切り替える
    ChangeScene("Logo");
}
