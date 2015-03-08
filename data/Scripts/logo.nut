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
