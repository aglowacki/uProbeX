/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/XrfAnalyzedCounts.h>

/*----------------src/mvc/XrfAnalyzedCounts.cpp \-----------------------------------------------------------*/

XrfAnalyzedCounts::XrfAnalyzedCounts(std::string name, int width, int height)
{

    _analysis_name = name;
    _width = width;
    _height = height;

}

/*---------------------------------------------------------------------------*/

XrfAnalyzedCounts::~XrfAnalyzedCounts()
{



}

/*---------------------------------------------------------------------------*/

void XrfAnalyzedCounts::add_element(std::string name)
{
    _counts.insert( {name, std::valarray<float>()} );
    _counts[name].resize( _width * _height );
}

/*---------------------------------------------------------------------------*/

float* XrfAnalyzedCounts::get_counts_ptr(std::string name)
{
    if (_counts.count(name) > 0)
        return &_counts[name][0];
    return nullptr;
}

/*---------------------------------------------------------------------------*/

std::vector<std::string> XrfAnalyzedCounts::get_count_names()
{
    std::vector<std::string> keys;
    for(auto& itr : _counts)
    {
        keys.push_back(itr.first);
    }
    return keys;
}

/*---------------------------------------------------------------------------*/

float XrfAnalyzedCounts::get_counts_max(std::string name)
{
    if (_counts.count(name) > 0)
        return _counts[name].max();
    return 0.0f;
}

/*---------------------------------------------------------------------------*/

float XrfAnalyzedCounts::get_counts_min(std::string name)
{
    if (_counts.count(name) > 0)
        return _counts[name].min();
    return 0.0f;
}

/*---------------------------------------------------------------------------*/
