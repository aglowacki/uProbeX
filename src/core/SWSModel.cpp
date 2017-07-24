/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <core/SWSModel.h>
#include <QStringList>
#include <QDir>
#include <QFileInfo>
#include <math.h>
#include <QDebug>
#include <LinearTransformer.h>
#include <QMessageBox>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPainter>
#include <QImageReader>
#include <QApplication>

using gstar::LinearTransformer;

const char NUM_SAMPLES_STR[] = {"Mosaic/Count"};
const char XYZ_FILE_HEADER[] = {"    No,       X,           Y,           Z,  \r\n"};

/*---------------------------------------------------------------------------*/

SWSModel::SWSModel(QString filepath)
{
   m_tiffModel = NULL;
   m_samples = NULL;
   m_model = NULL;
   m_numSamples = 0;
   m_numXSamples = 0;
   m_numYSamples = 0;
   m_tiffLoaded = false;

   try
   {

      QFileInfo info1(filepath);
      m_pathFile = filepath;

      m_datasetName = info1.completeBaseName();
      m_datasetPath = info1.path() + QDir::separator() + m_datasetName;
      qDebug()<<m_datasetPath;
      qDebug()<<m_datasetName;

      qDebug()<<info1.suffix();
      qDebug()<<info1.bundleName();
      qDebug()<<info1.completeBaseName();

      QSettings swsData(filepath, QSettings::IniFormat);

      //save all settings to QMap
      QStringList sl = swsData.allKeys();
      foreach(QString s, sl)
      {
         m_mapValueKey.insert(s, swsData.value(s));
      }

      m_numSamples = swsData.value(NUM_SAMPLES_STR).toInt();

      //Load directory files
      loadDirectory();

   }
   catch (std::string& s)
   {
      throw s;
   }
   catch (...)
   {
      throw std::string("Failed to open SWS workspace!");
   }

}

/*---------------------------------------------------------------------------*/

SWSModel::~SWSModel()
{

   if(m_samples != NULL)
   {
      delete [] m_samples;
      m_samples = NULL;
   }
   if(m_model != NULL)
   {
      delete m_model;
      m_model = NULL;
   }

   /*
   if(m_tiffModel != NULL)
   {
      delete m_tiffModel;
      m_tiffModel = NULL;
   }
   */
}

/*---------------------------------------------------------------------------*/

gstar::CoordinateModel* SWSModel::getCoordModel()
{

   return m_model;

}

/*---------------------------------------------------------------------------*/

int SWSModel::getPixelByteSize()
{

   if(m_tiffModel != NULL)
      return m_tiffModel->getPixelByteSize();
   return 1;

}

/*---------------------------------------------------------------------------*/

QString SWSModel::getDataPath()
{

   return m_pathFile;

}

/*---------------------------------------------------------------------------*/

int SWSModel::getImageDims(int imageIndex)
{

   if(m_tiffModel != NULL)
      return m_tiffModel->getImageDims(imageIndex);
   return 0;

}

/*---------------------------------------------------------------------------*/
/*
Selection SWSModel::getImageSelection()
{

   Selection s;
   if(m_tiffModel != NULL)
      return m_tiffModel->getImageSelection();
   return s;

}
*/
/*---------------------------------------------------------------------------*/

int SWSModel::getNumberOfImages()
{

   if(m_tiffModel != NULL)
      return m_tiffModel->getNumberOfImages();
   return 0;

}

/*---------------------------------------------------------------------------*/

int SWSModel::getRank()
{

   if(m_tiffModel != NULL)
      return m_tiffModel->getRank();
   return -1;

}

/*---------------------------------------------------------------------------*/

boost::shared_array<char> SWSModel::getReorderedImageData(int imageNum,
   int imageIndex,
   int heightIndex,
   int widthIndex)
{

   boost::shared_array<char> s;
   if(m_tiffModel != NULL)
      return m_tiffModel->getReorderedImageData(imageNum,
                                                imageIndex,
                                                heightIndex,
                                                widthIndex);
   return s;

}

/*---------------------------------------------------------------------------*/

