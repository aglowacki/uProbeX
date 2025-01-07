/*-----------------------------------------------------------------------------
 * Copyright (c) 2021, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/TIFF_Model.h>

#include <string>
#include <queue>
#include <vector>

#include <gstar/LinearTransformer.h>

#ifdef _BUILD_WITH_OPENCV
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#endif

using gstar::LinearTransformer;

using std::vector;
using std::pair;
using std::string;

//---------------------------------------------------------------------------

TIFF_Model::TIFF_Model() : VLM_Model()
{
    _loaded = false;
    //_tif_ptr = nullptr;
}

//---------------------------------------------------------------------------

TIFF_Model::~TIFF_Model()
{

}

//---------------------------------------------------------------------------

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
        _initializeCoordModel();
        if (_img.width() > 0 && _img.height() > 0)
        {
            _loaded = true;
        }
        else
        {
            #ifdef _BUILD_WITH_OPENCV
            cv::Mat mat = cv::imread(filepath.toStdString(), cv::IMREAD_GRAYSCALE | cv::IMREAD_ANYDEPTH);
            if (mat.rows > 0 && mat.cols > 0)
            {
                _loaded = true;
                _pixel_values.resize(mat.rows, mat.cols);
                double minVal;
                double maxVal;
                cv::Point minLoc;
                cv::Point maxLoc;

                cv::minMaxLoc(mat, &minVal, &maxVal, &minLoc, &maxLoc);
                double range = maxVal - minVal;

                _img = QImage(mat.cols, mat.rows, QImage::Format_ARGB32);
                float val = 0.f;
                for (int w = 0; w < mat.cols; w++)
                {
                    for (int h = 0; h < mat.rows; h++)
                    {
                        uchar depth = mat.type() & CV_MAT_DEPTH_MASK;
                        switch (depth) {
                        case CV_8U: 
                            val = (float)mat.at<unsigned char>(h, w);
                            break;
                        case CV_8S:
                            val = (float)mat.at<char>(h, w); 
                            break;
                        case CV_16U:
                            val = (float)mat.at<unsigned short>(h, w); 
                            break;
                        case CV_16S:
                            val = (float)mat.at<short>(h, w); 
                            break;
                        case CV_32S:
                            val = (float)mat.at<int>(h, w); 
                            break;
                        case CV_32F:
                            val = (float)mat.at<float>(h, w); 
                            break;
                        case CV_64F:
                            val = (float)mat.at<double>(h, w); 
                            break;
                        default:
                            val = 0.f; 
                            break;
                        }
                        _pixel_values(h, w) = val;
                        int c = val;
                        if(range > 0)
                        {
                            val = (val - minVal) / range;
                            c = val * 255;
                        }
                        _img.setPixelColor(w, h, QColor(c, c, c, 255));
                    }
                }
            }
            #endif
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

bool TIFF_Model::loaded()
{
    
    return _loaded;

}

//---------------------------------------------------------------------------

int TIFF_Model::getPixelByteSize()
{

    return _img.bitPlaneCount();

}

//---------------------------------------------------------------------------

void TIFF_Model::_initializeCoordModel()
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

QString TIFF_Model::getDataPath()
{

    return m_pathtiffFile;

}

//---------------------------------------------------------------------------

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

//---------------------------------------------------------------------------

int TIFF_Model::getNumberOfImages()
{

    return 1;

}

//---------------------------------------------------------------------------

int TIFF_Model::getRank()
{

    return 2;

}

//---------------------------------------------------------------------------

bool TIFF_Model::save_img(QString filename)
{
    #ifdef _BUILD_WITH_OPENCV
    QImageWriter writer(filename);
    if (false == writer.write(_img))
    {
        cv::Mat mat(_img.height(), _img.width(), CV_8UC4);
        for (int h = 0; h < mat.rows; ++h)
        {
            for (int w = 0; w < mat.cols; ++w)
            {                
                QColor c = _img.pixelColor(w,h);
                cv::Vec4b& bgra = mat.at<cv::Vec4b>(h, w);
                bgra[0] = c.blue(); // blue
                bgra[1] = c.green(); // Green
                bgra[2] = c.red(); // Red
                bgra[3] = UCHAR_MAX; // Alpha
            }
        }

        try
        {
            return imwrite(filename.toStdString(), mat);
        }
        catch (const cv::Exception& ex)
        {
            logW<<"Exception converting image to PNG format: "<< ex.what()<<"\n";
            return false;
        }
    }
    return true;
    #else
    return false;
    #endif
}

//---------------------------------------------------------------------------