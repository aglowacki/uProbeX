/*-----------------------------------------------------------------------------
 * Copyright (c) 2013, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef ANNOTATION_PROPERTY_H
#define ANNOTATION_PROPERTY_H

/*---------------------------------------------------------------------------*/

#include <QObject>
#include <QVariant>

#define DEF_STR_DISPLAY_NAME "DisplayName"

namespace gstar
{

/**
 * @brief The AnnotationProperty class
 */
class AnnotationProperty : public QObject
{

   Q_OBJECT

public:

    /**
    * @brief AnnotationProperty
    */
   AnnotationProperty();

   /**
    * @brief AnnotationProperty
    * @param name
    */
   AnnotationProperty(QString name);

   /**
    * @brief AnnotationProperty
    * @param name
    * @param value
    */
   AnnotationProperty(QString name, QVariant value);

   /**
    * @brief getName
    * @return
    */
   QString getName() const;

   /**
    * @brief getValue
    * @return
    */
   QVariant getValue() const;

   /**
    * @brief setName
    * @param name
    */
   void setName(const QString name);

   /**
    * @brief setValue
    * @param value
    */
   void setValue(const QVariant value);

signals:

   /**
    * @brief valueChanged
    */
   void valueChanged();

protected:

   /**
    * @brief m_displayName
    */
   QString m_displayName;

   /**
    * @brief m_value
    */
   QVariant m_value;

};

}

/*---------------------------------------------------------------------------*/

#endif // ANNOTATION_PROPERTY_H

/*---------------------------------------------------------------------------*/