void SWSModel::initializeCoordModel()
{

   vec2 topLeft;
   vec2 topRight;
   vec2 bottomLeft;

   double xScale = 1.0;
   double yScale = 1.0;
   double imgWidth;
   double imgHeight;

   int topLeftIdx = 0;
   int topRightIdx;
   int bottomLeftIdx;

   if(m_numSamples == 1)
   {
       if(m_tiffModel != NULL)
       {
          imgWidth = m_tiffModel->getImageDims(2);
          imgHeight = m_tiffModel->getImageDims(1);

          xScale = (m_samples[0].x * 2.0) / imgWidth;
          yScale = (m_samples[0].y * 2.0) / imgHeight;


          LinearTransformer* lt = new LinearTransformer();
          lt->setTopLeft(0, 0);
          lt->setScale(xScale, yScale, 1.0);
          lt->setDivider(1000.0, 1000.0, 1.0);
          m_model = new gstar::CoordinateModel(lt);
       }
   }
   if(m_numSamples == 2)
   {
       if(m_tiffModel != NULL)
       {
          imgWidth = m_tiffModel->getImageDims(2);
          imgHeight = m_tiffModel->getImageDims(1);

          xScale = abs(m_samples[1].location.x - m_samples[0].location.x) / imgWidth;
          yScale = abs(m_samples[1].location.y - m_samples[0].location.y) / imgHeight;


          LinearTransformer* lt = new LinearTransformer();
          lt->setTopLeft(topLeft.x, topLeft.y);
          lt->setScale(xScale, yScale, 1.0);
          lt->setDivider(1000.0, 1000.0, 1.0);
          m_model = new gstar::CoordinateModel(lt);
       }
   }
   if(m_numSamples > 1)
   {
      double xStride0 = m_samples[1].location.x - m_samples[0].location.x;
      double yStride0 = m_samples[1].location.y - m_samples[0].location.y;
      double xMax = xStride0 * 0.5;
      double yMax = yStride0 * 0.5;

      bool found = false;
      //find number of samples along X and Y axis
      for(unsigned int i=1; i<m_numSamples; i++)
      {
         double xStride1 = m_samples[i].location.x - m_samples[i-1].location.x;
         double yStride1 = m_samples[i].location.y - m_samples[i-1].location.y;

         double diffX = abs(xStride0 - xStride1);
         double diffY = abs(yStride0 - yStride1);
         //Find when X or Y stops incrementing
         if ( diffX > xMax &&
              diffY > yMax)
         {
            if(yStride1 == 0 && xStride1 > 0)
            {
                // case 2
                m_numYSamples = i;
                m_numXSamples = m_numSamples / m_numYSamples;
                bottomLeftIdx = m_numYSamples - 1;
                if(m_numXSamples % 2) // odd
                {
                   topRightIdx = m_numSamples - m_numYSamples;
                }
                else // even
                {
                   topRightIdx = m_numSamples - 1;
                }
                xStride0 = m_samples[bottomLeftIdx+1].x - m_samples[bottomLeftIdx].x;
                yStride0 = m_samples[topLeftIdx+1].y - m_samples[topLeftIdx].y;
            }
            else
            {
                // case 1
                m_numXSamples = i;
                m_numYSamples = m_numSamples / m_numXSamples;
                topRightIdx = m_numXSamples-1;
                //if num Y samples is even then bottom left index is numSamples - 1
                //if num Y samples is odd then bottom left index is numSamples - numXSamples
                if(m_numYSamples % 2) // if odd
                {
                   bottomLeftIdx = m_numSamples - m_numXSamples;
                }
                else // if even
                {
                   bottomLeftIdx = m_numSamples - 1;
                }
                xStride0 = m_samples[topLeftIdx+1].x - m_samples[topLeftIdx].x;
                yStride0 = m_samples[topRightIdx+1].y - m_samples[topRightIdx].y;
            }
            found = true;
            break;
         }
      }

      if (!found)
      {
         throw std::string("Error: Could not find bounds for SWS workspace!");
      }

      /*
       * Sample save order
       * case 1
       * 1  2  3  4         1  2  3  4       1  2  3        1  2  3
       * 8  7  6  5         8  7  6  5       6  5  4        6  5  4
       * 9  10 11 12        9  10 11 12      7  8  9        7  8  9
       * 16 15 14 13                         12 11 10
       *
       * or case 2
       *
       * 1 8 9  16          1  8  9          1  6  7  12     1  6  7
       * 2 7 10 15          2  7  10         2  5  8  11     2  5  8
       * 3 6 11 14          3  6  11         3  4  9  10     3  4  9
       * 4 5 12 13          4  5  12
       */

      xStride0 = abs(xStride0);
      yStride0 = abs(yStride0);
      xStride0 *= 0.5;
      yStride0 *= 0.5;

      //some images are at an angle where the top right
      if(m_samples[topLeftIdx].y < m_samples[topRightIdx].y)
      {
         topRight.x = m_samples[topRightIdx].x + xStride0;
         topRight.y = m_samples[topRightIdx].y - yStride0;

         topLeft.x = m_samples[topLeftIdx].x - xStride0;
         topLeft.y = topRight.y;
      }
      else if(m_samples[topLeftIdx].y > m_samples[topRightIdx].y)
      {
         topLeft.x = m_samples[topLeftIdx].x - xStride0;
         topLeft.y = m_samples[topLeftIdx].y - yStride0;

         topRight.x = m_samples[topRightIdx].x + xStride0;
         topRight.y = topLeft.y;
      }
      else
      {
         topLeft.x = m_samples[topLeftIdx].x - xStride0;
         topLeft.y = m_samples[topLeftIdx].y - yStride0;

         topRight.x = m_samples[topRightIdx].x + xStride0;
         topRight.y = m_samples[topRightIdx].y - yStride0;
      }

      bottomLeft.x = m_samples[bottomLeftIdx].x - xStride0;
      bottomLeft.y = m_samples[bottomLeftIdx].y + yStride0;

      if(m_tiffModel != NULL)
      {
         imgWidth = m_tiffModel->getImageDims(2);
         imgHeight = m_tiffModel->getImageDims(1);

         xScale = (topRight.x - topLeft.x) / imgWidth;
         yScale = (bottomLeft.y - topLeft.y) / imgHeight;


         LinearTransformer* lt = new LinearTransformer();
         lt->setTopLeft(topLeft.x, topLeft.y);
         lt->setScale(xScale, yScale, 1.0);
         lt->setDivider(1000.0, 1000.0, 1.0);
         m_model = new gstar::CoordinateModel(lt);

      }

   }
}

