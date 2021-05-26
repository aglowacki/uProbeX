/*-----------------------------------------------------------------------------
 * Copyright (c) 2021, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/TIFF_Model.h>

#include <string>
#include <queue>
#include <vector>

#include <gstar/LinearTransformer.h>
using gstar::LinearTransformer;

using std::vector;
using std::pair;
using std::string;

/*---------------------------------------------------------------------------*/

TIFF_Model::TIFF_Model() : VLM_Model()
{
    _loaded = false;
}

/*---------------------------------------------------------------------------*/

TIFF_Model::~TIFF_Model()
{

}

/*---------------------------------------------------------------------------*/

bool TIFF_Model::load(QString filepath)
{
    m_pathtiffFile = filepath;
    _loaded = false;
    try
    {
        QFileInfo info1(filepath);
        _pathFile = filepath;
        _datasetPath = info1.path() + QDir::separator() + info1.completeBaseName();
        QImageReader img_reader(filepath);
        _img = img_reader.read();
        //load xml markers and regions
        _load_xml_markers_and_regions();
        _loaded = true;

    }
    catch (std::string& s)
    {
        return false;
    }
    catch (...)
    {
        return false;
    }

    return true;
}

/*---------------------------------------------------------------------------*/

bool TIFF_Model::loaded()
{
    
    return _loaded;

}

/*---------------------------------------------------------------------------*/

int TIFF_Model::getPixelByteSize()
{

    return _img.bitPlaneCount();

}

/*---------------------------------------------------------------------------*/

void TIFF_Model::_initializeCoordModel()
{
    double xScale = 1.0;
    double yScale = 1.0;
    double imgWidth;
    double imgHeight;

    int topLeftIdx = 0;
    int topRightIdx;
    int bottomLeftIdx;

    imgWidth = _img.width();
    imgHeight = _img.height();

    LinearTransformer* lt = new LinearTransformer();
    lt->setTopLeft(0, 0);
    lt->setScale(xScale, yScale, 1.0);
    lt->setDivider(1000.0, 1000.0, 1.0);
    _coord_model = new gstar::CoordinateModel(lt);

}

/*---------------------------------------------------------------------------*/

QString TIFF_Model::getDataPath()
{

    return m_pathtiffFile;

}

/*---------------------------------------------------------------------------*/

int TIFF_Model::getImageDims(int imageIndex)
{

    switch (imageIndex)
    {
    case 0:
        return 1;
    case 1:
        return _img.height();
    case 2:
        return _img.width();
    }
return 0;
}

/*---------------------------------------------------------------------------*/

int TIFF_Model::getNumberOfImages()
{

    return 1;

}

/*---------------------------------------------------------------------------*/

int TIFF_Model::getRank()
{

    return 2;

}

/*---------------------------------------------------------------------------*/

