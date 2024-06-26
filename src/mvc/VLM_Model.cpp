/*-----------------------------------------------------------------------------
 * Copyright (c) 2021, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/VLM_Model.h>
#include <QStringList>
#include <gstar/GStarResource.h>
#include <QMessageBox>
#include "core/defines.h"

const QString STR_MARKERS = "markers";
const QString STR_MARKER = "marker";
const QString STR_REGION_MARKER = "regionmarker";
const QString STR_SOLVER = "solver";

/*----------------src/mvc/VLM_Model.cpp \-----------------------------------------------------------*/

VLM_Model::VLM_Model() : AbstractWindowModel()
{
    _coord_model = nullptr;

}

//---------------------------------------------------------------------------

VLM_Model::~VLM_Model()
{

 
   if(_coord_model != nullptr)
   {
      delete _coord_model;
      _coord_model = nullptr;
   }

}

//---------------------------------------------------------------------------

void VLM_Model::add_calib_marker(const QMap<QString, QString>& marker)
{
	_markersLoaded.append(marker);
}

//---------------------------------------------------------------------------

void VLM_Model::add_region_marker(const QMap<QString, QString>& marker)
{
	_regionMarkersLoaded.append(marker);
}

//---------------------------------------------------------------------------

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
			if (xml.name() == STR_MARKERS)
			{
				continue;
			}
			else if (xml.name() == STR_MARKER)
			{
				_markersLoaded.prepend(_parseMarker(xml));
				continue;
			}
			else if (xml.name() == STR_REGION_MARKER)
			{
				_regionMarkersLoaded.prepend(_parseRegionMarker(xml));
			}
			else if (xml.name() == STR_SOLVER)
			{
				// TODO: load and config solver 
				//_regionMarkersLoaded.prepend(_parseRegionMarker(xml));
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

//---------------------------------------------------------------------------

void VLM_Model::save_xml(QString filename)
{
 	QFile file(filename);

   if (!file.open(QIODevice::WriteOnly))
   {
      QMessageBox::warning(0, "Read only", "The file is in read only mode");
	  return;
   }
   QXmlStreamWriter* xmlWriter = new QXmlStreamWriter();
   xmlWriter->setDevice(&file);

   xmlWriter->writeStartDocument();
   xmlWriter->writeStartElement("markers");

	// Get the crossing marker information
	for (auto itr :_markersLoaded)
	{
		xmlWriter->writeStartElement("marker");
		xmlWriter->writeAttribute(gstar::UPROBE_COLOR,itr.value(gstar::UPROBE_COLOR, "#ff007f"));
		xmlWriter->writeAttribute(gstar::UPROBE_LIGHT_POS_X,itr.value(gstar::UPROBE_LIGHT_POS_X, "0"));
		xmlWriter->writeAttribute(gstar::UPROBE_LIGHT_POS_Y,itr.value(gstar::UPROBE_LIGHT_POS_Y, "0"));
		xmlWriter->writeAttribute(gstar::UPROBE_LIGHT_POS_Z,itr.value(gstar::UPROBE_LIGHT_POS_Z, "0"));
		xmlWriter->writeAttribute(gstar::UPROBE_REAL_POS_X,itr.value(gstar::UPROBE_REAL_POS_X, "0"));
		xmlWriter->writeAttribute(gstar::UPROBE_REAL_POS_Y,itr.value(gstar::UPROBE_REAL_POS_Y, "0"));
		xmlWriter->writeAttribute(gstar::UPROBE_PRED_POS_X,itr.value(gstar::UPROBE_PRED_POS_X, "0"));
		xmlWriter->writeAttribute(gstar::UPROBE_PRED_POS_Y,itr.value(gstar::UPROBE_PRED_POS_Y, "0"));
		xmlWriter->writeAttribute(gstar::UPROBE_MICRO_POS_X,itr.value(gstar::UPROBE_MICRO_POS_X, "0"));
		xmlWriter->writeAttribute(gstar::UPROBE_MICRO_POS_Y,itr.value(gstar::UPROBE_MICRO_POS_Y, "0"));
		xmlWriter->writeCharacters (itr.value(gstar::UPROBE_NAME, "_"));
		xmlWriter->writeEndElement();
	}

	// Get the region marker information
	for (auto itr :_regionMarkersLoaded)
	{
		xmlWriter->writeStartElement("regionmarker");
		
		xmlWriter->writeAttribute(gstar::UPROBE_COLOR,itr.value(gstar::UPROBE_COLOR, "#ff007f"));
		xmlWriter->writeAttribute(gstar::UPROBE_PRED_POS_X,itr.value(gstar::UPROBE_PRED_POS_X, "0"));
		xmlWriter->writeAttribute(gstar::UPROBE_PRED_POS_Y,itr.value(gstar::UPROBE_PRED_POS_Y, "0"));
		xmlWriter->writeAttribute(gstar::UPROBE_WIDTH,itr.value(gstar::UPROBE_WIDTH, "0"));
		xmlWriter->writeAttribute(gstar::UPROBE_HEIGHT,itr.value(gstar::UPROBE_HEIGHT, "0"));
		xmlWriter->writeAttribute(gstar::UPROBE_MICRO_POS_X,itr.value(gstar::UPROBE_MICRO_POS_X, "0"));
		xmlWriter->writeAttribute(gstar::UPROBE_MICRO_POS_Y,itr.value(gstar::UPROBE_MICRO_POS_Y, "0"));
		xmlWriter->writeAttribute(gstar::UPROBE_REAL_POS_X,itr.value(gstar::UPROBE_REAL_POS_X, "0"));
		xmlWriter->writeAttribute(gstar::UPROBE_REAL_POS_Y,itr.value(gstar::UPROBE_REAL_POS_Y, "0"));
		xmlWriter->writeAttribute(gstar::UPROBE_RECT_TLX,itr.value(gstar::UPROBE_RECT_TLX, "0"));
		xmlWriter->writeAttribute(gstar::UPROBE_RECT_TLY,itr.value(gstar::UPROBE_RECT_TLY, "0"));
		xmlWriter->writeAttribute(gstar::UPROBE_RECT_W,itr.value(gstar::UPROBE_RECT_W, "0"));
		xmlWriter->writeAttribute(gstar::UPROBE_RECT_H,itr.value(gstar::UPROBE_RECT_H, "0"));
		
		//xmlWriter->writeAttribute(gstar::UPROBE_SIZE,itr.value(gstar::UPROBE_SIZE, "20"));
		xmlWriter->writeCharacters (itr.value(gstar::UPROBE_NAME));
		xmlWriter->writeEndElement();
	}

	xmlWriter->writeEndElement();
	xmlWriter->writeEndDocument();

	delete xmlWriter;
}

//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------

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

//---------------------------------------------------------------------------

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

//---------------------------------------------------------------------------

gstar::CoordinateModel* VLM_Model::getCoordModel()
{

   return _coord_model;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------