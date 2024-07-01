/*-----------------------------------------------------------------------------
 * Copyright (c) 2013, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef INTENSITYLINEGRAPHICSITEM_H
#define INTENSITYLINEGRAPHICSITEM_H

//---------------------------------------------------------------------------

#include "LineGraphicsItem.h"

//---------------------------------------------------------------------------

namespace gstar
{

/**
 * @brief The IntensityLineGraphicsItem class
 */
class IntensityLineGraphicsItem : public LineGraphicsItem
{

public:

    /**
    * @brief IntensityLineGraphicsItem
    */
   IntensityLineGraphicsItem();

   /**
    * @brief className
    * @return
    */
   const QString displayName() const;

   /**
    * @brief duplicate
    * @return
    */
   AbstractGraphicsItem* duplicate();

protected slots:

   /**
    * @brief calculate
    */
   void calculate();

protected:

   /**
    * @brief m_intensity
    */
   AnnotationProperty* m_intensity;
};

}

//---------------------------------------------------------------------------

#endif // INTENSITYLINEGRAPHICSMODEL_H

//---------------------------------------------------------------------------
