/*-----------------------------------------------------------------------------
 * Copyright (c) 2021, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/TIFF_Model.h>

#include <string>
#include <queue>
#include <vector>

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

