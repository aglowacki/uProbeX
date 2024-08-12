/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef BLUESKY_PLAN_H
#define BLUESKY_PLAN_H

//---------------------------------------------------------------------------

#include <unordered_map>

//---------------------------------------------------------------------------
struct BlueskyParam
{
    BlueskyParam()
    {
        default_val = "0.0";
    }
    QString name;
    QString description;
    QString default_val;
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
