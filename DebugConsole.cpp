#include "DebugConsole.h"

#include <Windows.h>
#include <iostream>
#include "DxLib.h"

using namespace Kotone;

void DebugConsole::setConsole()
{
	// コンソールを割り当てる
	console_enable = AllocConsole();

	if (console_enable)
	{
		// 標準出力の出力先をコンソールに設定する
		freopen("CONOUT$", "w", stdout);

		// コンソールのウインドウハンドルを取得する
		HWND console_handle = GetConsoleWindow();

		// DXライブラリのウインドウハンドルを取得する(DXライブラリAPI)
		HWND dxlib_handle = DxLib::GetMainWindowHandle();

		// コンソール・DXライブラリそれぞれのウインドウハンドルの取得に成功したら
		if (console_handle != NULL && dxlib_handle != NULL)
		{
			// DXライブラリのウインドウ位置・サイズを格納する変数
			RECT tmp_rect;

			// DXライブラリのウインドウ位置を取得する
			GetWindowRect(dxlib_handle, &tmp_rect);

			// DXライブラリのウインドウの幅と高さを算出する
			int dxlib_width = tmp_rect.right - tmp_rect.left;
			int dxlib_height = tmp_rect.bottom - tmp_rect.top;

			// DXライブラリのウインドウ位置を左にずらす
			SetWindowPos(
				dxlib_handle, NULL,
				tmp_rect.left - dxlib_width / 2, tmp_rect.top,
				dxlib_width, dxlib_height, 0);

			// コンソールウインドウを、DXライブラリのウインドウの右側にずらす
			SetWindowPos(
				console_handle, NULL,
				tmp_rect.left + dxlib_width / 2, tmp_rect.top,
				dxlib_width, dxlib_height, 0);

			// DXライブラリのウインドウを前面に表示してアクティブにする
			SetForegroundWindow(dxlib_handle);
		}
	}
}

void DebugConsole::Finalize()
{
	// コンソールの割り当てに成功していれば
	if (console_enable)
	{
		// 割り当てられているコンソールを解放する
		FreeConsole();
	}
}