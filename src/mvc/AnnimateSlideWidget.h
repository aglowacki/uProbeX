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

   
private slots:
   void onToggle() 
   {
      if(_anim_enabled)
      {
         if(_isShown)
         {
            animateSlideIn();
         }
         else
         {
            animateSlideOut();
         }
      }  
   }

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
            _anim_hide->setEndValue(10);

            _anim_show->setStartValue(10);
            _anim_show->setEndValue(_saved_width);
         }

         _cur_state = SlideState::SlideIn;
         _anim_hide->start();
         _isShown = false;
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
         _isShown = true;
      }
   }

private:
   QPushButton *_btn_toggle;
   QWidget* _anim_widget;
   QPropertyAnimation *_anim_hide;
   QPropertyAnimation *_anim_show;
   bool _anim_enabled;
   SlideState _cur_state;
   int _saved_width;
   bool _isShown;
   bool _first;
};

//---------------------------------------------------------------------------

#endif /* TXM_ABSTRACT_WINDOW_CONTROLLER_H */

//---------------------------------------------------------------------------
