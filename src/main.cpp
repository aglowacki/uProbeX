/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <QApplication>

#include <core/uProbeX.h>
#include <QDateTime>
#include <QString>
#include <QFile>
#include <QTextStream>
#include "core/PythonLoader.h"
#include "mvc/LiveMapsElementsWidget.h"

/*---------------------------------------------------------------------------*/

int main(int argc, char** argv)
{

	QApplication app(argc, argv);
   //dark style
//   QFile* file = new QFile(":/qss/dark2.qss");
//   file->open(QFile::ReadOnly | QFile::Text);
//   QTextStream *stream = new QTextStream(file);
//   app.setStyleSheet(stream->readAll());

	if(argc > 1)
	{
		QString arg1 = QString(argv[1]);
		if(arg1 == "--testPython")
		{
			try
			{
				PythonLoader::inst()->init();
			}
			catch(PythonLoader::pyException ex)
			{
				qDebug()<<"error main "<<ex.what();
			}
			exit(0);
		}
		if(arg1 == "--live")
		{
   			Preferences m_preferences;
			QString strIp = m_preferences.readValueKey(Preferences::LastIP).toString();
			QString strPort = m_preferences.readValueKey(Preferences::LastPort).toString();
			LiveMapsElementsWidget *liveMapsViewer = new LiveMapsElementsWidget(strIp, strPort);
			liveMapsViewer->show();
			return app.exec();
		}
	}

	// Startup application
	uProbeX* widget = new uProbeX(nullptr);
	widget->setAttribute(Qt::WA_DeleteOnClose);
	widget->show();

	// Run
	return app.exec();

	// Clean up
	if (widget != nullptr)
	{
		delete widget;
	}
}

/*---------------------------------------------------------------------------*/
