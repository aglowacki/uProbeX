/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef DISPLAY_STAGE_H
#define DISPLAY_STAGE_H

/*---------------------------------------------------------------------------*/

#include <QImage>
#include <QObject>

#include <Array.h>
#include <proc/FinalStage.h>

/*---------------------------------------------------------------------------*/

/**
 * @brief After the image is processing, the stage goes to display on the
 * subwindow .
 */
class DisplayStage
: public QObject, public FinalStage
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   DisplayStage(Stage* parent = NULL);

   /**
    * Destructor.
    */
   ~DisplayStage();

   /**
    * Execute.
    */
   void execute();

   /**
    * Set input buffer.
    */
   void setInputBuffer(gstar::Array* buffer);

   /**
    * update.
    */
   void update();

signals:

   /**
    * Signal that a new frame is available
    *
    * @param inputData - image information including raw data and metadata.
    */
   void newFrame(const gstar::Array* inputData);

private:

   /**
    * Input buffer.
    */
   gstar::Array* m_inputBuffer;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
