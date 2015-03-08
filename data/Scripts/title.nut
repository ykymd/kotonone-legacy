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
