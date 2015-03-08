// ゲーム画面のノーツ
class Note {
    // 座標
    x = 0.0;
    y = 0.0;

    // 大きさ
    w = NOTE_W;
    h = NOTE_H;

    // 到達フレーム(判定ラインピッタリになるときのフレーム)
    frame = 0;

    // 速度(絶対値指定)
    v = 0.1;

	//レーン番号
	lane = LANE_ID.NONE;

	// 存在フラグ
	isExist = false;


    // コンストラクタ
    constructor(lane, v, frame){
        this.v = abs(v);
		this.lane = lane;
		this.frame = frame;

		setPosition();
    }

    //　ノーツを画面に表示させる
    function initNote()
    {
    	this.isExist = true;
    }

    // ロングノーツの長さを設定する
    function setNoteWidth(width)
    {
    	this.w = width;
    }

	function setPosition()
	{
		switch (lane){
		case LANE_ID.L1:
			x = HALF_LINE;
			y = LANE1_Y;
			v *= -1;
			break;
		case LANE_ID.L2:
			x = HALF_LINE;
			y = LANE2_Y;
			v *= -1;
			break;
		case LANE_ID.L3:
			x = HALF_LINE;
			y = LANE3_Y;
			v *= -1;
			break;
		case LANE_ID.L4:
			x = HALF_LINE;
			y = LANE4_Y;
			v *= -1;
			break;
		case LANE_ID.R1:
			x = HALF_LINE;
			y = LANE1_Y;
			break;
		case LANE_ID.R2:
			x = HALF_LINE;
			y = LANE2_Y;
			break;
		case LANE_ID.R3:
			x = HALF_LINE;
			y = LANE3_Y;
			break;
		case LANE_ID.R4:
			x = HALF_LINE;
			y = LANE4_Y;
			break;
		case LANE_ID.NONE:
			break;
		}
	}

	function checkOver(cnt)
	{
		if (x+w>=GAME_AREA.RIGHT || x<=GAME_AREA.LEFT)
		{
			print ("Remove:"+cnt);
			this.isExist = false;
		}
	}

	function onUpdate(cnt)
	{
		if (isExist){
			checkOver(cnt);
			x += v;
		}
	}

	function onDraw()
	{
		if (isExist){
			DrawBox(x, y, x + w, y + h, GetColor(255,255,0), TRUE);
		}
	}
};