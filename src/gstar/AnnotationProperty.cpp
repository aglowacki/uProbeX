/*-----------------------------------------------------------------------------
 * Copyright (c) 2013, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include "gstar/AnnotationProperty.h"

using namespace gstar;

/*---------------------------------------------------------------------------*/

AnnotationProperty::AnnotationProperty() : QObject()
{

}

/*---------------------------------------------------------------------------*/

AnnotationProperty::AnnotationProperty(QString name) : QObject()
{

   m_displayName = name;

}

/*---------------------------------------------------------------------------*/

AnnotationProperty::AnnotationProperty(QString name, QVariant value) : QObject()
{

   m_displayName = name;
   m_value = value;

}

/*---------------------------------------------------------------------------*/

QString AnnotationProperty::getName() const
{

   return m_displayName;

}

/*---------------------------------------------------------------------------*/

QVariant AnnotationProperty::getValue() const
{

   return m_value;

}

/*---------------------------------------------------------------------------*/

void AnnotationProperty::setName(const QString name)
{

   m_displayName = name;

}

/*---------------------------------------------------------------------------*/

void AnnotationProperty::setValue(const QVariant value)
{

   m_value = value;
   emit valueChanged();

}

/*---------------------------------------------------------------------------*/
