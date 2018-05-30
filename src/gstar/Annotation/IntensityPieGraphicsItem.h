/*-----------------------------------------------------------------------------
 * Copyright (c) 2013, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef INTENSITYPIEGRAPHICSITEM_H
#define INTENSITYPIEGRAPHICSITEM_H

/*---------------------------------------------------------------------------*/

#include "PieGraphicsItem.h"

/*---------------------------------------------------------------------------*/

namespace gstar
{

/**
 * @brief The IntensityPieGraphicsItem class
 */
class IntensityPieGraphicsItem : public PieGraphicsItem
{

public:

    /**
    * @brief IntensityPieGraphicsItem
    */
   IntensityPieGraphicsItem();

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

/*---------------------------------------------------------------------------*/

#endif // INTENSITYPIEGRAPHICSMODEL_H

/*---------------------------------------------------------------------------*/
