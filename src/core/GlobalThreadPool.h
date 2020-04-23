/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef Global_Thread_Pool_H
#define Global_Thread_Pool_H

/*---------------------------------------------------------------------------*/
#include "workflow/threadpool.h"

static ThreadPool global_threadpool(std::thread::hardware_concurrency()-1);

#endif

/*---------------------------------------------------------------------------*/
