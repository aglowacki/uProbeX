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

   void setAnimWidget(QWidget* w, QString btn_name);

   void setAnimEnabled(bool val);

protected:

   virtual void enterEvent(QEnterEvent *event) override 
   {
      if(_anim_enabled)
      {
         // Show the widget and start the animation
         if(_anim_widget!=nullptr)
         {
            _anim_widget->setVisible(true);
            _btn_hover->setVisible(false);
         }
         animateSlideIn();
      }
   }

   virtual void leaveEvent(QEvent *event) override 
   {
      if(_anim_enabled)
      {
         // Hide the widget and start the animation
         if(_anim_widget!=nullptr)
         {
            _anim_widget->setVisible(false);
            _btn_hover->setVisible(true);
         }
         animateSlideOut();
      }
   }

private slots:
   void animateSlideIn() 
   {
      // Animate the widget to slide in
      QPropertyAnimation *animation = new QPropertyAnimation(this, "visibleWidth");
      animation->setDuration(1000);
      animation->setStartValue(0);
      animation->setEndValue(width());
      animation->start();
   }

   void animateSlideOut() 
   {
      // Animate the widget to slide out
      QPropertyAnimation *animation = new QPropertyAnimation(this, "visibleWidth");
      animation->setDuration(1000);
      animation->setStartValue(width());
      animation->setEndValue(0);
      animation->start();
   }

private:
   QWidget* _anim_widget;
   QPushButton* _btn_hover;
   bool _anim_enabled;

};

//---------------------------------------------------------------------------

#endif /* TXM_ABSTRACT_WINDOW_CONTROLLER_H */

//---------------------------------------------------------------------------
