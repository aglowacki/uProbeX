/*-----------------------------------------------------------------------------
 * Copyright (c) 2021, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------------------------
#include "core/GlobalThreadPool.h"

std::mutex Global_Thread_Pool::_mutex;

Global_Thread_Pool* Global_Thread_Pool::_this_inst(nullptr);
//---------------------------------------------------------------------------

Global_Thread_Pool::Global_Thread_Pool() : ThreadPool(std::thread::hardware_concurrency() - 1)
{

}

//-----------------------------------------------------------------------------

Global_Thread_Pool* Global_Thread_Pool::inst()
{
    std::lock_guard<std::mutex> lock(_mutex);

    if (_this_inst == nullptr)
    {
        _this_inst = new Global_Thread_Pool();
    }
    return _this_inst;
}



//---------------------------------------------------------------------------
