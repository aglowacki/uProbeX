/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/MapsH5Model.h>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

/*----------------src/mvc/MapsH5Model.cpp \-----------------------------------------------------------*/

std::mutex MapsH5Model::_mutex;

MapsH5Model::MapsH5Model() : QObject()
{
    _scan_type_as_str = "";
    _filepath = "";
    _is_fully_loaded = false;
    _version = 0.0f;
    _loaded_scalers = false;
    _loaded_quantification = false;
    _loaded_scan = false;
    _loaded_integrated_spectra = false;
    _loaded_counts = false;
    _params_override = nullptr;
    _initialized_by_stream_block = false;
    _requested_rows = 0;
    _requested_cols = 0;

}

//---------------------------------------------------------------------------

MapsH5Model::~MapsH5Model()
{

    clear_analyzed_counts();

}

//---------------------------------------------------------------------------

void MapsH5Model::clear_analyzed_counts()
{

    for (auto itr : _analyzed_counts)
    {
        delete itr.second;
    }
    _analyzed_counts.clear();

}

//---------------------------------------------------------------------------

const std::map<std::string, data_struct::ArrayXXr<float>>* MapsH5Model::getScalers() const 
{
     return &_scalers; 
}

//---------------------------------------------------------------------------

bool MapsH5Model::getAnalyzedCounts(const std::string& analysis_type, data_struct::Fit_Count_Dict<float>& out_counts) const 
{
    std::lock_guard<std::mutex> lock(_mutex);
    out_counts.clear();

    if(analysis_type.length() > 0 && _analyzed_counts.count(analysis_type) > 0)
    {
        for (const auto& itr : *(_analyzed_counts.at(analysis_type)))
        {
            out_counts[itr.first] = itr.second;
        }
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------

bool MapsH5Model::getCountsOrScaler(const std::string& analysis_type, const std::string name, ArrayXXr<float>* out_counts) const 
{
    std::lock_guard<std::mutex> lock(_mutex);

    if(analysis_type.length() > 0 && _analyzed_counts.count(analysis_type) > 0)
    {
        if(_analyzed_counts.at(analysis_type)->count(name) > 0)
        {
            *out_counts = _analyzed_counts.at(analysis_type)->at(name);
            return true;
        }
    }
    if(_scalers.count(name) > 0)
    {
        *out_counts = _scalers.at(name);
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------

bool MapsH5Model::getAnalyzedQuantified(const std::string& analysis_type, const std::string& quant_type, data_struct::Fit_Count_Dict<float>& out_counts)
{
    std::lock_guard<std::mutex> lock(_mutex);
    out_counts.clear();
    std::unordered_map<std::string, Calibration_curve<double> > *calib_map = nullptr;
    if(analysis_type.length() > 0 && quant_type.length() > 0 && _analyzed_counts.count(analysis_type) > 0 && _scalers.count(quant_type) > 0)
    {
        if (analysis_type == STR_FIT_NNLS)
        {
            calib_map = &_quant_map_nnls;
        }
        else if(analysis_type == STR_FIT_GAUSS_MATRIX)
        {
            calib_map = &_quant_map_matrix;
        }
        else
        {
            return false;
        }
        if(calib_map->count(quant_type) > 0)
        {
            // get scaler quant type
            auto calib = calib_map->at(quant_type).calib_curve;
            auto quant_scaler = _scalers.at(quant_type);
            for (const auto& itr : *(_analyzed_counts[analysis_type]))
            {
                if(calib.count(itr.first) > 0)
                {
                    out_counts[itr.first] = itr.second / quant_scaler/ calib.at(itr.first);
                    float min_coef = out_counts.at(itr.first).minCoeff();
                    out_counts[itr.first] = out_counts.at(itr.first).unaryExpr([min_coef](float v) { return std::isfinite(v) ? v : min_coef; });
                }
                else
                {
                    out_counts[itr.first] = itr.second;  
                }
            }
            return true;
        }
        return false;
    }
    return false;
}

//---------------------------------------------------------------------------

std::vector<std::string> MapsH5Model::count_names()
{
    std::lock_guard<std::mutex> lock(_mutex);
    std::vector<std::string> count_names;

    for(auto& itr1 : _analyzed_counts)
    {
        for(auto& itr2 : *(itr1.second)) //fit counts
        {
            count_names.push_back(itr2.first);
        }
        break;//we only want one iteration
    }

    return count_names;
}

//---------------------------------------------------------------------------

std::vector<std::string> MapsH5Model::getAnalyzedTypes()
{
    std::lock_guard<std::mutex> lock(_mutex);
    std::vector<std::string> keys;
    for(auto &itr : _analyzed_counts)
    {
        keys.push_back(itr.first);
    }
    return keys;
}

//---------------------------------------------------------------------------

std::string MapsH5Model::_analysis_enum_to_str(data_struct::Fitting_Routines val)
{
    if(val == data_struct::Fitting_Routines::ROI)
    {
        return "ROI";
    }
    else if(val == data_struct::Fitting_Routines::GAUSS_TAILS)
    {
        return "Parameter Fitting";
    }
    else if(val == data_struct::Fitting_Routines::GAUSS_MATRIX)
    {
        return "Matrix Fitting";
    }
    else if(val == data_struct::Fitting_Routines::SVD)
    {
        return "SVD";
    }
    else if(val == data_struct::Fitting_Routines::NNLS)
    {
        return "NNLS";
    }
    return "";
}

//---------------------------------------------------------------------------

void MapsH5Model::set_fit_parameters_override(data_struct::Params_Override<double>* override)
{
    _params_override = override;
}

//---------------------------------------------------------------------------

void MapsH5Model::initialize_from_stream_block(data_struct::Stream_Block<float>* block)
{
    clear_analyzed_counts();

    _filepath = QString(block->dataset_directory->c_str());
    _datset_name = QString(block->dataset_name->c_str());

    if(block->spectra != nullptr)
    {
        _integrated_spectra.resize(block->spectra->size());
        _integrated_spectra.setZero(block->spectra->size());
        _integrated_spectra.add( block->spectra );
        _loaded_integrated_spectra = true;
        //emit model_int_spec_updated(false);
    }

    for(auto& itr : block->fitting_blocks)
    {
        std::string group_name = _analysis_enum_to_str(itr.first);

        data_struct::Fit_Count_Dict<float>* xrf_counts = new data_struct::Fit_Count_Dict<float>();
        _analyzed_counts.insert( {group_name, xrf_counts} );

        for(auto& itr2 : itr.second.fit_counts)
        {
            xrf_counts->emplace(std::pair<std::string,data_struct::ArrayXXr<float>>(itr2.first, data_struct::ArrayXXr<float>() ));
            xrf_counts->at(itr2.first).resize(block->height(), block->width());
            xrf_counts->at(itr2.first).setZero(block->height(), block->width());
            if((block->height() > block->row()) && (block->width() > block->col())) 
            { 
                xrf_counts->at(itr2.first)(block->row(), block->col()) = itr2.second;
            }
        }
    }
    emit model_data_updated();
}

//---------------------------------------------------------------------------

void MapsH5Model::clearAllMapRois()
{
    _map_rois.clear();
}

//---------------------------------------------------------------------------

void MapsH5Model::appendMapRoi(std::string name, struct Map_ROI roi)
{
    _map_rois[name] = roi;
}

//---------------------------------------------------------------------------

void MapsH5Model::saveAllRoiMaps(QString savename)
{
    QJsonObject rootJson;

    QJsonArray json_rois;
    for (auto& itr : _map_rois)
    {
        QJsonObject json_roi_object;
        int icolor = int(itr.second.color.rgba());
        json_roi_object[STR_MAP_ROI_NAME.c_str()] = itr.first.c_str();
        json_roi_object[STR_MAP_ROI_COLOR.c_str()] = icolor;
        json_roi_object[STR_MAP_ROI_COLOR_ALPHA.c_str()] = itr.second.color_alpha;

        // save pixel locations
        QJsonArray json_roi_pixels;
        for (auto& pItr : itr.second.pixel_list)
        {
            QJsonArray pixelPair;
            pixelPair.append(pItr.first);
            pixelPair.append(pItr.second);

            json_roi_pixels.append(pixelPair);
        }
        json_roi_object[STR_MAP_ROI_PIXEL_LOC.c_str()] = json_roi_pixels;

        // save int spectra
        QJsonArray json_arr_specs;

        QJsonObject json_int_spec;

        for (const auto &spec_itr : itr.second.int_spec)
        {
            QJsonArray json_spec;
            for (auto& iItr : spec_itr.second)
            {
                json_spec.append(iItr);
            }
            json_int_spec[STR_MAP_ROI_INT_SPEC_FILENAME.c_str()] = spec_itr.first.c_str();
            json_int_spec[STR_SPECTRA.c_str()] = json_spec;
            json_int_spec[STR_ELT.c_str()] = spec_itr.second.elapsed_livetime();
            json_int_spec[STR_ERT.c_str()] = spec_itr.second.elapsed_realtime();
            json_int_spec[STR_ICR.c_str()] = spec_itr.second.input_counts();
            json_int_spec[STR_OCR.c_str()] = spec_itr.second.output_counts();

            json_arr_specs.append(json_int_spec);
        }
        json_roi_object[STR_MAP_ROI_INT_SPEC.c_str()] = json_arr_specs;
        json_rois.append(json_roi_object);
    }

    rootJson[STR_MAPS_ROIS.c_str()] = json_rois;
    QByteArray save_data = QJsonDocument(rootJson).toJson();

    if (_map_rois.size() > 0)
    {
        QFile saveFile(savename);
        if (!saveFile.open(QIODevice::WriteOnly))
        {
            logW << "Couldn't open save file: " << savename.toStdString();
        }
        else
        {
            saveFile.write(save_data);
            saveFile.close();
        }
    }
}

//---------------------------------------------------------------------------

void MapsH5Model::saveAllRoiMaps()
{
    QDir model_dir = _dir;
    model_dir.cdUp();
    model_dir.cdUp();
    if (false == model_dir.cd(STR_MAPS_ROIS_DIR_NAME.c_str()))
    {
        model_dir.mkdir(STR_MAPS_ROIS_DIR_NAME.c_str());
        model_dir.cd(STR_MAPS_ROIS_DIR_NAME.c_str());
    }
    QFileInfo finfo(_dir.absolutePath());
    QString roi_file_name = model_dir.absolutePath() + QDir::separator() + finfo.baseName() + ".r0i";

    saveAllRoiMaps(roi_file_name);
}

//---------------------------------------------------------------------------

void MapsH5Model::loadAllRoiMaps()
{

    bool resave = false;
    QDir model_dir = _dir;
    model_dir.cdUp();
    model_dir.cdUp();
    if (false == model_dir.cd(STR_MAPS_ROIS_DIR_NAME.c_str()))
    {
        // no rois folder means no rois to load
        return;
    }

    QFileInfo finfo(_dir.absolutePath());
    QString roi_file_name = model_dir.absolutePath() + QDir::separator() + finfo.baseName() + ".r0i";

    QFile roiFile(roi_file_name);
    if (!roiFile.open(QIODevice::ReadOnly))
    {
        //logW << "Couldn't open save file: " << roi_file_name.toStdString();
        return;
    }

    _map_rois.clear();

    QJsonObject rootJson = QJsonDocument::fromJson(roiFile.readAll()).object();
    roiFile.close();

    if (rootJson.contains(STR_MAPS_ROIS.c_str()) && rootJson[STR_MAPS_ROIS.c_str()].isArray())
    {
        bool int_spec_loaded = false;
        QJsonArray rois = rootJson[STR_MAPS_ROIS.c_str()].toArray();
        for (int i = 0; i < rois.size(); ++i)
        {
            QJsonObject jsonRoi = rois[i].toObject();

            Map_ROI mroi;
            mroi.name = jsonRoi.value(STR_MAP_ROI_NAME.c_str()).toString().toStdString();
            mroi.color = QColor::fromRgba(QRgb(jsonRoi.value(STR_MAP_ROI_COLOR.c_str()).toInt()));
            mroi.color_alpha = jsonRoi.value(STR_MAP_ROI_COLOR_ALPHA.c_str()).toInt();
            QJsonArray jsonPixels = jsonRoi[STR_MAP_ROI_PIXEL_LOC.c_str()].toArray();
            for (int j = 0; j < jsonPixels.size(); ++j)
            {
                QJsonArray pixelPair = jsonPixels[j].toArray();
                std::pair<int, int> ppair;
                ppair.first = pixelPair[0].toInt();
                ppair.second = pixelPair[1].toInt();
                mroi.pixel_list.push_back(ppair);
            }
            // load array of int spectras
            if (jsonRoi.contains(STR_MAP_ROI_INT_SPEC.c_str()) && jsonRoi[STR_MAP_ROI_INT_SPEC.c_str()].isArray()) // if we have the int spec for this file
            {
                QJsonArray jsonIntSpecArr = jsonRoi[STR_MAP_ROI_INT_SPEC.c_str()].toArray();
                for (int m = 0; m < jsonIntSpecArr.size(); ++m)
                {
                    QJsonObject json_int_spec = jsonIntSpecArr[m].toObject();
                    if (json_int_spec.contains(STR_MAP_ROI_INT_SPEC_FILENAME.c_str()))
                    {
                        QString filename = json_int_spec[STR_MAP_ROI_INT_SPEC_FILENAME.c_str()].toString();
                        std::string stdFilename = filename.toStdString();
                        // if we found our int spectra then load it
                        if (filename == finfo.fileName())
                        {
                            QJsonArray json_spec_arr = json_int_spec[STR_SPECTRA.c_str()].toArray();
                            mroi.int_spec[stdFilename].resize(json_spec_arr.size());
                            for (int n = 0; n < json_spec_arr.size(); ++n)
                            {
                                mroi.int_spec[stdFilename](n) = json_spec_arr[n].toInt();
                            }
                            mroi.int_spec[stdFilename].elapsed_livetime(json_int_spec[STR_ELT.c_str()].toDouble());
                            mroi.int_spec[stdFilename].elapsed_realtime(json_int_spec[STR_ERT.c_str()].toDouble());
                            mroi.int_spec[stdFilename].input_counts(json_int_spec[STR_ICR.c_str()].toDouble());
                            mroi.int_spec[stdFilename].output_counts(json_int_spec[STR_OCR.c_str()].toDouble());
                            int_spec_loaded = true;
                        }
                    }
                }
            }
            else
            {
                int_spec_loaded = false;
            }

            if (false == int_spec_loaded)
            {
                std::string stdFileName = finfo.fileName().toStdString();
                // load it 
                if (io::file::HDF5_IO::inst()->load_integrated_spectra_analyzed_h5_roi(_dir.absolutePath().toStdString(), &(mroi.int_spec[stdFileName]), mroi.pixel_list))
                {
                    QJsonArray jsonIntSpecArr = jsonRoi[STR_MAP_ROI_INT_SPEC.c_str()].toArray();

                    QJsonObject json_int_spec;
                    json_int_spec[STR_MAP_ROI_INT_SPEC_FILENAME.c_str()] = finfo.fileName();

                    QJsonArray json_spec;
                    for (auto& iItr : mroi.int_spec[stdFileName])
                    {
                        json_spec.append(iItr);
                    }
                    json_int_spec[STR_SPECTRA.c_str()] = json_spec;
                    json_int_spec[STR_ELT.c_str()] = mroi.int_spec[stdFileName].elapsed_livetime();
                    json_int_spec[STR_ERT.c_str()] = mroi.int_spec[stdFileName].elapsed_realtime();
                    json_int_spec[STR_ICR.c_str()] = mroi.int_spec[stdFileName].input_counts();
                    json_int_spec[STR_OCR.c_str()] = mroi.int_spec[stdFileName].output_counts();

                    jsonIntSpecArr.append(json_int_spec);
                    jsonRoi[STR_MAP_ROI_INT_SPEC.c_str()] = jsonIntSpecArr;
                    rois[i] = jsonRoi;
                    resave = true;
                }
                else
                {
                    logW << "Failed to load roi spectra for " << _dir.absolutePath().toStdString() << "\n";
                }
            }
            _map_rois[mroi.name] = mroi;
            logI<< "Loading roi: "<< mroi.name<<"\n";
        }
        if (resave)
        {
            // save to r0i file
            if (!roiFile.open(QIODevice::WriteOnly))
            {
                logW << "Couldn't open save file: " << roi_file_name.toStdString();
            }
            rootJson[STR_MAPS_ROIS.c_str()] = rois;
            QByteArray save_data = QJsonDocument(rootJson).toJson();
            roiFile.write(save_data);
            roiFile.close();
        }
    }
}

//---------------------------------------------------------------------------

void MapsH5Model::update_from_stream_block(data_struct::Stream_Block<float>* block)
{
    if(block->spectra != nullptr)
    {
        _integrated_spectra.add( block->spectra );
        //wait until the FitSpectraWidget has a thread to update int spec
        //emit model_int_spec_updated(false);
    }

    if(_analyzed_counts.size() < 1)
    {
        initialize_from_stream_block(block);
    }
    else
    {
        for(auto& itr : block->fitting_blocks)
        {
            std::string group_name = _analysis_enum_to_str(itr.first);
            if(_analyzed_counts.count(group_name) > 0)
            {
                data_struct::Fit_Count_Dict<float>* xrf_counts = _analyzed_counts[group_name];
                if(xrf_counts != nullptr)
                {
                    for(auto& itr2 : itr.second.fit_counts)
                    {
                        if (xrf_counts->count(itr2.first) < 1)
                        {
                            initialize_from_stream_block(block);
                            break;
                        }
                        if((xrf_counts->at(itr2.first).rows() > block->row()) && (xrf_counts->at(itr2.first).cols() > block->col()))
                        {
                            if (std::isfinite(itr2.second))
                            {
                                std::lock_guard<std::mutex> lock(_mutex);
                                xrf_counts->at(itr2.first)(block->row(), block->col()) = itr2.second;
                            }
                            else
                            {
                                std::lock_guard<std::mutex> lock(_mutex);
                                xrf_counts->at(itr2.first)(block->row(), block->col()) = 0.0;
                            }
                        }
                    }
                }
            }
            else
            {
                initialize_from_stream_block(block);
            }
        }
    }
}

//---------------------------------------------------------------------------

void MapsH5Model::getIntegratedSpectra(data_struct::Spectra<double>& out_spectra)
{
    std::lock_guard<std::mutex> lock(_mutex);
    out_spectra = _integrated_spectra;
}

//---------------------------------------------------------------------------

bool MapsH5Model::load_x_y_motors_only(QString filepath, data_struct::ArrayTr<float> &x_arr, data_struct::ArrayTr<float> &y_arr)
{
    std::string x_axis_loc_9 = "/MAPS/x_axis";
    std::string x_axis_loc_10 = "/MAPS/Scan/x_axis";
    std::string y_axis_loc_9 = "/MAPS/y_axis";
    std::string y_axis_loc_10 = "/MAPS/Scan/y_axis";
    hid_t file_id, x_id, y_id;
    file_id = H5Fopen(filepath.toStdString().c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
    if(file_id < 0)
    {
        logW<<"Error opening file "<<filepath.toStdString() << "\n";
        return false;
    }

    x_id = H5Dopen(file_id, x_axis_loc_10.c_str(), H5P_DEFAULT);
    if(x_id < 0)
    {
        logW<<"Error opening"<<x_axis_loc_10.c_str()<<"\n";
        x_id = H5Dopen(file_id, x_axis_loc_9.c_str(), H5P_DEFAULT);
        if(x_id < 0)
        {
            H5Fclose(file_id);
            logW<<"Error opening"<<x_axis_loc_9.c_str()<<"\n";
            return false;
        }
    }

    y_id = H5Dopen(file_id, y_axis_loc_10.c_str(), H5P_DEFAULT);
    if(x_id < 0)
    {
        logW<<"Error opening"<<y_axis_loc_10.c_str()<<"\n";
        y_id = H5Dopen(file_id, y_axis_loc_9.c_str(), H5P_DEFAULT);
        if(y_id < 0)
        {
            H5Fclose(file_id);
            logW<<"Error opening"<<y_axis_loc_9.c_str()<<"\n";
            return false;
        }
    }

    if (x_id > -1 && y_id > -1)
    {
        hid_t x_space_id = H5Dget_space(x_id);
        hid_t y_space_id = H5Dget_space(y_id);
        
        hsize_t x_dims_in[1] = { 0 };
        hsize_t y_dims_in[1] = { 0 };
        int xstatus_n = H5Sget_simple_extent_dims(x_space_id, &x_dims_in[0], nullptr);
        int ystatus_n = H5Sget_simple_extent_dims(y_space_id, &y_dims_in[0], nullptr);
        if (xstatus_n > -1 && ystatus_n > -1)
        {
            x_arr.resize(x_dims_in[0]);
            hid_t error = H5Dread(x_id, H5T_NATIVE_FLOAT, x_space_id, x_space_id, H5P_DEFAULT, x_arr.data());
            if (error > 0)
            {
                logW << "Could not load x_axis\n";
            }
            y_arr.resize(y_dims_in[0]);
            error = H5Dread(y_id, H5T_NATIVE_FLOAT, y_space_id, y_space_id, H5P_DEFAULT, y_arr.data());
            if (error > 0)
            {
                logW << "Could not load y_axis\n";
            }
        }

        if (x_space_id > -1)
        {
            H5Sclose(x_space_id);
        }
        if (y_space_id > -1)
        {
            H5Sclose(y_space_id);
        }

        H5Dclose(x_id);
        H5Dclose(y_id);

    }

    H5Fclose(file_id);
    return true;
}

//---------------------------------------------------------------------------

bool MapsH5Model::load(QString filepath)
{
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    try
    {
        _filepath = filepath;
        _dir = QDir(filepath);
        _datset_name = QFileInfo(filepath).fileName();

        hid_t    file_id, maps_grp_id, analyzed_grp_id;
        
        file_id = H5Fopen(filepath.toStdString().c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
        if(file_id < 0)
        {
            logW<<"Error opening file "<<filepath.toStdString() << "\n";
            return false;
        }

        maps_grp_id = H5Gopen(file_id, "/MAPS", H5P_DEFAULT);
        if(maps_grp_id < 0)
        {
            H5Fclose(file_id);
            logW<<"Error opening group /MAPS";
            return false;
        }

        analyzed_grp_id = H5Gopen(maps_grp_id, "XRF_Analyzed", H5P_DEFAULT);
        if (analyzed_grp_id < 0)
        {
            _version = 9.0;
            logI << "/MAPS/XRF_Analyzed not found. Attempting to open V9 layout";
        }
        else
        {
            _version = 10.0;
        }

        
        if(_version < 10.0)
        {
            _is_fully_loaded = _load_version_9(maps_grp_id);
        }
        else
        {
            _is_fully_loaded = _load_version_10(file_id, maps_grp_id);
        }

        H5Gclose(maps_grp_id);
        
        // close all objects
        if (file_id > 0)
        {
            H5Fflush(file_id, H5F_SCOPE_LOCAL);

            ssize_t obj_cnt = H5Fget_obj_count(file_id, H5F_OBJ_DATASET | H5F_OBJ_LOCAL);
            if (obj_cnt > 0)
            {
                logI << "closing forgotten datasets: " << obj_cnt << "\n";
                hid_t* objs = new hid_t[obj_cnt];
                if (H5Fget_obj_ids(file_id, H5F_OBJ_DATASET, -1, objs) > -1)
                {
                    for (int i = 0; i < obj_cnt; i++)
                    {
                        H5Dclose(objs[i]);
                    }
                }
                delete[] objs;
            }
            obj_cnt = H5Fget_obj_count(file_id, H5F_OBJ_GROUP | H5F_OBJ_LOCAL);
            if (obj_cnt > 0)
            {
                logI << "closing forgotten groups: " << obj_cnt << "\n";
                hid_t* objs = new hid_t[obj_cnt];
                if (H5Fget_obj_ids(file_id, H5F_OBJ_GROUP, -1, objs) > -1)
                {
                    for (int i = 0; i < obj_cnt; i++)
                    {
                        H5Gclose(objs[i]);
                    }
                }
                delete[] objs;
            }
            obj_cnt = H5Fget_obj_count(file_id, H5F_OBJ_DATATYPE | H5F_OBJ_LOCAL);
            if (obj_cnt > 0)
            {
                logI << "closing forgotten datatypes: " << obj_cnt << "\n";
                hid_t* objs = new hid_t[obj_cnt];
                if (H5Fget_obj_ids(file_id, H5F_OBJ_DATATYPE, -1, objs) > -1)
                {
                    for (int i = 0; i < obj_cnt; i++)
                    {
                        H5Tclose(objs[i]);
                    }
                }
                delete[] objs;
            }
            obj_cnt = H5Fget_obj_count(file_id, H5F_OBJ_ATTR | H5F_OBJ_LOCAL);
            if (obj_cnt > 0)
            {
                logI << "closing forgotten attributes: " << obj_cnt << "\n";
                hid_t* objs = new hid_t[obj_cnt];
                if (H5Fget_obj_ids(file_id, H5F_OBJ_ATTR, -1, objs) > -1)
                {
                    for (int i = 0; i < obj_cnt; i++)
                    {
                        H5Aclose(objs[i]);
                    }
                }
                delete[] objs;
            }
            obj_cnt = H5Fget_obj_count(file_id, H5F_OBJ_ALL | H5F_OBJ_LOCAL);
            if (obj_cnt > 1) //file is still open
            {
                logI << "**** did not close total objects " << obj_cnt << "\n";
            }


            H5Fclose(file_id);
            file_id = -1;
        }
        else
        {
            logW << " could not close file because none is open" << "\n";
        }
    }
    catch (std::string& s)
    {
        throw s;
    }
    catch (...)
    {
        throw std::string("Failed to open Maps Analyzed dataset!");
    }

    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    logI << "load elapsed time: " << elapsed_seconds.count() << "s\n";
       
    return _is_fully_loaded;
}

//---------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------Version 10---------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------

bool MapsH5Model::_load_version_9(hid_t maps_grp_id)
{

    _loaded_quantification = _load_quantification_9(maps_grp_id);

    _loaded_scalers = _load_scalers_9(maps_grp_id);

    _loaded_scan = _load_scan_9(maps_grp_id);

    _loaded_integrated_spectra = _load_integrated_spectra_9(maps_grp_id);

    _loaded_counts = _load_counts_9(maps_grp_id);

    return (_loaded_quantification && _loaded_scalers && _loaded_scan && _loaded_integrated_spectra &&_loaded_counts);

}

//---------------------------------------------------------------------------

bool MapsH5Model::_load_quantification_9_single(hid_t maps_grp_id, std::string path, std::unordered_map<std::string, Calibration_curve<double> >&quant)
{
    hid_t dset_id, channels_dset_id, dspace_id, channels_dspace_id;
    hid_t memoryspace_id, memoryspace_name_id, error;
    hsize_t offset[3] = { 0,0,0 };
    hsize_t count[3] = { 1,1,1 };
    hsize_t offset_name[1] = { 0 };
    hsize_t count_name[1] = { 1 };
    char tmp_name[255] = { 0 };
    hid_t   filetype, memtype, status;
    double values[3] = { 0., 0., 0. };

    dset_id = H5Dopen(maps_grp_id, path.c_str(), H5P_DEFAULT);
    if (dset_id < 0)
    {
        logW << "Error opening group /MAPS/"<<path<<"\n";
        return false;
    }
    dspace_id = H5Dget_space(dset_id);

    channels_dset_id = H5Dopen(maps_grp_id, "channel_names", H5P_DEFAULT);
    if (channels_dset_id < 0)
    {
        logW << "Error opening group /MAPS/channel_names\n";
        return false;
    }
    channels_dspace_id = H5Dget_space(channels_dset_id);

    int rank = H5Sget_simple_extent_ndims(dspace_id);
    if (rank != 3)
    {
        logW << path << " rank is not equal to 3, unknown format!\n";
        return false;
    }
    hsize_t* dims_out = new hsize_t[rank];
    unsigned int status_n = H5Sget_simple_extent_dims(dspace_id, &dims_out[0], nullptr);

    filetype = H5Tcopy(H5T_C_S1);
    H5Tset_size(filetype, 256);
    memtype = H5Tcopy(H5T_C_S1);
    status = H5Tset_size(memtype, 255);

    for (int i = 0; i < 3; i++)
    {
        offset[i] = 0;
        count[i] = dims_out[i];
    }

    count[2] = 1;

    memoryspace_id = H5Screate_simple(3, count, nullptr);
    memoryspace_name_id = H5Screate_simple(1, count_name, nullptr);
    H5Sselect_hyperslab(memoryspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
    H5Sselect_hyperslab(memoryspace_name_id, H5S_SELECT_SET, offset_name, nullptr, count_name, nullptr);

    Calibration_curve<double> sr_current_curve(STR_SR_CURRENT);
    Calibration_curve<double> us_ic_curve(STR_US_IC);
    Calibration_curve<double> ds_ic_curve(STR_DS_IC);

    for (hsize_t el_idx = 0; el_idx < dims_out[2]; el_idx++)
    {
        offset[2] = el_idx;
        offset_name[0] = el_idx;
        memset(&tmp_name[0], 0, 254);
        H5Sselect_hyperslab(channels_dspace_id, H5S_SELECT_SET, offset_name, nullptr, count_name, nullptr);
        error = H5Dread(channels_dset_id, memtype, memoryspace_name_id, channels_dspace_id, H5P_DEFAULT, (void*)&tmp_name[0]);
        std::string el_name = std::string(tmp_name);
        H5Sselect_hyperslab(dspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
        error = H5Dread(dset_id, H5T_NATIVE_DOUBLE, memoryspace_id, dspace_id, H5P_DEFAULT, (void*)&values[0]);
        if (error > -1)
        {
            sr_current_curve.calib_curve[el_name] = values[0];
            us_ic_curve.calib_curve[el_name] = values[1];
            ds_ic_curve.calib_curve[el_name] = values[2];
        }
    }

    quant[STR_SR_CURRENT] = sr_current_curve;
    quant[STR_US_IC] = us_ic_curve;
    quant[STR_DS_IC] = ds_ic_curve;

    delete[]dims_out;

    H5Sclose(memoryspace_name_id);
    H5Sclose(memoryspace_id);
    H5Sclose(channels_dspace_id);
    H5Dclose(channels_dset_id);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);


    return true;
}

//---------------------------------------------------------------------------

bool MapsH5Model::_load_quantification_9(hid_t maps_grp_id)
{
    _load_quantification_9_single(maps_grp_id, QUANT_V9_LOC_MATRIX_STR, _quant_map_matrix);
    _load_quantification_9_single(maps_grp_id, QUANT_V9_LOC_NNLS_STR, _quant_map_nnls);
    _load_quantification_9_single(maps_grp_id, QUANT_V9_LOC_ROI_STR, _quant_map_roi);
    return true;
}

//---------------------------------------------------------------------------

bool MapsH5Model::_load_scalers_9(hid_t maps_grp_id)
{
    hid_t counts_dset_id, channels_dset_id, counts_dspace_id, channels_dspace_id, fit_int_spec_dset_id;
    hid_t memoryspace_id, memoryspace_name_id, error;
    hsize_t offset[3] = { 0,0,0 };
    hsize_t count[3] = { 1,1,1 };
    hsize_t offset_name[1] = { 0 };
    hsize_t count_name[1] = { 1 };
    char tmp_name[255] = { 0 };
    hid_t   filetype, memtype, status;


    counts_dset_id = H5Dopen(maps_grp_id, "scalers", H5P_DEFAULT);
    if (counts_dset_id < 0)
    {
        logW << "Error opening group /MAPS/scalers\n";
        return false;
    }
    counts_dspace_id = H5Dget_space(counts_dset_id);

    channels_dset_id = H5Dopen(maps_grp_id, "scaler_names", H5P_DEFAULT);
    if (channels_dset_id < 0)
    {
        H5Sclose(counts_dspace_id);
        H5Dclose(counts_dset_id);
        logW << "Error opening group /MAPS/scaler_names\n";
        return false;
    }
    channels_dspace_id = H5Dget_space(channels_dset_id);


    int rank = H5Sget_simple_extent_ndims(counts_dspace_id);
    if (rank != 3)
    {
        logW << "Error getting rank for /MAPS/scalers\n";
    }
    hsize_t* dims_out = new hsize_t[rank];
    unsigned int status_n = H5Sget_simple_extent_dims(counts_dspace_id, &dims_out[0], nullptr);

    filetype = H5Tcopy(H5T_C_S1);
    H5Tset_size(filetype, 256);
    memtype = H5Tcopy(H5T_C_S1);
    status = H5Tset_size(memtype, 255);

    for (int i = 0; i < 3; i++)
    {
        offset[i] = 0;
        count[i] = dims_out[i];
    }

    count[0] = 1;

    memoryspace_id = H5Screate_simple(3, count, nullptr);
    memoryspace_name_id = H5Screate_simple(1, count_name, nullptr);
    H5Sselect_hyperslab(memoryspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
    H5Sselect_hyperslab(memoryspace_name_id, H5S_SELECT_SET, offset_name, nullptr, count_name, nullptr);

    for (hsize_t el_idx = 0; el_idx < dims_out[0]; el_idx++)
    {
        offset[0] = el_idx;
        offset_name[0] = el_idx;
        memset(&tmp_name[0], 0, 254);
        H5Sselect_hyperslab(channels_dspace_id, H5S_SELECT_SET, offset_name, nullptr, count_name, nullptr);
        error = H5Dread(channels_dset_id, memtype, memoryspace_name_id, channels_dspace_id, H5P_DEFAULT, (void*)&tmp_name[0]);
        std::string el_name = std::string(tmp_name);
        // switch v9 names to v10
        if (el_name == STR_SR_CURRENT_V9)
        {
            el_name = STR_SR_CURRENT;
        }
        else if (el_name == STR_US_IC_V9)
        {
            el_name = STR_US_IC;
        }
        else if (el_name == STR_DS_IC_V9)
        {
            el_name = STR_DS_IC;
        }

        _scalers.emplace(std::pair<std::string, data_struct::ArrayXXr<float>>(el_name, data_struct::ArrayXXr<float>()));
        _scalers.at(el_name).resize(count[1], count[2]);

        H5Sselect_hyperslab(counts_dspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
        error = H5Dread(counts_dset_id, H5T_NATIVE_FLOAT, memoryspace_id, counts_dspace_id, H5P_DEFAULT, (void*)(_scalers.at(el_name).data()));
    }

    delete[]dims_out;

    H5Sclose(memoryspace_name_id);
    H5Sclose(memoryspace_id);
    H5Sclose(channels_dspace_id);
    H5Dclose(channels_dset_id);
    H5Sclose(counts_dspace_id);
    H5Dclose(counts_dset_id);

    //nan to 0.f
    for (auto& itr : _scalers)
    {
        itr.second = itr.second.unaryExpr([](float v) { return std::isfinite(v) ? v : 0.0f; });
    }

    return true;
}

//---------------------------------------------------------------------------

bool MapsH5Model::_load_scan_9(hid_t maps_grp_id)
{

    hid_t x_id, y_id;
    std::string x_axis_loc = "x_axis";
    std::string y_axis_loc = "y_axis";

    x_id = H5Dopen(maps_grp_id, x_axis_loc.c_str(), H5P_DEFAULT);
    y_id = H5Dopen(maps_grp_id, y_axis_loc.c_str(), H5P_DEFAULT);

    if (x_id > -1 && y_id > -1)
    {
        hid_t x_space_id = H5Dget_space(x_id);
        hid_t y_space_id = H5Dget_space(y_id);

        hsize_t x_dims_in[1] = { 0 };
        hsize_t y_dims_in[1] = { 0 };
        int xstatus_n = H5Sget_simple_extent_dims(x_space_id, &x_dims_in[0], nullptr);
        int ystatus_n = H5Sget_simple_extent_dims(y_space_id, &y_dims_in[0], nullptr);
        if (xstatus_n > -1 && ystatus_n > -1)
        {
            _x_axis.resize(x_dims_in[0]);
            hid_t error = H5Dread(x_id, H5T_NATIVE_FLOAT, x_space_id, x_space_id, H5P_DEFAULT, &_x_axis[0]);
            if (error > 0)
            {
                logW << "Could not load x_axis\n";
            }
            _y_axis.resize(y_dims_in[0]);
            error = H5Dread(y_id, H5T_NATIVE_FLOAT, y_space_id, y_space_id, H5P_DEFAULT, &_y_axis[0]);
            if (error > 0)
            {
                logW << "Could not load y_axis\n";
            }
        }

        if (x_space_id > -1)
        {
            H5Sclose(x_space_id);
        }
        if (y_space_id > -1)
        {
            H5Sclose(y_space_id);
        }

        H5Dclose(x_id);
        H5Dclose(y_id);

    }

    return true;
}

//---------------------------------------------------------------------------

bool MapsH5Model::_load_integrated_spectra_9(hid_t maps_grp_id)
{
    //   /MAPS/int_spec
    hid_t counts_dset_id, counts_dspace_id;
    hid_t max_chan_spec_id, max_chan_dspace_id;
    hid_t memoryspace_id, error;
    hsize_t offset[1] = {0};
    hsize_t count[1] = {1};
    hsize_t offset2[2] = { 0, 0 };
    hsize_t count2[2] = { 1, 1 };
    hid_t   filetype, memtype, status;

    counts_dset_id = H5Dopen(maps_grp_id, "int_spec", H5P_DEFAULT);
    if(counts_dset_id < 0)
    {
        logW<<"Error opening group /MAPS/int_spec\n";
        return false;
    }
    counts_dspace_id = H5Dget_space(counts_dset_id);

    int rank = H5Sget_simple_extent_ndims(counts_dspace_id);
    if (rank != 1)
    {
        logW<<"Error opening group /MAPS/int_spec\n";
    }
    hsize_t* dims_out = new hsize_t[rank];
    unsigned int status_n = H5Sget_simple_extent_dims(counts_dspace_id, &dims_out[0], nullptr);

    for (int i=0; i < rank; i++)
    {
       offset[i] = 0;
       count[i] = dims_out[i];
    }

    _integrated_spectra.resize(count[0]);
    memoryspace_id = H5Screate_simple(1, count, nullptr);
    H5Sselect_hyperslab (memoryspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
    H5Sselect_hyperslab (counts_dspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
    error = H5Dread (counts_dset_id, H5T_NATIVE_DOUBLE, memoryspace_id, counts_dspace_id, H5P_DEFAULT, (void*)(_integrated_spectra.data()));

    delete []dims_out;
    H5Sclose(memoryspace_id);

    max_chan_spec_id = H5Dopen(maps_grp_id, "max_chan_spec", H5P_DEFAULT);
    if (max_chan_spec_id > -1)
    {
        max_chan_dspace_id = H5Dget_space(max_chan_spec_id);
        rank = H5Sget_simple_extent_ndims(max_chan_dspace_id);
        hsize_t* dims_out = new hsize_t[rank];
        unsigned int status_n = H5Sget_simple_extent_dims(max_chan_dspace_id, &dims_out[0], nullptr);

        for (int i = 0; i < rank; i++)
        {
            offset2[i] = 0;
            count2[i] = dims_out[i];
        }
        count2[0] = 1;

        memoryspace_id = H5Screate_simple(1, &count2[1], nullptr);

        ArrayDr* fit_int_spec = new ArrayDr(dims_out[1]);
        H5Sselect_hyperslab(max_chan_dspace_id, H5S_SELECT_SET, offset2, nullptr, count2, nullptr);
        error = H5Dread(max_chan_spec_id, H5T_NATIVE_DOUBLE, memoryspace_id, max_chan_dspace_id, H5P_DEFAULT, (void*)(fit_int_spec->data()));
        if (error > -1)
        {
            _max_chan_spec_dict.insert({ "Max_Channels", fit_int_spec });
            
        }
        else
        {
            delete fit_int_spec;
        }

        offset2[0] = 1;
        fit_int_spec = new ArrayDr(dims_out[1]);
        H5Sselect_hyperslab(max_chan_dspace_id, H5S_SELECT_SET, offset2, nullptr, count2, nullptr);
        error = H5Dread(max_chan_spec_id, H5T_NATIVE_DOUBLE, memoryspace_id, max_chan_dspace_id, H5P_DEFAULT, (void*)(fit_int_spec->data()));
        if (error > -1)
        {
            _max_chan_spec_dict.insert({ "Max_10_Channels", fit_int_spec });
            
        }
        else
        {
            delete fit_int_spec;
        }

        offset2[0] = 2;
        fit_int_spec = new ArrayDr(dims_out[1]);
        H5Sselect_hyperslab(max_chan_dspace_id, H5S_SELECT_SET, offset2, nullptr, count2, nullptr);
        error = H5Dread(max_chan_spec_id, H5T_NATIVE_DOUBLE, memoryspace_id, max_chan_dspace_id, H5P_DEFAULT, (void*)(fit_int_spec->data()));
        if (error > -1)
        {
            _fit_int_spec_dict.insert({ STR_FIT_GAUSS_MATRIX, fit_int_spec });
        }
        else
        {
            delete fit_int_spec;
        }

		offset2[0] = 3;
		fit_int_spec = new ArrayDr(dims_out[1]);
		H5Sselect_hyperslab(max_chan_dspace_id, H5S_SELECT_SET, offset2, nullptr, count2, nullptr);
		error = H5Dread(max_chan_spec_id, H5T_NATIVE_DOUBLE, memoryspace_id, max_chan_dspace_id, H5P_DEFAULT, (void*)(fit_int_spec->data()));
		if (error > -1)
		{
			_fit_int_spec_dict.insert({ "SVD", fit_int_spec });
		}
		else
		{
			delete fit_int_spec;
		}

		offset2[0] = 4;
		fit_int_spec = new ArrayDr(dims_out[1]);
		H5Sselect_hyperslab(max_chan_dspace_id, H5S_SELECT_SET, offset2, nullptr, count2, nullptr);
		error = H5Dread(max_chan_spec_id, H5T_NATIVE_DOUBLE, memoryspace_id, max_chan_dspace_id, H5P_DEFAULT, (void*)(fit_int_spec->data()));
		if (error > -1)
		{
			_fit_int_spec_dict.insert({ "Background", fit_int_spec });
		}
		else
		{
			delete fit_int_spec;
		}

        delete[]dims_out;
        H5Sclose(memoryspace_id);
        H5Sclose(max_chan_dspace_id);
        H5Dclose(max_chan_spec_id);
    }

    H5Sclose(counts_dspace_id);
    H5Dclose(counts_dset_id);

    return true;

}

//---------------------------------------------------------------------------

bool MapsH5Model::_load_counts_9(hid_t maps_grp_id)
{
    hid_t analyzed_grp_id;

    std::string analyzed_groups[] = {"XRF_fits", "XRF_roi_plus", "XRF_roi", };

    for( std::string group_name : analyzed_groups )
    {
        _load_analyzed_counts_9(maps_grp_id, group_name);
    }

    return true;
}

//---------------------------------------------------------------------------

bool MapsH5Model::_load_analyzed_counts_9(hid_t analyzed_grp_id, std::string group_name)
{

    hid_t counts_dset_id, channels_dset_id, counts_dspace_id, channels_dspace_id;
    hid_t memoryspace_id, memoryspace_name_id, error;
    hsize_t offset[3] = {0,0,0};
    hsize_t count[3] = {1,1,1};
    hsize_t offset_name[1] = {0};
    hsize_t count_name[1] = {1};
    char tmp_name[255] = {0};
    hid_t   filetype, memtype, status;

    counts_dset_id = H5Dopen(analyzed_grp_id, group_name.c_str(), H5P_DEFAULT);
    if(counts_dset_id < 0)
    {
        logW<<"Error opening group /MAPS/"<<group_name.c_str()<<"\n";
        return false;
    }
    counts_dspace_id = H5Dget_space(counts_dset_id);

    channels_dset_id = H5Dopen(analyzed_grp_id, "channel_names", H5P_DEFAULT);
    if(channels_dset_id < 0)
    {
        H5Sclose(counts_dspace_id);
        H5Dclose(counts_dset_id);
        logW<<"Error opening group /MAPS/"<<group_name.c_str() << "\n";
        return false;
    }
    channels_dspace_id = H5Dget_space(channels_dset_id);

    int rank = H5Sget_simple_extent_ndims(counts_dspace_id);
    if (rank != 3)
    {
        logW<<"Error opening group /MAPS/"<<group_name.c_str() << "\n";
    }
    hsize_t* dims_out = new hsize_t[rank];
    unsigned int status_n = H5Sget_simple_extent_dims(counts_dspace_id, &dims_out[0], nullptr);

    filetype = H5Tcopy (H5T_C_S1);
    H5Tset_size (filetype, 256);
    memtype = H5Tcopy (H5T_C_S1);
    status = H5Tset_size (memtype, 255);

    for (int i=0; i < 3; i++)
    {
       offset[i] = 0;
       count[i] = dims_out[i];
    }

    count[0] = 1;

    data_struct::Fit_Count_Dict<float>* xrf_counts = new data_struct::Fit_Count_Dict<float>();
    // convert v9 to v10
    if (group_name == STR_FITS_V9)
    {
        group_name = STR_FIT_GAUSS_MATRIX;
    }
    else if (group_name == STR_ROI_PLUS_V9)
    {
        group_name = STR_FIT_NNLS;
    }
    else if (group_name == STR_ROI_V9)
    {
        group_name = STR_FIT_ROI;
    }
    _analyzed_counts.insert( {group_name, xrf_counts} );

    memoryspace_id = H5Screate_simple(3, count, nullptr);
    memoryspace_name_id = H5Screate_simple(1, count_name, nullptr);
    H5Sselect_hyperslab (memoryspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
    H5Sselect_hyperslab (memoryspace_name_id, H5S_SELECT_SET, offset_name, nullptr, count_name, nullptr);

    for(hsize_t el_idx=0; el_idx < dims_out[0]; el_idx++)
    {
        offset[0] = el_idx;
        offset_name[0] = el_idx;
        memset(&tmp_name[0], 0, 254);
        H5Sselect_hyperslab (channels_dspace_id, H5S_SELECT_SET, offset_name, nullptr, count_name, nullptr);
        error = H5Dread (channels_dset_id, memtype, memoryspace_name_id, channels_dspace_id, H5P_DEFAULT, (void*)&tmp_name[0]);
        std::string el_name = std::string(tmp_name);
        xrf_counts->emplace(std::pair<std::string,data_struct::ArrayXXr<float>>(el_name, data_struct::ArrayXXr<float>() ));
        xrf_counts->at(el_name).resize(count[1], count[2]);

        H5Sselect_hyperslab (counts_dspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
        error = H5Dread (counts_dset_id, H5T_NATIVE_FLOAT, memoryspace_id, counts_dspace_id, H5P_DEFAULT, (void*)(xrf_counts->at(el_name).data()));
    }

    delete []dims_out;

    H5Sclose(memoryspace_name_id);
    H5Sclose(memoryspace_id);
    H5Sclose(channels_dspace_id);
    H5Dclose(channels_dset_id);
    H5Sclose(counts_dspace_id);
    H5Dclose(counts_dset_id);

    //nan to 0.f
    for(auto& itr : *xrf_counts)
    {
        itr.second = itr.second.unaryExpr([](float v) { return std::isfinite(v)? v : 0.0f; });
    }

    return true;
}

//------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------Version 10---------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------

bool MapsH5Model::_load_version_10(hid_t file_id, hid_t maps_grp_id)
{
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_seconds;
    start = std::chrono::system_clock::now();

    _loaded_quantification = _load_quantification_10(maps_grp_id);

    end = std::chrono::system_clock::now();
    elapsed_seconds = end - start;
    logI << "load quantification elapsed time: " << elapsed_seconds.count() << "s\n";
    start = std::chrono::system_clock::now();

    _loaded_scalers = _load_scalers_10(maps_grp_id);

    end = std::chrono::system_clock::now();
    elapsed_seconds = end - start;
    logI << "load scalers elapsed time: " << elapsed_seconds.count() << "s\n";
    start = std::chrono::system_clock::now();

    _loaded_scan = _load_scan_10(maps_grp_id);

    end = std::chrono::system_clock::now();
    elapsed_seconds = end - start;
    logI << "load scan info elapsed time: " << elapsed_seconds.count() << "s\n";
    start = std::chrono::system_clock::now();

    _loaded_integrated_spectra = _load_integrated_spectra_10(file_id);

    end = std::chrono::system_clock::now();
    elapsed_seconds = end - start;
    logI << "load int spec elapsed time: " << elapsed_seconds.count() << "s\n";
    start = std::chrono::system_clock::now();

    _loaded_counts = _load_counts_10(maps_grp_id);

    end = std::chrono::system_clock::now();
    elapsed_seconds = end - start;
    logI << "load counts elapsed time: " << elapsed_seconds.count() << "s\n";

    _load_fit_parameters_10(maps_grp_id);

    if(_scan_type_as_str == STR_SCAN_TYPE_TIME_BASED_2D_MAP)
    {
        if(_load_interferometer_10(maps_grp_id))
        {
            // generate elemental maps from interferometer positions.
            _genreate_maps_from_interferometer();
        }
    }

    return (_loaded_quantification && _loaded_scalers && _loaded_scan && _loaded_integrated_spectra &&_loaded_counts);

}

//---------------------------------------------------------------------------

bool MapsH5Model::_load_quantifier(hid_t grp_id, std::string str_quantifier, std::unordered_map<std::string, Calibration_curve<double> >& quant, std::map<std::string, std::unordered_map<std::string, Element_Quant<double>*>>& e_quants)
{

    std::string calib_curve_str = STR_CALIB_CURVE + str_quantifier;
    Calibration_curve<double> calibration(str_quantifier);
    hid_t dspace_id = -1;
    hid_t error = -1;
    hid_t   filetype, memtype, status;
    hsize_t offset[2] = { 0,0 };
    hsize_t count[2] = { 1,1 };
    hsize_t offset_name[2] = { 0, 0 };
    hsize_t count_name[2] = { 1, 1 };
    char tmp_name[255] = { 0 };
    double values[3] = { 0., 0., 0. };

    hid_t dset_id = H5Dopen(grp_id, calib_curve_str.c_str(), H5P_DEFAULT);
    if (dset_id < 0)
    {
        logW << "Error opening datasaet /MAPS/" << calib_curve_str << "\n";
        //return false;
    }
    else
    {
        dspace_id = H5Dget_space(dset_id);
    }

    if (dspace_id < 0)
    {
        logW << "Error opening space /MAPS/" << calib_curve_str << "\n";
        H5Dclose(dset_id);
        return false;
    }


    hid_t channels_dset_id = H5Dopen(grp_id, STR_CALIB_LABELS.c_str(), H5P_DEFAULT);
    if (channels_dset_id < 0)
    {
        H5Sclose(dspace_id);
        H5Dclose(dset_id);
        logW << "Error opening dataset /MAPS/" << STR_CALIB_LABELS << "\n";
        return false;
    }
    hid_t channels_dspace_id = H5Dget_space(channels_dset_id);
    
    if (channels_dspace_id < 0)
    {

        H5Sclose(dspace_id);
        H5Dclose(dset_id);
        H5Dclose(channels_dset_id);
        logW << "Error opening space /MAPS/" << STR_CALIB_LABELS << "\n";
        H5Dclose(dset_id);
        return false;
    }

    int rank = H5Sget_simple_extent_ndims(dspace_id);
    if (rank != 2)
    {
        logW << str_quantifier << " rank is not equal to 3, unknown format!\n";
        H5Sclose(channels_dspace_id);
        H5Sclose(dspace_id);
        H5Dclose(dset_id);
        H5Dclose(channels_dset_id);
        return false;
    }

    hsize_t* dims_out = new hsize_t[rank];
    unsigned int status_n = H5Sget_simple_extent_dims(dspace_id, &dims_out[0], nullptr);

    filetype = H5Tcopy(H5T_C_S1);
    H5Tset_size(filetype, 256);
    memtype = H5Tcopy(H5T_C_S1);
    status = H5Tset_size(memtype, 255);

    for (int i = 0; i < rank; i++)
    {
        offset[i] = 0;
        count[i] = dims_out[i];
    }

    count[1] = 1;

    hid_t memoryspace_id = H5Screate_simple(rank, count, nullptr);
    hid_t memoryspace_name_id = H5Screate_simple(rank, count_name, nullptr);
    H5Sselect_hyperslab(memoryspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
    H5Sselect_hyperslab(memoryspace_name_id, H5S_SELECT_SET, offset_name, nullptr, count_name, nullptr);

    for (hsize_t el_idx = 0; el_idx < dims_out[1]; el_idx++)
    {
        offset[1] = el_idx;
        offset_name[1] = el_idx;
        memset(&tmp_name[0], 0, 254);
        H5Sselect_hyperslab(channels_dspace_id, H5S_SELECT_SET, offset_name, nullptr, count_name, nullptr);
        error = H5Dread(channels_dset_id, memtype, memoryspace_name_id, channels_dspace_id, H5P_DEFAULT, (void*)&tmp_name[0]);
        std::string el_name = std::string(tmp_name);
        if (dset_id > -1)
        {
            H5Sselect_hyperslab(dspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
            error = H5Dread(dset_id, H5T_NATIVE_DOUBLE, memoryspace_id, dspace_id, H5P_DEFAULT, (void*)&values[0]);
        }
        
        if (error > -1)
        {
            calibration.calib_curve[el_name] = values[0];
            calibration.calib_curve[el_name + "_L"] = values[1];
            calibration.calib_curve[el_name + "_M"] = values[2];
        }
    }

    quant[str_quantifier] = calibration;
    
    count[0] = 1;
    count[1] = 1;
    hid_t memspace_id = H5Screate_simple(1, count, nullptr);

    // try to open quantification values for refitting
    std::string str_element_info = str_quantifier + STR_ELEMENT_INFO_VALUES;
    hid_t qv_id = H5Dopen(grp_id, str_element_info.c_str(), H5P_DEFAULT);
    if (qv_id > -1)
    {
        hid_t qv_dspace = H5Dget_space(qv_id);
        unsigned int status_n = H5Sget_simple_extent_dims(qv_dspace, &dims_out[0], nullptr);
        for (int i = 0; i < dims_out[0]; i++)
        {
            offset[0] = i;
            offset[1] = 7; // get Z first 
            int Z = -1;
            H5Sselect_hyperslab(qv_dspace, H5S_SELECT_SET, offset, nullptr, count, nullptr);
            error = H5Dread(qv_id, H5T_NATIVE_INT, memspace_id, qv_dspace, H5P_DEFAULT, (void*)&Z);
            
            if (Z < 100)
            {
                data_struct::Element_Quant<double>* e_quant = new data_struct::Element_Quant<double>(Z);
                
                offset[1] = 0;
                H5Sselect_hyperslab(qv_dspace, H5S_SELECT_SET, offset, nullptr, count, nullptr);
                error = H5Dread(qv_id, H5T_NATIVE_DOUBLE, memspace_id, qv_dspace, H5P_DEFAULT, (void*)&(e_quant->weight));

                offset[1] = 1;
                H5Sselect_hyperslab(qv_dspace, H5S_SELECT_SET, offset, nullptr, count, nullptr);
                error = H5Dread(qv_id, H5T_NATIVE_DOUBLE, memspace_id, qv_dspace, H5P_DEFAULT, (void*)&(e_quant->absorption));

                offset[1] = 2;
                H5Sselect_hyperslab(qv_dspace, H5S_SELECT_SET, offset, nullptr, count, nullptr);
                error = H5Dread(qv_id, H5T_NATIVE_DOUBLE, memspace_id, qv_dspace, H5P_DEFAULT, (void*)&(e_quant->transmission_Be));

                offset[1] = 3;
                H5Sselect_hyperslab(qv_dspace, H5S_SELECT_SET, offset, nullptr, count, nullptr);
                error = H5Dread(qv_id, H5T_NATIVE_DOUBLE, memspace_id, qv_dspace, H5P_DEFAULT, (void*)&(e_quant->transmission_Ge));

                offset[1] = 4;
                H5Sselect_hyperslab(qv_dspace, H5S_SELECT_SET, offset, nullptr, count, nullptr);
                error = H5Dread(qv_id, H5T_NATIVE_DOUBLE, memspace_id, qv_dspace, H5P_DEFAULT, (void*)&(e_quant->yield));
                
                offset[1] = 5;
                H5Sselect_hyperslab(qv_dspace, H5S_SELECT_SET, offset, nullptr, count, nullptr);
                error = H5Dread(qv_id, H5T_NATIVE_DOUBLE, memspace_id, qv_dspace, H5P_DEFAULT, (void*)&(e_quant->transmission_through_Si_detector));

                offset[1] = 6;
                H5Sselect_hyperslab(qv_dspace, H5S_SELECT_SET, offset, nullptr, count, nullptr);
                error = H5Dread(qv_id, H5T_NATIVE_DOUBLE, memspace_id, qv_dspace, H5P_DEFAULT, (void*)&(e_quant->transmission_through_air));

                offset[1] = 8;
                H5Sselect_hyperslab(qv_dspace, H5S_SELECT_SET, offset, nullptr, count, nullptr);
                error = H5Dread(qv_id, H5T_NATIVE_DOUBLE, memspace_id, qv_dspace, H5P_DEFAULT, (void*)&(e_quant->e_cal_ratio));
                
                offset[1] = 9;
                H5Sselect_hyperslab(qv_dspace, H5S_SELECT_SET, offset, nullptr, count, nullptr);
                error = H5Dread(qv_id, H5T_NATIVE_DOUBLE, memspace_id, qv_dspace, H5P_DEFAULT, (void*)&(e_quant->calib_curve_val));
                
                e_quants[str_quantifier][data_struct::Element_Symbols[Z]] = e_quant;
            }
        }
        H5Sclose(qv_dspace);
        H5Dclose(qv_id);
    }
    

    delete [] dims_out;
    if(memspace_id > -1)
    {
        H5Sclose(memspace_id);
    }

    H5Sclose(memoryspace_name_id);
    H5Sclose(memoryspace_id);
    H5Sclose(channels_dspace_id);
    H5Dclose(channels_dset_id);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);

    return true;
}

//---------------------------------------------------------------------------


bool MapsH5Model::_load_quantification_10_single(hid_t maps_grp_id, std::string path, std::unordered_map<std::string, Calibration_curve<double> >& quant, std::map<std::string, std::unordered_map<std::string, Element_Quant<double>*>>& e_quants)
{
    hid_t grp_id = -1;
    
    grp_id = H5Gopen(maps_grp_id, path.c_str(), H5P_DEFAULT);
    if (grp_id < 0)
    {
        logW << "Error opening group /MAPS/" << path << "\n";
        return false;
    }

    _load_quantifier(grp_id, STR_SR_CURRENT, quant, e_quants);
    _load_quantifier(grp_id, STR_US_IC, quant, e_quants);
    _load_quantifier(grp_id, STR_US_FM, quant, e_quants);
    _load_quantifier(grp_id, STR_DS_IC, quant, e_quants);

    H5Gclose(grp_id);

    return true;
}

//---------------------------------------------------------------------------

bool MapsH5Model::_load_quantification_10(hid_t maps_grp_id)
{
    _load_quantification_10_single(maps_grp_id, QUANT_V10_LOC_MATRIX_STR, _quant_map_matrix, _all_element_quants[STR_FIT_GAUSS_MATRIX]);
    _load_quantification_10_single(maps_grp_id, QUANT_V10_LOC_NNLS_STR, _quant_map_nnls, _all_element_quants[STR_FIT_NNLS]);
    _load_quantification_10_single(maps_grp_id, QUANT_V10_LOC_ROI_STR, _quant_map_roi, _all_element_quants[STR_FIT_ROI]);
    _load_quantification_standard_10(maps_grp_id);
    return true;
}

//---------------------------------------------------------------------------

bool MapsH5Model::_load_quantification_standard_10(hid_t maps_grp_id)
{
    hid_t ns_dset_id = -1, ns_dspace_id = -1;
    hid_t st_dset_id = -1, st_dspace_id = -1;
    hid_t wn_dset_id = -1, wn_dspace_id = -1;
    hid_t w_dset_id = -1, w_dspace_id = -1;
    hsize_t count_name[1] = { 1 };
    hsize_t offset[1] = { 0 };
    hsize_t count[1] = { 1 };
    hid_t memoryspace_name_id = H5Screate_simple(1, count_name, nullptr);
    int num_standards = 0;
    herr_t err;

    hid_t memtype = H5Tcopy(H5T_C_S1);
    hid_t status = H5Tset_size(memtype, 254);

    ns_dset_id = H5Dopen(maps_grp_id, QUANT_V10_NUM_STANDARDS_STR.c_str(), H5P_DEFAULT);
    if (ns_dset_id < 0)
    {
        logW << "Error opening group /MAPS/" << QUANT_V10_NUM_STANDARDS_STR << "\n";
        return false;
    }
    else
    {
        ns_dspace_id = H5Dget_space(ns_dset_id);
    }
    
    // read number of standards 
    err = H5Dread(ns_dset_id, H5T_NATIVE_INT, ns_dspace_id, ns_dspace_id, H5P_DEFAULT, &num_standards);
    
    if (err == 0)
    {
        // read in each standard
        for (int i = 0; i < num_standards; i++)
        {
            std::string str_std_name = "";
            std::vector<std::string> element_names;
            std::vector<double> element_weights;


            std::string standard_name_str = QUANT_V10_STANDARD_STR + std::to_string(i) + "/" + STR_STANDARD_NAME;
            st_dset_id = H5Dopen(maps_grp_id, standard_name_str.c_str(), H5P_DEFAULT);
            if (st_dset_id < 0)
            {
                logW << "Error opening group /MAPS/" << standard_name_str << "\n";
                continue;
            }
            else
            {
                st_dspace_id = H5Dget_space(st_dset_id);
            }

            if (st_dset_id > -1)
            {
                char c_standard_name[256] = { 0 };
                err = H5Dread(st_dset_id, memtype, memoryspace_name_id, st_dspace_id, H5P_DEFAULT, (void*)&c_standard_name[0]);
                if (err > -1)
                {
                    str_std_name = std::string(c_standard_name);
                }
                H5Sclose(st_dspace_id);
                H5Dclose(st_dset_id);
            }

            // element weight names
            std::string standard_weight_name_str = QUANT_V10_STANDARD_STR + std::to_string(i) + "/" + STR_ELEMENT_WEIGHTS_NAMES;
            wn_dset_id = H5Dopen(maps_grp_id, standard_weight_name_str.c_str(), H5P_DEFAULT);
            if (st_dset_id < 0)
            {
                logW << "Error opening group /MAPS/" << standard_weight_name_str << "\n";
                continue;
            }
            else
            {
                wn_dspace_id = H5Dget_space(wn_dset_id);
            }

            if (wn_dset_id > -1)
            {                          
                status = H5Sget_simple_extent_dims(wn_dspace_id, &count[0], nullptr);
                memtype = H5Tcopy(H5T_C_S1);
                status = H5Tset_size(memtype, 254);

                int amt = count[0];
                count[0] = 1;
                hid_t memoryspace_id = H5Screate_simple(1, count, nullptr);
                for (int i = 0; i < amt; i++)
                {
                    char name[256] = { 0 };
                    offset[0] = i;
                    H5Sselect_hyperslab(wn_dspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
                    if (0 == H5Dread(wn_dset_id, memtype, memoryspace_id, wn_dspace_id, H5P_DEFAULT, (void*)&name[0]))
                    {
                        element_names.push_back(std::string(name));
                    }
                }

                H5Sclose(memoryspace_id);
                H5Sclose(wn_dspace_id);
                H5Dclose(wn_dset_id);
            }

            // element weights
            std::string standard_weight_str = QUANT_V10_STANDARD_STR + std::to_string(i) + "/" + STR_ELEMENT_WEIGHTS;            
            w_dset_id = H5Dopen(maps_grp_id, standard_weight_str.c_str(), H5P_DEFAULT);
            if (st_dset_id < 0)
            {
                logW << "Error opening group /MAPS/" << standard_weight_str << "\n";
                continue;
            }
            else
            {
                w_dspace_id = H5Dget_space(w_dset_id);
            }

            if (w_dset_id > -1)
            {
                status = H5Sget_simple_extent_dims(w_dspace_id, &count[0], nullptr);                
                int amt = count[0];
                count[0] = 1;
                hid_t memoryspace_id = H5Screate_simple(1, count, nullptr);
                for (int i = 0; i < amt; i++)
                {
                    double val;
                    offset[0] = i;
                    H5Sselect_hyperslab(w_dspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
                    if (0 == H5Dread(w_dset_id, H5T_NATIVE_DOUBLE, memoryspace_id, w_dspace_id, H5P_DEFAULT, (void*)&val))
                    {
                        element_weights.push_back(val);
                    }
                }
                H5Sclose(memoryspace_id);
                H5Sclose(w_dspace_id);
                H5Dclose(w_dset_id);
            }

            if (str_std_name.length() > 0 && element_names.size() == element_weights.size())
            {
                _quant_standards.emplace_back(data_struct::Quantification_Standard<double>(str_std_name, element_names, element_weights, false, false));
            }

        }
    }

    if (ns_dspace_id > -1)
    {
        H5Sclose(ns_dspace_id);
    }
    if (ns_dset_id > -1)
    {
        H5Dclose(ns_dset_id);
    }
    return true;
}

//---------------------------------------------------------------------------

bool MapsH5Model::_load_scalers_10(hid_t maps_grp_id)
{
    hid_t sub_grp_id, counts_dset_id, channels_dset_id, counts_dspace_id, channels_dspace_id, fit_int_spec_dset_id;
    hid_t memoryspace_id, memoryspace_name_id, error;
    hsize_t offset[3] = { 0,0,0 };
    hsize_t count[3] = { 1,1,1 };
    hsize_t offset_name[1] = { 0 };
    hsize_t count_name[1] = { 1 };
    char tmp_name[255] = { 0 };
    hid_t   filetype, memtype, status;

    sub_grp_id = H5Gopen(maps_grp_id, "Scalers", H5P_DEFAULT);
    if (sub_grp_id < 0)
    {
        logW << "Error opening group /MAPS/Scalers\n";
        return false;
    }

    counts_dset_id = H5Dopen(sub_grp_id, "Values", H5P_DEFAULT);
    if (counts_dset_id < 0)
    {
        H5Gclose(sub_grp_id);
        logW << "Error opening group /MAPS/Scalers/Values\n";
        return false;
    }
    counts_dspace_id = H5Dget_space(counts_dset_id);

    channels_dset_id = H5Dopen(sub_grp_id, "Names", H5P_DEFAULT);
    if (channels_dset_id < 0)
    {
        H5Sclose(counts_dspace_id);
        H5Dclose(counts_dset_id);
        H5Gclose(sub_grp_id);
        logW << "Error opening group /MAPS/Scalers/Names\n";
        return false;
    }
    channels_dspace_id = H5Dget_space(channels_dset_id);


    int rank = H5Sget_simple_extent_ndims(counts_dspace_id);
    if (rank != 3)
    {
        logW << "Error getting rank for /MAPS/Scalers/Values\n";
    }
    hsize_t* dims_out = new hsize_t[rank];
    unsigned int status_n = H5Sget_simple_extent_dims(counts_dspace_id, &dims_out[0], nullptr);

    //memtype = H5Dget_type(channels_dset_id);
    //filetype = H5Tcopy(H5T_C_S1);
    //H5Tset_size(filetype, 256);
    memtype = H5Tcopy(H5T_C_S1);
    status = H5Tset_size(memtype, 254);

    for (int i = 0; i < 3; i++)
    {
        offset[i] = 0;
        count[i] = dims_out[i];
    }

    //count[0] = 1;

    memoryspace_id = H5Screate_simple(3, count, nullptr);
    memoryspace_name_id = H5Screate_simple(1, count_name, nullptr);
    H5Sselect_hyperslab(memoryspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
    H5Sselect_hyperslab(memoryspace_name_id, H5S_SELECT_SET, offset_name, nullptr, count_name, nullptr);

    float *data_arr = new float[dims_out[0] * dims_out[1] * dims_out[2]];
    H5Sselect_hyperslab(counts_dspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
    error = H5Dread(counts_dset_id, H5T_NATIVE_FLOAT, memoryspace_id, counts_dspace_id, H5P_DEFAULT, (void*)(data_arr));
    if (error > -1)
    {
        hsize_t doffset = 0;
        for (hsize_t el_idx = 0; el_idx < dims_out[0]; el_idx++)
        {
            offset[0] = el_idx;
            offset_name[0] = el_idx;
            memset(&tmp_name[0], 0, 254);
            H5Sselect_hyperslab(channels_dspace_id, H5S_SELECT_SET, offset_name, nullptr, count_name, nullptr);
            error = H5Dread(channels_dset_id, memtype, memoryspace_name_id, channels_dspace_id, H5P_DEFAULT, (void*)&tmp_name[0]);
            if (error > -1)
            {
                std::string el_name = std::string(tmp_name);
                _scalers.emplace(std::pair<std::string, data_struct::ArrayXXr<float>>(el_name, data_struct::ArrayXXr<float>()));
                _scalers.at(el_name).resize(count[1], count[2]);
                doffset = (el_idx * count[1] * count[2]);
                memcpy((void*)(_scalers.at(el_name).data()), &data_arr[doffset], count[1] * count[2] * sizeof(float));
                
                //H5Sselect_hyperslab(counts_dspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
                //error = H5Dread(counts_dset_id, H5T_NATIVE_FLOAT, memoryspace_id, counts_dspace_id, H5P_DEFAULT, (void*)(_scalers.at(el_name).data()));
            }
        }
    }
    else
    {
        logW<<"Could not load scalers from hdf5\n";
    }
    delete [] data_arr;
    delete[]dims_out;
    H5Tclose(memtype);
    H5Sclose(memoryspace_name_id);
    H5Sclose(memoryspace_id);
    H5Sclose(channels_dspace_id);
    H5Dclose(channels_dset_id);
    H5Sclose(counts_dspace_id);
    H5Dclose(counts_dset_id);
    H5Gclose(sub_grp_id);

    //nan to 0.f
    for (auto& itr : _scalers)
    {
        itr.second = itr.second.unaryExpr([](float v) { return std::isfinite(v) ? v : 0.0f; });
    }

    return true;
}

//---------------------------------------------------------------------------

bool MapsH5Model::_load_scan_10(hid_t maps_grp_id)
{
    std::string extra_pvs_desc = "Scan/Extra_PVs/Description";
    std::string extra_pvs_name = "Scan/Extra_PVs/Names";
    std::string extra_pvs_unit = "Scan/Extra_PVs/Unit";
    std::string extra_pvs_val = "Scan/Extra_PVs/Values";
    std::string x_axis_loc = "Scan/x_axis";
    std::string y_axis_loc = "Scan/y_axis";
    std::string req_cols_loc = STR_SCAN+"/"+STR_REQUESTED_COLS;
    std::string req_rows_loc = STR_SCAN+"/"+STR_REQUESTED_ROWS;
    std::string scan_type_loc = "Scan/scan_type";
    std::string scan_polarity_pattern_loc = "Scan/"+STR_POLARITY_PATTERN;
    hsize_t offset[1] = { 0, };
    hsize_t count[1] = { 1 };
    hid_t   filetype, memtype, status;
    char tmp_name[255] = {0};
    char tmp_name2[255] = {0};

    hid_t desc_id = H5Dopen(maps_grp_id, extra_pvs_desc.c_str(), H5P_DEFAULT);
    hid_t name_id = H5Dopen(maps_grp_id, extra_pvs_name.c_str(), H5P_DEFAULT);
    hid_t unit_id = H5Dopen(maps_grp_id, extra_pvs_unit.c_str(), H5P_DEFAULT);
    hid_t val_id = H5Dopen(maps_grp_id, extra_pvs_val.c_str(), H5P_DEFAULT);
    hid_t x_id = H5Dopen(maps_grp_id, x_axis_loc.c_str(), H5P_DEFAULT);
    hid_t y_id = H5Dopen(maps_grp_id, y_axis_loc.c_str(), H5P_DEFAULT);
    hid_t scan_type_id = H5Dopen(maps_grp_id, scan_type_loc.c_str(), H5P_DEFAULT);
    hid_t polarity_pattern_id = H5Dopen(maps_grp_id, scan_polarity_pattern_loc.c_str(), H5P_DEFAULT);
    hid_t req_rows_id = H5Dopen(maps_grp_id, req_rows_loc.c_str(), H5P_DEFAULT);
    hid_t req_cols_id = H5Dopen(maps_grp_id, req_cols_loc.c_str(), H5P_DEFAULT);

    filetype = H5Tcopy(H5T_C_S1);
    H5Tset_size(filetype, 256);
    memtype = H5Tcopy(H5T_C_S1);
    status = H5Tset_size(memtype, 255);

    if(scan_type_id > -1)
    {
        hid_t dataspace_id = H5Dget_space(scan_type_id);
        if(dataspace_id > -1)
        {
            hid_t error = H5Dread(scan_type_id, memtype, dataspace_id, dataspace_id, H5P_DEFAULT, (void*)&tmp_name[0]);
            _scan_type_as_str = std::string(tmp_name);
            H5Sclose(dataspace_id);
        }
        H5Dclose(scan_type_id);
    }
    if(req_rows_id > -1)
    {
        hid_t dataspace_id = H5Dget_space(req_rows_id);
        if(dataspace_id > -1)
        {
            hid_t error = H5Dread(req_rows_id, H5T_NATIVE_INT32, dataspace_id, dataspace_id, H5P_DEFAULT, (void*)&_requested_rows);
            H5Sclose(dataspace_id);
        }
        H5Dclose(req_rows_id);
    }
    if(req_cols_id > -1)
    {
        hid_t dataspace_id = H5Dget_space(req_cols_id);
        if(dataspace_id > -1)
        {
            hid_t error = H5Dread(req_cols_id, H5T_NATIVE_INT32, dataspace_id, dataspace_id, H5P_DEFAULT, (void*)&_requested_cols);
            H5Sclose(dataspace_id);
        }
        H5Dclose(req_cols_id);
    }

    if(polarity_pattern_id > -1)
    {
        hid_t dataspace_id = H5Dget_space(polarity_pattern_id);
        if(dataspace_id > -1)
        {
            hid_t error = H5Dread(polarity_pattern_id, memtype, dataspace_id, dataspace_id, H5P_DEFAULT, (void*)&tmp_name2[0]);
            _polarity_pattern_str = std::string(tmp_name2);
            H5Sclose(dataspace_id);
        }
        H5Dclose(polarity_pattern_id);
    }

    if (desc_id > -1 && name_id > -1 && unit_id > -1 && val_id > -1)
    {
        hid_t dataspace_id = H5Dget_space(desc_id);
        int rank = H5Sget_simple_extent_ndims(dataspace_id);
        if (rank == 1)
        {
            hsize_t dims_in[1] = { 0 };
            int status_n = H5Sget_simple_extent_dims(dataspace_id, &dims_in[0], nullptr);
            if (status_n > -1)
            {

                hid_t memoryspace_id = H5Screate_simple(1, count, nullptr);
                for (hsize_t i = 0; i < dims_in[0]; i++)
                {
                    data_struct::Extra_PV epv;
                    offset[0] = i;
                    H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
                    H5Sselect_hyperslab(name_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
                    H5Sselect_hyperslab(unit_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
                    H5Sselect_hyperslab(val_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);


                    memset(&tmp_name[0], 0, 254);
                    hid_t error = H5Dread(desc_id, memtype, memoryspace_id, dataspace_id, H5P_DEFAULT, (void*)&tmp_name[0]);
                    epv.description = std::string(tmp_name);

                    
                    memset(&tmp_name[0], 0, 254);
                    error = H5Dread(name_id, memtype, memoryspace_id, dataspace_id, H5P_DEFAULT, (void*)&tmp_name[0]);
                    epv.name = std::string(tmp_name);

                    
                    memset(&tmp_name[0], 0, 254);
                    error = H5Dread(unit_id, memtype, memoryspace_id, dataspace_id, H5P_DEFAULT, (void*)&tmp_name[0]);
                    epv.unit = std::string(tmp_name);

                    
                    memset(&tmp_name[0], 0, 254);
                    error = H5Dread(val_id, memtype, memoryspace_id, dataspace_id, H5P_DEFAULT, (void*)&tmp_name[0]);
                    epv.value = std::string(tmp_name);


                    _scan_info.extra_pvs.push_back(epv);
                }
                if(memoryspace_id > -1)
                {
                    H5Sclose(memoryspace_id);
                }
            }
        }
        if(dataspace_id > -1)
        {
            H5Sclose(dataspace_id);
        }
    }
    if (desc_id > -1)
    {
        H5Dclose(desc_id);
    }
    if (name_id > -1)
    {
        H5Dclose(name_id);
    }
    if (unit_id > -1)
    {
        H5Dclose(unit_id);
    }
    if (val_id > -1)
    {
        H5Dclose(val_id);
    }

    if (x_id > -1 && y_id > -1)
    {
        hid_t x_space_id = H5Dget_space(x_id);
        hid_t y_space_id = H5Dget_space(y_id);
        
        hsize_t x_dims_in[1] = { 0 };
        hsize_t y_dims_in[1] = { 0 };
        int xstatus_n = H5Sget_simple_extent_dims(x_space_id, &x_dims_in[0], nullptr);
        int ystatus_n = H5Sget_simple_extent_dims(y_space_id, &y_dims_in[0], nullptr);
        if (xstatus_n > -1 && ystatus_n > -1)
        {
            _x_axis.resize(x_dims_in[0]);
            hid_t error = H5Dread(x_id, H5T_NATIVE_FLOAT, x_space_id, x_space_id, H5P_DEFAULT, &_x_axis[0]);
            if (error > 0)
            {
                logW << "Could not load x_axis\n";
            }
            _y_axis.resize(y_dims_in[0]);
            error = H5Dread(y_id, H5T_NATIVE_FLOAT, y_space_id, y_space_id, H5P_DEFAULT, &_y_axis[0]);
            if (error > 0)
            {
                logW << "Could not load y_axis\n";
            }
        }

        if (x_space_id > -1)
        {
            H5Sclose(x_space_id);
        }
        if (y_space_id > -1)
        {
            H5Sclose(y_space_id);
        }

        H5Dclose(x_id);
        H5Dclose(y_id);

    }


    return true;
}

//---------------------------------------------------------------------------

bool MapsH5Model::_load_integrated_spectra_10(hid_t file_id)
{
    hid_t dset_id;
    hid_t memoryspace_id, error;
    hsize_t offset[1] = { 0,};
    hsize_t count[1] = { 1 };

    std::string max_path = "/MAPS/Spectra/" + STR_INT_SPEC + "/" + STR_MAX_CHANNELS_INT_SPEC;
    std::string max_10_path = "/MAPS/Spectra/" + STR_INT_SPEC + "/" + STR_MAX10_INT_SPEC;

    if ( is_polar_xanes_scan() )
    {
        std::string lhcp_path = "/MAPS/Spectra/" + STR_INT_SPEC + "/" + STR_LHCP_SPECTRA;
        dset_id = H5Dopen(file_id, lhcp_path.c_str(), H5P_DEFAULT);
        if (dset_id > -1)
        {
            hid_t dataspace_id = H5Dget_space(dset_id);
            int rank = H5Sget_simple_extent_ndims(dataspace_id);
            if (rank == 1)
            {
                hsize_t dims_in[1] = { 0 };
                int status_n = H5Sget_simple_extent_dims(dataspace_id, &dims_in[0], nullptr);
                if (status_n > -1)
                {
                    for (int i = 0; i < rank; i++)
                    {
                        offset[i] = 0;
                        count[i] = dims_in[i];
                    }

                    _lhcp_integrated_spectra.resize(dims_in[0]);

                    count[0] = dims_in[0];

                    memoryspace_id = H5Screate_simple(1, count, nullptr);
                    H5Sselect_hyperslab(memoryspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
                    H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);

                    error = H5Dread(dset_id, H5T_NATIVE_DOUBLE, memoryspace_id, dataspace_id, H5P_DEFAULT, (void*)&(_lhcp_integrated_spectra)[0]);
                    H5Sclose(memoryspace_id);
                }
            }
            H5Sclose(dataspace_id);
            H5Dclose(dset_id);
        }

        std::string rhcp_path = "/MAPS/Spectra/" + STR_INT_SPEC + "/" + STR_RHCP_SPECTRA;
        dset_id = H5Dopen(file_id, rhcp_path.c_str(), H5P_DEFAULT);
        if (dset_id > -1)
        {
            hid_t dataspace_id = H5Dget_space(dset_id);
            int rank = H5Sget_simple_extent_ndims(dataspace_id);
            if (rank == 1)
            {
                hsize_t dims_in[1] = { 0 };
                int status_n = H5Sget_simple_extent_dims(dataspace_id, &dims_in[0], nullptr);
                if (status_n > -1)
                {
                    for (int i = 0; i < rank; i++)
                    {
                        offset[i] = 0;
                        count[i] = dims_in[i];
                    }

                    _rhcp_integrated_spectra.resize(dims_in[0]);

                    count[0] = dims_in[0];

                    memoryspace_id = H5Screate_simple(1, count, nullptr);
                    H5Sselect_hyperslab(memoryspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
                    H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);

                    error = H5Dread(dset_id, H5T_NATIVE_DOUBLE, memoryspace_id, dataspace_id, H5P_DEFAULT, (void*)&(_rhcp_integrated_spectra)[0]);
                    H5Sclose(memoryspace_id);
                }
            }
            H5Sclose(dataspace_id);
            H5Dclose(dset_id);
        }
    }
    

    dset_id = H5Dopen(file_id, max_path.c_str(), H5P_DEFAULT);
    if (dset_id > -1)
    {
        hid_t dataspace_id = H5Dget_space(dset_id);
        int rank = H5Sget_simple_extent_ndims(dataspace_id);
        if (rank == 1)
        {
            hsize_t dims_in[1] = { 0 };
            int status_n = H5Sget_simple_extent_dims(dataspace_id, &dims_in[0], nullptr);
            if (status_n > -1)
            {
                for (int i = 0; i < rank; i++)
                {
                    offset[i] = 0;
                    count[i] = dims_in[i];
                }

                ArrayDr* spectra = new ArrayDr(dims_in[0]);

                count[0] = dims_in[0];

                memoryspace_id = H5Screate_simple(1, count, nullptr);
                H5Sselect_hyperslab(memoryspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
                H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);

                error = H5Dread(dset_id, H5T_NATIVE_DOUBLE, memoryspace_id, dataspace_id, H5P_DEFAULT, (void*)&(*spectra)[0]);
                if (error > -1)
                {
                    _max_chan_spec_dict.insert({ "Max_Channels", spectra });

                }
                else
                {
                    delete spectra;
                }
                H5Sclose(memoryspace_id);
            }
        }
        H5Sclose(dataspace_id);
        H5Dclose(dset_id);
    }

    dset_id = H5Dopen(file_id, max_10_path.c_str(), H5P_DEFAULT);
    if (dset_id > -1)
    {
        hid_t dataspace_id = H5Dget_space(dset_id);
        int rank = H5Sget_simple_extent_ndims(dataspace_id);
        if (rank == 1)
        {
            hsize_t dims_in[1] = { 0 };
            int status_n = H5Sget_simple_extent_dims(dataspace_id, &dims_in[0], nullptr);
            if (status_n > -1)
            {
                for (int i = 0; i < rank; i++)
                {
                    offset[i] = 0;
                    count[i] = dims_in[i];
                }

                ArrayDr* spectra = new ArrayDr(dims_in[0]);

                count[0] = dims_in[0];

                memoryspace_id = H5Screate_simple(1, count, nullptr);
                H5Sselect_hyperslab(memoryspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
                H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);

                error = H5Dread(dset_id, H5T_NATIVE_DOUBLE, memoryspace_id, dataspace_id, H5P_DEFAULT, (void*)&(*spectra)[0]);
                if (error > -1)
                {
                    _max_chan_spec_dict.insert({ "Max_10_Channels", spectra });

                }
                else
                {
                    delete spectra;
                }
                H5Sclose(memoryspace_id);
            }
        }
        H5Sclose(dataspace_id);
        H5Dclose(dset_id);
    }

    return io::file::HDF5_IO::inst()->load_integrated_spectra_analyzed_h5(_filepath.toStdString(), &_integrated_spectra);

}

//---------------------------------------------------------------------------

bool MapsH5Model::get_copy_lhcp_rhcp_spectra(data_struct::Spectra<double>& out_lhcp_spectra, data_struct::Spectra<double>& out_rhcp_spectra)
{
    out_lhcp_spectra = _lhcp_integrated_spectra;
    out_rhcp_spectra = _rhcp_integrated_spectra;
    return true;
}

//---------------------------------------------------------------------------

bool MapsH5Model::load_pixel_spectra(const std::pair<int, int>& point, ArrayDr& spectra)
{
    std::string mca_arr_path_10 = "/MAPS/Spectra/mca_arr";
    std::string mca_arr_path_9 = "/MAPS/mca_arr";

    hid_t file_id = H5Fopen(_filepath.toStdString().c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
    if(file_id < 0)
    {
        logW<<"Error opening file "<<_filepath.toStdString() << "\n";
        return false;
    }

    hid_t dset_id = H5Dopen(file_id, mca_arr_path_10.c_str(), H5P_DEFAULT);
    if (dset_id < 0)
    {
        dset_id = H5Dopen(file_id, mca_arr_path_9.c_str(), H5P_DEFAULT);
        if (dset_id < 0)
        {
            H5Fclose(file_id);;
            return false;
        }
    }

    bool isReadVal = false;
    hid_t dataspace_id = H5Dget_space(dset_id);
    int rank = H5Sget_simple_extent_ndims(dataspace_id);
    if (rank == 3)
    {
        hsize_t offset[3] = { 0, 0, 0 };
        hsize_t count[3] = { 0, 1, 1 };
        hsize_t dims_in[3] = { 0, 0, 0 };
        int status_n = H5Sget_simple_extent_dims(dataspace_id, &dims_in[0], nullptr);
        if (status_n > -1)
        {
            count[0] = dims_in[0];
            spectra.resize(dims_in[0]);
            if(point.second < dims_in[1] && point.first < dims_in[2])
            {
                offset[1] = point.second;
                offset[2] = point.first;
                hid_t memoryspace_id = H5Screate_simple(3, count, nullptr);
				H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);

				herr_t err = H5Dread(dset_id, H5T_NATIVE_DOUBLE, memoryspace_id, dataspace_id, H5P_DEFAULT, spectra.data());
                if (err > -1)
                {
                    isReadVal = true;
                }
                H5Sclose(memoryspace_id);
            }
            else
            {
                logW<<"Can not load out of bounds. Requested: row "<<point.first<<" , col "<< point.second << " dims = "<<dims_in[1]<<" , "<<dims_in[2]<<"\n"; 
            }
        }
        else
        {
            logW<<"Could not read mca_arr dims.\n";
        }
    }
    else
    {
        logW<<"Rank is "<<rank<<" , should == 3, can not load this.\n";
    }
    H5Sclose(dataspace_id);
    H5Dclose(dset_id);
    H5Fclose(file_id);
    return isReadVal;
}

//---------------------------------------------------------------------------

bool MapsH5Model::_load_counts_10(hid_t maps_grp_id)
{

    hid_t analyzed_grp_id;

    std::string analyzed_groups[] = {"Fitted", "NNLS", "ROI", "SVD"};

    analyzed_grp_id = H5Gopen(maps_grp_id, "XRF_Analyzed", H5P_DEFAULT);
    if(analyzed_grp_id < 0)
    {
        logW<<"Error opening group /MAPS/XRF_Analyzed\n";
        return false;
    }

    for( std::string group_name : analyzed_groups )
    {
        _load_analyzed_counts_10(analyzed_grp_id, group_name);
    }

    H5Gclose(analyzed_grp_id);

    return true;
}

//---------------------------------------------------------------------------

bool MapsH5Model::_load_analyzed_counts_10(hid_t analyzed_grp_id, std::string group_name)
{

    hid_t sub_grp_id, counts_dset_id, channels_dset_id, counts_dspace_id, channels_dspace_id, fit_int_spec_dset_id;
    hid_t memoryspace_id, memoryspace_name_id, error;
    hsize_t offset[3] = {0,0,0};
    hsize_t count[3] = {1,1,1};
    hsize_t offset_name[1] = {0};
    hsize_t count_name[1] = {1};
    char tmp_name[255] = {0};
    hid_t   filetype, memtype, status;

    sub_grp_id = H5Gopen(analyzed_grp_id, group_name.c_str(), H5P_DEFAULT);
    if(sub_grp_id < 0)
    {
        logW<<"Error opening group /MAPS/XRF_Analyzed/"<<group_name.c_str() << "\n";
        return false;
    }

    counts_dset_id = H5Dopen(sub_grp_id, "Counts_Per_Sec", H5P_DEFAULT);
    if(counts_dset_id < 0)
    {
        H5Gclose(sub_grp_id);
        logW<<"Error opening group /MAPS/XRF_Analyzed/"<<group_name.c_str()<<"/Counts_Per_Sec\n";
        return false;
    }
    counts_dspace_id = H5Dget_space(counts_dset_id);

    channels_dset_id = H5Dopen(sub_grp_id, "Channel_Names", H5P_DEFAULT);
    if(channels_dset_id < 0)
    {
        H5Sclose(counts_dspace_id);
        H5Dclose(counts_dset_id);
        H5Gclose(sub_grp_id);
        logW<<"Error opening group /MAPS/XRF_Analyzed/"<<group_name.c_str()<<"/Channel_Names\n";
        return false;
    }
    channels_dspace_id = H5Dget_space(channels_dset_id);

    
	fit_int_spec_dset_id = H5Dopen(sub_grp_id, STR_FIT_INT_SPEC.c_str(), H5P_DEFAULT);
	if (fit_int_spec_dset_id > -1)
	{
		hid_t dataspace_id = H5Dget_space(fit_int_spec_dset_id);
		int rank = H5Sget_simple_extent_ndims(dataspace_id);
		if (rank == 1)
		{
			hsize_t dims_in[1] = { 0 };
			int status_n = H5Sget_simple_extent_dims(dataspace_id, &dims_in[0], nullptr);
			if (status_n > -1)
			{
				for (int i = 0; i < rank; i++)
				{
					offset[i] = 0;
					count[i] = dims_in[i];
				}

				ArrayDr* spectra = new ArrayDr(dims_in[0]);

				count[0] = dims_in[0];

				memoryspace_id = H5Screate_simple(1, count, nullptr);
				H5Sselect_hyperslab(memoryspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
				H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);

				error = H5Dread(fit_int_spec_dset_id, H5T_NATIVE_DOUBLE, memoryspace_id, dataspace_id, H5P_DEFAULT, (void*)&(*spectra)[0]);
                if ( is_polar_xanes_scan() )
                {
                    *spectra = *spectra / 2.0;
                }

				_fit_int_spec_dict.insert({ group_name , spectra });
			}
		}
	}
	
    fit_int_spec_dset_id = H5Dopen(sub_grp_id, STR_FIT_INT_BACKGROUND.c_str(), H5P_DEFAULT);
	if (fit_int_spec_dset_id > -1)
	{
		hid_t dataspace_id = H5Dget_space(fit_int_spec_dset_id);
		int rank = H5Sget_simple_extent_ndims(dataspace_id);
		if (rank == 1)
		{
			hsize_t dims_in[1] = { 0 };
			int status_n = H5Sget_simple_extent_dims(dataspace_id, &dims_in[0], nullptr);
			if (status_n > -1)
			{
				for (int i = 0; i < rank; i++)
				{
					offset[i] = 0;
					count[i] = dims_in[i];
				}

				ArrayDr* spectra = new ArrayDr(dims_in[0]);

				count[0] = dims_in[0];

				memoryspace_id = H5Screate_simple(1, count, nullptr);
				H5Sselect_hyperslab(memoryspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
				H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);

				error = H5Dread(fit_int_spec_dset_id, H5T_NATIVE_DOUBLE, memoryspace_id, dataspace_id, H5P_DEFAULT, (void*)&(*spectra)[0]);
				//_fit_int_spec_dict.insert({ group_name+"_Background" , spectra });
                if ( is_polar_xanes_scan() )
                {
                    *spectra = *spectra / 2.0;
                }
				_fit_int_spec_dict.insert({ "Background" , spectra });
			}
		}
	}

    int rank = H5Sget_simple_extent_ndims(counts_dspace_id);
    if (rank != 3)
    {
        logW<<"Error getting rank for  /MAPS/XRF_Analyzed/"<<group_name.c_str()<<"/Channel_Names\n";
    }
    hsize_t* dims_out = new hsize_t[rank];
    unsigned int status_n = H5Sget_simple_extent_dims(counts_dspace_id, &dims_out[0], nullptr);

    filetype = H5Tcopy (H5T_C_S1);
    H5Tset_size (filetype, 256);
    memtype = H5Tcopy (H5T_C_S1);
    status = H5Tset_size (memtype, 255);

    for (int i=0; i < 3; i++)
    {
       offset[i] = 0;
       count[i] = dims_out[i];
    }

    //count[0] = 1;

    data_struct::Fit_Count_Dict<float> *xrf_counts = new data_struct::Fit_Count_Dict<float>();
    _analyzed_counts.insert( {group_name, xrf_counts} );

    memoryspace_id = H5Screate_simple(3, count, nullptr);
    memoryspace_name_id = H5Screate_simple(1, count_name, nullptr);
    H5Sselect_hyperslab (memoryspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
    H5Sselect_hyperslab (memoryspace_name_id, H5S_SELECT_SET, offset_name, nullptr, count_name, nullptr);

    float *data_arr = new float[dims_out[0] * dims_out[1] * dims_out[2]];
    H5Sselect_hyperslab(counts_dspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
    error = H5Dread(counts_dset_id, H5T_NATIVE_FLOAT, memoryspace_id, counts_dspace_id, H5P_DEFAULT, (void*)(data_arr));
    if(error >-1 )
    {
        hsize_t doffset = 0;
        for(hsize_t el_idx=0; el_idx < dims_out[0]; el_idx++)
        {
            offset[0] = el_idx;
            offset_name[0] = el_idx;
            memset(&tmp_name[0], 0, 254);
            H5Sselect_hyperslab (channels_dspace_id, H5S_SELECT_SET, offset_name, nullptr, count_name, nullptr);
            error = H5Dread (channels_dset_id, memtype, memoryspace_name_id, channels_dspace_id, H5P_DEFAULT, (void*)&tmp_name[0]);
            if(error > -1)
            {
                std::string el_name = std::string(tmp_name);
                xrf_counts->emplace(std::pair<std::string,data_struct::ArrayXXr<float>>(el_name, data_struct::ArrayXXr<float>() ));
                xrf_counts->at(el_name).resize(count[1], count[2]);

                doffset = (el_idx * count[1] * count[2]);
                memcpy((void*)(xrf_counts->at(el_name).data()), &data_arr[doffset], count[1] * count[2] * sizeof(float));
                
                //H5Sselect_hyperslab (counts_dspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
                //error = H5Dread (counts_dset_id, H5T_NATIVE_FLOAT, memoryspace_id, counts_dspace_id, H5P_DEFAULT, (void*)(xrf_counts->at(el_name).data()));
            }
        }
    }
    else
    {
        logW<<"Could not load counts!\n";
    }
    delete []data_arr;
    delete []dims_out;

    H5Sclose(memoryspace_name_id);
    H5Sclose(memoryspace_id);
    H5Sclose(channels_dspace_id);
    H5Dclose(channels_dset_id);
    H5Sclose(counts_dspace_id);
    H5Dclose(counts_dset_id);
    H5Gclose(sub_grp_id);

    //nan to 0.f
    for(auto& itr : *xrf_counts)
    {
        itr.second = itr.second.unaryExpr([](float v) { return std::isfinite(v)? v : 0.0f; });
    }

    return true;
}


//---------------------------------------------------------------------------

bool MapsH5Model::_load_interferometer_10(hid_t maps_grp_id)
{
    bool retval = false;
    std::string vals_str = "/"+STR_MAPS+"/"+STR_SCAN+"/"+STR_INTERFEROMETER+"/"+STR_VALUES;
    hid_t dset_val_id = H5Dopen(maps_grp_id, vals_str.c_str(), H5P_DEFAULT);
    if(dset_val_id > -1)
    {
        hid_t dspace_vals = H5Dget_space(dset_val_id);
        int rank_val = H5Sget_simple_extent_ndims(dspace_vals);
        if (rank_val == 2 )
        {
            hsize_t dims_in[2] = { 0, 0 };
            H5Sget_simple_extent_dims(dspace_vals, &dims_in[0], nullptr);
            _interferometer_arr.resize(dims_in[0], dims_in[1]);
            herr_t status = H5Dread(dset_val_id, H5T_NATIVE_FLOAT, dspace_vals, dspace_vals, H5P_DEFAULT, _interferometer_arr.data());
            if(status > -1)
            {
                retval = true;
            }
        }
        H5Sclose(dspace_vals);
        H5Dclose(dset_val_id);
        return retval;
    }

    return retval;
}

//---------------------------------------------------------------------------

void MapsH5Model::_genreate_maps_from_interferometer()
{
    unsigned int disc_x = 100;
    unsigned int disc_y = 100;
    if(_requested_cols > 0)
    {
        disc_x = _requested_cols-7;
    }
    if(_requested_rows > 0)
    {
        disc_y = _requested_rows-6;
    }
    // ISN currently saved 24 points
    if(_interferometer_arr.cols() > 6)
    {
        Eigen::Index x_axis_idx = 7;
        Eigen::Index y_axis_idx = 3;

        logI<<"Generating maps using interferometer cols: x axis = interferometer[7], y axis = interferometer[3]\n";
        float min_x = std::numeric_limits<float>::max();
        float max_x = std::numeric_limits<float>::min();

        float min_y = std::numeric_limits<float>::max();
        float max_y = std::numeric_limits<float>::min();
        // last row is all 0's , need to fix in xrf_maps
        
        for(Eigen::Index r = 0; r <_interferometer_arr.rows(); r++)
        {
            min_x = std::min(min_x, _interferometer_arr(r,x_axis_idx));
            max_x = std::max(max_x, _interferometer_arr(r,x_axis_idx));

            min_y = std::min(min_y, _interferometer_arr(r,y_axis_idx));
            max_y = std::max(max_y, _interferometer_arr(r,y_axis_idx));
        }
        logI<<min_x<<" "<<max_x<<" : "<<min_y<<" "<<max_y<<"\n";
        //unsigned int x_inc = (max_x - min_x) / disc_x;
        //unsigned int y_inc = (max_y - min_y) / disc_y;

        _x_axis.resize(disc_x);
        _y_axis.resize(disc_y);

        std::map<std::string, data_struct::Fit_Count_Dict<float>> tmp_analyzed_counts;
        std::map<std::string, data_struct::ArrayXXr<float>> tmp_scalers = _scalers;
        // copy _analyzed_counts to tmp_analyzed_counts and resize _analyzed_counts to discretized size
        for(auto& itr: _analyzed_counts)
        {
            tmp_analyzed_counts.emplace(itr.first, data_struct::Fit_Count_Dict<float>());
            for(auto& itr2: *(itr.second))
            {
                tmp_analyzed_counts[itr.first][itr2.first].resize(itr2.second.rows(), itr2.second.cols());
                memcpy(tmp_analyzed_counts[itr.first][itr2.first].data(), itr2.second.data(), itr2.second.rows()*itr2.second.cols()*sizeof(float));
                itr2.second.resize(disc_y, disc_x);
                itr2.second.Zero(disc_y, disc_x);
            }
        }
        for(auto& itr: _scalers)
        {
            itr.second.resize(disc_y, disc_x);
            itr.second.Zero(disc_y, disc_x);
        }

        for(Eigen::Index r = 0; r <_interferometer_arr.rows(); r++)
        {
            float x_per = (_interferometer_arr(r,x_axis_idx) - min_x) / (max_x - min_x);
            
            unsigned int x_idx = x_per * disc_x;

            float y_per = (_interferometer_arr(r,y_axis_idx) - min_y) / (max_y - min_y); 
            
            unsigned int y_idx = y_per * disc_y;
            //unsigned int x_idx = _interferometer_arr(r,x_axis_idx) - min_x / x_inc;
            //unsigned int y_idx = _interferometer_arr(r,y_axis_idx) - min_y / y_inc;
            x_idx = std::min(x_idx, (disc_x-1));
            y_idx = std::min(y_idx, (disc_y-1));

            _x_axis[x_idx] = _interferometer_arr(r,x_axis_idx);
            _y_axis[y_idx] = _interferometer_arr(r,y_axis_idx);

            logI<<x_idx << " : "<< y_idx << "\n";
            for(auto& itr: _analyzed_counts)
            {
                tmp_analyzed_counts.emplace(itr.first, data_struct::Fit_Count_Dict<float>());
                for(auto& itr2: *(itr.second))
                {
                    itr2.second(y_idx, x_idx) = tmp_analyzed_counts[itr.first][itr2.first](0,r);
                }
            }
            for(auto& itr: _scalers)
            {
                itr.second(y_idx, x_idx) =  tmp_scalers[itr.first](0, r);
            }
        }
            
    }
    else
    {
        logW<<"Unknown interferometer layput. Can not generate maps\n";
    }
}

//---------------------------------------------------------------------------

bool MapsH5Model::_load_fit_parameters_10(hid_t maps_grp_id)
{
    std::string param_names_str = "Fit_Parameters_Override/Fit_Parameters_Names";
    std::string param_vals_str = "Fit_Parameters_Override/Fit_Parameters_Values";
    char tmp_name[255] = {0};
    hid_t dset_name_id = H5Dopen(maps_grp_id, param_names_str.c_str(), H5P_DEFAULT);
    hid_t dset_val_id = H5Dopen(maps_grp_id, param_vals_str.c_str(), H5P_DEFAULT);

    if(dset_name_id > -1 && dset_val_id > -1)
    {
        hid_t dspace_names = H5Dget_space(dset_name_id);
        hid_t dspace_vals = H5Dget_space(dset_val_id);

        int rank_name = H5Sget_simple_extent_ndims(dspace_names);
        int rank_val = H5Sget_simple_extent_ndims(dspace_vals);
        if (rank_name == 1 && rank_val == 2 )
        {
            hsize_t offset[1] = { 0 };
            hsize_t count[1] = { 1 };
            hsize_t dims_in[1] = { 0 };

            hsize_t offset2[2] = { 0, 0};
            hsize_t count2[2] = { 1, 4 };
            hsize_t dims_in2[2] = { 0, 0 };
            H5Sget_simple_extent_dims(dspace_names, &dims_in[0], nullptr);
            H5Sget_simple_extent_dims(dspace_vals, &dims_in2[0], nullptr);


            if(dims_in[0] != dims_in2[0])
            {
                logW<<"Fit params names dims do not match values dims: "<<dims_in[0] << " "<<dims_in2[0]<<". Setting it to the min of both.\n";
                size_t minval = std::min(dims_in[0], dims_in2[0]);
                dims_in[0] = minval;
                dims_in2[0] = minval;
            }

            hid_t memoryspace_id = H5Screate_simple(1, count, nullptr);
            hid_t memoryspace_id2 = H5Screate_simple(2, count2, nullptr);
            
            hid_t memtype = H5Tcopy(H5T_C_S1);
            H5Tset_size(memtype, 255);

            for(size_t i = 0; i < dims_in[0]; i++)
            {
                offset[0] = i;
                memset(&tmp_name[0], 0, 254);
                H5Sselect_hyperslab(dspace_names, H5S_SELECT_SET, offset, nullptr, count, nullptr);
                herr_t status = H5Dread(dset_name_id, memtype, memoryspace_id, dspace_names, H5P_DEFAULT, (void*)&tmp_name[0]);

                double fvals[4];
                offset2[0] = i;
                H5Sselect_hyperslab(dspace_vals, H5S_SELECT_SET, offset2, nullptr, count2, nullptr);
                herr_t status2 = H5Dread(dset_val_id, H5T_NATIVE_DOUBLE, memoryspace_id2, dspace_vals, H5P_DEFAULT, (void*)&fvals[0]);
                if (status > -1 && status2 > -1)
                {
                    std::string el_name = std::string(tmp_name);
                    // save fit param
                    data_struct::Fit_Param<double> fp;
                    fp.name = el_name;
                    fp.value = fvals[0];
                    if( std::isfinite(fvals[1]) )
                    {
                        fp.min_val = fvals[1];
                    }
                    if( std::isfinite(fvals[2]) )
                    {
                        fp.max_val = fvals[2];
                    }
                    if( std::isfinite(fvals[3]) )
                    {
                        //fp. = fvals[3];
                    }
                    _file_fit_params.add_parameter(fp);
                }
            }
            H5Sclose(memoryspace_id);
            H5Sclose(memoryspace_id2);
        }
        else
        {
            logW<<"Rank for name should be 1 but is "<<rank_name<<" and rank for values should be 2 but is "<<rank_val<<"\n";
        }
        
        H5Sclose(dspace_names);
        H5Sclose(dspace_vals);

        H5Dclose(dset_name_id);
        H5Dclose(dset_val_id);
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------

bool MapsH5Model::is_polar_xanes_scan()
{
    if(_scan_type_as_str == STR_SCAN_TYPE_POLAR_XANES)
    {
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------

QStringList MapsH5Model::get_calibration_curve_scalers(std::string analysis_type)
{
    QStringList keys;
    if (analysis_type == STR_FIT_GAUSS_MATRIX)
    {
        for (auto& itr : _quant_map_matrix)
        {
            keys.append(itr.first.c_str());
        }
    }
    else if (analysis_type == STR_FIT_NNLS)
    {
        for (auto& itr : _quant_map_nnls)
        {
            keys.append(itr.first.c_str());
        }
   
    }
    else if (analysis_type == STR_FIT_ROI)
    {
        for (auto& itr : _quant_map_roi)
        {
            keys.append(itr.first.c_str());
        }
        
    }

    return keys;
}

//---------------------------------------------------------------------------

Calibration_curve<double>* MapsH5Model::get_calibration_curve(std::string analysis_type, std::string scaler_name)
{
    if (analysis_type == STR_FIT_GAUSS_MATRIX)
    {
        if (_quant_map_matrix.count(scaler_name) > 0)
        {
            return &_quant_map_matrix[scaler_name];
        }
    }
    else if (analysis_type == STR_FIT_NNLS)
    {
        if (_quant_map_nnls.count(scaler_name) > 0)
        {
            return &_quant_map_nnls[scaler_name];
        }
    }
    else if (analysis_type == STR_FIT_ROI)
    {
        if (_quant_map_roi.count(scaler_name) > 0)
        {
            return &_quant_map_roi[scaler_name];
        }
    }

    return nullptr;
}

const std::unordered_map < std::string, Element_Quant<double>*>& MapsH5Model::get_quant_fit_info(std::string analysis_type, std::string scaler_name)
{
    return _all_element_quants[analysis_type][scaler_name];
}

//---------------------------------------------------------------------------

void MapsH5Model::generateNameLists(QString analysis_type, std::vector<std::string> &names)
{

    data_struct::Fit_Count_Dict<float> fit_counts;
    getAnalyzedCounts(analysis_type.toStdString(), fit_counts);
    const std::map<std::string, data_struct::ArrayXXr<float>>* scalers = getScalers();
    if (scalers != nullptr)
    {
        for (auto& itr : *scalers)
        {
            fit_counts.insert(itr);
        }
    }

	std::vector<std::string> element_lines;
	std::vector<std::string> scalers_to_add_first;
	std::vector<std::string> final_counts_to_add_before_scalers;
	gen_insert_order_lists(element_lines, scalers_to_add_first, final_counts_to_add_before_scalers);

    // insert in z order
    for (std::string el_name : element_lines)
    {
        if (fit_counts.count(el_name) > 0)
        {
            names.push_back(el_name);
            fit_counts.erase(el_name);
        }
    }

    // add end of element list that are not elements
    for (auto& itr : final_counts_to_add_before_scalers)
    {
        if (fit_counts.count(itr) > 0)
        {
            names.push_back(itr);
            fit_counts.erase(itr);
        }
    }

    // add scalers in certain order
    for (auto& itr : scalers_to_add_first)
    {
        if (fit_counts.count(itr) > 0)
        {
            names.push_back(itr);
            fit_counts.erase(itr);
        }
    }

    // add rest of scalers
    for (auto& itr : fit_counts)
    {
        names.push_back(itr.first);
    }

}

//---------------------------------------------------------------------------

QPixmap MapsH5Model::gen_pixmap(const GenerateImageProp& props, ArrayXXr<float>& normalized) const
{

    //std::chrono::time_point<std::chrono::system_clock> start, end;
    //start = std::chrono::system_clock::now();
    if(props.analysis_type.length() < 1 || props.element.length() < 1)
    {
        return QPixmap();
    }

    bool draw = getCountsOrScaler(props.analysis_type, props.element, &normalized);
    int height = 0;
    int width = 0;

    if (draw)
    {
        height = static_cast<int>(normalized.rows());
        width = static_cast<int>(normalized.cols());
        
        if (props.log_color)
        {
            normalized = normalized.log10();
            normalized = normalized.unaryExpr([](float v) { return std::isfinite(v) ? v : 0.0f; });
        }

        // add to width for color maps ledgend
        int cm_ledgend = width * .05; // add 5% width for ledgend
        if (cm_ledgend == 0)
        {
            cm_ledgend = 3;
        }
        if (height <= 3 || props.show_legend == false)
        {
            cm_ledgend = 0;
        }

                
        QImage image(width + cm_ledgend, height, QImage::Format_Indexed8);
        
        if(props.selected_colormap != nullptr)
        {
            image.setColorTable(*props.selected_colormap);
        }
        else
        {
            QVector<QRgb> gray_colormap;
            for (int i = 0; i < 256; i++)
            {
                gray_colormap.append(qRgb(i, i, i));
            }
            image.setColorTable(gray_colormap);
        }
        

        float counts_max;
        float counts_min;
        // show normalized by mass calib curve
        if (props.normalizer != nullptr && props.calib_curve != nullptr)
        {
            if (props.calib_curve->calib_curve.count(props.element) > 0)
            {
                double calib_val = props.calib_curve->calib_curve.at(props.element);
                normalized /= (*props.normalizer);
                normalized /= calib_val;
                float min_coef = normalized.minCoeff();
                if (std::isfinite(min_coef) == false)
                {
                    min_coef = 0.0f;
                }
                normalized = normalized.unaryExpr([min_coef](float v) { return std::isfinite(v) ? v : min_coef; });
            }
        }
        else if (props.normalizer != nullptr)
        {
            // show normalized just by scaler
            normalized /= (*props.normalizer);
            float min_coef = normalized.minCoeff();
            if (std::isfinite(min_coef) == false)
            {
                min_coef = 0.0f;
            }
            normalized = normalized.unaryExpr([min_coef](float v) { return std::isfinite(v) ? v : min_coef; });
        }
        
        if(props.contrast_limits == STR_FULL_IMAGE)
        {
            counts_max = normalized.maxCoeff();
            counts_min = normalized.minCoeff();
            
            if (props.global_contrast)
            {
                // normalize contrast
                counts_max = counts_min + ((counts_max - counts_min) * props.contrast_max);
                counts_min = counts_min + ((counts_max - counts_min) * props.contrast_min);
            }
            else
            {
                //get user min max from contrast control
                counts_max = props.contrast_max;
                counts_min = props.contrast_min;
            }
        }
        else if (props.contrast_limits == STR_CENTER_2_3_IMAGE 
            || props.contrast_limits == STR_CENTER_1_3_IMAGE
            || props.contrast_limits == STR_CENTER_1_4_IMAGE
            || props.contrast_limits == STR_CENTER_1_6_IMAGE)
        {
            int sub_height = 0;
            int sub_width = 0;
            int center_x = normalized.cols() / 2;
            int center_y = normalized.rows() / 2;

            counts_max = std::numeric_limits<float>::min();
            counts_min = std::numeric_limits<float>::max();
            

            if (props.contrast_limits == STR_CENTER_2_3_IMAGE )
            {
                sub_height = (normalized.rows() / 3 );
                sub_width = (normalized.cols() / 3 );
            }
            else if (props.contrast_limits == STR_CENTER_1_3_IMAGE)
            {
                sub_height = (normalized.rows() / 3 ) / 2;
                sub_width = (normalized.cols() / 3 ) / 2;
            }
            else if (props.contrast_limits == STR_CENTER_1_4_IMAGE)
            {
                sub_height = (normalized.rows() / 4 ) / 2;
                sub_width = (normalized.cols() / 4 ) / 2;
            }
            else if (props.contrast_limits == STR_CENTER_1_6_IMAGE)
            {
                sub_height = (normalized.rows() / 5 ) / 2;
                sub_width = (normalized.cols() / 5 ) / 2;
            }
            for(int r = center_y - sub_height; r < center_y + sub_height; r++)
            {
                for(int c = center_x - sub_width; c < center_x + sub_width; c++)
                {
                    float val = normalized(r,c);
                    counts_max = std::max(val, counts_max);
                    counts_min = std::min(val, counts_min);
                }
            }
        }
        
        //int bpl = image.bytesPerLine();
        //unsigned char *data = new unsigned char[height * bpl];
        
        float max_min = counts_max - counts_min;
        for (int row = 0; row < height; ++row)
        {
            for (int col = 0; col < width; ++col)
            {
                //first clamp the data to max min
                float cnts = normalized(row, col);
                cnts = std::min(counts_max, cnts);
                cnts = std::max(counts_min, cnts);
                //data[row * bpl + col] = static_cast<unsigned char>(((cnts - counts_min) / max_min) * 255);
                //convert to pixel
                unsigned char data = (unsigned char)(((cnts - counts_min) / max_min) * 255);
                image.setPixel(col, row, data);
            }
        }
        //memcpy(image.bits(), data, height * bpl);
        //delete [] data;

        if (height > 3 || props.show_legend == true)
        {
            // add color map ledgend
            int startH = height * .1;
            int endH = height - startH;
            int startW = width + 1;
            int endW = width + cm_ledgend;
            if (cm_ledgend == 3)
            {
                endW--;
            }

            float inc = 255.0 / float(endH - startH);
            float fcol = 255.0;

            for (int row = 0; row < height; row++)
            {
                if (row >= startH && row <= endH)
                {
                    for (int col = width; col < width + cm_ledgend; col++)
                    {
                        if (col >= startW && col <= endW)
                        {
                            image.setPixel(col, row, static_cast<unsigned char>(fcol));
                        }
                        else
                        {
                            image.setPixel(col, row, static_cast<unsigned char>(127));
                        }
                    }
                    fcol -= inc;
                    fcol = std::max(fcol, float(0.0));
                }
                else
                {
                    for (int col = width; col < width + cm_ledgend; col++)
                    {
                        image.setPixel(col, row, static_cast<unsigned char>(127));
                    }
                }
            }
        }
        
        if (props.invert_y)
        {
            image = image.mirrored(false, true);
        }

        //end = std::chrono::system_clock::now();
        //std::chrono::duration<double> elapsed_seconds = end - start;
        //logI << "=-=-=-=-=-=- Total elapsed time: " << elapsed_seconds.count() << "s =-=-=-=-=-=-=-\n" << std::endl; //endl will flush the print.
       
        return QPixmap::fromImage(image.convertToFormat(QImage::Format_RGB32));
    }

    return QPixmap();
}

//---------------------------------------------------------------------------

void gen_insert_order_list(std::vector<std::string> &all_lines)
{
    std::vector<std::string> scalers_to_add_first;
    std::vector<std::string> final_counts_to_add_before_scalers;
    gen_insert_order_lists(all_lines, scalers_to_add_first, final_counts_to_add_before_scalers);
    for(auto itr : scalers_to_add_first)
    {
        all_lines.push_back(itr);
    }
    for(auto itr : final_counts_to_add_before_scalers)
    {
        all_lines.push_back(itr);
    }
}

//---------------------------------------------------------------------------

void gen_insert_order_lists(std::vector<std::string> &element_lines, std::vector<std::string> &scalers_to_add_first, std::vector<std::string>& final_counts_to_add_before_scalers)
{
    element_lines.clear();
    scalers_to_add_first.clear();
    final_counts_to_add_before_scalers.clear();

    //create save ordered vector by element Z number with K , L, M lines
    for (std::string el_name : data_struct::Element_Symbols)
    {
        element_lines.push_back(el_name);
    }
    for (std::string el_name : data_struct::Element_Symbols)
    {
        element_lines.push_back(el_name + "_L");
    }
    for (std::string el_name : data_struct::Element_Symbols)
    {
        element_lines.push_back(el_name + "_M");
    }

   
    final_counts_to_add_before_scalers.push_back(STR_COHERENT_SCT_AMPLITUDE);
    final_counts_to_add_before_scalers.push_back(STR_COMPTON_AMPLITUDE);
    final_counts_to_add_before_scalers.push_back(STR_SUM_ELASTIC_INELASTIC_AMP);
    final_counts_to_add_before_scalers.push_back(STR_TOTAL_FLUORESCENCE_YIELD);
    final_counts_to_add_before_scalers.push_back(STR_NUM_ITR);
    final_counts_to_add_before_scalers.push_back(STR_RESIDUAL);

    scalers_to_add_first.push_back(STR_SR_CURRENT);
    scalers_to_add_first.push_back(STR_US_IC);
    scalers_to_add_first.push_back(STR_US_FM);
    scalers_to_add_first.push_back(STR_DS_IC);
    scalers_to_add_first.push_back(STR_ELT);
    scalers_to_add_first.push_back(STR_ELAPSED_LIVE_TIME);
    scalers_to_add_first.push_back(STR_ERT);
    scalers_to_add_first.push_back(STR_ELAPSED_REAL_TIME);
    scalers_to_add_first.push_back(STR_INPUT_COUNTS);
    scalers_to_add_first.push_back(STR_ICR);
    scalers_to_add_first.push_back("INCNT");
    scalers_to_add_first.push_back(STR_OUTPUT_COUNTS);
    scalers_to_add_first.push_back(STR_OCR);
    scalers_to_add_first.push_back("OUTCNT");
    scalers_to_add_first.push_back(STR_DEAD_TIME);
    scalers_to_add_first.push_back("abs_cfg");
    scalers_to_add_first.push_back("abs_ic");
    scalers_to_add_first.push_back("H_dpc_cfg");
    scalers_to_add_first.push_back("V_dpc_cfg");
    scalers_to_add_first.push_back("DPC1_IC");
    scalers_to_add_first.push_back("DPC2_IC");
    scalers_to_add_first.push_back("dia1_dpc_cfg");
    scalers_to_add_first.push_back("dia2_dpc_cfg");
    scalers_to_add_first.push_back("CFG_1");
    scalers_to_add_first.push_back(STR_CFG_2);
    scalers_to_add_first.push_back(STR_CFG_3);
    scalers_to_add_first.push_back(STR_CFG_4);
    scalers_to_add_first.push_back(STR_CFG_5);
    scalers_to_add_first.push_back("CFG_6");
    scalers_to_add_first.push_back("CFG_7");
    scalers_to_add_first.push_back("CFG_8");
    scalers_to_add_first.push_back("CFG_9");

}

