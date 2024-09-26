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
static std::ostringstream strCout;

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
			QString h_msg(msg);
			int cnt = 0;
			switch (type)
			{
			case QtDebugMsg:
				cnt += h_msg.count("Info: ");
				cnt += h_msg.count("Warning: ");
				cnt += h_msg.count("Error: ");
				h_msg.replace("Info: ", "<span style=\" font-weight: bold; \">Info: </span>");
				h_msg.replace("\033[1;31mError: \033[0;m", "<span style=\"color : red; font-weight: bold; \">Error: </span>");
				h_msg.replace("\033[1;33mWarning: \033[0;m", "<span style=\"color : yellow; font-weight: bold; \">Warning: </span>");
				if (cnt > 0)
				{
					h_msg.replace("\n", "<br>");
				}
				uProbeX::log_textedit->insertHtml(h_msg);
				uProbeX::log_textedit->ensureCursorVisible();
				break;
			case QtWarningMsg:
				h_msg = "<span style=\"color : yellow; \">" + msg + "</span><br />";
				uProbeX::log_textedit->insertHtml(h_msg);
				break;
			case QtCriticalMsg:
				h_msg = "<span style=\"color : red; \">" + msg + "</span><br />";
				uProbeX::log_textedit->insertHtml(h_msg);
				break;
			case QtFatalMsg:
				//abort();
				break;
			}
		}
	}
}

/*---------------------------------------------------------------------------*/
// This redirects console logs from DLL's to out log widget
void readSTDOUT()
{
	std::string line = strCout.str();
	strCout.str(std::string());
	if (line.length() > 0)
	{
		qDebug() << line.c_str();
	}
}

/*---------------------------------------------------------------------------*/

int main(int argc, char** argv)
{

	QApplication app(argc, argv);
	//dark style
	
	QString str_theme = Preferences::inst()->getValue(STR_PFR_THEME).toString();
	if (str_theme.length() > 0 && str_theme != STR_DEFAULT)
	{
		QString theme_file = "../themes/" + str_theme + ".qss";
		QFile* file = new QFile(theme_file);
		file->open(QFile::ReadOnly | QFile::Text);
		QTextStream* stream = new QTextStream(file);
		app.setStyleSheet(stream->readAll());
	}
	qInstallMessageHandler(myMessageOutput);

	std::cout.rdbuf(strCout.rdbuf());
	QTimer timer;
	QObject::connect(&timer, &QTimer::timeout, readSTDOUT);
	timer.start(1000);

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


    // get location of where we are running from and use it to find ref files
    std::string exe_loc = std::string(argv[0]);
    int prog_idx = exe_loc.find("bin/uProbeX");
    if (prog_idx > 0)
    {
        exe_loc = exe_loc.substr(0, prog_idx);
    }
	else // windows
	{
		int prog_idx = exe_loc.find("bin\\uProbeX.exe");
		if (prog_idx > 0)
		{
			exe_loc = exe_loc.substr(0, prog_idx);
		}
		else
		{
			exe_loc = ".";
		}
	}

    //////// HENKE and ELEMENT INFO /////////////
    const std::string element_csv_filename = exe_loc + "./reference/xrf_library.csv";
    const std::string element_henke_filename = exe_loc + "./reference/henke.xdr";
    const std::string scaler_lookup_yaml = exe_loc + "./reference/Scaler_to_PV_map.yaml";

    if (false == io::file::load_scalers_lookup(scaler_lookup_yaml))
    {
        logE << " Could not load " << scaler_lookup_yaml << ". Won't be able to translate from PV to Label for scalers!\n";
    }

    //load element information
    if (false == io::file::load_element_info<float>(element_henke_filename, element_csv_filename))
    {
        logE << "loading element information: " << "\n";
        return -1;
    }
    if (false == io::file::load_element_info<double>(element_henke_filename, element_csv_filename))
    {
        logE << "loading element information: " << "\n";
        return -1;
    }
    
#if defined _WIN32 || defined __CYGWIN__
	FreeConsole();
#endif
	// Startup application
	uProbeX* widget = new uProbeX(nullptr);
	//widget->setAttribute(Qt::WA_DeleteOnClose);
	widget->setWindowState(Qt::WindowMaximized);
	widget->show();
	
	// Run
	int ret = app.exec();

	running = false;

	// Clean up
	if (widget != nullptr)
	{
		delete widget;
	}

	return ret;
}

/*---------------------------------------------------------------------------*/
