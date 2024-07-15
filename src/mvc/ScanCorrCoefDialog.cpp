/*-----------------------------------------------------------------------------
 * Copyright (c) 2023, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/ScanCorrCoefDialog.h>
#include <QDesktopServices>
#include "stats/correlation_coefficient.h"
#include "mvc/ScatterPlotView.h"

//---------------------------------------------------------------------------

ScanCorrCoefDialog::ScanCorrCoefDialog(QWidget *parent) : QWidget(parent)
{ 
    _running = false;
    _createLayout();
}

//---------------------------------------------------------------------------

ScanCorrCoefDialog::~ScanCorrCoefDialog()
{

 
}

//---------------------------------------------------------------------------

void ScanCorrCoefDialog::_createLayout()
{

    _progressBarFiles = new QProgressBar();
    _progressBarFiles->setRange(0, 100);

    _btn_run = new QPushButton("Scan");
    connect(_btn_run, &QPushButton::released, this, &ScanCorrCoefDialog::onRun);
    _btn_cancel = new QPushButton("Close");
    connect(_btn_cancel, &QPushButton::released, this, &ScanCorrCoefDialog::onClose);

    _processing_grp = new QGroupBox();
    QVBoxLayout* v_proc_layout = new QVBoxLayout();
    QHBoxLayout* h_layout = new QHBoxLayout();
    
    _le_restrict_coef = new QLineEdit();
    _le_restrict_coef->setValidator(new QDoubleValidator(0.0, 1.0, 3, this));
    _le_restrict_coef->setText("0.85");

    _ck_use_elements = new QCheckBox("Use Elements");
    _ck_use_elements->setChecked(true);

    _ck_use_scalers = new QCheckBox("Use Scalers");
    _ck_use_scalers->setChecked(false);

    _ck_use_rois = new QCheckBox("Use ROI's if available");
    _ck_use_rois->setChecked(false);

    QHBoxLayout* hbox = new QHBoxLayout();
    QLabel* lbl_Coef_Rest = new QLabel("Only save when Corr Coef is greater than:");
    hbox->addWidget(lbl_Coef_Rest);
    hbox->addWidget(_le_restrict_coef);

    v_proc_layout->addItem(h_layout);
    v_proc_layout->addItem(hbox);
    v_proc_layout->addWidget(_ck_use_elements);
    v_proc_layout->addWidget(_ck_use_scalers);
    //v_proc_layout->addWidget(_ck_use_rois);

    _processing_grp->setLayout(v_proc_layout);
    _processing_grp->setTitle("Scan Correlation Coefficient");

    QHBoxLayout* buttonlayout = new QHBoxLayout();
    buttonlayout->addWidget(_btn_run);
    buttonlayout->addWidget(_btn_cancel);

    QHBoxLayout* proc_save_layout = new QHBoxLayout();
    proc_save_layout->addWidget(_processing_grp);
    
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addItem(proc_save_layout);
    layout->addItem(buttonlayout);
    layout->addWidget(_progressBarFiles);
    
    setLayout(layout);
}

//---------------------------------------------------------------------------

void ScanCorrCoefDialog::setRunEnabled(bool val)
{
    _processing_grp->setEnabled(val);
    _btn_run->setEnabled(val); 
}

//---------------------------------------------------------------------------

void ScanCorrCoefDialog::setViewProps(bool blog10, bool bdark, bool bgrid)
{
    _blog10 = blog10;
    _bdark = bdark;
    _bgrid = bgrid;
}

//---------------------------------------------------------------------------

void ScanCorrCoefDialog::setModel(std::string analysis_type, MapsH5Model* model)
{
    _model = model; 
    _analysis_type = analysis_type;
}

//---------------------------------------------------------------------------

void ScanCorrCoefDialog::status_callback(size_t cur_block, size_t total_blocks)
{
    
    if (_total_blocks != total_blocks)
    {
        _total_blocks = total_blocks;
        _progressBarFiles->setRange(0, _total_blocks);
    }

    _progressBarFiles->setValue(cur_block);
    
    QCoreApplication::processEvents();
}

//---------------------------------------------------------------------------

double proc_corr_coef(MapsH5Model* model, std::string analysis_type, double restrict_coef, std::pair<std::string, std::string> name_pair)
{
    data_struct::ArrayXXr<float> x_map;
    data_struct::ArrayXXr<float> y_map;
    assert(model != nullptr);

    data_struct::Fit_Count_Dict<float> fit_counts;
    model->getAnalyzedCounts(analysis_type, fit_counts);
    std::map<std::string, data_struct::ArrayXXr<float>>* scalers = model->getScalers();

    if (fit_counts.count(name_pair.first) > 0)
    {
        x_map = fit_counts.at(name_pair.first);
    }
    else if (scalers->count(name_pair.first) > 0)
    {
        x_map = scalers->at(name_pair.first);
    }

    if (fit_counts.count(name_pair.second) > 0)
    {
        y_map = fit_counts.at(name_pair.second);
    }
    else if (scalers->count(name_pair.second) > 0)
    {
        y_map = scalers->at(name_pair.second);
    }

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
            n++;
        }
    }

    double corr_coef = find_coefficient(x_arr, y_arr);

    return corr_coef;
}

//---------------------------------------------------------------------------

void ScanCorrCoefDialog::onRun()
{
    //int foundCnt = 0;
    _btn_run->setEnabled(false);
    _running = true;
    QCoreApplication::processEvents();
    if (_model != nullptr)
    {
        bool use_elements = _ck_use_elements->isChecked();
        bool use_scalers = _ck_use_scalers->isChecked();
        //bool use_rois = _ck_use_rois->isChecked();

        std::vector<std::string> total_names;
        std::vector<std::pair<std::string, std::string>> el_pair_names;

        if (use_elements)
        {
            std::vector<std::string> count_names = _model->count_names();
            for (auto& itr : count_names)
            {
                total_names.push_back(itr);
            }
        }

        if (use_scalers)
        {
            std::map<std::string, data_struct::ArrayXXr<float>>* scalers = _model->getScalers();
            for (auto& itr : *scalers)
            {
                total_names.push_back(itr.first);
            }
        }

        double restrict_coef = _le_restrict_coef->text().toDouble();
        while (total_names.size() > 0)
        {
            int last = total_names.size() - 1;
            for (int i = 0; i < last; i++)
            {
                el_pair_names.push_back({ total_names[last], total_names[i] });
            }
            total_names.pop_back();
        }
        int total = el_pair_names.size();
        /*
        auto map_rois = _model->get_map_rois();
        if (_ck_use_rois->isChecked());
        {
            total *= map_rois.size();
        }
        */
        status_callback(0, total);

        ScatterPlotView* view = new ScatterPlotView(_blog10, _bdark, nullptr);
        view->setGridLinesVisible(_bgrid);
        view->setModel(_model);
        view->setAnalysisType(QString(_analysis_type.c_str()));
        int cur = 0;
        /*
        if (use_rois)
        {
            for (auto* roi_itr : map_rois)
            {
                for (int i = 0; i < el_pair_names.size(); i++)
                {
                    double val = proc_corr_coef(_model, _analysis_type, restrict_coef, el_pair_names[i]);
                    if (val > restrict_coef || val < -restrict_coef)
                    {
                        //logI << key << " : " << val << "\n";
                        view->setXYAxis(QString(roi_itr.first.c_str()), QString(el_pair_names[i].first.c_str()), QString(el_pair_names[i].second.c_str()));
                        view->resize(1920, 1080);
                        view->exportPngCsv();
                    }
                    cur++;
                    status_callback(cur, total);
                    QCoreApplication::processEvents();
                    if (false == _running)
                    {
                        break;
                    }
                }
            }
        }
        else
        {
        */
            for (int i = 0; i < el_pair_names.size(); i++)
            {
                double val = proc_corr_coef(_model, _analysis_type, restrict_coef, el_pair_names[i]);
                if (val > restrict_coef || val < -restrict_coef)
                {
                    //logI << key << " : " << val << "\n";
                    view->setXYAxis("", QString(el_pair_names[i].first.c_str()), QString(el_pair_names[i].second.c_str()));
                    view->resize(1920, 1080);
                    view->exportPngCsv();
                }
                cur++;
                status_callback(cur, total);
                QCoreApplication::processEvents();
                if (false == _running)
                {
                    break;
                }
            }
        //}
        delete view;

        QDir dir = _model->getDir();
        dir.cdUp();
        dir.cdUp();
        dir.cd("output");
        dir.mkdir(STR_SCATTER_PLOT);
        dir.cd(STR_SCATTER_PLOT);
        if (false == QDesktopServices::openUrl(QUrl::fromLocalFile(dir.absolutePath())))
        {
            logE << "Failed to open dir " << dir.absolutePath().toStdString() << "\n";
        }
    }
    _running = false;
    _btn_run->setEnabled(true);
    close();

}

//---------------------------------------------------------------------------

void ScanCorrCoefDialog::onClose()
{
    _running = false;
    close();
}

//---------------------------------------------------------------------------
