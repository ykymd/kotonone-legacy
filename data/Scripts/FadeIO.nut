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
