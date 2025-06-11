/*-----------------------------------------------------------------------------
 * Copyright (c) 2025, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <QVBoxLayout>
#include <mvc/PolarXanesWidget.h>

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

    QHBoxLayout* hbox = new QHBoxLayout();
    hbox->addWidget(_cb_analysis);
    hbox->addWidget(_cb_elements);

    QLayout* layout = new QVBoxLayout();
    layout->addItem(hbox);
    layout->addWidget(_spectra_widget);
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
    if(_model != nullptr)
    {
        _spectra_widget->clearAllSpectra();
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
                    ArrayDr lhcp_element;
                    ArrayDr lhcp_energy;
                    ArrayDr rhcp_element;
                    ArrayDr rhcp_energy;
                    lhcp_element.resize(element_arr.cols());
                    lhcp_energy.resize(element_arr.cols());
                    rhcp_element.resize(element_arr.cols());
                    rhcp_energy.resize(element_arr.cols());
                    for (int c = 0; c < element_arr.cols(); c++)
                    {
                        lhcp_element[c] = (static_cast<double>(element_arr(0,c)) / static_cast<double>(us_ic_map(0,c)) ) - (static_cast<double>(element_arr(1,c)) / static_cast<double>(us_ic_map(1,c)));
                        //logI<<lhcp_element[c]<<"\n";
                        //rhcp_element[c] = ;
                        lhcp_energy[c] = static_cast<double>(energy_map(0,c));
                        //rhcp_energy[c] = static_cast<double>(energy_map(1,c));
                    }
                    QString name = "(LHCP/I0)-(RHCP/I0)" + element_name;
                    _spectra_widget->append_spectra(name, &lhcp_element, &lhcp_energy, nullptr, true);
                    _spectra_widget->setDisplayRange(lhcp_energy.minCoeff(), lhcp_energy.maxCoeff(), lhcp_element.minCoeff(), lhcp_element.maxCoeff());

                    //name = "RHCP_" + element_name;
                    //_spectra_widget->append_spectra(name, &rhcp_element, &rhcp_energy);
                }
            }
        }
    }
}

//---------------------------------------------------------------------------