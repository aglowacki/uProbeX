/*-----------------------------------------------------------------------------
 * Copyright (c) 2025, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/RoiSelectorDialog.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

//---------------------------------------------------------------------------

RoiSelectorDialog::RoiSelectorDialog(QWidget *parent) : QDialog(parent)
{

   setWindowModality(Qt::WindowModal);

   QVBoxLayout* vboxLayout = new QVBoxLayout();

   QPushButton* btnUse = new QPushButton("Continue");
   QPushButton* btnCancel = new QPushButton("Cancel");

   _cb_rois = new QComboBox();
   _cb_background = new QComboBox();

   connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
   connect(btnUse, &QPushButton::clicked, this, &QDialog::accept);

   QHBoxLayout* buttonLayout = new QHBoxLayout();
   buttonLayout->setSpacing(5);
   buttonLayout->addWidget(btnUse);
   buttonLayout->addWidget(btnCancel);

   
   QHBoxLayout* hRois = new QHBoxLayout();
   hRois->addWidget(new QLabel("ROI:"));
   hRois->addWidget(_cb_rois);

   QHBoxLayout* hBackgrounds = new QHBoxLayout();
   hBackgrounds->addWidget(new QLabel("Background:"));
   hBackgrounds->addWidget(_cb_background);
   
   vboxLayout->addLayout(hRois);
   vboxLayout->addLayout(hBackgrounds);
   vboxLayout->addLayout(buttonLayout);

   setLayout(vboxLayout);

}

//---------------------------------------------------------------------------

RoiSelectorDialog::~RoiSelectorDialog()
{

}

//---------------------------------------------------------------------------

void RoiSelectorDialog::setROIs(QStringList items)
{
   _cb_rois->clear();
   for(auto &itr : items)
   {
      _cb_rois->addItem(itr);
   }
}

//---------------------------------------------------------------------------

void RoiSelectorDialog::setBackgrounds(QStringList items)
{
   _cb_background->clear();
   _cb_background->addItem("SNIP Algorithm");
   /* TODO: wait for feature to be fully impl before allowing other backgrounds
   for(auto &itr : items)
   {
      _cb_background->addItem(itr);
   }
   */
}

//---------------------------------------------------------------------------
