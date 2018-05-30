/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <gstar/RangeWidget.h>

using namespace gstar;
/*---------------------------------------------------------------------------*/

RangeWidget::RangeWidget(QWidget* parent)
: QWidget(parent)
{

   m_btnFirst = new QPushButton();
   m_btnFirst -> setIcon(QIcon(":/images/first.png"));
   m_btnFirst -> setFlat(true);
   m_btnFirst -> setFixedSize(32, 32);
   connect(m_btnFirst, SIGNAL(clicked()), this, SLOT(clickFirst()));

   m_btnPrevious = new QPushButton();
   m_btnPrevious -> setIcon(QIcon(":/images/previous.png"));
   m_btnPrevious -> setFlat(true);
   m_btnPrevious -> setFixedSize(32, 32);
   connect(m_btnPrevious, SIGNAL(clicked()), this, SLOT(clickPrevious()));

   m_spinValue = new QSpinBox();
   m_spinValue -> setButtonSymbols(QAbstractSpinBox::NoButtons);
   m_spinValue -> setFixedWidth(120);
   m_spinValue -> setMinimum(0);
   m_spinValue -> setMaximum(1000);
   m_spinValue -> setValue(0);
   m_spinValue -> setSuffix(tr(" / 1000"));
   connect(m_spinValue, SIGNAL(valueChanged(int)),
           this, SLOT(spinChanged(int)));

   m_btnNext = new QPushButton();
   m_btnNext -> setIcon(QIcon(":/images/next.png"));
   m_btnNext -> setFlat(true);
   m_btnNext -> setFixedSize(32, 32);
   connect(m_btnNext, SIGNAL(clicked()), this, SLOT(clickNext()));

   m_btnLast = new QPushButton();
   m_btnLast -> setIcon(QIcon(":/images/last.png"));
   m_btnLast -> setFlat(true);
   m_btnLast -> setFixedSize(32, 32);
   connect(m_btnLast, SIGNAL(clicked()), this, SLOT(clickLast()));

   QHBoxLayout* layout = new QHBoxLayout();
   layout -> setContentsMargins(0, 0, 0, 0);
   layout -> setSpacing(1);
   layout -> setAlignment(Qt::AlignLeft);
   layout -> addWidget(m_btnFirst);
   layout -> addWidget(m_btnPrevious);
   layout -> addWidget(m_spinValue);
   layout -> addWidget(m_btnNext);
   layout -> addWidget(m_btnLast);

   setLayout(layout);

   m_id = 0;

}

/*---------------------------------------------------------------------------*/

RangeWidget::~RangeWidget()
{

}

/*---------------------------------------------------------------------------*/

void RangeWidget::clickFirst()
{

   int v = m_spinValue -> minimum();
   m_spinValue -> setValue(v);

}

/*---------------------------------------------------------------------------*/

void RangeWidget::clickPrevious()
{

   int v = m_spinValue -> value();
   m_spinValue -> setValue(v - 1);

}

/*---------------------------------------------------------------------------*/

void RangeWidget::clickNext()
{

   int v = m_spinValue -> value();
   m_spinValue -> setValue(v + 1);

}

/*---------------------------------------------------------------------------*/

void RangeWidget::clickLast()
{

   int v = m_spinValue -> maximum();
   m_spinValue -> setValue(v);

}

/*---------------------------------------------------------------------------*/

void RangeWidget::setId(int id)
{

   m_id = id;

}

/*---------------------------------------------------------------------------*/

void RangeWidget::setMaximum(int max)
{

   m_spinValue -> setMaximum(max);
   m_spinValue -> setSuffix(tr(" / %1").arg(max));

}

/*---------------------------------------------------------------------------*/

void RangeWidget::setMinimum(int min)
{

   m_spinValue -> setMinimum(min);

}

/*---------------------------------------------------------------------------*/

void RangeWidget::setValue(int v)
{

   if (v < m_spinValue->minimum() || v > m_spinValue->maximum())
      return;

   m_spinValue->setValue(v);

}

/*---------------------------------------------------------------------------*/

void RangeWidget::spinChanged(int value)
{

   emit valueChanged(value, m_id);

}

/*---------------------------------------------------------------------------*/

int RangeWidget::value()
{

   return m_spinValue -> value();

}

/*---------------------------------------------------------------------------*/

void RangeWidget::setRangewidgetStatus(bool m_rangeEnable)
{
   m_btnFirst -> setEnabled(m_rangeEnable);
   m_btnPrevious -> setEnabled(m_rangeEnable);
   m_spinValue -> setEnabled(m_rangeEnable);
   m_btnNext -> setEnabled(m_rangeEnable);
   m_btnLast -> setEnabled(m_rangeEnable);
}

/*---------------------------------------------------------------------------*/
void RangeWidget::setRangeWidgetVisible(bool m_rangeVisible)
{
   m_btnFirst -> setVisible(m_rangeVisible);
   m_btnPrevious -> setVisible(m_rangeVisible);
   m_spinValue -> setVisible(m_rangeVisible);
   m_btnNext -> setVisible(m_rangeVisible);
   m_btnLast -> setVisible(m_rangeVisible);
}

/*---------------------------------------------------------------------------*/
