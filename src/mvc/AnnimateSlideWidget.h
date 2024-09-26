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
#include <QCoreApplication>

//---------------------------------------------------------------------------

class AnnimateSlideWidget : public QWidget
{

   Q_OBJECT

   enum class SlideState {SlideIn, SlideOut};

public:

   AnnimateSlideWidget(QWidget *parent = nullptr);

   ~AnnimateSlideWidget(){}

   void setAnimWidget(QWidget* w, QWidget* container_widget=nullptr); 

   void setAnimEnabled(bool val);

protected:

   virtual void enterEvent(QEnterEvent *event) override 
   {
      if(_anim_enabled)
      {
         animateSlideOut();
      }  
   }

   virtual void leaveEvent(QEvent *event) override 
   {
      if(_anim_enabled)
      {
         animateSlideIn();  
      }
   }

private slots:
   void animateSlideIn() 
   {
      // Animate the widget to slide in
      if ( _anim_hide->state() == QAbstractAnimation::State::Stopped 
         && _anim_show->state() == QAbstractAnimation::State::Stopped
         && _cur_state == SlideState::SlideOut)
      {
         if(_first)
         {
            _saved_width = _anim_widget->width();
            _first = false;
            _anim_hide->setStartValue(_saved_width);
            _anim_hide->setEndValue(20);

            _anim_show->setStartValue(20);
            _anim_show->setEndValue(_saved_width);
         }

         _cur_state = SlideState::SlideIn;
         _anim_hide->start();
      }
   }

   void animateSlideOut() 
   {
      if ( _anim_hide->state() == QAbstractAnimation::State::Stopped 
         && _anim_show->state() == QAbstractAnimation::State::Stopped
         && _cur_state == SlideState::SlideIn)
      {
          _cur_state = SlideState::SlideOut;
         _anim_show->start();
      }
   }

private:
   QWidget* _anim_widget;
   QPropertyAnimation *_anim_hide;
   QPropertyAnimation *_anim_show;
   bool _anim_enabled;
   SlideState _cur_state;
   int _saved_width;
   bool _first;
};

//---------------------------------------------------------------------------

#endif /* TXM_ABSTRACT_WINDOW_CONTROLLER_H */

//---------------------------------------------------------------------------
