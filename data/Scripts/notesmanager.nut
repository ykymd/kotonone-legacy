// 新しいノーツを生成。ノート管理用テーブルに追加する
function newNote(note_info)
{
    local note = null;
    note = Note(note_info.lane, note_info.speed, note_info.frame);
    note.isExist = true;

    if (note != null){
        g_NotesTable[g_NotesNum++] <- note;
    }
}
