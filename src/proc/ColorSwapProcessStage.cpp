/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 * Created: Mar 25 2014
 * Author: keyue
 *---------------------------------------------------------------------------*/

#include <proc/ColorSwapProcessStage.h>
#include <proc/Histogram.h>
#include <proc/FinalStage.h>

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <QDateTime>

#include <QDebug>
#include <QString>

using gstar::Array;

/*---------------------------------------------------------------------------*/

ColorSwapProcessStage::ColorSwapProcessStage()
: ProcessStage(), m_firstColor(2), m_secondColor(1), m_thirdColor(0)
{

   m_inputBuffer = NULL;
   m_outputBuffer = NULL;

}

/*---------------------------------------------------------------------------*/

ColorSwapProcessStage::~ColorSwapProcessStage()
{

   if (m_outputBuffer != NULL)
   {
      delete m_outputBuffer;
      m_outputBuffer = NULL;
   }

}

/*---------------------------------------------------------------------------*/

void ColorSwapProcessStage::execute()
{

   if (m_inputBuffer == NULL)
      return;

   // Get rank
   unsigned long rank = m_inputBuffer->getRank();
   // Get type
   Array::DataType type = m_inputBuffer->getType();

   // Get dimensions
   unsigned long long dims[2];
   m_inputBuffer->getDims(dims, 2);

   // Check image properties; only process 32 bit RGB images
   if (rank != 2) {
      return;
   }
   if (dims[0] <= 0 || dims[1] <= 0) {
      return;
   }
   if (type != Array::BGRA32) {
      return;
   }

   // Creating the output buffer
   unsigned long long size = dims[0] * dims[1] * 4;
   if (m_outputBuffer == NULL)
   {
      m_outputBuffer = new Array(size);
   }
   else
   {
      // Check if our existing buffer is big enough to hold the new image.
      if (m_outputBuffer->getBytes() < size) {
         delete m_outputBuffer;
         m_outputBuffer = new Array(size);
      }
   }

   // Set output buffer parameters
   m_outputBuffer->setRank(2);
   m_outputBuffer->setType(Array::BGRA32);
   m_outputBuffer->setDimensions(dims, 2);

   // Convert here
   uchar* inBuffer = m_inputBuffer->getBuffer();
   uchar* outBuffer = m_outputBuffer->getBuffer();

   getColorImage(inBuffer, outBuffer, dims[0], dims[1], type);

   // TODO: Refactor next stage code to a parent class.
   // pass the data to next-stage if any
   Stage* next = this->getNextStage();
   if (next == NULL)
      return;

   int t = next->getStageType();
   switch (t)
   {
   case Stage::STAGE_PROCESS:
   {
      ProcessStage* pstage = dynamic_cast<ProcessStage*> (next);
      if (pstage != NULL)
      {
         pstage->setInputBuffer(m_outputBuffer);
         pstage->execute();
      }
      break;
   }

   case Stage::STAGE_FINAL:
   {
      FinalStage* fstage = (FinalStage*) next;
      fstage->setInputBuffer(m_outputBuffer);
      fstage->execute();
      break;
   }
   default:
      break;
   }

}

/*---------------------------------------------------------------------------*/

void ColorSwapProcessStage::getColorImage(unsigned char* origImage,
                                          unsigned char* swapImage,
                                          int width,
                                          int height,
                                          int type)
{

   Q_UNUSED(type);

   if (swapImage == NULL || origImage == NULL) return;

   for (unsigned int r = 0 ; r < (unsigned int)height ; r++)
   {
      for (unsigned int c = 0 ; c < (unsigned int)width ; c++)
      {
         int index = (r * width + c);
         swapImage[(index * 4) + 0] = origImage[(index * 4) + m_firstColor];
         swapImage[(index * 4) + 1] = origImage[(index * 4) + m_secondColor];
         swapImage[(index * 4) + 2] = origImage[(index * 4) + m_thirdColor];
         swapImage[(index * 4) + 3] = 255;
      }
   }

}

/*---------------------------------------------------------------------------*/

Array* ColorSwapProcessStage::getOutputBuffer()
{

   return m_outputBuffer;

}

/*---------------------------------------------------------------------------*/

void ColorSwapProcessStage::setInputBuffer(Array* buffer)
{

   m_inputBuffer = buffer;

}

/*---------------------------------------------------------------------------*/

void ColorSwapProcessStage::setRGBSequence(int first, int second, int third)
{

   m_firstColor = first;
   m_secondColor = second;
   m_thirdColor = third;
}

/*---------------------------------------------------------------------------*/

void ColorSwapProcessStage::update()
{

   if (getParentStage() != NULL)
   {
      getParentStage()->update();
   }

}

/*---------------------------------------------------------------------------*/

