/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef XRF_ANALYZED_COUNTS_H_H
#define XRF_ANALYZED_COUNTS_H

/*---------------------------------------------------------------------------*/

#include <QString>
#include <unordered_map>
#include <valarray>

/*---------------------------------------------------------------------------*/

class XrfAnalyzedCounts
{
public:

    XrfAnalyzedCounts(std::string name, int width, int height);

    ~XrfAnalyzedCounts();

    const int& width() { return _width; }

    const int& height() { return _height; }

    void add_element(std::string name);

    float* get_counts_ptr(std::string name);

    float get_counts_max(std::string name);

    float get_counts_min(std::string name);

protected:

    std::string _analysis_name;
    std::unordered_map<std::string, std::valarray<float>> _counts;
    int _width;
    int _height;
};


/*---------------------------------------------------------------------------*/

#endif /* XRF_ANALYZED_COUNTS_H */

/*---------------------------------------------------------------------------*/
