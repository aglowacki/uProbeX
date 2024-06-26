/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef SOLVERPARAMETERWIDGET_H_
#define SOLVERPARAMETERWIDGET_H_

#include <QWidget>

#include <preferences/SolverTable.h>

//---------------------------------------------------------------------------

/**
 * @brief Widget that has includes coefficient table and option table, which is
 * called coefficient table and option table. It controls that enable, diable
 * of the column editing, hide of the add, delete button.
 */
class SolverParameterWidget
: public QWidget
{

   Q_OBJECT

public:

   /**
    * Constructor
    *
    * @param parent - the Qt parent widget
    */
   SolverParameterWidget(QWidget* parent = 0);

   /**
    * Default destructor
    */
   ~SolverParameterWidget();

   /**
    * @brief addCoefficientItem
    * @param attr
    */
   void addCoefficientItem(Attribute attr);

   /**
    * @brief addCoefficientItems
    * @param solverAttrs
    */
   void addCoefficientItems(QList<Attribute> solverAttrs);

   /**
    * @brief addCoefficientItem
    * @param attr
    */
   void addOptionItem(Attribute attr);

   /**
    * @brief addOptionItems
    * @param solverAttrs
    */
   void addOptionItems(QList<Attribute> solverAttrs);

   /**
    * @brief clearAllCoefficient
    */
   void removeCoefficientItems();


   /**
    * @brief clearAllOption
    */
   void removeOptionItems();

   /**
    * @brief addCoefficientItem
    * @return coefficient
    */
   QStringList getCoefficientAttrs();

   /**
    * @brief getCoefficientAttrsList
    * @return coefficient
    */
   QList<Attribute> getCoefficientAttrsList();

   QMap<QString, double> getCoefficientAttrsMap();

   QMap<QString, double> getSelectedCoefficientAttrsMap();

   /**
    * @brief getOptionAttrs
    * @return options
    */
   QStringList getOptionAttrs();

   /**
    * @brief getOptionAttrsList
    * @return options
    */
   QList<Attribute> getOptionAttrsList();

   QMap<QString, double> getOptionAttrsMap();

   QMap<QString, double> getSelectedOptionAttrsMap();

   /**
    * @brief setButtionVisible
    * @param visible
    */
   void setButtionVisible(bool visible);

   /**
    * @brief setCoefficientAttrs
    * @param attr
    */
   void setCoefficientAttrs(QStringList attrs);

   /**
    * @brief setCoefficientAttrsFromList
    * @param attrList
    */
   void setCoefficientAttrsFromList(QList<Attribute> attrList);

   void setOptionAttrsFromList(QList<Attribute> attrList);

   /**
    * @brief setDefaultCoefficient
    */
   void setDefaultCoefficient();

   /**
    * @brief setDefaultCoefficient
    */
   void setDefaultOption();

   /**
    * @brief setOptionAttrs
    * @param attr
    */
   void setOptionAttrs(QStringList attrs);

   /**
    * setNMModel
    *
    * @param nmModel
    */
   void setNMModel();

signals:

   /**
    * @brief coefficientItemChanged
    */
   void coefficientItemChanged();

   /**
    * @brief optionItemChanged
    */
   void optionItemChanged();

private:

   /**
    * @brief m_coefficientTable
    */
   SolverTable* m_coefficientTable;

   /**
    * @brief m_optionTable
    */
   SolverTable* m_optionTable;



};

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------

