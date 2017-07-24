/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef COLOR_LEVELS_PROCESS_STAGE_H
#define COLOR_LEVELS_PROCESS_STAGE_H

/*---------------------------------------------------------------------------*/

#include <proc/ProcessStage.h>

#include <ColorTransferFunction.h>
#include <Array.h>

#include <QList>

using gstar::ColorTransferFunction;

class Histogram;
/*---------------------------------------------------------------------------*/

/**
 * @brief This stage happens after we get the image from the input buffer and
 * then write the results to the output buffer, you can get relative data from
 * the array, such as the min and max value. You can set the transfer function
 * to the image in the buffer.
 */
class ColorLevelsProcessStage
        : public ProcessStage
{

public:

    /**
    * Constructor.
    */
    ColorLevelsProcessStage();

    /**
    * Destructor.
    */
    ~ColorLevelsProcessStage();

    /**
    * Process each Array from the input buffer and write results to the
    * output buffer.
    *
    * @param input - the input Array to process
    * @param output - the output Array in to which results should be written
    */
    void execute();

    /**
    * Get the minimum value in the latest image array
    */
    double getImageMin();

    /**
    * Get the maximum value in the latest image array
    */
    double getImageMax();

    /**
    * Get the minimum value associated with the data type
    * of the most recent image
    */
    double getRangeMin();

    /**
    * Get the maximum value associated with the data type
    * of the most recent image
    */
    double getRangeMax();

    /**
    * Set the transfer function to use while displaying color
    */
    void setColorTransferFunction(ColorTransferFunction ctf);

    /**
    * Set histogram.
    */
    void setHistogram(Histogram* hist);

    /**
    * Set input buffer.
    */
    void setInputBuffer(gstar::Array* input);

    /**
    * Update.
    */
    void update();

private:
    /**
    * Set overall data range based on the data type
    */
    void setRangeMinMax(gstar::Array::DataType type);

    /**
    * Set the min and max value found in the data
    */
    void setDataMinMax(gstar::Array* input);

    /**
    * Get buffer integer value.
    */
    int getBufferIntegerValue(uchar * buffer,
                              long offset,
                              gstar::Array::DataType type);

    /**
    * Get color image.
    */
    void getColorImage(ColorTransferFunction ctf,
                       unsigned char* grayImage,
                       unsigned char* clrImage,
                       int width,
                       int height,
                       int type);

private:
    /**
    * Color transfer function.
    */
    ColorTransferFunction m_ctf;

    /**
    * Min range.
    */
    double m_rangeMin;

    /**
    * Max range.
    */
    double m_rangeMax;

    /**
    * Min image.
    */
    double m_imageMin;

    /**
    * Max image.
    */
    double m_imageMax;

    /**
    * Input buffer passed in by the source stage.
    */
    gstar::Array* m_inputBuffer;

    /**
    * Output buffer produced by this class.
    */
    gstar::Array* m_outputBuffer;

    /**
    * Histogram.
    */
    Histogram* m_histogram;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
