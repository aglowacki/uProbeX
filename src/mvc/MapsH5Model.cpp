/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/MapsH5Model.h>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

/*----------------src/mvc/MapsH5Model.cpp \-----------------------------------------------------------*/

MapsH5Model::MapsH5Model() : QObject()
{

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

}

/*---------------------------------------------------------------------------*/

MapsH5Model::~MapsH5Model()
{

    clear_analyzed_counts();

}

/*---------------------------------------------------------------------------*/

void MapsH5Model::clear_analyzed_counts()
{

    for (auto itr : _analyzed_counts)
    {
        delete itr.second;
    }
    _analyzed_counts.clear();

}

/*---------------------------------------------------------------------------*/

void MapsH5Model::getAnalyzedCounts(std::string analysis_type, data_struct::Fit_Count_Dict<float>& out_counts)
{
    std::lock_guard<std::mutex> lock(_mutex);
    out_counts.clear();

    if(analysis_type.length() > 0 && _analyzed_counts.count(analysis_type) > 0)
    {
        for (const auto& itr : *(_analyzed_counts[analysis_type]))
        {
            out_counts[itr.first] = itr.second;
        }

    }
}

/*---------------------------------------------------------------------------*/

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

/*---------------------------------------------------------------------------*/

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

/*---------------------------------------------------------------------------*/

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

/*---------------------------------------------------------------------------*/

void MapsH5Model::set_fit_parameters_override(data_struct::Params_Override<double>* override)
{
    _params_override = override;
}

/*---------------------------------------------------------------------------*/

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
            xrf_counts->emplace(std::pair<std::string,EMatrixF>(itr2.first, EMatrixF() ));
            xrf_counts->at(itr2.first).resize(block->height(), block->width());
            xrf_counts->at(itr2.first).setZero(block->height(), block->width());
            xrf_counts->at(itr2.first)(block->row(), block->col()) = itr2.second;
        }
    }
    emit model_data_updated();
}

/*---------------------------------------------------------------------------*/

void MapsH5Model::clearAllMapRois()
{
    _map_rois.clear();
}

/*---------------------------------------------------------------------------*/

void MapsH5Model::appendMapRoi(std::string name, struct Map_ROI roi)
{
    _map_rois[name] = roi;
}

/*---------------------------------------------------------------------------*/

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

        QJsonArray json_spec;
        for (auto& iItr : itr.second.int_spec)
        {
            json_spec.append(iItr);
        }
        json_int_spec[STR_MAP_ROI_INT_SPEC_FILENAME.c_str()] = finfo.fileName();
        json_int_spec[STR_SPECTRA.c_str()] = json_spec;
        json_int_spec[STR_ELT.c_str()] = itr.second.int_spec.elapsed_livetime();
        json_int_spec[STR_ERT.c_str()] = itr.second.int_spec.elapsed_realtime();
        json_int_spec[STR_ICR.c_str()] = itr.second.int_spec.input_counts();
        json_int_spec[STR_OCR.c_str()] = itr.second.int_spec.output_counts();
        
        json_arr_specs.append(json_int_spec);

        json_roi_object[STR_MAP_ROI_INT_SPEC.c_str()] = json_arr_specs;
        json_rois.append(json_roi_object);
    }

    rootJson[STR_MAPS_ROIS.c_str()] = json_rois;
    QByteArray save_data = QJsonDocument(rootJson).toJson();
    
    if (_map_rois.size() > 0)
    {
        QFile saveFile(roi_file_name);
        if (!saveFile.open(QIODevice::WriteOnly))
        {
            logW << "Couldn't open save file: " << roi_file_name.toStdString();
        }
        else
        {
            saveFile.write(save_data);
            saveFile.close();
        }
    }
}

