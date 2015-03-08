// 速度と判定フレームから出現フレームを算出する
function appearTime(frame, v)
{
    //出現から判定ラインまでのフレーム数
    return frame - (GAME_AREA_WIDTH/2) / v;
}

tmp_time <- appearTime(300, 1.0);
print ("time="+tmp_time);

// 楽譜データを格納する配列
g_ScoreTable = {
    [30] = [
        { lane=LANE_ID.L4, speed=1.0, frame=0, },
    ],
}