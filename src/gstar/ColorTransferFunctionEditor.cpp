/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <gstar/ColorTransferFunctionEditor.h>
#include <QDebug>
using namespace gstar;

/*---------------------------------------------------------------------------*/

ColorTransferFunctionEditor::ColorTransferFunctionEditor(QWidget* widget)
: QWidget(widget)
{

   // Get application path
   m_appPath = QCoreApplication::applicationDirPath();


   // Background color
   QPalette pal = this -> palette();
   pal.setColor(this -> backgroundRole(), Qt::white);
   this -> setPalette(pal);

   // Autofill background
   setAutoFillBackground(true);


   //m_totalRange[0] = 0.0;
   //m_totalRange[1] = 255.0;


   m_red = new ControlPointWidget(this);
   m_red -> setColor(ControlPointPicker::Red);

   m_green = new ControlPointWidget(this);
   m_green -> setColor(ControlPointPicker::Green);

   m_blue = new ControlPointWidget(this);
   m_blue -> setColor(ControlPointPicker::Blue);

   m_display = new ColorGradientDisplay(this);

   m_btnLock = new QPushButton();
   m_btnLock -> setIcon(QIcon(":images/lock.png"));
   m_btnLock -> setCheckable(true);
   m_btnLock -> setFlat(true);
   m_btnLock -> setFixedSize(32, 32);
   m_btnLock -> setChecked(true);

   // Auto levels button
   m_btnAuto = new QPushButton(tr("Auto Levels"));
   m_btnAuto -> setFixedHeight(32);
   m_btnAuto->setVisible(true);

   // Connect clicked to RequestAutoLevels()
   connect(m_btnAuto, SIGNAL(clicked()),
           this, SIGNAL(requestAutoLevels()));
   

   QHBoxLayout* btnLayout = new QHBoxLayout();
   btnLayout -> addWidget(m_btnLock);
   btnLayout -> addStretch();
   btnLayout -> addWidget(m_btnAuto);


   // Predefined color transfer function buttons
   m_btnColorRed = new QPushButton(tr("Red"));
   connect(m_btnColorRed, SIGNAL(clicked()),
           this, SLOT(predefinedColorClicked()));
   m_btnColorGreen = new QPushButton(tr("Green"));
   connect(m_btnColorGreen, SIGNAL(clicked()),
           this, SLOT(predefinedColorClicked()));
   m_btnColorBlue = new QPushButton(tr("Blue"));
   connect(m_btnColorBlue, SIGNAL(clicked()),
           this, SLOT(predefinedColorClicked()));
   m_btnColorHot = new QPushButton(tr("Hot"));
   connect(m_btnColorHot, SIGNAL(clicked()),
           this, SLOT(predefinedColorClicked()));
   m_btnColorGray = new QPushButton(tr("Gray"));
   connect(m_btnColorGray, SIGNAL(clicked()),
           this, SLOT(predefinedColorClicked()));
   m_btnColorIce = new QPushButton(tr("Ice"));
   connect(m_btnColorIce, SIGNAL(clicked()),
           this, SLOT(predefinedColorClicked()));

   // Predefined color buttons layout
   /*
   QGridLayout* predefinedLayout = new QGridLayout();
   predefinedLayout -> setSpacing(0);
   predefinedLayout -> setContentsMargins(0, 0, 0, 0);
   predefinedLayout -> addWidget(m_btnColorRed, 0, 0);
   predefinedLayout -> addWidget(m_btnColorGreen, 1, 0);
   predefinedLayout -> addWidget(m_btnColorBlue, 2, 0);
   predefinedLayout -> addWidget(m_btnColorHot, 0, 1);
   predefinedLayout -> addWidget(m_btnColorGray, 1, 1);
   predefinedLayout -> addWidget(m_btnColorIce, 2, 1);
   */

   QVBoxLayout* layout = new QVBoxLayout();
   layout -> addWidget(m_red);
   layout -> addWidget(m_green);
   layout -> addWidget(m_blue);
   layout -> addWidget(m_display);
   layout -> addLayout(btnLayout);
   //layout -> addLayout(predefinedLayout);
   layout -> setSpacing(11);
   layout -> setContentsMargins(3, 3, 3, 3);

   setLayout(layout);

   setMinimumHeight(300);


   pointsChanged();

   connect(m_red, SIGNAL(subRangeChanged(double, double)),
           this, SLOT(redRangeChanged(double, double)));
   connect(m_green, SIGNAL(subRangeChanged(double, double)),
           this, SLOT(greenRangeChanged(double, double)));
   connect(m_blue, SIGNAL(subRangeChanged(double, double)),
           this, SLOT(blueRangeChanged(double, double)));


   connect(m_red, SIGNAL(controlPointsChanged(const QPolygonF&)),
           this, SLOT(pointsChanged()));
   connect(m_green, SIGNAL(controlPointsChanged(const QPolygonF&)),
           this, SLOT(pointsChanged()));
   connect(m_blue, SIGNAL(controlPointsChanged(const QPolygonF&)),
           this, SLOT(pointsChanged()));

}

/*---------------------------------------------------------------------------*/

