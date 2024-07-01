/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef SOLVER_WIDGET_H
#define SOLVER_WIDGET_H

#include <QWidget>
#include <QVector>
#include <QMap>
#include <QPushButton>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>

//---------------------------------------------------------------------------

/**
 * @brief Widget that will show the old and new value of the solver
 *
 */ 

class SolverWidget : public QWidget
{
    Q_OBJECT

public:

   /**
    * Default constructor
    */
   SolverWidget(QWidget *parent = 0);

   /**
    * Default destructor
    */
   ~SolverWidget();

   /**
    * @brief setCoefs
    * @param oldVals
    * @param newVals
    */
   void setCoefs(QMap<QString, double> oldVals, QMap<QString, double> newVals);

   /**
    * @brief setStatusString
    * @param val
    */
   void setStatusString(QString val);

   /**
    * @brief setUseBtnEnabled
    * @param val
    */
   void setUseBtnEnabled(bool val);

signals:

   /**
    * @brief updateVariables
    */
   void useUpdatedVariables(const QMap<QString, double> vars);

   /**
    * @brief cancelUpdatedVariables
    */
   void cancelUpdatedVariables();

private slots:

   /**
    * @brief cancel
    */
   void cancel();

   /**
    * @brief useNewValues
    */
   void useNewValues();

protected:

   /**
    * @brief appendVariable
    * @param row
    * @param name
    */
   void appendVariable(int row, QString name, double original, double newVal);

private:

   /**
    * @brief Add first link button.
    */
   QPushButton* m_btnCancel;

   /**
    * @brief m_btnUse
    */
   QPushButton* m_btnUse;

   /**
    * @brief m_gridLayout
    */
   QGridLayout* m_gridLayout;

   /**
    * @brief m_lblStatus
    */
   QLabel* m_lblStatus;

   /**
    * @brief m_oldCoefs
    */
   QMap<QString, double> m_oldCoefs;

   /**
    * @brief m_newCoefs
    */
   QMap<QString, double> m_newCoefs;

};

//---------------------------------------------------------------------------

#endif // CHAIN_LOOP_WIDGET_H

//---------------------------------------------------------------------------
