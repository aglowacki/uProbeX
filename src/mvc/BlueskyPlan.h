/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef BLUESKY_PLAN_H
#define BLUESKY_PLAN_H

//---------------------------------------------------------------------------

#include <unordered_map>

enum class BlueskyParamType { Int = 1, Double = 2, String = 3 };

//---------------------------------------------------------------------------

struct BlueskyResult
{
    BlueskyResult()
    {
        time_start = 0.0;
        time_stop = 0.0;
        msg = "";
    }
    QString exit_status;
    QString run_uids;
    // 'scan_ids': [2],
    double time_start;
    double time_stop;
    QString msg;
    QString traceback;
};

//---------------------------------------------------------------------------
struct BlueskyParam
{
    BlueskyParam()
    {
        default_val = "";
    }
    QString name;
    QString description;
    QString default_val;
    BlueskyParamType kind;
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

struct BlueskyPlan
{
    BlueskyPlan()
    {
        uuid="";
        user="";
    }
    QString name;
    QString type;
    QString description;
    QString module;
    std::vector<BlueskyParam> parameters;
    BlueskyResult result;
    QString uuid;
    QString user;
};


//---------------------------------------------------------------------------

#endif /*  */

//---------------------------------------------------------------------------
