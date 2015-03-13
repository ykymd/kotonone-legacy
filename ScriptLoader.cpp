#include "ScriptLoader.h"
#include <vector>
#include "tstring.h"
#include <boost/algorithm/string.hpp>


ScriptLoader::ScriptLoader()
{
}


ScriptLoader::~ScriptLoader()
{
}

// Squirrel側の関数を取得する
Sqrat::Function ScriptLoader::getFunction(const SQChar* callback_func)
{
	// "Reload.seriaize" のように、テーブル内の関数を指す文字列が渡された場合に
	// ドット区切りで分割して、wordsに格納していく
	std::vector<tstring> words;
	boost::split(words, callback_func, boost::is_any_of("."));

	// 最初はルートテーブルから
	Sqrat::Table tbl = Sqrat::RootTable();

	// 最後のテーブル(vecの末尾よりひとつ手前)を取得する
	for (int i = 1, n = words.size(); i<n; i++)
	{
		tbl = tbl.GetSlot(words[i - 1].c_str());
	}

	// 関数(vecの末尾)を取得する
	return Sqrat::Function(tbl, words[words.size() - 1].c_str());
}
