//　ロゴ表示画面のクラス
Result <- {
	//soundHandle = 0,	//音声用のハンドル
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