/*---------------------------------------------------------------------------*/

bool SWSModel::loadDirectory()
{

   bool loaded = true;

   if(QDir(m_datasetPath).exists())
   {
      //Search for Tiles.dat, XYZPositions.txt and dsetname.pmg
      if( loadXYZ() && loadPMG() )
      {
         if(false == loadTiff())
         {
            if(loadTiles())
               m_tiffLoaded = true;
            else
            {
               m_tiffLoaded = false;
               loaded = false;
               QMessageBox::critical(0, "uProbeX",
                        "Failed to open associated TIFF file.");

            }
         }
         if(loaded)
         {
            initializeCoordModel();
         }
      }

   }
   else
   {
      loaded = false;
   }

   return loaded;

}

/*---------------------------------------------------------------------------*/

bool SWSModel::readPMGInt(QFile *pmgFile, QString ID, int& data)
{

   QString line = pmgFile->readLine();
   QStringList fields = line.split(' ');

   for(int i=0;i<fields.length()-1; i++)
   {
      if(fields[i] == ID)
      {
         data = fields[i+1].toInt();
         return true;
      }
   }

   qDebug()<<"Failed to load PMG file header from dataset. Missing '"+ID+"'";
   return false;
}

/*---------------------------------------------------------------------------*/

bool SWSModel::readPMGDoubleArray(QFile *pmgFile,
                                  QString ID,
                                  double* data,
                                  int arrSize)
{

   QString line = pmgFile->readLine();
   QStringList fields = line.split(' ');
   fields.removeAll("");

   for(int i=0;i<fields.length()-1; i++)
   {
      if(fields[i] == ID)
      {
         if (arrSize <= fields.length() - i)
         {
            for(int j=0; j<arrSize; j++)
            {
               data[j] = fields[i+(j+1)].toDouble();
            }
            return true;
         }
         else
         {
            qDebug()<<"Failed to load PMG file header from dataset. Not enough parameters '"+ID+"'";
            return false;
         }
      }
   }

   qDebug()<<"Failed to load PMG file header from dataset. Missing '"+ID+"'";
   return false;

}

/*---------------------------------------------------------------------------*/

bool SWSModel::readPMGString(QFile *pmgFile, QString ID, QString& data)
{

   QString line = pmgFile->readLine();
   QStringList fields = line.split(' ');

   for(int i=0;i<fields.length()-1; i++)
   {
      if(fields[i] == ID)
      {
         data = fields[i+1];
         data.remove("\r");
         data.remove("\n");
         return true;
      }
   }

   if(fields.length() == 1)
   {
      fields[0].remove('\r');
      fields[0].remove('\n');
      if(fields[0] == ID)
      {
         return true;
      }
   }
   qDebug()<<"Failed to load PMG file header from dataset. Missing '"+ID+"'";
   return false;

}

