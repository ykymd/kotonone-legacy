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


//