ColorTransferFunction ColorTransferFunctionEditor::getColorTransferFunction()
{

   return m_ctf;

}

/*---------------------------------------------------------------------------*/

void ColorTransferFunctionEditor::predefinedColorClicked()
{

   // Get sender
   QPushButton* btn = dynamic_cast<QPushButton*>(sender());
   
   // Emit signal to request a predefined transfer function
   emit requestPredefinedColor(btn -> text());

}

/*---------------------------------------------------------------------------*/

void ColorTransferFunctionEditor::pointsChanged()
{

   vector<double> iRed;
   vector<double> vRed;

   QPolygonF redPts = m_red -> getPoints();

   for (int i = 0 ; i < redPts.size() ; i++) {
      iRed.push_back(redPts[i].x());
      vRed.push_back(redPts[i].y());
   }


   vector<double> iGreen;
   vector<double> vGreen;

   QPolygonF greenPts = m_green -> getPoints();

   for (int i = 0 ; i < greenPts.size() ; i++) {
      iGreen.push_back(greenPts[i].x());
      vGreen.push_back(greenPts[i].y());
   }


   vector<double> iBlue;
   vector<double> vBlue;

   QPolygonF bluePts = m_blue -> getPoints();

   for (int i = 0 ; i < bluePts.size() ; i++) {
      iBlue.push_back(bluePts[i].x());
      vBlue.push_back(bluePts[i].y());
   }

   m_ctf.setRed(iRed, vRed);
   m_ctf.setGreen(iGreen, vGreen);
   m_ctf.setBlue(iBlue, vBlue);
   //m_ctf.setTotalRange(m_totalRange[0], m_totalRange[1]);
   m_ctf.update();

   m_display -> setColorTransferFunction(m_ctf);

   emit transferFunctionUpdated(m_ctf);

}

/*---------------------------------------------------------------------------*/

void ColorTransferFunctionEditor::setColorFunction(ColorTransferFunction ctf)
{

   disconnect(m_red, SIGNAL(controlPointsChanged(const QPolygonF&)),
              this, SLOT(pointsChanged()));
   disconnect(m_green, SIGNAL(controlPointsChanged(const QPolygonF&)),
            this, SLOT(pointsChanged()));
   disconnect(m_blue, SIGNAL(controlPointsChanged(const QPolygonF&)),
              this, SLOT(pointsChanged()));

   double min, max;

   int curDataType = m_ctf.getDataType();
   int newDataType = ctf.getDataType();

   //if the data types are not equal restretch the points
   bool bRestretch = (curDataType != newDataType);

   m_ctf.setRed(ctf.getRedIndex(), ctf.getRedValue());
   m_ctf.setGreen(ctf.getGreenIndex(), ctf.getRedValue());
   m_ctf.setBlue(ctf.getBlueIndex(), ctf.getRedValue());

   // Must set total range before subranges, otherwise ControlPointSlider
   // resets min and max in SetRange
   //m_ctf.getTotalRange(&min, &max);

   ctf.getTotalRange(&min, &max);

   m_red -> setTotalRange(min, max);
   m_green -> setTotalRange(min, max);
   m_blue -> setTotalRange(min, max);

   ctf.getRedSubRange(&min, &max);
   m_ctf.setRedSubRange(min, max);
   m_red -> setSubRange(min, max);

   ctf.getGreenSubRange(&min, &max);
   m_ctf.setGreenSubRange(min, max);
   m_green -> setSubRange(min, max);

   ctf.getBlueSubRange(&min, &max);
   m_ctf.setBlueSubRange(min, max);
   m_blue -> setSubRange(min, max);


   vector<double> redI = ctf.getRedIndex();
   vector<double> redV = ctf.getRedValue();
   QPolygonF redPts;
   for (unsigned int i = 0 ; i < redI.size() ; i++)
   {
      redPts.push_back(QPointF(redI[i], redV[i]));
   }

   m_red -> setPoints(redPts, bRestretch);

   vector<double> greenI = ctf.getGreenIndex();
   vector<double> greenV = ctf.getGreenValue();
   QPolygonF greenPts;
   for (unsigned int i = 0 ; i < greenI.size() ; i++)
   {
      greenPts.push_back(QPointF(greenI[i], greenV[i]));
   }
   m_green -> setPoints(greenPts, bRestretch);

   vector<double> blueI = ctf.getBlueIndex();
   vector<double> blueV = ctf.getBlueValue();
   QPolygonF bluePts;
   for (unsigned int i = 0 ; i < blueI.size() ; i++)
   {
      bluePts.push_back(QPointF(blueI[i], blueV[i]));
   }
   m_blue -> setPoints(bluePts, bRestretch);

   connect(m_red, SIGNAL(controlPointsChanged(const QPolygonF&)),
           this, SLOT(pointsChanged()));
   connect(m_green, SIGNAL(controlPointsChanged(const QPolygonF&)),
         this, SLOT(pointsChanged()));
   connect(m_blue, SIGNAL(controlPointsChanged(const QPolygonF&)),
           this, SLOT(pointsChanged()));

}

/*---------------------------------------------------------------------------*/