/*---------------------------------------------------------------------------*/

bool SWSModel::loadPMG()
{

   bool loaded = true;

   int tmpNumSamples = 0;
   double tmpDoubles[2] = {0.0, 0.0};
   QString tmpStr = "";
   QFile pmgFile(m_datasetPath+QDir::separator()+m_datasetName+".pmg");

   if(pmgFile.exists())
   {
     if (pmgFile.open(QIODevice::ReadOnly))
     {
        if( !readPMGInt(&pmgFile, "VISPIECES", tmpNumSamples ) )
        {
           return false;
        }
        if ((unsigned int) tmpNumSamples != m_numSamples)
        {
           qDebug()<<"Warning: Number of samples does not match PMG file and XYZ dataset";
        }

        //read scale
        if( !readPMGDoubleArray(&pmgFile, "VISSCALE", tmpDoubles, 1) )
        {
            return false;
        }
        m_pmgScale = tmpDoubles[0];

        //read image reduction
        if( !readPMGDoubleArray(&pmgFile, "IMAGEREDUCTION", tmpDoubles, 1) )
        {
            return false;
        }
        m_pmgImageReduction = tmpDoubles[0];

        //Read the sample peices
        for(unsigned int i=0; i < m_numSamples; i++)
        {
           //read 'PIECE
           readPMGString(&pmgFile, "PIECE", tmpStr);

           //read 'PATH
           if( readPMGString(&pmgFile, "PATH", tmpStr) )
           {
              tmpStr = tmpStr.remove('<');
              tmpStr = tmpStr.remove('>');
              m_samples[i].path = tmpStr;
           }

           //read 'SCALE'
           if( !readPMGDoubleArray(&pmgFile, "SCALE", tmpDoubles, 1) )
           {
               return false;
           }
           m_samples[i].scale = tmpDoubles[0];

           //read 'LOCATION'
           if( !readPMGDoubleArray(&pmgFile, "LOCATION", tmpDoubles, 2) )
           {
               return false;
           }
           m_samples[i].location.x = tmpDoubles[0];
           m_samples[i].location.y = tmpDoubles[1];

           //read Top Left 'CORNER'
           if( !readPMGDoubleArray(&pmgFile, "CORNER", tmpDoubles, 2) )
           {
               return false;
           }
           m_samples[i].cornerTopLeft.x = tmpDoubles[0];
           m_samples[i].cornerTopLeft.y = tmpDoubles[1];

           //read Top Right 'CORNER'
           if( !readPMGDoubleArray(&pmgFile, "CORNER", tmpDoubles, 2) )
           {
               return false;
           }
           m_samples[i].cornerTopRight.x = tmpDoubles[0];
           m_samples[i].cornerTopRight.y = tmpDoubles[1];

           //read Bottom Right 'CORNER'
           if( !readPMGDoubleArray(&pmgFile, "CORNER", tmpDoubles, 2) )
           {
               return false;
           }
           m_samples[i].cornerBottomRight.x = tmpDoubles[0];
           m_samples[i].cornerBottomRight.y = tmpDoubles[1];

           //read Bottom Left 'CORNER'
           if( !readPMGDoubleArray(&pmgFile, "CORNER", tmpDoubles, 2) )
           {
               return false;
           }
           m_samples[i].cornerBottomLeft.x = tmpDoubles[0];
           m_samples[i].cornerBottomLeft.y = tmpDoubles[1];

           //read 'ENDPIECE'
           readPMGString(&pmgFile, "ENDPIECE", tmpStr);
        }
        pmgFile.close();
     }
     else
     {
        loaded = false;
        QMessageBox::warning(0, "Error", "Could not read file"+m_datasetPath+QDir::separator()+m_datasetName+".pmg");
     }
   }
   else
   {
      loaded = false;
      QMessageBox::warning(0, "Error", "Could not find file"+m_datasetPath+QDir::separator()+m_datasetName+".pmg");
   }


   return loaded;

}

/*---------------------------------------------------------------------------*/

bool SWSModel::tiffLoaded()
{

   return m_tiffLoaded;

}

/*---------------------------------------------------------------------------*/

