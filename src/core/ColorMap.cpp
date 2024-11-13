/*
* Copyright (c) 2016, UChicago Argonne, LLC. All rights reserved.
*
* Copyright 2016. UChicago Argonne, LLC. This software was produced 
* under U.S. Government contract DE-AC02-06CH11357 for Argonne National 
* Laboratory (ANL), which is operated by UChicago Argonne, LLC for the 
* U.S. Department of Energy. The U.S. Government has rights to use, 
* reproduce, and distribute this software.  NEITHER THE GOVERNMENT NOR 
* UChicago Argonne, LLC MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR 
* ASSUMES ANY LIABILITY FOR THE USE OF THIS SOFTWARE.  If software is 
* modified to produce derivative works, such modified software should 
* be clearly marked, so as not to confuse it with the version available 
* from ANL.

* Additionally, redistribution and use in source and binary forms, with 
* or without modification, are permitted provided that the following 
* conditions are met:
*
*   * Redistributions of source code must retain the above copyright 
*     notice, this list of conditions and the following disclaimer. 
*
*   * Redistributions in binary form must reproduce the above copyright 
*     notice, this list of conditions and the following disclaimer in 
*     the documentation and/or other materials provided with the 
*     distribution. 
*
*   * Neither the name of UChicago Argonne, LLC, Argonne National 
*     Laboratory, ANL, the U.S. Government, nor the names of its 
*     contributors may be used to endorse or promote products derived 
*     from this software without specific prior written permission. 

* THIS SOFTWARE IS PROVIDED BY UChicago Argonne, LLC AND CONTRIBUTORS 
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL UChicago 
* Argonne, LLC OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
* POSSIBILITY OF SUCH DAMAGE.
*/

#include <core/ColorMap.h>
#include "core/defines.h"
#include <QCoreApplication>
#include <QTextStream>
#include <QStringList>
#include <QDir>

//---------------------------------------------------------------------------

ColorMap* ColorMap::_inst = nullptr;

ColorMap::ColorMap()
{


}

//---------------------------------------------------------------------------

ColorMap::~ColorMap()
{
    
    
}

//---------------------------------------------------------------------------

ColorMap* ColorMap::inst()
{

    if (_inst == nullptr)
        _inst = new ColorMap();

    return _inst;

}

//---------------------------------------------------------------------------

QVector<QRgb>* ColorMap::get_color_map(QString name)
{
    if (_color_maps.count(name) > 0)
    {
        return &_color_maps[name];
    }
    return nullptr;
}

//---------------------------------------------------------------------------

void ColorMap::reload_color_maps()
{
    QString colormapDir = QCoreApplication::applicationDirPath();
    QDir directory(colormapDir);
    directory.cd("../colormaps");
    
    if (!directory.exists())
    {
            QString warn_msg = "Cannot find the "+ colormapDir +" directory";
            qWarning("%s", warn_msg.toStdString().c_str());
            //logW<< "Cannot find the " << directory.path()<<"\n";
            return;
    }
    directory.setFilter(QDir::Files | QDir::NoSymLinks);

    QFileInfoList list = directory.entryInfoList();

    for (int i = 0; i < list.size(); ++i)
    {
        QFileInfo fileInfo = list.at(i);
        if (fileInfo.suffix() == "csv")
        {
            QFile inputFile(fileInfo.absoluteFilePath());
            if (inputFile.open(QIODevice::ReadOnly))
            {
                QVector<QRgb> vrgb;
                QTextStream in(&inputFile);
                while (!in.atEnd())
                {
                    QString line = in.readLine();
                    QStringList values = line.split(',');
                    if (values.size() == 3)
                    {
                        vrgb.append(qRgb(values[0].toInt(), values[1].toInt(), values[2].toInt()));
                    }
                }
                inputFile.close();
                _color_maps[fileInfo.baseName()] = vrgb;
            }
            else
            {
                QString warn_msg = "Cannot open colormap " + fileInfo.absoluteFilePath();
                qWarning("%s", warn_msg.toStdString().c_str());
            }
           
            
        }
    }

}

//---------------------------------------------------------------------------