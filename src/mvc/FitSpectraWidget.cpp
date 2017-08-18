/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/FitSpectraWidget.h>


#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QDebug>

#include <math.h>

/*---------------------------------------------------------------------------*/

FitSpectraWidget::FitSpectraWidget(QWidget* parent) : QWidget(parent)
{

    _fit_thread = nullptr;
    createLayout();
/*
    _action_check_log10 = new QAction("Toggle Log10", this);
    _action_check_log10->setCheckable(true);
    _action_check_log10->setChecked(_display_log10);
    connect(_action_check_log10, SIGNAL(triggered()), this, SLOT(_check_log10()));

    _contextMenu = new QMenu(("Context menu"), this);
    _contextMenu->addAction(_action_check_log10);

    this->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(ShowContextMenu(const QPoint &)));
            */
}

/*---------------------------------------------------------------------------*/

FitSpectraWidget::~FitSpectraWidget()
{

}

/*---------------------------------------------------------------------------*/

void FitSpectraWidget::createLayout()
{
    std::vector<std::string> bound_types {"Not Initialized", "Fixed", "Limited Low High", "Limited Low", "Limited High", "Fit"};

    _spectra_widget = new SpectraWidget();

    _fit_params_table_model = new FitParamsTableModel();
    ComboBoxDelegate *cbDelegate = new ComboBoxDelegate(bound_types);

    _fit_params_table = new QTableView();
    _fit_params_table->setModel(_fit_params_table_model);
    _fit_params_table->sortByColumn(0, Qt::AscendingOrder);
    _fit_params_table->setItemDelegateForColumn(5, cbDelegate);

    QLayout* layout = new QVBoxLayout();
    layout->addWidget(_spectra_widget);
    layout->addWidget(_fit_params_table);
    setLayout(layout);
}

/*---------------------------------------------------------------------------*/

void FitSpectraWidget::setModels(data_struct::xrf::Fit_Parameters* fit_params,
                                    data_struct::xrf::Fit_Element_Map_Dict *elements_to_fit,
                                    MapsH5Model* h5_model)
{
    _h5_model = h5_model;
    _elements_to_fit = elements_to_fit;
    _fit_params = fit_params;

    _fit_params_table_model->setFitParams(_fit_params);

    _spectra_widget->append_spectra("Integrated Spectra", _h5_model->getIntegratedSpectra());
/*
    if(_fit_thread != nullptr)
    {
        _fit_thread->join();
        delete _fit_thread;
    }
    _fit_thread = new std::thread( [this]()
    {
    */
        if(_fit_params != nullptr && _elements_to_fit != nullptr)
        {
            data_struct::xrf::Spectra fit_spec = _h5_model->fit_integrated_spectra(*_fit_params, _elements_to_fit);
            for(int i=0; i<fit_spec.size(); i++)
            {
             if(fit_spec[i] <= 0.0)
                 fit_spec[i] = 0.1;
            }
            _spectra_widget->append_spectra("Fit Spectra", &fit_spec);
        }
   // });

}
