// スレッド用の名前空間
Thread <- {
    Thread_Object = null,
};

// スレッドを作成する
function Thread::createThread(func)
{
	// 中断状態であればスレッドを終了
	stopThread();

	// オブジェクトの作成
	Thread_Object = newthread(func);
}

// スレッドを終了する
function Thread::stopThread()
{
	if(Thread_Object != null)
	{
		if(Thread_Object.getstatus() != "idle")
		{
			Thread_Object.wakeup(true);
		}
	}
}

// スレッドを実行/再開する関数
function Thread::executeThread()
{
    // スレッドオブジェクトが作成されており、
    if(Thread_Object != null)
    {
        // 中断状態なら、再開する
        if( Thread_Object.getstatus() == "suspended" )
        {
            return Thread_Object.wakeup(false);
        }
            // アイドル状態なら、実行する
        else if( Thread_Object.getstatus() == "idle" )
        {
            return Thread_Object.call();
        }
    }
}

// 引数に指定した回数、中断処理を行う関数
function Thread::wait(frame)
{
    for(local i=0; i<frame; i++)
    {
        if( suspend(false) ) return true;
    }
    return false;
}

// キーが押されるまで中断処理を行う関数
function Thread::waitPushKey()
{
    do {
        if( suspend(false) ) return true;
    } while( !GetJoypadInputStatePushed(INPUT_TYPE.ONE) );

    return false;
}
