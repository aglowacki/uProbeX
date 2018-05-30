/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <gstar/ControlPointWidget.h>
#include <gstar/HistogramPlot.h>

using namespace gstar;

/*---------------------------------------------------------------------------*/

ControlPointWidget::ControlPointWidget(QWidget* widget)
: QWidget(widget)
{

   m_ctrlPointPicker = new ControlPointPicker();

   m_ctrlPointSlider = new ControlPointSlider();

   m_histogram = new HistogramPlot(m_ctrlPointPicker);

   connect(m_ctrlPointSlider, SIGNAL(minMaxChanged(double, double)),
           m_ctrlPointPicker, SLOT(setSubRange(double, double)));

   connect(m_ctrlPointSlider, SIGNAL(minMaxChanged(double, double)),
           this, SLOT(subRangeUpdated(double, double)));

   connect(m_ctrlPointPicker, SIGNAL(controlPointsChanged(const QPolygonF&)),
           this, SLOT(pointsChanged(const QPolygonF&)));

   m_validatorMin = new QIntValidator(0, 65536, this);
   m_validatorMax = new QIntValidator(0, 65536, this);

   m_editMin = new QLineEdit();
   m_editMin -> setFixedWidth(50);
   m_editMin -> setValidator(m_validatorMin);

   m_editMax = new QLineEdit();
   m_editMax -> setFixedWidth(50);
   m_editMax -> setValidator(m_validatorMax);

   connect(m_editMin, SIGNAL(editingFinished()),
           this, SLOT(textMinSubRangeChanged()));
   connect(m_editMax, SIGNAL(editingFinished()),
           this, SLOT(textMaxSubRangeChanged()));

   QHBoxLayout* hlayout = new QHBoxLayout();
   hlayout -> addWidget(m_editMin);
   hlayout -> addStretch();
   hlayout -> addWidget(m_editMax);
   hlayout -> setContentsMargins(0, 2, 0, 0);

   QVBoxLayout* layout = new QVBoxLayout();
   layout -> addWidget(m_ctrlPointPicker);
   layout -> addWidget(m_ctrlPointSlider);
   layout -> addLayout(hlayout);
   layout -> setSpacing(0);
   layout -> setContentsMargins(0, 0, 0, 0);

   setLayout(layout);

}

/*---------------------------------------------------------------------------*/

QPolygonF ControlPointWidget::getPoints() const
{

   return m_ctrlPointPicker -> getPoints();

}

/*---------------------------------------------------------------------------*/

void ControlPointWidget::getTotalRange(double* min, double* max)
{

   m_ctrlPointPicker -> getTotalRange(min, max);

}

/*---------------------------------------------------------------------------*/

void ControlPointWidget::pointsChanged(const QPolygonF& points)
{

   emit controlPointsChanged(points);

}

/*---------------------------------------------------------------------------*/

void ControlPointWidget::setColor(int color)
{

   m_ctrlPointPicker -> setColor(color);

}

/*---------------------------------------------------------------------------*/

void ControlPointWidget::setPoints(const QPolygonF& points, bool bRestretch)
{

   m_ctrlPointPicker->setPoints(points, bRestretch);

}

/*---------------------------------------------------------------------------*/

void ControlPointWidget::setSubRange(double min, double max)
{

   m_ctrlPointSlider -> setMinMax(min, max);
   m_ctrlPointPicker -> setSubRange(min, max);

   m_editMin -> setText(QString::number((int) min));
   m_editMax -> setText(QString::number((int) max));

}

/*---------------------------------------------------------------------------*/

void ControlPointWidget::setTotalRange(double min, double max)
{

   double curMin, curMax;
   m_ctrlPointSlider->getMinMax(&curMin, &curMax);

   if (curMin != min || curMax != max)
   {
      m_ctrlPointSlider -> setMinMax(min, max);
   }

   m_ctrlPointSlider -> setRange(min, max);

   m_ctrlPointPicker -> setTotalRange(min, max);

   m_validatorMin -> setRange((int) min, (int) max);
   m_validatorMax -> setRange((int) min, (int) max);

}

/*---------------------------------------------------------------------------*/

void ControlPointWidget::subRangeUpdated(double min, double max)
{

   emit subRangeChanged(min, max);

   m_editMin -> setText(QString::number((int) min));
   m_editMax -> setText(QString::number((int) max));

}

/*---------------------------------------------------------------------------*/

void ControlPointWidget::textMaxSubRangeChanged()
{

   double min = (m_editMin -> text()).toDouble();
   double max = (m_editMax -> text()).toDouble();

   if (max < min) {
      max = min;
      m_editMax -> setText(QString::number((int) max));
   }

   m_ctrlPointSlider -> setMinMax(min, max);
   m_ctrlPointPicker -> setSubRange(min, max);

   emit subRangeChanged(min, max);

}

/*---------------------------------------------------------------------------*/

void ControlPointWidget::textMinSubRangeChanged()
{

   double min = (m_editMin -> text()).toDouble();
   double max = (m_editMax -> text()).toDouble();

   if (min > max) {
      min = max;
      m_editMin -> setText(QString::number((int) min));
   }

   m_ctrlPointSlider -> setMinMax(min, max);
   m_ctrlPointPicker -> setSubRange(min, max);

   emit subRangeChanged(min, max);

}

/*---------------------------------------------------------------------------*/

void ControlPointWidget::updateHistogram(QList<int> points)
{

   m_histogram->updatePoints(points);

}

/*---------------------------------------------------------------------------*/

void ControlPointWidget::setPickerRange(double min, double max)
{
   m_ctrlPointSlider -> setMinMax(min, max);
   m_ctrlPointSlider -> setRange(min, max);

   m_ctrlPointPicker->setPickerRange(min, max);

}

/*---------------------------------------------------------------------------*/




