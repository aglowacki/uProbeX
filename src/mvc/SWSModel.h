/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef SWS_MODEL_H
#define SWS_MODEL_H

/*---------------------------------------------------------------------------*/

#include <mvc/VLM_Model.h>

#include <QSettings>
#include <QString>

#include <gstar/CoordinateModel.h>

/*---------------------------------------------------------------------------*/

/**
 * @brief Model for SWS workspace. Used with SWSWidget
 */
class SWSModel : public VLM_Model
{

public:

    /**
    * Constructor
    */
    SWSModel();

    /**
    * Destructor
    */
    virtual ~SWSModel();

    /**
     * @brief load
     * @param filepath
     * @return
     */
    virtual bool load(QString filepath);

    /**
    * @brief getCoordModel
    * @return
    */
    gstar::CoordinateModel* getCoordModel();

    /**
    * @brief Get data type
    */
    virtual int getPixelByteSize();

    /**
    * @brief Get image dims
    * @param imageIndex
    */
    virtual int getImageDims(int imageIndex);

    /**
    * @brief Get total number of images in the data-set.
    */
    virtual int getNumberOfImages();

    /**
    * @brief Get the total number of dimensions in the dataset.
    */
    virtual int getRank();

    virtual uchar* getBytes();

    virtual QImage* getImage();
    /**
    * @brief tiffLoaded
    * @return whether the tiff is loaded
    */
    virtual bool loaded();

protected:

    /**
    * @brief initializeCoordModel
    */
    virtual void _initializeCoordModel();

    /**
    * @brief loadDirectory
    * @return directory is loaded
    */
    bool loadDirectory();

    /**
    * @brief loadPMG
    * @return PMG is loaded
    */
    bool loadPMG();

    /**
    * @brief loadTiff
    * @return tiff is loaded
    */
    bool loadTiff();

    /**
    * @brief loadTiles
    * @return Tiles is loaded
    */
    bool loadTiles();

	//void load_xml_markers_and_regions();

	void check_and_load_autosave();

    /**
    * @brief loadXYZ
    * @return XYZ is loaded
    */
    bool loadXYZ();

    /**
    * @brief readPMGInt
    * @param pmgFile
    * @param ID
    * @param data
    * @return true or false
    */
    bool readPMGInt(QFile *pmgFile, QString ID, int& data);

    /**
    * @brief readPMGFloatArray
    * @param pmgFile
    * @param ID
    * @param data
    * @param arrSize
    * @return true or false
    */
    bool readPMGDoubleArray(QFile *pmgFile,
                            QString ID,
                            double* data,
                            int arrSize);

    /**
    * @brief readPMGString
    * @param pmgFile
    * @param ID
    * @param data
    * @return true or false
    */
    bool readPMGString(QFile *pmgFile, QString ID, QString& data);

private:

    struct vec2
    {
        double x;
        double y;
    };

    struct SubSample
    {
        unsigned int idx;
        double x;
        double y;
        double z;
        QString path;
        float scale;
        vec2 location;
        vec2 cornerTopLeft;
        vec2 cornerTopRight;
        vec2 cornerBottomRight;
        vec2 cornerBottomLeft;
    };

    /**
    * @brief m_datasetName
    */
    QString m_datasetName;

    /**
    * @brief m_mapValueKey
    */
    QMap<QString, QVariant> m_mapValueKey;

    /**
    * @brief m_numSamples
    */
    unsigned int m_numSamples;

    /**
    * @brief m_numXSamples
    */
    unsigned int m_numXSamples;

    /**
    * @brief m_numYSamples
    */
    unsigned int m_numYSamples;

    /**
    * @brief m_pmgScale
    */
    double m_pmgScale;

    /**
    * @brief m_pmgImageReduction
    */
    double m_pmgImageReduction;

    // For tiff
    QImage _tif_img;

    /**
    * @brief m_tiffLoaded
    */
    bool m_tiffLoaded;

    /**
    * @brief xyzArray
    */
    SubSample *m_samples;
};


/*---------------------------------------------------------------------------*/

#endif /* SWSModel_H_ */

/*---------------------------------------------------------------------------*/
