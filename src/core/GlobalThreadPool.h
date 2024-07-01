/*-----------------------------------------------------------------------------
 * Copyright (c) 2021, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef Global_Thread_Pool_H
#define Global_Thread_Pool_H

//---------------------------------------------------------------------------
#include "core/defines.h"
#include "workflow/threadpool.h"

class DLL_EXPORT Global_Thread_Pool : public ThreadPool
{
public:

    static Global_Thread_Pool* inst();

    ~Global_Thread_Pool();

private:

    Global_Thread_Pool();

    static Global_Thread_Pool* _this_inst;

    static std::mutex _mutex;

};

#endif

//---------------------------------------------------------------------------
