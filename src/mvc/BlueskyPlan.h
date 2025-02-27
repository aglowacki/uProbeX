/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef BLUESKY_PLAN_H
#define BLUESKY_PLAN_H

//---------------------------------------------------------------------------

#include <unordered_map>

enum class BlueskyParamType { Bool = 0, Int = 1, Double = 2, String = 3 };

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

    void setValue(QString val)
    {
        bool found_letter = false;
        bool found_point = false;
        for (int i = 0; i < val.length(); ++i) 
        {
            if (val[i].isLetter()) 
            {
                found_letter = true;
                break;
            }
            if(val[i] == '.')
            {
                found_point = true;
            }
        }
        if(found_letter)
        {
            kind = BlueskyParamType::String;
        }
        else if (found_point)
        {
            kind = BlueskyParamType::Double;
        }
        else
        {
            kind = BlueskyParamType::Int; 
        }
        default_val = val;
    }
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

struct BlueskyPlan
{
    BlueskyPlan()
    {
        uuid="";
        user="";
        filename = "";
    }
    QString name;
    QString type;
    QString filename;
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
