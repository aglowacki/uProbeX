/*-----------------------------------------------------------------------------
 * Copyright (c) 2025, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <QVBoxLayout>
#include <mvc/PolarXanesWidget.h>
#include <QDesktopServices>

//---------------------------------------------------------------------------

PolarXanesWidget::PolarXanesWidget(QWidget* parent) : QWidget(parent)
{
    _model = nullptr;
    createLayout();
}

//---------------------------------------------------------------------------

PolarXanesWidget::~PolarXanesWidget()
{

}

//---------------------------------------------------------------------------

void PolarXanesWidget::createLayout()
{
    
    _spectra_widget = new SpectraWidget();
    _spectra_widget->set_log10(false);

    _cb_analysis = new QComboBox();
    connect(_cb_analysis, &QComboBox::currentTextChanged, this, &PolarXanesWidget::onSelecAnalysisOrElement);
    _cb_elements = new QComboBox();
    connect(_cb_elements, &QComboBox::currentTextChanged, this, &PolarXanesWidget::onSelecAnalysisOrElement);

    _btn_export = new QPushButton("Export");
    _btn_export->setMaximumWidth(100);
    connect(_btn_export, &QPushButton::pressed, this, &PolarXanesWidget::onExportPressed);

    QHBoxLayout* hbox = new QHBoxLayout();
    hbox->addWidget(_cb_analysis);
    hbox->addWidget(_cb_elements);
    hbox->addWidget(_btn_export);

    QLayout* layout = new QVBoxLayout();
    layout->addItem(hbox);
    layout->addWidget(_spectra_widget);
    layout->setContentsMargins(0, 0, 0, 0);
    
    setLayout(layout);

}

//---------------------------------------------------------------------------

void PolarXanesWidget::onSelecAnalysisOrElement(QString val)
{
    _plot_new_element(_cb_analysis->currentText(), _cb_elements->currentText());
}

//---------------------------------------------------------------------------

void PolarXanesWidget::setModel(MapsH5Model* model)
{
    _model = model;
    if(model != nullptr)
    {
        _cb_analysis->clear();
        _cb_elements->clear();
        std::vector<std::string> analysis_types = _model->getAnalyzedTypes();
        QString current_a, current_e;
        std::string cur_a;
        bool first = true;
        for(auto& itr: analysis_types)
        {
            QString newVal = QString(itr.c_str());
            _cb_analysis->addItem(newVal);
            if (first)
            {
                current_a = newVal;
                cur_a = itr;
                first = false;
            }
        }
        data_struct::Fit_Count_Dict<float> element_counts;
        _model->getAnalyzedCounts(cur_a, element_counts);
        first = true;
        for(auto& itr: element_counts)
        {
            QString newVal = QString(itr.first.c_str());
            _cb_elements->addItem(newVal);
            if (first)
            {
                current_e = newVal;
                first = false;
            }
        }

        _plot_new_element(current_a, current_e);
    }
}

//---------------------------------------------------------------------------

void PolarXanesWidget::_plot_new_element(QString analysis_name, QString element_name)
{
    ArrayDr element_diff;
    ArrayDr element_avg;
    ArrayDr energy;
    _spectra_widget->clearAllSpectra();
    if( _generate_plot_axis(analysis_name, element_name, element_diff, element_avg, energy ) )
    {
        _spectra_widget->append_spectra("(LHCP/I0)-(RHCP/I0)", &element_diff, &energy, nullptr, true);
        _spectra_widget->append_spectra("((LHCP/I0)+(RHCP/I0)) / 2", &element_avg, &energy, nullptr, true);

        double main_min = std::min (element_diff.minCoeff() , element_avg.minCoeff());
        double main_max = std::max (element_diff.maxCoeff() , element_avg.maxCoeff());
        //need to check min max for nan

        _spectra_widget->setDisplayRange(energy.minCoeff(), energy.maxCoeff(), main_min, main_max);
    }
}

//---------------------------------------------------------------------------

bool PolarXanesWidget::_generate_plot_axis(QString analysis_name, QString element_name, ArrayDr &out_element_diff, ArrayDr &out_element_avg, ArrayDr &out_energy )
{
    if(_model != nullptr)
    {
        const std::map<std::string, data_struct::ArrayXXr<float>>* scalers = _model->getScalers();
        if(scalers != nullptr)
        {
            int us_ic_cnt = scalers->count(STR_US_IC);
            if (us_ic_cnt < 1)
            {
                logW<<"Missing ion chamber "<<STR_US_IC<<"\n";
            }
            int energy_cnt = scalers->count(STR_ENERGY);
            if (energy_cnt < 1)
            {
                logW<<"Missing energy arays "<<STR_ENERGY<<"\n";
            }
            if (us_ic_cnt > 0 && energy_cnt > 0)
            {
                data_struct::ArrayXXr<float> us_ic_map = scalers->at(STR_US_IC);
                data_struct::ArrayXXr<float> energy_map = scalers->at(STR_ENERGY);
                // 2 rows : 0 is lhcp , 1 rhcp
                if(analysis_name.length() > 0 && element_name.length() > 0)
                {
                    data_struct::Fit_Count_Dict<float> element_counts;
                    _model->getAnalyzedCounts(analysis_name.toStdString(), element_counts);
                    
                    data_struct::ArrayXXr<float> element_arr = element_counts.at(element_name.toStdString());

                    int incr = _model->get_polarity_pattern().length();
                    // test LRRL
                    //[0,] LL
                    //[1,] RR
                    // test LRRLLRRL
                    // [0,] LLLL
                    // [1,] RRRR
                    
                    if(incr > 0)
                    {
                        incr /= 2;
                        int arr_size = element_arr.cols() / incr; 
                        out_element_diff.resize(arr_size);
                        out_energy.resize(arr_size);
                        out_element_avg.resize(arr_size);
                        
                        for (int c = 0, idx = 0; c < element_arr.cols(); c+=incr, idx++)
                        {
                            if(c+incr <= element_arr.cols())
                            {
                                double sum_p = (static_cast<double>(element_arr(0,c)) / static_cast<double>(us_ic_map(0,c)) );
                                for(int i = 1; i < incr; i++)
                                {
                                    sum_p += (static_cast<double>(element_arr(0,c+i)) / static_cast<double>(us_ic_map(0,c+i)) );
                                }

                                double sum_n = (static_cast<double>(element_arr(1,c)) / static_cast<double>(us_ic_map(1,c)) );
                                for(int i = 1; i < incr; i++)
                                {
                                    sum_n += (static_cast<double>(element_arr(1,c+1)) / static_cast<double>(us_ic_map(1,c+1)) );
                                }

                                out_element_diff[idx] = sum_p - sum_n;
                                out_element_avg[idx] = (sum_p + sum_n)/ 2.0;                        

                                out_energy[idx] = static_cast<double>(energy_map(0,c));
                            }
                            else
                            {
                                if(idx > 0)
                                {
                                    out_element_diff[idx] = out_element_diff[idx - 1]; 
                                    out_element_avg[idx] = out_element_avg[idx - 1];
                                    out_energy[idx] = out_energy[idx - 1];
                                }
                            }
        
                        }
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
        }
    }
    return false;
}


//---------------------------------------------------------------------------

void PolarXanesWidget::onExportPressed()
{
    if (_model != nullptr)
    {
        QDir dir = _model->getDir();
        QFileInfo finfo(dir.absolutePath());

        QDateTime date = QDateTime::currentDateTime();
        QString formattedTime = date.toString("yyyy_MM_dd_hh_mm_ss");
        QByteArray formattedTimeMsg = formattedTime.toLocal8Bit();

        dir.cdUp();
        dir.cdUp();
        dir.cd("output");
        dir.mkdir(QString(STR_SCAN_TYPE_POLAR_XANES.c_str()));
        dir.cd(QString(STR_SCAN_TYPE_POLAR_XANES.c_str()));

        QString apath = dir.absolutePath() + QDir::separator() + finfo.fileName() + QString( _cb_analysis->currentText() + "_" + _cb_elements->currentText() + "_" + formattedTime);
        
        QPixmap pixmap = this->grab();
        QString png_path = QDir::cleanPath(apath + ".png");
        if (false == pixmap.save(png_path, "PNG"))
        {
            logE << "Could not save PNG for " << png_path.toStdString() << "\n";
        }
        
        QString csv_path = QDir::cleanPath(apath + ".csv");
        _exportPolarXanesCSV(csv_path);
        if (false == QDesktopServices::openUrl(QUrl::fromLocalFile(dir.absolutePath())))
        {
            logE << "Failed to open dir " << dir.absolutePath().toStdString() << "\n";
        }
    }
}

//---------------------------------------------------------------------------

void PolarXanesWidget::_exportPolarXanesCSV(QString filePath)
{
    ArrayDr element_diff;
    ArrayDr element_avg;
    ArrayDr energy;
    QString analysis_name = _cb_analysis->currentText();
    QString element_name = _cb_elements->currentText();
    if( _generate_plot_axis(analysis_name, element_name, element_diff, element_avg, energy ) )
    {
        std::ofstream out_stream(filePath.toStdString());
        if (out_stream.is_open())
        {
            out_stream << "ascii information for file: " << _model->getDatasetName().toStdString() << "\n";
            out_stream << "Analysis Type: " << analysis_name.toStdString() << "\n";
            out_stream << "Element Lines: " << element_name.toStdString() << "\n";
            out_stream << "Num Points: " << energy.size() << "\n";
            
            out_stream << "element_diff, element_avg, energy\n";
            for (int i = 0; i < energy.size(); i++)
            {
                out_stream << element_diff[i] << "," << element_avg[i] << "," << energy[i] << "\n";
            }
        }
        else
        {
            logE << "Could not open for writing plot CSV : " << filePath.toStdString() << "\n";
        }
        out_stream.close();
    }
}

//---------------------------------------------------------------------------