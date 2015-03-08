//　ゲーム画面のクラス
GamePlay <-{
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

    // 曲名のセット
    songTitle = g_bms.GetTitle();

    // BPMのセット
    bpmLabel = g_bms.GetBPM();

    // ハイスピード値を取得
    highSpeed = g_bms.GetHighSpeed();

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

    showJudge[0] = 0;
    showJudge[1] = 0;

    operationLock = false;

    //再生開始
    g_bms.Play();
}

function GamePlay::finalize()
{
    print("GamePlay final");

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
    g_bms.Stop();

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
    local cnt;
    if (cnt in g_ScoreTable)
    {
        foreach (val in g_ScoreTable[cnt]){
            newNote(val);
        }
    }
}

function GamePlay::judgeNote()
{
    if (operationLock)
        return;

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
}

// 更新処理(毎フレーム呼ばれる)
function GamePlay::update()
{
    //if (!g_bms.isStart()){
    //    //再生開始
    //    g_bms.Play();
    //}

    judgeNote();

    getScoreData();
    getHitArrowKey();
    getHitPadKey();

    if (g_bms.isStart() && g_bms.isFinished()){
        print("Song Finished.");
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

    // フェードイン/アウト用の処理を行う
    FadeIO.draw();
}
