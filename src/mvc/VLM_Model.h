/*-----------------------------------------------------------------------------
 * Copyright (c) 2021, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef VLM_MODEL_H
#define VLM_MODEL_H

//---------------------------------------------------------------------------

#include <QString>

#include <mvc/AbstractWindowModel.h>
#include <gstar/CoordinateModel.h>

//---------------------------------------------------------------------------

/**
 * @brief Model for SWS workspace. Used with SWSWidget
 */
class VLM_Model : public AbstractWindowModel
{

public:

    /**
    * Constructor
    */
    VLM_Model();

    /**
    * Destructor
    */
    virtual ~VLM_Model();

    /**
     * @brief load
     * @param filepath
     * @return
     */
    virtual bool load(QString filepath) = 0;

    void save_xml(QString filename);

    virtual bool loaded() = 0;

    /**
     * @brief getFilePath
     * @return
     */
    QString getFilePath(){return _pathFile;}

    QString getDataPath() { return _datasetPath; }
    
    gstar::CoordinateModel* getCoordModel();

	const QList< QMap<QString, QString> >& getMarkers() { return _markersLoaded; }

	const QList< QMap<QString, QString> >& getRegionMarkers() {return _regionMarkersLoaded;}

    void add_calib_marker(const QMap<QString, QString>& marker);

    void add_region_marker(const QMap<QString, QString>& marker);

protected:
    
    virtual void _initializeCoordModel() = 0;
    
	void _load_xml_markers_and_regions();
    /*
	void _check_and_load_autosave();
    */
	QMap<QString, QString> _parseMarker(QXmlStreamReader& xml);

	QMap<QString, QString> _parseRegionMarker(QXmlStreamReader& xml);
    
    gstar::CoordinateModel* _coord_model;
    
    QString _datasetPath;
    
    QString _pathFile;

	QList< QMap<QString, QString> > _markersLoaded;

	QList< QMap<QString, QString> > _regionMarkersLoaded;

};


//---------------------------------------------------------------------------

#endif /* VLM_Model_H_ */

//---------------------------------------------------------------------------
