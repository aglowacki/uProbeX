/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <core/TIFFModel.h>

#include <string>
#include <queue>
#include <vector>

using gstar::Array;

using std::vector;
using std::pair;
using std::string;

/*---------------------------------------------------------------------------*/

TIFFModel::TIFFModel(QString filepath, std::string datapath)
{

    Q_UNUSED(datapath);

    m_pathtiffFile = filepath;

    try
    {

        QImageReader img_reader(filepath);
        _img = img_reader.read();

    }
    catch (std::string& s)
    {
        throw s;
    }
    catch (...)
    {
        throw std::string("Failed to open TIFF file!");
    }

}

/*---------------------------------------------------------------------------*/

TIFFModel::~TIFFModel()
{

}

/*---------------------------------------------------------------------------*/

int TIFFModel::getPixelByteSize()
{

    return _img.bitPlaneCount();

}

/*---------------------------------------------------------------------------*/

QString TIFFModel::getDataPath()
{

    return m_pathtiffFile;

}

/*---------------------------------------------------------------------------*/

int TIFFModel::getImageDims(int imageIndex)
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

}

/*---------------------------------------------------------------------------*/

int TIFFModel::getNumberOfImages()
{

    return 1;

}

/*---------------------------------------------------------------------------*/

int TIFFModel::getRank()
{

    return 2;

}

/*---------------------------------------------------------------------------*/

