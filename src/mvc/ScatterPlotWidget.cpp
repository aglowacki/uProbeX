/*-----------------------------------------------------------------------------
 * Copyright (c) 2023, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/ScatterPlotWidget.h>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QDateTime>
#include <QDesktopServices>
#include <QCoreApplication>
#include "preferences/Preferences.h"
#include "stats/correlation_coefficient.h"

//---------------------------------------------------------------------------

ScatterPlotWidget::ScatterPlotWidget(QWidget* parent) : QWidget(parent)
{
    _createLayout();
}

/*---------------------------------------------------------------------------*/

ScatterPlotWidget::~ScatterPlotWidget()
{
    Preferences::inst()->setValue(STR_PRF_ScatterPlot_NumWindows, (int)_plot_view_list.size());
    Preferences::inst()->setValue(STR_PRF_ScatterPlot_Log10, _ck_display_log10->isChecked());
    Preferences::inst()->save();
    _plot_view_list.clear();
}

/*---------------------------------------------------------------------------*/

void ScatterPlotWidget::_createLayout()
{

    _subPlotLayout = new QHBoxLayout();

    // read pref to get number of windows
    int num_wins = Preferences::inst()->getValue(STR_PRF_ScatterPlot_NumWindows).toInt();
    bool _display_log10 = Preferences::inst()->getValue(STR_PRF_ScatterPlot_Log10).toBool();
    bool display_gird_lines = Preferences::inst()->getValue(STR_PRF_ScatterPlot_GridLines).toBool();
    bool dark_theme = Preferences::inst()->getValue(STR_PFR_SCATTER_DARK_BACKGROUND).toBool();
    
    _ck_display_log10 = new QCheckBox("Display log10");
    _ck_display_log10->setChecked(_display_log10);
    connect(_ck_display_log10, &QCheckBox::stateChanged, this, &ScatterPlotWidget::set_log10);

    _ck_display_grid_lines = new QCheckBox("Display Grid Lines");
    _ck_display_grid_lines->setChecked(display_gird_lines);
    connect(_ck_display_grid_lines, &QCheckBox::stateChanged, this, &ScatterPlotWidget::setGridLinesVisible);

    _ck_black_background = new QCheckBox("Black Background");
    _ck_black_background->setChecked(dark_theme);
    connect(_ck_black_background, &QCheckBox::stateChanged, this, &ScatterPlotWidget::setBlackBackground);

    /*
    _cb_shape = new QComboBox();
    _cb_shape->addItem("Circle");
    _cb_shape->addItem("Rectangle");
    connect(_cb_shape, qOverload<const QString&>(&QComboBox::currentIndexChanged), this, &ScatterPlotWidget::onShapeChange);
    */

    _btn_add = new QPushButton("Add Plot");
    connect(_btn_add, &QPushButton::released, this, &ScatterPlotWidget::onAdd);
    _btn_del = new QPushButton("Remove Plot");
    connect(_btn_del, &QPushButton::released, this, &ScatterPlotWidget::onDel);
    _btn_scan_corr_coef = new QPushButton("Scan Corr Coef");
    connect(_btn_scan_corr_coef, &QPushButton::released, this, &ScatterPlotWidget::onScan);
    _btn_save_png = new QPushButton("Save PNG");
    connect(_btn_save_png, &QPushButton::released, this, &ScatterPlotWidget::onSavePng);

    if (num_wins < 1)
    {
        num_wins = 1;
    }

    for (int i = 0; i < num_wins; i++)
    {
        _plot_view_list.push_back(new ScatterPlotView(_display_log10, dark_theme, nullptr));
        _subPlotLayout->addWidget(_plot_view_list[i]);
    }

    _sp_maker_size = new QSpinBox();
    _sp_maker_size->setPrefix("Marker Size:");
    _sp_maker_size->setRange(1, 100);
    _sp_maker_size->setSingleStep(1.0);
    _sp_maker_size->setValue(1);
    connect(_sp_maker_size, qOverload<int>(&QSpinBox::valueChanged), this, &ScatterPlotWidget::updateMarkerSize);


    _options_layout = new QHBoxLayout();
    _options_layout->addWidget(_btn_scan_corr_coef);
    _options_layout->addWidget(_ck_display_log10);
    _options_layout->addWidget(_ck_display_grid_lines);
    _options_layout->addWidget(_ck_black_background);
    _options_layout->addWidget(_sp_maker_size);
    //options_layout->addWidget(new QLabel("Marker Shape:"));
    //options_layout->addWidget(_cb_shape);
    _options_layout->addWidget(_btn_add);
    _options_layout->addWidget(_btn_del);
    _options_layout->addWidget(_btn_save_png);

 //   options_layout->addItem(new QSpacerItem(9999, 40, QSizePolicy::Maximum));

    _mainlayout = new QVBoxLayout();
    _mainlayout->addItem(_subPlotLayout);
    _mainlayout->addItem(_options_layout);


    setGridLinesVisible(Preferences::inst()->getValue(STR_PRF_ScatterPlot_GridLines).toBool());
    setBlackBackground(Preferences::inst()->getValue(STR_PFR_SCATTER_DARK_BACKGROUND).toBool());
    set_log10(Preferences::inst()->getValue(STR_PRF_ScatterPlot_Log10).toBool());
    updateMarkerSize(Preferences::inst()->getValue(STR_PRF_ScatterPlot_Size).toInt());
    onShapeChange(Preferences::inst()->getValue(STR_PFR_MARKER_SHAPE).toString());

    setLayout(_mainlayout);

}

