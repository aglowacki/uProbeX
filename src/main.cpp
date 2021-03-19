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

bool running = true;
 /*---------------------------------------------------------------------------*/

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	if (running)
	{
		//QProcess::readAllStandardOutput();
		if (uProbeX::log_textedit == nullptr)
		{
			QByteArray localMsg = msg.toLocal8Bit();
			switch (type) {
			case QtDebugMsg:
				fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
				break;
			case QtWarningMsg:
				fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
				break;
			case QtCriticalMsg:
				fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
				break;
			case QtFatalMsg:
				fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
				abort();
			}
		}
		else
		{
			switch (type) {
			case QtDebugMsg:
			case QtWarningMsg:
			case QtCriticalMsg:
				// redundant check, could be removed, or the 
				// upper if statement could be removed
				if (uProbeX::log_textedit != nullptr)
					uProbeX::log_textedit->append(msg);
				break;
			case QtFatalMsg:
				abort();
			}
		}
	}
}

/*---------------------------------------------------------------------------*/
// This redirects console logs from DLL's to out log widget
void readSTDOUT()
{
	std::string line;
	ostringstream strCout;
	cout.rdbuf(strCout.rdbuf());

	while (running)
	{
		line = strCout.str();
		strCout.str(std::string());
		if (line.length() > 0)
		{
			qDebug() << line.c_str();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

/*---------------------------------------------------------------------------*/

int main(int argc, char** argv)
{

	QApplication app(argc, argv);
	//TODO: read pref to see if dark or light theme
   //dark style
//   QFile* file = new QFile(":/qss/dark2.qss");
//   file->open(QFile::ReadOnly | QFile::Text);
//   QTextStream *stream = new QTextStream(file);
//   app.setStyleSheet(stream->readAll());

	qInstallMessageHandler(myMessageOutput);

	std::thread stdio_stream_thread(readSTDOUT);

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
				logE<<"error main "<<ex.what();
			}
			exit(0);
		}
		if(arg1 == "--live")
		{
			QString strIp = Preferences::inst()->getValue(STR_PRF_LastIP).toString();
			QString strPort = Preferences::inst()->getValue(STR_PRF_LastPort).toString();
			LiveMapsElementsWidget *liveMapsViewer = new LiveMapsElementsWidget(strIp, strPort);
			liveMapsViewer->show();
			return app.exec();
		}
	}
#if defined _WIN32 || defined __CYGWIN__
	FreeConsole();
#endif
	// Startup application
	uProbeX* widget = new uProbeX(nullptr);
	//widget->setAttribute(Qt::WA_DeleteOnClose);
	widget->show();
	
	// Run
	int ret = app.exec();

	running = false;

	stdio_stream_thread.join();

	// Clean up
	if (widget != nullptr)
	{
		delete widget;
	}

	return ret;
}

/*---------------------------------------------------------------------------*/
