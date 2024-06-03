/*-----------------------------------------------------------------------------
 * Copyright (c) 2023, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/ScatterPlotView.h>
#include <QVBoxLayout>
#include <QDateTime>
#include <QDesktopServices>
#include <QCoreApplication>
#include "preferences/Preferences.h"
#include "stats/correlation_coefficient.h"

 //---------------------------------------------------------------------------

ScatterPlotView::ScatterPlotView(bool display_log10, bool black_background, QWidget* parent) : QWidget(parent)
{

    _model = nullptr;
    _curAnalysis = QString(STR_FIT_NNLS.c_str());

    _axisXLog10 = new QLogValueAxis();
    _axisXLog10->setTitleText("");
    _axisXLog10->setLabelFormat("%.1e");
    _axisXLog10->setBase(10.0);

    _axisX = new QValueAxis();
    _axisX->setTitleText("");
    _axisX->setLabelFormat("%f");
    
    _axisYLog10 = new QLogValueAxis();
    _axisYLog10->setTitleText("");
    _axisYLog10->setLabelFormat("%.1e");
    _axisYLog10->setBase(10.0);

    _axisY = new QValueAxis();
    _axisY->setTitleText("");
    _axisY->setLabelFormat("%f");

    _chart = new QChart();

    _lb_roi = new QLabel("ROI:");
    _lb_roi->setAlignment(Qt::AlignRight);
    _cb_roi = new QComboBox();
    _cb_roi->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    connect(_cb_roi, &QComboBox::currentTextChanged, this, &ScatterPlotView::onNameChange);
    _cb_x_axis_element = new QComboBox();
    _cb_x_axis_element->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    connect(_cb_x_axis_element, &QComboBox::currentTextChanged, this, &ScatterPlotView::onNameChange);
    _cb_y_axis_element = new QComboBox();
    _cb_y_axis_element->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    connect(_cb_y_axis_element, &QComboBox::currentTextChanged, this, &ScatterPlotView::onNameChange);

    _chartView = new QChartView(_chart);

    //setRenderHint(QPainter::Antialiasing);
    _scatter_series = new QScatterSeries();
    _scatter_series->setColor(QColor(Qt::blue));
    QString marker_shape = Preferences::inst()->getValue(STR_PFR_MARKER_SHAPE).toString();
    if (marker_shape == "Circle")
    {
        _scatter_series->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    }
    else
    {
        _scatter_series->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    }
    
    setBlackBackground(Preferences::inst()->getValue(STR_PFR_SCATTER_DARK_BACKGROUND).toBool());
    _scatter_series->setBorderColor(Qt::transparent);
    _scatter_series->setMarkerSize(1.0);
    //_scatter_series->setUseOpenGL(true); // causes exception when deconstructor called.
    _chart->addSeries(_scatter_series);
    _display_log10 = display_log10;
    if (_display_log10)
    {
        _chart->addAxis(_axisXLog10, Qt::AlignBottom);
        _chart->addAxis(_axisYLog10, Qt::AlignLeft);
        _scatter_series->attachAxis(_axisXLog10);
        _scatter_series->attachAxis(_axisYLog10);
    }
    else
    {
        _chart->addAxis(_axisX, Qt::AlignBottom);
        _chart->addAxis(_axisY, Qt::AlignLeft);
        _scatter_series->attachAxis(_axisX);
        _scatter_series->attachAxis(_axisY);
    }

    if (black_background)
    {
        _chart->setBackgroundBrush(QBrush(QColor("black")));
    }

    _lb_corr_coef = new QLabel();
    _lb_corr_coef->setAlignment(Qt::AlignCenter);

    QLabel* lb_xaxis = new QLabel("X Axis:");
    lb_xaxis->setAlignment(Qt::AlignRight | Qt::AlignCenter);

    QLabel* lb_yaxis = new QLabel("Y Axis:");
    lb_yaxis->setAlignment(Qt::AlignRight | Qt::AlignCenter);

    QLabel* lb_coef = new QLabel("Correlation Coefficient:");
    lb_coef->setAlignment(Qt::AlignRight | Qt::AlignCenter);

    QHBoxLayout* hbox = new QHBoxLayout();
    hbox->addWidget(_lb_roi);
    hbox->addWidget(_cb_roi);
    hbox->addWidget(lb_xaxis);
    hbox->addWidget(_cb_x_axis_element);
    hbox->addWidget(lb_yaxis);
    hbox->addWidget(_cb_y_axis_element);
    hbox->addWidget(lb_coef);
    hbox->addWidget(_lb_corr_coef);
    //hbox->addItem(new QSpacerItem(9999, 40, QSizePolicy::Maximum));

    QVBoxLayout* vbox = new QVBoxLayout();
    vbox->addItem(hbox);
    vbox->addWidget(_chartView);

    setGridLinesVisible(Preferences::inst()->getValue(STR_PRF_ScatterPlot_GridLines).toBool());

    setLayout(vbox);

}
 
//---------------------------------------------------------------------------

ScatterPlotView::~ScatterPlotView()
{
    _model = nullptr;
}

//---------------------------------------------------------------------------

void ScatterPlotView::exportPngCsv()
{
    if (_model != nullptr)
    {
        QDir dir = _model->getDir();
        QFileInfo finfo(dir.absolutePath());

        //QPixmap pixmap(rect().size());
        //render(&pixmap, QPoint(), QRegion(rect()));

        QPixmap pixmap = _chartView->grab();
        QDateTime date = QDateTime::currentDateTime();
        QString formattedTime = date.toString("yyyy.MM.dd_hh.mm.ss");
        QByteArray formattedTimeMsg = formattedTime.toLocal8Bit();

        dir.cdUp();
        dir.cdUp();
        dir.cd("output");

        QString roi_name = _cb_roi->currentText();
        QString apath;
        if (roi_name.length() > 0)
        {
            apath = dir.absolutePath() + QDir::separator() + finfo.fileName() + QString("_scatter_" + _curAnalysis + "_" + roi_name + "-" + _cb_x_axis_element->currentText() + "-" + _cb_y_axis_element->currentText() + "_" + formattedTime);
        }
        else
        {
            apath = dir.absolutePath() + QDir::separator() + finfo.fileName() + QString("_scatter_" + _curAnalysis + "_" + _cb_x_axis_element->currentText() + "-" + _cb_y_axis_element->currentText() + "_" + formattedTime);
        }
        QString png_path = QDir::cleanPath(apath + ".png");
        if (false == pixmap.save(png_path, "PNG"))
        {
            logE << "Could not save PNG for " << png_path.toStdString() << "\n";
        }
        else
        {
            if (false == QDesktopServices::openUrl(QUrl::fromLocalFile(dir.absolutePath())))
            {
                logE << "Failed to open dir " << dir.absolutePath().toStdString() << "\n";
            }
        }

        QString csv_path = QDir::cleanPath(apath + ".csv");
        _exportScatterPlotCSV(csv_path);
       
    }
}

//---------------------------------------------------------------------------

void ScatterPlotView::_exportScatterPlotCSV(QString filePath)
{
    if (_model != nullptr)
    {
        const std::unordered_map<std::string, Map_ROI> rois = _model->get_map_rois();
        data_struct::ArrayXXr<float> x_map;
        data_struct::ArrayXXr<float> y_map;
        std::vector<float> x_motor = _model->get_x_axis();
        std::vector<float> y_motor = _model->get_y_axis();
        QString roi_name = _cb_roi->currentText(); 
        if (_getXY_Maps(x_map, y_map))
        {
            std::ofstream out_stream(filePath.toStdString());
            if (out_stream.is_open())
            {
                out_stream << "ascii information for file: " << _model->getDatasetName().toStdString() << "\n";
                out_stream << "Analysis Type: " << _curAnalysis.toStdString() << "\n";
                if (roi_name.length() > 0)
                {
                    out_stream << "ROI Name: " << roi_name.toStdString() << "\n";
                }
                out_stream << "X Axis Name: " << _cb_x_axis_element->currentText().toStdString() << "\n";
                out_stream << "Y Axis Name: " << _cb_y_axis_element->currentText().toStdString() << "\n";
                out_stream << "Units: Cts/s\n";
                out_stream << "Rows: " << x_map.rows() << "\n";
                out_stream << "Cols: " << x_map.cols() << "\n";
                out_stream << "X Index, Y Index, X Value, Y Value, X Motor, Y Motor \n";
                if (rois.count(roi_name.toStdString()) > 0)
                {
                    Map_ROI map_roi = rois.at(roi_name.toStdString());
                    for (auto& itr : map_roi.pixel_list)
                    {
                        out_stream << itr.first << "," << itr.second << "," << x_map(itr.second, itr.first) << "," << y_map(itr.second, itr.first) << "," << x_motor[itr.first] << ","<< y_motor[itr.second]<< "\n";
                    }
                }
                else
                {
                    for (int y = 0; y < x_map.rows(); y++)
                    {
                        for (int x = 0; x < x_map.cols(); x++)
                        {
                            out_stream << x << "," << y << "," << x_map(y, x) << "," << y_map(y, x) << x_motor[x] << ","<< y_motor[y]<< "\n";
                        }
                    }
                }
            }
            else
            {
                logE << "Could not open for writing scatter plot CSV : " << filePath.toStdString() << "\n";
            }
            out_stream.close();
        }
    }
}

//---------------------------------------------------------------------------

void ScatterPlotView::setXYAxis(QString roi_name, QString x_axis_name, QString y_axis_name)
{
    _cb_roi->setCurrentText(roi_name);
    _cb_x_axis_element->setCurrentText(x_axis_name);
    _cb_y_axis_element->setCurrentText(y_axis_name);
    _updatePlot();
}

//---------------------------------------------------------------------------

void ScatterPlotView::setShape(QScatterSeries::MarkerShape shape)
{
    _scatter_series->setMarkerShape(shape);
    _updatePlot();
}

//---------------------------------------------------------------------------

void ScatterPlotView::setBlackBackground(int val)
{
    if (val == 0)
    {
        _chart->setBackgroundBrush(QBrush(QColor("white")));
    }
    else
    {
        _chart->setBackgroundBrush(QBrush(QColor("black")));
    }
}

//---------------------------------------------------------------------------

void ScatterPlotView::setLog10(int val)
{
    
    if (val == 0)
    {
        _display_log10 = false;
        _scatter_series->detachAxis(_axisXLog10);
        _scatter_series->detachAxis(_axisYLog10);

        _chart->removeAxis(_axisXLog10);
        _chart->removeAxis(_axisYLog10);

        _chart->addAxis(_axisX, Qt::AlignBottom);
        _chart->addAxis(_axisY, Qt::AlignLeft);

        _scatter_series->attachAxis(_axisX);
        _scatter_series->attachAxis(_axisY);
    }
    else
    {
        _display_log10 = true;
        _scatter_series->detachAxis(_axisX);
        _scatter_series->detachAxis(_axisY);
        _chart->removeAxis(_axisX);
        _chart->removeAxis(_axisY);

        _chart->addAxis(_axisXLog10, Qt::AlignBottom);
        _chart->addAxis(_axisYLog10, Qt::AlignLeft);
        _scatter_series->attachAxis(_axisXLog10);
        _scatter_series->attachAxis(_axisYLog10);
    }
    _updatePlot();

}

//---------------------------------------------------------------------------

void ScatterPlotView::onNameChange(QString name)
{
    _updatePlot();
}

//---------------------------------------------------------------------------

void ScatterPlotView::setModel(MapsH5Model* model)
{
    _model = model;
    _updateNames();
}

//---------------------------------------------------------------------------

void ScatterPlotView::setAnalysisType(QString name)
{
    _curAnalysis = name;
    _updateNames();
}

//---------------------------------------------------------------------------

void ScatterPlotView::_updateNames()
{
    if (_model != nullptr)
    {
        const std::unordered_map<std::string, Map_ROI> rois = _model->get_map_rois();

        _cb_roi->clear();
        _cb_roi->addItem(" ");
        int itemAmt = 0;
        for (auto& itr : rois)
        {
            _cb_roi->addItem(QString(itr.first.c_str()));
            itemAmt++;
        }
        if(itemAmt == 0)
        {
            _cb_roi->setEnabled(false);
        }
        else
        {
            _cb_roi->setEnabled(true);
        }
        std::vector<std::string> map_names;
        _model->generateNameLists(_curAnalysis, map_names);
        QString xSavedName = _cb_x_axis_element->currentText();
        QString ySavedName = _cb_y_axis_element->currentText();
        _cb_x_axis_element->clear();
        _cb_y_axis_element->clear();

        _cb_x_axis_element->addItem(" ");
        _cb_y_axis_element->addItem(" ");
        for (auto& itr : map_names)
        {
            _cb_x_axis_element->addItem(QString(itr.c_str()));
            _cb_y_axis_element->addItem(QString(itr.c_str()));
        }

        int xIdx = _cb_x_axis_element->findText(xSavedName);
        int yIdx = _cb_y_axis_element->findText(ySavedName);
        if (xIdx > -1)
        {
            _cb_x_axis_element->setCurrentIndex(xIdx);
        }
        if (yIdx > -1)
        {
            _cb_y_axis_element->setCurrentIndex(yIdx);
        }
    }
    _updatePlot();
}

//---------------------------------------------------------------------------

void ScatterPlotView::updateMarkerSize(qreal val)
{
    _scatter_series->setMarkerSize(val);
    _updatePlot();
}

//---------------------------------------------------------------------------

void ScatterPlotView::setGridLinesVisible(int val)
{
    if (val == 0)
    {
        _axisX->setGridLineVisible(false);
        _axisY->setGridLineVisible(false);
        _axisXLog10->setGridLineVisible(false);
        _axisYLog10->setGridLineVisible(false);
    }
    else
    {
        _axisX->setGridLineVisible(true);
        _axisY->setGridLineVisible(true);
        _axisXLog10->setGridLineVisible(true);
        _axisYLog10->setGridLineVisible(true);
    }
}

//---------------------------------------------------------------------------

bool ScatterPlotView::_getXY_Maps(data_struct::ArrayXXr<float> &x_map, data_struct::ArrayXXr<float> &y_map)
{
    bool foundX = false;
    bool foundY = false;

    if (_model != nullptr)
    {
        std::string xName = _cb_x_axis_element->currentText().toStdString();
        std::string yName = _cb_y_axis_element->currentText().toStdString();

        data_struct::Fit_Count_Dict<float> fit_counts;
        _model->getAnalyzedCounts(_curAnalysis.toStdString(), fit_counts);
        std::map<std::string, data_struct::ArrayXXr<float>>* scalers = _model->getScalers();

        int xCnt = fit_counts.count(xName);
        if (xCnt != 0)
        {
            x_map = fit_counts.at(xName);
            foundX = true;
        }
        else
        {
            xCnt = scalers->count(xName);
            if (xCnt != 0)
            {
                x_map = scalers->at(xName);
                foundX = true;
            }
        }


        int yCnt = fit_counts.count(yName);
        if (yCnt != 0)
        {
            y_map = fit_counts.at(yName);
            foundY = true;
        }
        else
        {
            yCnt = scalers->count(yName);
            if (yCnt != 0)
            {
                y_map = scalers->at(yName);
                foundY = true;
            }
        }
    }
    return foundX && foundY;
}

//---------------------------------------------------------------------------

void ScatterPlotView::_updatePlot()
{
    std::string xName = _cb_x_axis_element->currentText().toStdString();
    std::string yName = _cb_y_axis_element->currentText().toStdString();


    if (_model != nullptr)
    {
        data_struct::ArrayXXr<float> x_map;
        data_struct::ArrayXXr<float> y_map;

        if (_getXY_Maps(x_map, y_map))
        {
            _chart->removeSeries(_scatter_series);
            _scatter_series->clear();

            if (_display_log10)
            {
                x_map = x_map.unaryExpr([](float v) { return (v <= 0.0f) ? 0.0000001f : log10(v); });
                y_map = y_map.unaryExpr([](float v) { return (v <= 0.0f) ? 0.0000001f : log10(v); });
            }

            const std::unordered_map<std::string, Map_ROI> rois = _model->get_map_rois();

            QString roi_name = _cb_roi->currentText(); 

            if (rois.count(roi_name.toStdString()) > 0)
            {
                data_struct::ArrayXXr<float> x_map_roi;
                data_struct::ArrayXXr<float> y_map_roi;
                Map_ROI map_roi = rois.at(roi_name.toStdString());
                x_map_roi.resize(1, map_roi.pixel_list.size());
                y_map_roi.resize(1, map_roi.pixel_list.size());
                int i = 0;
                for (auto& itr : map_roi.pixel_list)
                {
                    x_map_roi(0,i) = x_map(itr.second, itr.first);
                    y_map_roi(0,i) = y_map(itr.second, itr.first);
                    i++;
                }
                x_map = x_map_roi;
                y_map = y_map_roi;
            }
            float xMinVal = x_map.minCoeff();
            float yMinVal = y_map.minCoeff();

            float xMaxVal = x_map.maxCoeff();
            float yMaxVal = y_map.maxCoeff();

            Eigen::Array<double, Eigen::Dynamic, Eigen::RowMajor> x_arr;
            Eigen::Array<double, Eigen::Dynamic, Eigen::RowMajor> y_arr;
            x_arr.resize(x_map.cols() * x_map.rows());
            y_arr.resize(x_map.cols() * x_map.rows());
            int n = 0;

            for (int y = 0; y < x_map.rows(); y++)
            {
                for (int x = 0; x < x_map.cols(); x++)
                {
                    x_arr(n) = (double)x_map(y, x);
                    y_arr(n) = (double)y_map(y, x);
                    _scatter_series->append(x_map(y,x), y_map(y,x));
                    n++;
                }
            }

            double corr_coef = find_coefficient(x_arr, y_arr);
            _lb_corr_coef->setText(QString::number(corr_coef));
            _scatter_series->setBorderColor(Qt::transparent);
            _chart->addSeries(_scatter_series);

            _axisX->setTitleText(_cb_x_axis_element->currentText());
            _axisY->setTitleText(_cb_y_axis_element->currentText());
            _axisXLog10->setTitleText(_cb_x_axis_element->currentText());
            _axisYLog10->setTitleText(_cb_y_axis_element->currentText());
            QString sname = _cb_x_axis_element->currentText() + "/" + _cb_y_axis_element->currentText();
            
            if (_display_log10)
            {
                _axisXLog10->setRange(xMinVal, xMaxVal);
                _axisYLog10->setRange(yMinVal, yMaxVal);
            }
            else
            {
                _axisX->setRange(xMinVal, xMaxVal);
                _axisY->setRange(yMinVal, yMaxVal);
            }

            _scatter_series->setName(sname);
        }
    }

}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/