bool SWSModel::loadTiff()
{

   try
   {
      QFile file1(m_datasetPath+".tiff");
      QFile file2(m_datasetPath+".tif");

      if(file1.exists() == true)
      {
         m_tiffModel = new TIFFModel(m_datasetPath+".tiff", "");
         m_tiffLoaded = true;
      }
      else if(file2.exists() == true)
      {
         m_tiffModel = new TIFFModel(m_datasetPath+".tif", "");
         m_tiffLoaded = true;
      }
      else
      {
         m_tiffLoaded = false;
        //throw std::string("Failed to open find TIFF file for SWS workspace!");
      }
   }
   catch (...)
   {
      m_tiffLoaded = false;
      //throw std::string("Failed to open TIFF from SWS workspace!");
   }

   return m_tiffLoaded;
}

/*---------------------------------------------------------------------------*/

bool SWSModel::loadTiles()
{

   QApplication::setOverrideCursor(Qt::WaitCursor);

   bool loaded = false;

   QGraphicsScene* gscene = new QGraphicsScene();

   qDebug()<<"Supported image formats "<<QImageReader::supportedImageFormats();

   for(unsigned int i=0; i<m_numSamples; i++)
   {
      //m_samples[i].m_tiffModel = NULL;
      QString fileName = m_datasetPath+QDir::separator()+m_samples[i].path;
      QFileInfo tiffFileInfo(fileName);
      if(false == tiffFileInfo.exists())
      {
         //Sometimes the PMG file says the path is a bmp file
         // instead of tiff
         fileName = tiffFileInfo.path()+QDir::separator()+tiffFileInfo.baseName()+".tif";
         QFileInfo tiffFile(fileName);
         if(false == tiffFile.exists())
         {
            continue;
         }
      }


      //m_samples[i].m_tiffModel = new TIFFModel(fileName);

      QImage *tiffImage = new QImage();
      if (tiffImage->load(fileName, "tif"))
      {
         //QGraphicsPixmapItem* pixItem = gscene->addPixmap(QPixmap::fromImage(tiffImage->convertToFormat(QImage::Format_ARGB32)));
         QGraphicsPixmapItem* pixItem = gscene->addPixmap(QPixmap::fromImage(*tiffImage));
         //pixItem->scale(m_pmgScale, m_pmgScale);
         //pixItem->scale(m_samples[i].scale, m_samples[i].scale);
         double sx, sy;

         sx = m_samples[i].cornerTopLeft.x / m_samples[i].scale;
         sx /= m_pmgImageReduction;
         sy = m_samples[i].cornerTopLeft.y / m_samples[i].scale;
         sy /= m_pmgImageReduction;
         QTransform trans;
         trans.translate(sx, sy);
         pixItem->setTransform(trans);
         //pixItem->translate(sx, sy);
      }
      else
      {
         qDebug()<<"Failed to load image "<<fileName;
      }
   }

   gscene->clearSelection();
   gscene->setSceneRect(gscene->itemsBoundingRect());
   QImage image(gscene->sceneRect().size().toSize(), QImage::Format_RGB888);  // Create the image with the exact size of the shrunk scene
   //image.fill(Qt::transparent);

   QPainter painter(&image);
   gscene->render(&painter);
   if(false == image.save(m_datasetPath+".tif", "tif"))
   {
      qDebug()<<"Failed to save mosaic image "<<m_datasetPath<<".tif";
   }

   QApplication::restoreOverrideCursor();

   return loadTiff();

}

/*---------------------------------------------------------------------------*/

bool SWSModel::loadXYZ()
{

   bool loaded = true;

   QFile xyzFile(m_datasetPath+QDir::separator()+"XYZPositions.txt");

   if(xyzFile.exists())
   {
     if (xyzFile.open(QIODevice::ReadOnly))
     {
         m_samples = new SubSample[m_numSamples];

         QString line = xyzFile.readLine();
         if(line != XYZ_FILE_HEADER)
         {
            qDebug()<<"Warning: XYZPositions.txt header is not correct.";
         }
         for(unsigned int i=0; i < m_numSamples; i++)
         {
            line = xyzFile.readLine();
            QStringList fields = line.split(',');
            //need 4 values
            if(fields.length() > 3)
            {
               m_samples[i].idx = fields[0].toInt();
               m_samples[i].x = fields[1].toDouble();
               m_samples[i].y = fields[2].toDouble();
               m_samples[i].z = fields[3].toDouble();
            }
            else
            {
               delete m_samples;
               loaded = false;
               break;
            }
         }
     }
     else
        loaded = false;
   }
   else
      loaded = false;

   return loaded;

}

/*---------------------------------------------------------------------------*/
