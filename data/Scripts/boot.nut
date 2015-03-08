// 定数およびグローバル変数のロード
SQInclude("const.nut");
SQInclude("var.nut");

// ライブラリのロード
SQInclude("FadeIO.nut");
SQInclude("thread.nut");
SQInclude("resources.nut");

//SQInclude("note_table.nut");
//SQInclude("note.nut");
//SQInclude("notesmanager.nut");

//　リソースのロード
SQInclude("SongList.nut");

// 各クラスのロード
SQInclude("songwnd.nut");

// 各シーンクラスのロード
SQInclude("logo.nut");
SQInclude("title.nut");
SQInclude("select.nut");
SQInclude("gameplay.nut");
SQInclude("result.nut");

// ゲーム起動時に呼び出される関数
function boot()
{
    // 画像ファイル群を読み込む
    loadResource();

    // 音声ファイルを初期化する
    InitSoundMem();

    // タイトルシーンへと切り替える
    ChangeScene("Select");
}
