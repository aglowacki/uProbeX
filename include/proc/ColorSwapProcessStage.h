/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 * Created: Mar 25 2014
 * Author: keyue
 *---------------------------------------------------------------------------*/

#ifndef COLOR_SWAP_PROCESS_STAGE_H_
#define COLOR_SWAP_PROCESS_STAGE_H_

/*---------------------------------------------------------------------------*/

#include <proc/ProcessStage.h>

#include <ColorTransferFunction.h>
#include <Array.h>

#include <QList>

using gstar::ColorTransferFunction;

class Histogram;
/*---------------------------------------------------------------------------*/

/**
 * @brief This stage happens after we get RGB image from the input buffer and
 * then write the results to the output buffer, you can get relative data from
 * the array, such as the min and max value. You can set the transfer function
 * to the image in the buffer.
 */
class ColorSwapProcessStage
: public ProcessStage
{

public:

   /**
    * Constructor.
    */
   ColorSwapProcessStage();

   /**
    * Destructor.
    */
   ~ColorSwapProcessStage();

   /**
    * Process each Array from the input buffer and write results to the
    * output buffer.
    *
    * @param input - the input Array to process
    * @param output - the output Array in to which results should be written
    */
   void execute();

   /**
    * Set input buffer.
    */
   void setInputBuffer(gstar::Array* input);

   /**
    * Set the sequence of RGB color in the image.
    */
   void setRGBSequence(int first, int second, int third);

   /**
    * Update.
    */
   void update();

private:

   /**
    * Get buffer integer value.
    */
   int getBufferIntegerValue(uchar * buffer,
                             long offset,
                             gstar::Array::DataType type);

   /**
    * Get color image.
    */
   void getColorImage(unsigned char* origImage,
                      unsigned char* swapImage,
                      int width,
                      int height,
                      int type);

private:

   /**
    * Input buffer passed in by the source stage.
    */
   gstar::Array* m_inputBuffer;

   /**
    * Output buffer produced by this class.
    */
   gstar::Array* m_outputBuffer;

   /**
    * First color in the RGB image.
    */
   int m_firstColor;

   /**
    * Second color in the RGB image.
    */
   int m_secondColor;

   /**
    * Third color in the RGB image.
    */
   int m_thirdColor;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
