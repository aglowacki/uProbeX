/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef RULER_UNITS_DIALOG_H
#define RULER_UNITS_DIALOG_H

/*---------------------------------------------------------------------------*/

#include <QDialog>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

/*---------------------------------------------------------------------------*/

/**
 * @brief Dialog for setting Units per pixel.
 */
class RulerUnitsDialog
: public QDialog
{

   Q_OBJECT

public:

   /**
    * @brief RulerUnitsDialog
    * @param parent
    * @param f
    */
   RulerUnitsDialog(QWidget* parent = 0, Qt::WindowFlags f = Qt::Widget);

   /**
    * @brief getUnitLabel
    * @return
    */
   QString getUnitLabel();

   /**
    * @brief getUnitsPerPixelX
    * @return
    */
   double getUnitsPerPixelX();

   /**
    * @brief getUnitsPerPixelY
    * @return
    */
   double getUnitsPerPixelY();

   /**
    * @brief setUnitLabel
    * @param label
    */
   void setUnitLabel(QString label);

   /**
    * @brief setUnitsPerPixelX
    * @param x
    */
   void setUnitsPerPixelX(double x);

   /**
    * @brief setUnitsPerPixelY
    * @param y
    */
   void setUnitsPerPixelY(double y);

private slots:

   /**
    * Overridden accept() method.
    */
   void accept();

   /**
    * Overridden reject() method.
    */
   void reject();

private:

   /**
    * OK button
    */
   QPushButton* m_btnOK;

   /**
    * Cancel button
    */
   QPushButton* m_btnCancel;

   /**
    * Start value
    */
   QLineEdit* m_unitLabel;

   /**
    * End value
    */
   QDoubleSpinBox* m_unitsPerPixelX;

   /**
    * Step size / delta
    */
   QDoubleSpinBox* m_unitsPerPixelY;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
