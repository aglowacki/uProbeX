/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include "gstar/CoordinateWidget.h"

using namespace gstar;
//---------------------------------------------------------------------------

CoordinateWidget::CoordinateWidget(bool vertical, QWidget* parent)
: QWidget(parent)
{

   m_model = nullptr;
   m_lblPixelXCoordinate = nullptr;
   m_lblPixelYCoordinate = nullptr;

   m_lblXUnits = new QLabel("");
//   m_lblXUnits->setMinimumWidth(40);

   m_lblYUnits = new QLabel("");
//   m_lblYUnits->setMinimumWidth(40);

   m_lblZUnits = new QLabel("");
//   m_lblZUnits->setMinimumWidth(40);

   //Add status bar underneath
   m_lblPixelXCoordinate = new QLabel(tr("0"));
   m_lblPixelXCoordinate->setMinimumWidth(50);
   m_lblPixelYCoordinate = new QLabel(tr("0"));
   m_lblPixelYCoordinate->setMinimumWidth(50);
   m_lblPixelZCoordinate = new QLabel(tr("0"));
   m_lblPixelZCoordinate->setMinimumWidth(50);

   m_lblPixelXLabel = new QLabel(" X Coordinate :");
//   m_lblPixelXLabel->setMinimumWidth(40);
   m_lblPixelYLabel = new QLabel(" Y Coordinate :");
//   m_lblPixelYLabel->setMinimumWidth(40);
   m_lblPixelZLabel = new QLabel(" Z Coordinate :");
//   m_lblPixelZLabel->setMinimumWidth(40);
   if (false == vertical)
   {
       QHBoxLayout* coordHlayout = new QHBoxLayout();
       coordHlayout->addWidget(m_lblPixelXLabel);
       coordHlayout->addWidget(m_lblPixelXCoordinate);
       coordHlayout->addWidget(m_lblXUnits);
       coordHlayout->addWidget(m_lblPixelYLabel);
       coordHlayout->addWidget(m_lblPixelYCoordinate);
       coordHlayout->addWidget(m_lblYUnits);
       coordHlayout->addWidget(m_lblPixelZLabel);
       coordHlayout->addWidget(m_lblPixelZCoordinate);
       coordHlayout->addWidget(m_lblZUnits);
       coordHlayout->addStretch(1);

       coordHlayout->setContentsMargins(QMargins(0, 0, 0, 0));

       setLayout(coordHlayout);
   }
   else
   {
       QVBoxLayout* coordVlayout = new QVBoxLayout();
       QHBoxLayout* coordHlayout1 = new QHBoxLayout();
       coordHlayout1->addWidget(m_lblPixelXLabel);
       coordHlayout1->addWidget(m_lblPixelXCoordinate);
       coordHlayout1->addWidget(m_lblXUnits);
       //coordHlayout1->addStretch(1);
       QHBoxLayout* coordHlayout2 = new QHBoxLayout();
       coordHlayout2->addWidget(m_lblPixelYLabel);
       coordHlayout2->addWidget(m_lblPixelYCoordinate);
       coordHlayout2->addWidget(m_lblYUnits);
       //coordHlayout2->addStretch(1);
       QHBoxLayout* coordHlayout3 = new QHBoxLayout();
       coordHlayout3->addWidget(m_lblPixelZLabel);
       coordHlayout3->addWidget(m_lblPixelZCoordinate);
       coordHlayout3->addWidget(m_lblZUnits);
       //coordHlayout3->addStretch(1);

       coordHlayout1->setContentsMargins(QMargins(0, 0, 0, 0));
       coordHlayout2->setContentsMargins(QMargins(0, 0, 0, 0));
       coordHlayout3->setContentsMargins(QMargins(0, 0, 0, 0));

       coordVlayout->addItem(coordHlayout1);
       coordVlayout->addItem(coordHlayout2);
       coordVlayout->addItem(coordHlayout3);
       coordVlayout->setContentsMargins(QMargins(0, 0, 0, 0));
       setLayout(coordVlayout);
   }
//   m_decimalPreci = 2;
}

//---------------------------------------------------------------------------

CoordinateWidget::~CoordinateWidget()
{

}

//---------------------------------------------------------------------------

void CoordinateWidget::setLabel(QString xLabel, QString yLabel)
{

   m_lblPixelXLabel->setText(xLabel);
   m_lblPixelYLabel->setText(yLabel);

}

//---------------------------------------------------------------------------

void CoordinateWidget::setLabel(QString xLabel, QString yLabel, QString zLabel)
{

   m_lblPixelXLabel->setText(xLabel);
   m_lblPixelYLabel->setText(yLabel);
   m_lblPixelZLabel->setText(zLabel);

}

//---------------------------------------------------------------------------

void CoordinateWidget::setVisible(bool xVisible, bool yVisible, bool zVisible)
{

    m_lblPixelXLabel->setVisible(xVisible);
    m_lblPixelXCoordinate->setVisible(xVisible);
    m_lblXUnits->setVisible(xVisible);

    m_lblPixelYLabel->setVisible(yVisible);
    m_lblPixelYCoordinate->setVisible(yVisible);
    m_lblYUnits->setVisible(yVisible);

    m_lblPixelZLabel->setVisible(zVisible);
    m_lblPixelZCoordinate->setVisible(zVisible);
    m_lblZUnits->setVisible(zVisible);


}

//---------------------------------------------------------------------------

void CoordinateWidget::setCoordinate(int x, int y)
{

   if(m_model != nullptr)
   {
      double dx = 0.0;
      double dy = 0.0;
      double dz = 0.0;
      m_model->runTransformer(x, y, 0.0, &dx, &dy, &dz);

      m_lblPixelXCoordinate->setText(QString::number(dx));
      m_lblPixelYCoordinate->setText(QString::number(dy));
   }
   else
   {
      m_lblPixelXCoordinate->setText(QString::number(x));
      m_lblPixelYCoordinate->setText(QString::number(y));
   }

}

//---------------------------------------------------------------------------

void CoordinateWidget::setCoordinate(double x, double y, double z)
{

   if(m_model != nullptr)
   {
      double dx = 0.0;
      double dy = 0.0;
      double dz = 0.0;
      m_model->runTransformer(x, y, z, &dx, &dy, &dz);
      m_lblPixelXCoordinate->setText(QString::number(dx));
      m_lblPixelYCoordinate->setText(QString::number(dy));
      m_lblPixelZCoordinate->setText(QString::number(dz));
   }
   else
   {
       m_lblPixelXCoordinate->setText(QString::number(x));
       m_lblPixelYCoordinate->setText(QString::number(y));
       m_lblPixelZCoordinate->setText(QString::number(z));
   }

}

//---------------------------------------------------------------------------

void CoordinateWidget::setModel(CoordinateModel* model)
{

   m_model = model;

}

//---------------------------------------------------------------------------

void CoordinateWidget::setnullptr()
{

   m_lblPixelXCoordinate->setText("");
   m_lblPixelYCoordinate->setText("");
   m_lblPixelZCoordinate->setText("");

}

//---------------------------------------------------------------------------

void CoordinateWidget::setUnitsLabel(QString units)
{

   m_lblXUnits->setText(units);
   m_lblYUnits->setText(units);
   m_lblZUnits->setText(units);

}

//---------------------------------------------------------------------------


