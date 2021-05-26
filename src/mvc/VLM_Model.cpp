/*-----------------------------------------------------------------------------
 * Copyright (c) 2021, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/VLM_Model.h>
#include <QStringList>
#include <gstar/GStarResource.h>
#include <QMessageBox>

 //#include <QDir>
 //#include <QFileInfo>

//#include <QGraphicsScene>
//#include <QGraphicsPixmapItem>
//#include <QPainter>
//#include <QImageReader>
//#include <QApplication>


//using gstar::LinearTransformer;


/*----------------src/mvc/VLM_Model.cpp \-----------------------------------------------------------*/

VLM_Model::VLM_Model()
{
    _coord_model = nullptr;

}

/*---------------------------------------------------------------------------*/

VLM_Model::~VLM_Model()
{

 
   if(_coord_model != nullptr)
   {
      delete _coord_model;
      _coord_model = nullptr;
   }

}

/*---------------------------------------------------------------------------*/

void VLM_Model::_load_xml_markers_and_regions()
{

	QFile* file = new QFile(_datasetPath + ".xml");

	if (!file->exists())
	{
		return;
	}

	if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QMessageBox::critical(nullptr,	"VLM_Model", "Couldn't open maker xml", QMessageBox::Ok);
		return;
	}

	QXmlStreamReader xml(file);

	while (!xml.atEnd() && !xml.hasError())
	{
		// Read next element
		QXmlStreamReader::TokenType token = xml.readNext();

		// If token is just StartDocument, we'll go to next
		if (token == QXmlStreamReader::StartDocument)
		{
			continue;
		}

		if (token == QXmlStreamReader::StartElement)
		{
			if (xml.name() == "markers")
			{
				continue;
			}
			if (xml.name() == "marker")
			{
				_markersLoaded.prepend(_parseMarker(xml));
				continue;
			}
			if (xml.name() == "regionmarker")
			{
				_regionMarkersLoaded.prepend(_parseRegionMarker(xml));
			}
		}
	}

	// Error handling.
	if (xml.hasError())
	{
		QMessageBox::critical(nullptr,"VLM_Widget::parseXML",xml.errorString(),QMessageBox::Ok);
	}

	xml.clear();

}

/*---------------------------------------------------------------------------*/
/*
void VLM_Model::check_and_load_autosave()
{
	QString originalPath;

	try
	{
		QStringList slist = m_pathFile.split('.');
		if (slist.length() > 0)
		{
			m_datasetPath = slist[0] + ".xml";
			originalPath = m_datasetPath;

			QString autosavedTemporaryFile = m_pathFile + ".tmp";
			QFile autosavedTmpFile(autosavedTemporaryFile);
			if (autosavedTmpFile.exists()) {
				QFileInfo fileInfo(autosavedTmpFile);
				QDateTime lastModified = fileInfo.lastModified();

				QMessageBox msgBox;
				msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
				msgBox.setDefaultButton(QMessageBox::No);
				msgBox.setIcon(QMessageBox::Warning);
				msgBox.setText("It looks like last time this application was used something went wrong, "
					"would you like to restore auto-safe data from " + lastModified.toString());
				int ret = msgBox.exec();

				if (ret == QMessageBox::Yes)
				{
					m_datasetPath = autosavedTemporaryFile;
				}
			}
		}
		else
			throw std::string("Error:: Could not identify file!");
	}
	catch (std::string& s)
	{
		throw s;
	}
	catch (...)
	{
		throw std::string("Failed to open VLM workspace!");
	}


	m_datasetPath = originalPath;

}
*/
/*---------------------------------------------------------------------------*/

QMap<QString, QString> VLM_Model::_parseMarker(QXmlStreamReader& xml)
{

	QMap<QString, QString> marker;

	QXmlStreamAttributes attributes = xml.attributes();

	if (attributes.hasAttribute(gstar::UPROBE_COLOR))
	{
		marker.insert(gstar::UPROBE_COLOR, attributes.value(gstar::UPROBE_COLOR).toString());
	}
	if (attributes.hasAttribute(gstar::UPROBE_LIGHT_POS_X))
	{
		marker.insert(gstar::UPROBE_LIGHT_POS_X,
			attributes.value(gstar::UPROBE_LIGHT_POS_X).toString());
	}
	if (attributes.hasAttribute(gstar::UPROBE_REAL_POS_X))
	{
		marker.insert(gstar::UPROBE_REAL_POS_X,
			attributes.value(gstar::UPROBE_REAL_POS_X).toString());
	}
	if (attributes.hasAttribute(gstar::UPROBE_LIGHT_POS_Y))
	{
		marker.insert(gstar::UPROBE_LIGHT_POS_Y,
			attributes.value(gstar::UPROBE_LIGHT_POS_Y).toString());
	}
	if (attributes.hasAttribute(gstar::UPROBE_REAL_POS_Y))
	{
		marker.insert(gstar::UPROBE_REAL_POS_Y,
			attributes.value(gstar::UPROBE_REAL_POS_Y).toString());
	}
	if (attributes.hasAttribute(gstar::UPROBE_LIGHT_POS_Z))
	{
		marker.insert(gstar::UPROBE_LIGHT_POS_Z,
			attributes.value(gstar::UPROBE_LIGHT_POS_Z).toString());
	}
	if (attributes.hasAttribute(gstar::UPROBE_MICRO_POS_X))
	{
		marker.insert(gstar::UPROBE_MICRO_POS_X,
			attributes.value(gstar::UPROBE_MICRO_POS_X).toString());
	}
	if (attributes.hasAttribute(gstar::UPROBE_MICRO_POS_Y))
	{
		marker.insert(gstar::UPROBE_MICRO_POS_Y,
			attributes.value(gstar::UPROBE_MICRO_POS_Y).toString());
	}
	if (attributes.hasAttribute(gstar::UPROBE_PRED_POS_X))
	{
		marker.insert(gstar::UPROBE_PRED_POS_X,
			attributes.value(gstar::UPROBE_PRED_POS_X).toString());
	}
	if (attributes.hasAttribute(gstar::UPROBE_PRED_POS_Y))
	{
		marker.insert(gstar::UPROBE_PRED_POS_Y,
			attributes.value(gstar::UPROBE_PRED_POS_Y).toString());
	}

	xml.readNext();
	if (xml.tokenType() == QXmlStreamReader::Characters)
	{
		marker.insert(gstar::UPROBE_NAME, xml.text().toString());
	}

	return marker;

}

