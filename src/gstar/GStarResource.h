/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef GSTAR_RESOURCE_H
#define GSTAR_RESOURCE_H

//---------------------------------------------------------------------------

#include <string>

//---------------------------------------------------------------------------

/**
 * @brief The GStar resource namespace
 *
 * This is the namespace for all things related to the GStar global resource
 * namespace.
 */
namespace gstar {

   /**
    * @brief UPROBECOLOR
    */
   const static QString UPROBE_COLOR = "Color";

   /**
    * @brief UPROBE_LIGHT_POS_X
    */
   const static QString UPROBE_LIGHT_POS_X = "LX";

   /**
    * @brief UPROBECOLOR
    */
   const static QString UPROBE_LIGHT_POS_Y = "LY";

   /**
    * @brief UPROBE_LIGHT_POS_Y
    */
   const static QString UPROBE_LIGHT_POS_Z = "LZ";

   /**
    * @brief UPROBE_MICRO_POS_X
    */
   const static QString UPROBE_MICRO_POS_X = "MX";

   /**
    * @brief UPROBE_MICRO_POS_Y
    */
   const static QString UPROBE_MICRO_POS_Y = "MY";

   /**
    * @brief UPROBE_PRED_POS_X
    */
   const static QString UPROBE_PRED_POS_X = "PX";

   /**
    * @brief UPROBE_PRED_POS_Y
    */
   const static QString UPROBE_PRED_POS_Y = "PY";

   const static QString UPROBE_CENTER_POS_X = "CenterX";

   const static QString UPROBE_CENTER_POS_Y = "CenterY";

   /**
    * @brief UPROBE_REAL_POS_X
    */
   const static QString UPROBE_REAL_POS_X = "RX";

   /**
    * @brief UPROBE_REAL_POS_Y
    */
   const static QString UPROBE_REAL_POS_Y = "RY";

   /**
    * @brief UPROBE_NAME
    */
   const static QString UPROBE_NAME = "DisplayName";

   /**
    * @brief UPROBE_RECT_TLX
    */
   const static QString UPROBE_RECT_TLX = "TopLeftX";

   /**
    * @brief UPROBE_RECT_TLY
    */
   const static QString UPROBE_RECT_TLY = "TopLeftY";

   /**
    * @brief UPROBE_RECT_W
    */
   const static QString UPROBE_RECT_W = "RectWidth";

   /**
    * @brief UPROBE_RECT_H
    */
   const static QString UPROBE_RECT_H = "RectHeight";

   /**
    * @brief UPROBE_WIDTH
    */
   const static QString UPROBE_WIDTH = "Width";

   /**
    * @brief UPROBE_HEIGHT
    */
   const static QString UPROBE_HEIGHT = "Height";

   /**
    * @brief UPROBE_SIZE
    */
   const static QString UPROBE_SIZE = "Size";


   const static QString SCAN_REGION_QUEUE_STATUS = "Queue Status";

   const static QString SCAN_REGION_PLAN_TYPE = "Plan Type";
};

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
