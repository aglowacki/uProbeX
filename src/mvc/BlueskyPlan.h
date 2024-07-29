/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef BLUESKY_PLAN_H
#define BLUESKY_PLAN_H

//---------------------------------------------------------------------------

#include <unordered_map>

//---------------------------------------------------------------------------
struct BlueSkyParam
{
    BlueSkyParam()
    {
        default_val = "0.0";
    }
    QString name;
    QString description;
    QString default_val;
};

struct BlueskyPlan
{
    BlueskyPlan(){}
    QString name;
    QString description;
    QString module;
    std::unordered_map<QString, BlueSkyParam> parameters;
};


//---------------------------------------------------------------------------

#endif /*  */

//---------------------------------------------------------------------------
