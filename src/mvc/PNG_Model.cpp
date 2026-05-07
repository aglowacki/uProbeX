/*-----------------------------------------------------------------------------
 * Copyright (c) 2021, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/PNG_Model.h>

#include <string>
#include <queue>
#include <vector>

#include <gstar/LinearTransformer.h>

using gstar::LinearTransformer;

using std::vector;
using std::pair;
using std::string;

//---------------------------------------------------------------------------

PNG_Model::PNG_Model() : VLM_Model()
{
    _loaded = false;
}

//---------------------------------------------------------------------------

PNG_Model::~PNG_Model()
{

}

//---------------------------------------------------------------------------

bool PNG_Model::load(QString filepath)
{
    m_pathFile = filepath;
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
        _initializeCoordModel();
        if (_img.width() > 0 && _img.height() > 0)
        {
            _loaded = true;
        }
    }
    catch (std::string& s)
    {
        logE << s << "\n";
        return false;
    }
    catch (std::exception e)
    {
        logE << e.what() << "\n";
        return false;
    }

    return _loaded;
}

//---------------------------------------------------------------------------

bool PNG_Model::loaded()
{
    
    return _loaded;

}

//---------------------------------------------------------------------------

int PNG_Model::getPixelByteSize()
{

    return _img.bitPlaneCount();

}

//---------------------------------------------------------------------------

void PNG_Model::_initializeCoordModel()
{
    double xScale = 1.0;
    double yScale = 1.0;

    LinearTransformer* lt = new LinearTransformer();
    lt->setTopLeft(0, 0);
    lt->setScale(xScale, yScale, 1.0);
    lt->setDivider(1.0, 1.0, 1.0);
    _coord_model = new gstar::CoordinateModel(lt);

}

//---------------------------------------------------------------------------

QString PNG_Model::getDataPath()
{

    return m_pathFile;

}

//---------------------------------------------------------------------------

int PNG_Model::getImageDims(int imageIndex)
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

//---------------------------------------------------------------------------

int PNG_Model::getNumberOfImages()
{

    return 1;

}

//---------------------------------------------------------------------------

int PNG_Model::getRank()
{

    return 2;

}

//---------------------------------------------------------------------------

bool PNG_Model::save_img(QString filename)
{
    QFileInfo fileInfo(filename);
    QString baseDirectory = fileInfo.absolutePath(); // Gets the absolute path of the parent directory

    QDir dir(baseDirectory);
    if (!dir.exists()) 
    {
        if (dir.mkpath("."))
        { // Create the directory and any necessary parent directories
            logI << "Directory created successfully:" << baseDirectory.toStdString()<<"\n";
        }
        else 
        {
            logE << "Failed to create directory:" << baseDirectory.toStdString()<<"\n";
        }
    } 
    else 
    {
        logI << "Directory already exists:" << baseDirectory.toStdString()<<"\n";
    }

    QImageWriter writer(filename, "PNG");
    if (false == writer.write(_img)) 
    {
        logI << "Failed to write" << filename.toStdString()<<"\n";
    }
    return true;
    
}

//---------------------------------------------------------------------------