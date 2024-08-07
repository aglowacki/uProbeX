/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef AnnimateSlideWidget_H
#define AnnimateSlideWidget_H

//---------------------------------------------------------------------------

//#include <preferences/Preferences.h>
#include <QWidget>
#include <QPropertyAnimation>
#include <QPushButton>

//---------------------------------------------------------------------------

class AnnimateSlideWidget : public QWidget
{

   Q_OBJECT

public:

   AnnimateSlideWidget(QWidget *parent = nullptr);

   ~AnnimateSlideWidget(){}

   void setAnimWidget(QWidget* w, QWidget* container_widget=nullptr); 

   void setAnimEnabled(bool val);

protected:

   virtual void enterEvent(QEnterEvent *event) override 
   {
      if(_anim_enabled && false == _running)
      {
         _running = true;
         animateSlideOut();
      }
   }

   virtual void leaveEvent(QEvent *event) override 
   {
      if(_anim_enabled && false == _running)
      {
         _running = true;
         animateSlideIn();  
      }
   }

private slots:
   void animateSlideIn() 
   {
      // Animate the widget to slide in
      if(_first)
      {
         _saved_width = _anim_widget->width();
         _first = false;
      }
      _anim_hide->setStartValue(_saved_width);
      _anim_hide->setEndValue(20);

      _anim_show->setStartValue(20);
      _anim_show->setEndValue(_saved_width);

      _anim_hide->start();
   }

   void animateSlideOut() 
   {
      _anim_show->start();
   }

   void onFinished()
   {
      _running = false;
   }

private:
   QWidget* _anim_widget;
   QPropertyAnimation *_anim_hide;
   QPropertyAnimation *_anim_show;
   bool _anim_enabled;
   bool _first;
   bool _running;
   int _saved_width;
};

//---------------------------------------------------------------------------

#endif /* TXM_ABSTRACT_WINDOW_CONTROLLER_H */

//---------------------------------------------------------------------------
