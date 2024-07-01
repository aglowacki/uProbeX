/*-----------------------------------------------------------------------------
 * Copyright (c) 2013, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef RULERGRAPHICSITEM_H
#define RULERGRAPHICSITEM_H

//---------------------------------------------------------------------------

#include "LineGraphicsItem.h"

//---------------------------------------------------------------------------

namespace gstar
{

/**
 * @brief The RulerGraphicsItem class
 */
class RulerGraphicsItem : public LineGraphicsItem
{
public:

    /**
    * @brief RulerGraphicsItem
    * @param parent
    */
   RulerGraphicsItem(AbstractGraphicsItem* parent = 0);

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
    * @brief m_length
    */
   AnnotationProperty* m_length;

};

}

//---------------------------------------------------------------------------

#endif // RULERGRAPHICSITEM_H

//---------------------------------------------------------------------------