//---------------------------------------------------------------------------

void ScatterPlotWidget::onAdd()
{
    bool _display_log10 = Preferences::inst()->getValue(STR_PRF_ScatterPlot_Log10).toBool();
    bool display_gird_lines = Preferences::inst()->getValue(STR_PRF_ScatterPlot_GridLines).toBool();
    bool dark_theme = Preferences::inst()->getValue(STR_PFR_SCATTER_DARK_BACKGROUND).toBool();
    
    _plot_view_list.push_back(new ScatterPlotView(_display_log10, dark_theme, this));
    int idx = _plot_view_list.size() - 1;
    _subPlotLayout->addWidget(_plot_view_list[idx]);
    _plot_view_list[idx]->setModel(_plot_view_list[idx - 1]->getModel());
    setGridLinesVisible(display_gird_lines);

    update();
    _plot_view_list[idx]->show();


    //updateMarkerSize(Preferences::inst()->getValue(STR_PRF_ScatterPlot_Size).toInt());
    //onShapeChange(Preferences::inst()->getValue(STR_PFR_MARKER_SHAPE).toString());

    Preferences::inst()->setValue(STR_PRF_ScatterPlot_NumWindows, (int)_plot_view_list.size());
}

//---------------------------------------------------------------------------

void ScatterPlotWidget::onDel()
{
    int amt = _plot_view_list.size();
    if (amt > 1)
    {
        _subPlotLayout->removeWidget(_plot_view_list[amt - 1]);
        _plot_view_list[amt - 1]->setParent(nullptr);
        delete _plot_view_list[amt - 1];
        _plot_view_list.pop_back();
        Preferences::inst()->setValue(STR_PRF_ScatterPlot_NumWindows, (int)_plot_view_list.size());
    }
}

//---------------------------------------------------------------------------

void ScatterPlotWidget::onShapeChange(QString val)
{
    if (val == "Circle")
    {
        for (auto& itr : _plot_view_list)
        {
            itr->setShape(QScatterSeries::MarkerShapeCircle);
        }
        Preferences::inst()->setValue(STR_PFR_MARKER_SHAPE, "Circle");
    }
    else
    {
        for (auto& itr : _plot_view_list)
        {
            itr->setShape(QScatterSeries::MarkerShapeRectangle);
        }
        Preferences::inst()->setValue(STR_PFR_MARKER_SHAPE, "Rectangle");
    }

}

//---------------------------------------------------------------------------

void ScatterPlotWidget::setBlackBackground(int val)
{
    for (auto& itr : _plot_view_list)
    {
        itr->setBlackBackground(val);
    }

    Preferences::inst()->setValue(STR_PFR_SCATTER_DARK_BACKGROUND, val);
}

//---------------------------------------------------------------------------

void ScatterPlotWidget::updateMarkerSize(int val)
{
    val = std::min(val, 1);
    for (auto& itr : _plot_view_list)
    {
        itr->updateMarkerSize(val);
    }

    Preferences::inst()->setValue(STR_PRF_ScatterPlot_Size, val);
}

//---------------------------------------------------------------------------

void ScatterPlotWidget::setAnalysisType(QString name)
{
    for (auto& itr : _plot_view_list)
    {
        itr->setAnalysisType(name);
    }
}

//---------------------------------------------------------------------------

void ScatterPlotWidget::setQuantType(QString name)
{
    for (auto& itr : _plot_view_list)
    {
        itr->setQuantType(name);
    }
}

//---------------------------------------------------------------------------

void ScatterPlotWidget::setModel(MapsH5Model* model)
{
    if (model != nullptr)
    {
        for (auto& itr : _plot_view_list)
        {
            itr->setModel(model);
        }
    }
}

//---------------------------------------------------------------------------

void ScatterPlotWidget::set_log10(int val)
{

    Preferences::inst()->setValue(STR_PRF_ScatterPlot_Log10, _ck_display_log10->isChecked());
    for (auto& itr : _plot_view_list)
    {
        itr->setLog10(val);
    }

    Preferences::inst()->setValue(STR_PRF_ScatterPlot_Log10, val);
}

//---------------------------------------------------------------------------

void ScatterPlotWidget::setGridLinesVisible(int val)
{
    Preferences::inst()->setValue(STR_PRF_ScatterPlot_GridLines, _ck_display_grid_lines->isChecked());
    for (auto& itr : _plot_view_list)
    {
        itr->setGridLinesVisible(val);
    }
}

//---------------------------------------------------------------------------

void ScatterPlotWidget::onSavePng()
{
    for (auto& itr : _plot_view_list)
    {
        itr->exportPngCsv();
    }
}

//---------------------------------------------------------------------------

void ScatterPlotWidget::onScan()
{
    _scan_corr_coef_dialog.setModel(_plot_view_list[0]->getAnalysisType().toStdString(), _plot_view_list[0]->getModel());
    _scan_corr_coef_dialog.setViewProps(_ck_display_log10->isChecked(), _ck_black_background->isChecked(), _ck_display_grid_lines->isChecked());
    _scan_corr_coef_dialog.show();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/