void ColorTransferFunctionEditor::setSubRange(double min, double max)
{

   m_red -> setSubRange(min, max);
   m_ctf.setRedSubRange(min, max);
   m_green -> setSubRange(min, max);
   m_ctf.setGreenSubRange(min, max);
   m_blue -> setSubRange(min, max);
   m_ctf.setBlueSubRange(min, max);

}

/*---------------------------------------------------------------------------*/

void ColorTransferFunctionEditor::setTotalRange(double min, double max)
{

   disconnect(m_red, SIGNAL(controlPointsChanged(const QPolygonF&)),
              this, SLOT(pointsChanged()));
   disconnect(m_green, SIGNAL(controlPointsChanged(const QPolygonF&)),
            this, SLOT(pointsChanged()));
   disconnect(m_blue, SIGNAL(controlPointsChanged(const QPolygonF&)),
              this, SLOT(pointsChanged()));

   // Clean and reinitialize the min max
//   m_red->setPickerRange(min, max);
//   m_green->setPickerRange(min, max);
//   m_blue->setPickerRange(min, max);

   //m_totalRange[0] = min;
   //m_totalRange[1] = max;

   m_red->setTotalRange(min, max);
   m_green->setTotalRange(min, max);
   m_blue->setTotalRange(min, max);

   m_ctf.setTotalRange(min, max);

   pointsChanged();

   connect(m_red, SIGNAL(controlPointsChanged(const QPolygonF&)),
           this, SLOT(pointsChanged()));
   connect(m_green, SIGNAL(controlPointsChanged(const QPolygonF&)),
         this, SLOT(pointsChanged()));
   connect(m_blue, SIGNAL(controlPointsChanged(const QPolygonF&)),
           this, SLOT(pointsChanged()));

}

/*---------------------------------------------------------------------------*/

void ColorTransferFunctionEditor::redRangeChanged(double min, double max)
{

   disconnect(m_green, SIGNAL(subRangeChanged(double, double)),
           this, SLOT(greenRangeChanged(double, double)));
   disconnect(m_blue, SIGNAL(subRangeChanged(double, double)),
           this, SLOT(blueRangeChanged(double, double)));

   if (m_btnLock -> isChecked()) {
      m_green -> setSubRange(min, max);
      m_ctf.setGreenSubRange(min, max);
      m_blue -> setSubRange(min, max);
      m_ctf.setBlueSubRange(min, max);
   }

   m_ctf.setRedSubRange(min, max);

   connect(m_green, SIGNAL(subRangeChanged(double, double)),
           this, SLOT(greenRangeChanged(double, double)));
   connect(m_blue, SIGNAL(subRangeChanged(double, double)),
           this, SLOT(blueRangeChanged(double, double)));

}

/*---------------------------------------------------------------------------*/

void ColorTransferFunctionEditor::greenRangeChanged(double min, double max)
{

   disconnect(m_red, SIGNAL(subRangeChanged(double, double)),
           this, SLOT(redRangeChanged(double, double)));
   disconnect(m_blue, SIGNAL(subRangeChanged(double, double)),
           this, SLOT(blueRangeChanged(double, double)));

   if (m_btnLock -> isChecked()) {
      m_red -> setSubRange(min, max);
      m_ctf.setRedSubRange(min, max);
      m_blue -> setSubRange(min, max);
      m_ctf.setBlueSubRange(min, max);
   }

   m_ctf.setGreenSubRange(min, max);

   connect(m_red, SIGNAL(subRangeChanged(double, double)),
           this, SLOT(redRangeChanged(double, double)));
   connect(m_blue, SIGNAL(subRangeChanged(double, double)),
           this, SLOT(blueRangeChanged(double, double)));

}

/*---------------------------------------------------------------------------*/

void ColorTransferFunctionEditor::blueRangeChanged(double min, double max)
{

   disconnect(m_red, SIGNAL(subRangeChanged(double, double)),
           this, SLOT(redRangeChanged(double, double)));
   disconnect(m_green, SIGNAL(subRangeChanged(double, double)),
           this, SLOT(greenRangeChanged(double, double)));

   if (m_btnLock -> isChecked()) {
      m_red -> setSubRange(min, max);
      m_ctf.setRedSubRange(min, max);
      m_green -> setSubRange(min, max);
      m_ctf.setGreenSubRange(min, max);
   }

   m_ctf.setBlueSubRange(min, max);

   connect(m_red, SIGNAL(subRangeChanged(double, double)),
           this, SLOT(redRangeChanged(double, double)));
   connect(m_green, SIGNAL(subRangeChanged(double, double)),
           this, SLOT(greenRangeChanged(double, double)));

}

/*---------------------------------------------------------------------------*/

void ColorTransferFunctionEditor::updateHistogram(QList<int> points)
{

   m_red->updateHistogram(points);
   m_green->updateHistogram(points);
   m_blue->updateHistogram(points);

}

/*---------------------------------------------------------------------------*/

void ColorTransferFunctionEditor::setCTFTotalRange(double min, double max)
{

   //m_totalRange[0] = min;
   //m_totalRange[1] = max;
   m_ctf.setTotalRange(min, max);

}

/*---------------------------------------------------------------------------*/