/*---------------------------------------------------------------------------*/

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
        logW << "Couldn't open save file: " << roi_file_name.toStdString();
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
                        // if we found our int spectra then load it
                        if (filename == finfo.fileName())
                        {
                            QJsonArray json_spec_arr = json_int_spec[STR_SPECTRA.c_str()].toArray();
                            mroi.int_spec.resize(json_spec_arr.size());
                            for (int n = 0; n < json_spec_arr.size(); ++n)
                            {
                                mroi.int_spec(n) = json_spec_arr[n].toInt();
                            }
                            mroi.int_spec.elapsed_livetime(json_int_spec[STR_ELT.c_str()].toDouble());
                            mroi.int_spec.elapsed_realtime(json_int_spec[STR_ERT.c_str()].toDouble());
                            mroi.int_spec.input_counts(json_int_spec[STR_ICR.c_str()].toDouble());
                            mroi.int_spec.output_counts(json_int_spec[STR_OCR.c_str()].toDouble());
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
                // load it 
                if (io::file::HDF5_IO::inst()->load_integrated_spectra_analyzed_h5_roi(_dir.absolutePath().toStdString(), &(mroi.int_spec), mroi.pixel_list))
                {
                    QJsonArray jsonIntSpecArr = jsonRoi[STR_MAP_ROI_INT_SPEC.c_str()].toArray();

                    QJsonObject json_int_spec;
                    json_int_spec[STR_MAP_ROI_INT_SPEC_FILENAME.c_str()] = finfo.fileName();

                    QJsonArray json_spec;
                    for (auto& iItr : mroi.int_spec)
                    {
                        json_spec.append(iItr);
                    }
                    json_int_spec[STR_SPECTRA.c_str()] = json_spec;
                    json_int_spec[STR_ELT.c_str()] = mroi.int_spec.elapsed_livetime();
                    json_int_spec[STR_ERT.c_str()] = mroi.int_spec.elapsed_realtime();
                    json_int_spec[STR_ICR.c_str()] = mroi.int_spec.input_counts();
                    json_int_spec[STR_OCR.c_str()] = mroi.int_spec.output_counts();

                    jsonIntSpecArr.append(json_int_spec);
                    jsonRoi[STR_MAP_ROI_INT_SPEC.c_str()] = jsonIntSpecArr;
                    rois[i] = jsonRoi;
                    resave = true;
                }
            }
            _map_rois[mroi.name] = mroi;
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

/*---------------------------------------------------------------------------*/

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
                for(auto& itr2 : itr.second.fit_counts)
                {
					if (xrf_counts->count(itr2.first) < 1)
					{
						initialize_from_stream_block(block);
						break;
					}
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
            else
            {
                initialize_from_stream_block(block);
            }
        }
    }
}

void MapsH5Model::getIntegratedSpectra(data_struct::Spectra<double>& out_spectra)
{
    std::lock_guard<std::mutex> lock(_mutex);
    out_spectra = _integrated_spectra;
}

/*---------------------------------------------------------------------------*/