/*---------------------------------------------------------------------------*/

QMap<QString, QString> VLM_Model::_parseRegionMarker(QXmlStreamReader& xml)
{


	QMap<QString, QString> marker;

	QXmlStreamAttributes attributes = xml.attributes();

	if (attributes.hasAttribute(gstar::UPROBE_COLOR))
	{
		marker.insert(gstar::UPROBE_COLOR, attributes.value(gstar::UPROBE_COLOR).toString());
	}
	if (attributes.hasAttribute(gstar::UPROBE_REAL_POS_X))
	{
		marker.insert(gstar::UPROBE_REAL_POS_X,
			attributes.value(gstar::UPROBE_REAL_POS_X).toString());
	}
	if (attributes.hasAttribute(gstar::UPROBE_REAL_POS_Y))
	{
		marker.insert(gstar::UPROBE_REAL_POS_Y,
			attributes.value(gstar::UPROBE_REAL_POS_Y).toString());
	}
	if (attributes.hasAttribute(gstar::UPROBE_PRED_POS_X))
	{
		marker.insert(gstar::UPROBE_PRED_POS_X,
			attributes.value(gstar::UPROBE_PRED_POS_X).toString());
	}
	if (attributes.hasAttribute(gstar::UPROBE_PRED_POS_Y))
	{
		marker.insert(gstar::UPROBE_PRED_POS_Y,
			attributes.value(gstar::UPROBE_PRED_POS_Y).toString());
	}
	if (attributes.hasAttribute(gstar::UPROBE_RECT_TLX))
	{
		marker.insert(gstar::UPROBE_RECT_TLX,
			attributes.value(gstar::UPROBE_RECT_TLX).toString());
	}
	if (attributes.hasAttribute(gstar::UPROBE_RECT_TLY))
	{
		marker.insert(gstar::UPROBE_RECT_TLY,
			attributes.value(gstar::UPROBE_RECT_TLY).toString());
	}
	if (attributes.hasAttribute(gstar::UPROBE_RECT_W))
	{
		marker.insert(gstar::UPROBE_RECT_W,
			attributes.value(gstar::UPROBE_RECT_W).toString());
	}
	if (attributes.hasAttribute(gstar::UPROBE_RECT_H))
	{
		marker.insert(gstar::UPROBE_RECT_H,
			attributes.value(gstar::UPROBE_RECT_H).toString());
	}

	xml.readNext();
	if (xml.tokenType() == QXmlStreamReader::Characters)
	{
		marker.insert(gstar::UPROBE_NAME, xml.text().toString());
	}

	return marker;

}

/*---------------------------------------------------------------------------*/

gstar::CoordinateModel* VLM_Model::getCoordModel()
{

   return _coord_model;

}

/*---------------------------------------------------------------------------*/
/*
void VLM_Model::_initializeCoordModel()
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
       if(m_tiffModel != nullptr)
       {
          imgWidth = m_tiffModel->getImageDims(2);
          imgHeight = m_tiffModel->getImageDims(1);

          xScale = (m_samples[0].x * 2.0) / imgWidth;
          yScale = (m_samples[0].y * 2.0) / imgHeight;


          LinearTransformer* lt = new LinearTransformer();
          lt->setTopLeft(0, 0);
          lt->setScale(xScale, yScale, 1.0);
          lt->setDivider(1000.0, 1000.0, 1.0);
          _coord_model = new gstar::CoordinateModel(lt);
       }
   }
   if(m_numSamples == 2)
   {
       if(m_tiffModel != nullptr)
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
         throw std::string("Error: Could not find bounds for VLM workspace!");
      }

      *
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
       *

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

      if(m_tiffModel != nullptr)
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
*/

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/