/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef SWS_MODEL_H
#define SWS_MODEL_H

/*---------------------------------------------------------------------------*/

#include <mvc/AbstractWindowModel.h>
#include <mvc/TIFFModel.h>

#include <QSettings>
#include <QString>

#include <gstar/CoordinateModel.h>

/*---------------------------------------------------------------------------*/

/**
 * @brief Model for SWS workspace. Used with SWSWidget
 */
class SWSModel : public AbstractWindowModel
{

public:

    /**
    * Constructor
    */
    SWSModel();

    /**
    * Destructor
    */
    ~SWSModel();

    /**
     * @brief load
     * @param filepath
     * @return
     */
    bool load(QString filepath);

    /**
     * @brief getFilePath
     * @return
     */
    QString getFilePath(){return m_pathFile;}

    /**
    * @brief getCoordModel
    * @return
    */
    gstar::CoordinateModel* getCoordModel();

    /**
    * @brief Get data type
    */
    int getPixelByteSize();

    /**
    * @brief Get tiff path
    */
    QString getDataPath();

    /**
    * @brief Get image dims
    * @param imageIndex
    */
    int getImageDims(int imageIndex);

    /**
    * @brief Get total number of images in the data-set.
    */
    int getNumberOfImages();

    /**
    * @brief Get the total number of dimensions in the dataset.
    */
    int getRank();

    virtual uchar* getBytes() { return m_tiffModel->getBytes(); }

    virtual QImage* getImage() { return m_tiffModel->getImage(); }
    /**
    * @brief tiffLoaded
    * @return whether the tiff is loaded
    */
    bool tiffLoaded();

protected:

    /**
    * @brief initializeCoordModel
    */
    void initializeCoordModel();

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
    * @brief m_model
    */
    gstar::CoordinateModel* m_model;

    /**
    * @brief m_datasetPath
    */
    QString m_datasetPath;

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
    * Opened by the playback window.
    */
    QString m_pathFile;

    /**
    * @brief m_pmgScale
    */
    double m_pmgScale;

    /**
    * @brief m_pmgImageReduction
    */
    double m_pmgImageReduction;

    /**
    * Tiff model
    */
    TIFFModel *m_tiffModel;

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