bool MapsH5Model::load(QString filepath)
{
    try
    {
        _filepath = filepath;
        _dir = QDir(filepath);
        _datset_name = QFileInfo(filepath).fileName();
        //_is_loaded = ERROR_LOADING;
        //std::chrono::time_point<std::chrono::system_clock> start, end;
        //start = std::chrono::system_clock::now();

        //logW<<" MapsH5Model loading "<< filepath.toStdString() << "\n";

        hid_t    file_id, dset_id, dataspace_id, maps_grp_id, memoryspace_id, analyzed_grp_id;
        herr_t   error;
        //hsize_t offset[1] = {0};
        hsize_t count[1] = {1};

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

        /*
        dset_id = H5Dopen2(maps_grp_id, "version", H5P_DEFAULT);
        if(dset_id < 0)
        {
            H5Gclose(maps_grp_id);
            H5Fclose(file_id);
            logW<<"Error opening dataset /MAPS/version";
            return false;
        }
        dataspace_id = H5Dget_space(dset_id);

        memoryspace_id = H5Screate_simple(1, count, nullptr);

        error = H5Dread (dset_id, H5T_NATIVE_FLOAT, memoryspace_id, dataspace_id, H5P_DEFAULT, (void*)&_version);
        
        if(error != 0)
        {
            return false;
        }
        */
        if(_version < 10.0)
        {
            _is_fully_loaded = _load_version_9(maps_grp_id);
        }
        else
        {
            _is_fully_loaded = _load_version_10(file_id, maps_grp_id);
        }

        //H5Sclose(memoryspace_id);
        //H5Sclose(dataspace_id);
        //H5Dclose(dset_id);
        H5Gclose(maps_grp_id);
        //H5Fclose(file_id);

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

        //end = std::chrono::system_clock::now();
        //std::chrono::duration<double> elapsed_seconds = end-start;
        //std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    }
    catch (std::string& s)
    {
        throw s;
    }
    catch (...)
    {
        throw std::string("Failed to open Maps Analyzed dataset!");
    }

    return _is_fully_loaded;
}

/*---------------------------------------------------------------------------*/

bool MapsH5Model::load_roi(const std::vector<QPoint> &roi_list, data_struct::Spectra<double>& spec)
{
    if (_version < 10.0)
    {
        return _load_roi_9(roi_list, spec);
    }
    return _load_roi_10(roi_list, spec);
}

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

/*---------------------------------------------------------------------------*/

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
        logE << "Error opening group /MAPS/"<<path<<"\n";
        return false;
    }
    dspace_id = H5Dget_space(dset_id);

    channels_dset_id = H5Dopen(maps_grp_id, "channel_names", H5P_DEFAULT);
    if (channels_dset_id < 0)
    {
        logE << "Error opening group /MAPS/channel_names\n";
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

/*---------------------------------------------------------------------------*/

bool MapsH5Model::_load_quantification_9(hid_t maps_grp_id)
{
    _load_quantification_9_single(maps_grp_id, QUANT_V9_LOC_MATRIX_STR, _quant_map_matrix);
    _load_quantification_9_single(maps_grp_id, QUANT_V9_LOC_NNLS_STR, _quant_map_nnls);
    _load_quantification_9_single(maps_grp_id, QUANT_V9_LOC_ROI_STR, _quant_map_roi);
    return true;
}

/*---------------------------------------------------------------------------*/

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

        _scalers.emplace(std::pair<std::string, EMatrixF>(el_name, EMatrixF()));
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

/*---------------------------------------------------------------------------*/

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

/*---------------------------------------------------------------------------*/

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
        logE<<"Error opening group /MAPS/int_spec\n";
        return false;
    }
    counts_dspace_id = H5Dget_space(counts_dset_id);

    int rank = H5Sget_simple_extent_ndims(counts_dspace_id);
    if (rank != 1)
    {
        logE<<"Error opening group /MAPS/int_spec\n";
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

/*---------------------------------------------------------------------------*/

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

/*---------------------------------------------------------------------------*/

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
        logE<<"Error opening group /MAPS/"<<group_name.c_str()<<"\n";
        return false;
    }
    counts_dspace_id = H5Dget_space(counts_dset_id);

    channels_dset_id = H5Dopen(analyzed_grp_id, "channel_names", H5P_DEFAULT);
    if(channels_dset_id < 0)
    {
        H5Sclose(counts_dspace_id);
        H5Dclose(counts_dset_id);
        logE<<"Error opening group /MAPS/"<<group_name.c_str() << "\n";
        return false;
    }
    channels_dspace_id = H5Dget_space(channels_dset_id);

    int rank = H5Sget_simple_extent_ndims(counts_dspace_id);
    if (rank != 3)
    {
        logE<<"Error opening group /MAPS/"<<group_name.c_str() << "\n";
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
        xrf_counts->emplace(std::pair<std::string,EMatrixF>(el_name, EMatrixF() ));
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

/*---------------------------------------------------------------------------*/

bool MapsH5Model::_load_roi_9(const std::vector<QPoint>& roi_list, data_struct::Spectra<double>& spec)
{
    return false;

}


//------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------Version 10---------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------

bool MapsH5Model::_load_version_10(hid_t file_id, hid_t maps_grp_id)
{

    _loaded_quantification = _load_quantification_10(maps_grp_id);

    _loaded_scalers = _load_scalers_10(maps_grp_id);

    _loaded_scan = _load_scan_10(maps_grp_id);

    _loaded_integrated_spectra = _load_integrated_spectra_10(file_id);

    _loaded_counts = _load_counts_10(maps_grp_id);

    return (_loaded_quantification && _loaded_scalers && _loaded_scan && _loaded_integrated_spectra &&_loaded_counts);

}

/*---------------------------------------------------------------------------*/


bool MapsH5Model::_load_quantification_10_single(hid_t maps_grp_id, std::string path, std::unordered_map<std::string, Calibration_curve<double> >& quant)
{
    hid_t channels_dset_id = -1, channels_dspace_id = -1;
    hid_t sr_dset_id = -1, sr_dspace_id = -1;
    hid_t us_dset_id = -1, us_dspace_id = -1;
    hid_t ds_dset_id = -1, ds_dspace_id = -1;
    hid_t grp_id = -1;
    hid_t memoryspace_id = -1, memoryspace_name_id = -1;
    hid_t error = -1;
    hid_t sr_error = -1;
    hid_t us_error = -1;
    hid_t ds_error = -1;
    hsize_t offset[2] = { 0,0 };
    hsize_t count[2] = { 1,1 };
    hsize_t offset_name[2] = { 0, 0 };
    hsize_t count_name[2] = { 1, 1 };
    char tmp_name[255] = { 0 };
    hid_t   filetype, memtype, status;
    double sr_values[3] = { 0., 0., 0. };
    double us_values[3] = { 0., 0., 0. };
    double ds_values[3] = { 0., 0., 0. };

    grp_id = H5Gopen(maps_grp_id, path.c_str(), H5P_DEFAULT);
    if (grp_id < 0)
    {
        logE << "Error opening group /MAPS/" << path << "\n";
        return false;
    }


    sr_dset_id = H5Dopen(grp_id, STR_CALIB_CURVE_SR_CUR.c_str(), H5P_DEFAULT);
    if (sr_dset_id < 0)
    {
        logW << "Error opening group /MAPS/" << STR_CALIB_CURVE_SR_CUR << "\n";
        //return false;
    }
    else
    {
        sr_dspace_id = H5Dget_space(sr_dset_id);
    }

    us_dset_id = H5Dopen(grp_id, STR_CALIB_CURVE_US_IC.c_str(), H5P_DEFAULT);
    if (us_dset_id < 0)
    {
        logW << "Error opening group /MAPS/" << STR_CALIB_CURVE_US_IC << "\n";
        //return false;
    }
    else
    {
        us_dspace_id = H5Dget_space(us_dset_id);
    }

    ds_dset_id = H5Dopen(grp_id, STR_CALIB_CURVE_DS_IC.c_str(), H5P_DEFAULT);
    if (ds_dset_id < 0)
    {
        logW << "Error opening group /MAPS/" << STR_CALIB_CURVE_DS_IC << "\n";
        //return false;
    }
    else
    {
        ds_dspace_id = H5Dget_space(ds_dset_id);
    }

    channels_dset_id = H5Dopen(grp_id, STR_CALIB_LABELS.c_str(), H5P_DEFAULT);
    if (channels_dset_id < 0)
    {
        logE << "Error opening group /MAPS/" << STR_CALIB_LABELS << "\n";
        return false;
    }
    channels_dspace_id = H5Dget_space(channels_dset_id);

    hid_t found_space_id = -1;
    if (sr_dspace_id > -1)
    {
        found_space_id = sr_dspace_id;
    }
    else if (us_dset_id > -1)
    {
        found_space_id = us_dset_id;
    }
    else if (ds_dset_id > -1)
    {
        found_space_id = ds_dset_id;
    }
    else
    {
        return false;
    }

    int rank = H5Sget_simple_extent_ndims(found_space_id);
    if (rank != 2)
    {
        logE << path << " rank is not equal to 3, unknown format!\n";
        return false;
    }
    hsize_t* dims_out = new hsize_t[rank];
    unsigned int status_n = H5Sget_simple_extent_dims(found_space_id, &dims_out[0], nullptr);

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

    memoryspace_id = H5Screate_simple(rank, count, nullptr);
    memoryspace_name_id = H5Screate_simple(rank, count_name, nullptr);
    H5Sselect_hyperslab(memoryspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
    H5Sselect_hyperslab(memoryspace_name_id, H5S_SELECT_SET, offset_name, nullptr, count_name, nullptr);

    Calibration_curve<double> sr_current_curve(STR_SR_CURRENT);
    Calibration_curve<double> us_ic_curve(STR_US_IC);
    Calibration_curve<double> ds_ic_curve(STR_DS_IC);

    for (hsize_t el_idx = 0; el_idx < dims_out[1]; el_idx++)
    {
        offset[1] = el_idx;
        offset_name[1] = el_idx;
        memset(&tmp_name[0], 0, 254);
        H5Sselect_hyperslab(channels_dspace_id, H5S_SELECT_SET, offset_name, nullptr, count_name, nullptr);
        error = H5Dread(channels_dset_id, memtype, memoryspace_name_id, channels_dspace_id, H5P_DEFAULT, (void*)&tmp_name[0]);
        std::string el_name = std::string(tmp_name);
        if (sr_dset_id > -1)
        {
            H5Sselect_hyperslab(sr_dspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
            sr_error = H5Dread(sr_dset_id, H5T_NATIVE_DOUBLE, memoryspace_id, sr_dspace_id, H5P_DEFAULT, (void*)&sr_values[0]);
        }
        if (us_dset_id > -1)
        {
            H5Sselect_hyperslab(us_dspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
            us_error = H5Dread(us_dset_id, H5T_NATIVE_DOUBLE, memoryspace_id, us_dspace_id, H5P_DEFAULT, (void*)&us_values[0]);
        }
        if (ds_dset_id > -1)
        {
            H5Sselect_hyperslab(ds_dspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
            ds_error = H5Dread(ds_dset_id, H5T_NATIVE_DOUBLE, memoryspace_id, ds_dspace_id, H5P_DEFAULT, (void*)&ds_values[0]);
        }
        if (error > -1 && sr_error > -1)
        {
            sr_current_curve.calib_curve[el_name] = sr_values[0];
            sr_current_curve.calib_curve[el_name + "_L"] = sr_values[1];
            sr_current_curve.calib_curve[el_name + "_M"] = sr_values[2];
        }
        if (error > -1 && us_error > -1)
        {
            us_ic_curve.calib_curve[el_name] = us_values[0];
            us_ic_curve.calib_curve[el_name + "_L"] = us_values[1];
            us_ic_curve.calib_curve[el_name + "_M"] = us_values[2];
        }
        if (error > -1 && ds_error > -1)
        {
            ds_ic_curve.calib_curve[el_name] = ds_values[0];
            ds_ic_curve.calib_curve[el_name + "_L"] = ds_values[1];
            ds_ic_curve.calib_curve[el_name + "_M"] = ds_values[2];
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
    H5Sclose(us_dspace_id);
    H5Dclose(us_dset_id);
    H5Sclose(ds_dspace_id);
    H5Dclose(ds_dset_id);
    H5Sclose(sr_dspace_id);
    H5Dclose(sr_dset_id);
    H5Gclose(grp_id);


    return true;
}

/*---------------------------------------------------------------------------*/

bool MapsH5Model::_load_quantification_10(hid_t maps_grp_id)
{
    _load_quantification_10_single(maps_grp_id, QUANT_V10_LOC_MATRIX_STR, _quant_map_matrix);
    _load_quantification_10_single(maps_grp_id, QUANT_V10_LOC_NNLS_STR, _quant_map_nnls);
    _load_quantification_10_single(maps_grp_id, QUANT_V10_LOC_ROI_STR, _quant_map_roi);
    return true;
}

/*---------------------------------------------------------------------------*/

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
        logE << "Error opening group /MAPS/Scalers\n";
        return false;
    }

    counts_dset_id = H5Dopen(sub_grp_id, "Values", H5P_DEFAULT);
    if (counts_dset_id < 0)
    {
        H5Gclose(sub_grp_id);
        logE << "Error opening group /MAPS/Scalers/Values\n";
        return false;
    }
    counts_dspace_id = H5Dget_space(counts_dset_id);

    channels_dset_id = H5Dopen(sub_grp_id, "Names", H5P_DEFAULT);
    if (channels_dset_id < 0)
    {
        H5Sclose(counts_dspace_id);
        H5Dclose(counts_dset_id);
        H5Gclose(sub_grp_id);
        logE << "Error opening group /MAPS/Scalers/Names\n";
        return false;
    }
    channels_dspace_id = H5Dget_space(channels_dset_id);


    int rank = H5Sget_simple_extent_ndims(counts_dspace_id);
    if (rank != 3)
    {
        logE << "Error getting rank for /MAPS/Scalers/Values\n";
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
        if (error > -1)
        {
            std::string el_name = std::string(tmp_name);
            _scalers.emplace(std::pair<std::string, EMatrixF>(el_name, EMatrixF()));
            _scalers.at(el_name).resize(count[1], count[2]);

            H5Sselect_hyperslab(counts_dspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
            error = H5Dread(counts_dset_id, H5T_NATIVE_FLOAT, memoryspace_id, counts_dspace_id, H5P_DEFAULT, (void*)(_scalers.at(el_name).data()));
        }
    }

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

/*---------------------------------------------------------------------------*/

bool MapsH5Model::_load_scan_10(hid_t maps_grp_id)
{
    hid_t desc_id, name_id, unit_id, val_id, x_id, y_id;
    std::string extra_pvs_desc = "Scan/Extra_PVs/Description";
    std::string extra_pvs_name = "Scan/Extra_PVs/Names";
    std::string extra_pvs_unit = "Scan/Extra_PVs/Unit";
    std::string extra_pvs_val = "Scan/Extra_PVs/Values";
    std::string x_axis_loc = "Scan/x_axis";
    std::string y_axis_loc = "Scan/y_axis";
    hsize_t offset[1] = { 0, };
    hsize_t count[1] = { 1 };
    hid_t   filetype, memtype, status;
    char tmp_name[255];

    desc_id = H5Dopen(maps_grp_id, extra_pvs_desc.c_str(), H5P_DEFAULT);
    name_id = H5Dopen(maps_grp_id, extra_pvs_name.c_str(), H5P_DEFAULT);
    unit_id = H5Dopen(maps_grp_id, extra_pvs_unit.c_str(), H5P_DEFAULT);
    val_id = H5Dopen(maps_grp_id, extra_pvs_val.c_str(), H5P_DEFAULT);
    x_id = H5Dopen(maps_grp_id, x_axis_loc.c_str(), H5P_DEFAULT);
    y_id = H5Dopen(maps_grp_id, y_axis_loc.c_str(), H5P_DEFAULT);

    filetype = H5Tcopy(H5T_C_S1);
    H5Tset_size(filetype, 256);
    memtype = H5Tcopy(H5T_C_S1);
    status = H5Tset_size(memtype, 255);

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
            }
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

/*---------------------------------------------------------------------------*/

bool MapsH5Model::_load_integrated_spectra_10(hid_t file_id)
{
    hid_t dset_id;
    hid_t memoryspace_id, error;
    hsize_t offset[1] = { 0,};
    hsize_t count[1] = { 1 };

    std::string max_path = "/MAPS/Spectra/" + STR_INT_SPEC + "/" + STR_MAX_CHANNELS_INT_SPEC;
    std::string max_10_path = "/MAPS/Spectra/" + STR_INT_SPEC + "/" + STR_MAX10_INT_SPEC;

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

/*---------------------------------------------------------------------------*/

bool MapsH5Model::_load_counts_10(hid_t maps_grp_id)
{

    hid_t analyzed_grp_id;

    std::string analyzed_groups[] = {"Fitted", "NNLS", "ROI", "SVD"};

    analyzed_grp_id = H5Gopen(maps_grp_id, "XRF_Analyzed", H5P_DEFAULT);
    if(analyzed_grp_id < 0)
    {
        logE<<"Error opening group /MAPS/XRF_Analyzed\n";
        return false;
    }

    for( std::string group_name : analyzed_groups )
    {
        _load_analyzed_counts_10(analyzed_grp_id, group_name);
    }

    H5Gclose(analyzed_grp_id);

    return true;
}

/*---------------------------------------------------------------------------*/

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
        logE<<"Error opening group /MAPS/XRF_Analyzed/"<<group_name.c_str() << "\n";
        return false;
    }

    counts_dset_id = H5Dopen(sub_grp_id, "Counts_Per_Sec", H5P_DEFAULT);
    if(counts_dset_id < 0)
    {
        H5Gclose(sub_grp_id);
        logE<<"Error opening group /MAPS/XRF_Analyzed/"<<group_name.c_str()<<"/Counts_Per_Sec\n";
        return false;
    }
    counts_dspace_id = H5Dget_space(counts_dset_id);

    channels_dset_id = H5Dopen(sub_grp_id, "Channel_Names", H5P_DEFAULT);
    if(channels_dset_id < 0)
    {
        H5Sclose(counts_dspace_id);
        H5Dclose(counts_dset_id);
        H5Gclose(sub_grp_id);
        logE<<"Error opening group /MAPS/XRF_Analyzed/"<<group_name.c_str()<<"/Channel_Names\n";
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

    count[0] = 1;

    data_struct::Fit_Count_Dict<float> *xrf_counts = new data_struct::Fit_Count_Dict<float>();
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
        xrf_counts->emplace(std::pair<std::string,EMatrixF>(el_name, EMatrixF() ));
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
    H5Gclose(sub_grp_id);

    //nan to 0.f
    for(auto& itr : *xrf_counts)
    {
        itr.second = itr.second.unaryExpr([](float v) { return std::isfinite(v)? v : 0.0f; });
    }

    return true;
}

/*---------------------------------------------------------------------------*/

bool MapsH5Model::_load_roi_10(const std::vector<QPoint>& roi_list, data_struct::Spectra<double>& spec)
{

    return false;
}

/*---------------------------------------------------------------------------*/

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

/*---------------------------------------------------------------------------*/

void MapsH5Model::generateNameLists(QString analysis_type, std::vector<std::string> &names)
{

    data_struct::Fit_Count_Dict<float> fit_counts;
    getAnalyzedCounts(analysis_type.toStdString(), fit_counts);
    std::map<std::string, data_struct::ArrayXXr<float>>* scalers = getScalers();
    if (scalers != nullptr)
    {
        for (auto& itr : *scalers)
        {
            fit_counts.insert(itr);
        }
    }

    //create save ordered vector by element Z number with K , L, M lines
    std::vector<std::string> element_lines;
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

    std::vector<std::string> final_counts_to_add_before_scalers;
    final_counts_to_add_before_scalers.push_back(STR_COHERENT_SCT_AMPLITUDE);
    final_counts_to_add_before_scalers.push_back(STR_COMPTON_AMPLITUDE);
    final_counts_to_add_before_scalers.push_back(STR_SUM_ELASTIC_INELASTIC_AMP);
    final_counts_to_add_before_scalers.push_back(STR_TOTAL_FLUORESCENCE_YIELD);
    final_counts_to_add_before_scalers.push_back(STR_NUM_ITR);
    final_counts_to_add_before_scalers.push_back(STR_RESIDUAL);

    std::vector<std::string> scalers_to_add_first;
    scalers_to_add_first.push_back(STR_SR_CURRENT);
    scalers_to_add_first.push_back(STR_US_IC);
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

/*---------------------------------------------------------------------------*/