/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef BLUESKY_PLAN_H
#define BLUESKY_PLAN_H

//---------------------------------------------------------------------------

#include <unordered_map>

enum class BlueskyParamType { Numeral = 1, String = 3 };


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
    std::unordered_map<QString, BlueskyParam> parameters;
    QString uuid;
    QString user;
};


//---------------------------------------------------------------------------

#endif /*  */

//---------------------------------------------------------------------------
