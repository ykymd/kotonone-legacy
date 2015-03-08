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