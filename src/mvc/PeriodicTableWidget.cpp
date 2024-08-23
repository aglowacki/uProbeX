/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/PeriodicTableWidget.h>
#include <QLabel>

//---------------------------------------------------------------------------

PeriodicTableWidget::PeriodicTableWidget(QWidget *parent) : QWidget(parent)
{
    _layout = new QGridLayout();
    _layout->setHorizontalSpacing(0);
    
    
    _layout->addWidget(_gen_el_btn("H"), 0,0);
    _layout->addWidget(_gen_el_btn("He"), 0,17);

    _layout->addWidget(_gen_el_btn("Li"), 1,0);    
    _layout->addWidget(_gen_el_btn("Be"), 1,1);    
    _layout->addWidget(_gen_el_btn("B"), 1,12);    
    _layout->addWidget(_gen_el_btn("C"), 1,13);    
    _layout->addWidget(_gen_el_btn("N"), 1,14);    
    _layout->addWidget(_gen_el_btn("O"), 1,15);    
    _layout->addWidget(_gen_el_btn("F"), 1,16);    
    _layout->addWidget(_gen_el_btn("Ne"), 1,17);

    _layout->addWidget(_gen_el_btn("Na"), 2,0);
    _layout->addWidget(_gen_el_btn("Mg"), 2,1);
    _layout->addWidget(_gen_el_btn("Al"), 2,12);
    _layout->addWidget(_gen_el_btn("Si"), 2,13);
    _layout->addWidget(_gen_el_btn("P"), 2,14);
    _layout->addWidget(_gen_el_btn("S"), 2,15);
    _layout->addWidget(_gen_el_btn("Cl"), 2,16);
    _layout->addWidget(_gen_el_btn("Ar"), 2,17);

    _layout->addWidget(_gen_el_btn("K"), 3,0);
    _layout->addWidget(_gen_el_btn("Ca"), 3,1);
    _layout->addWidget(_gen_el_btn("Sc"), 3,2);
    _layout->addWidget(_gen_el_btn("Ti"), 3,3);
    _layout->addWidget(_gen_el_btn("V"), 3,4);
    _layout->addWidget(_gen_el_btn("Cr"), 3,5);
    _layout->addWidget(_gen_el_btn("Mn"), 3,6);
    _layout->addWidget(_gen_el_btn("Fe"), 3,7);
    _layout->addWidget(_gen_el_btn("Co"), 3,8);
    _layout->addWidget(_gen_el_btn("Ni"), 3,9);
    _layout->addWidget(_gen_el_btn("Cu"), 3,10);
    _layout->addWidget(_gen_el_btn("Zn"), 3,11);
    _layout->addWidget(_gen_el_btn("Ga"), 3,12);
    _layout->addWidget(_gen_el_btn("Ge"), 3,13);
    _layout->addWidget(_gen_el_btn("As"), 3,14);
    _layout->addWidget(_gen_el_btn("Se"), 3,15);
    _layout->addWidget(_gen_el_btn("Br"), 3,16);
    _layout->addWidget(_gen_el_btn("Kr"), 3,17);

    _layout->addWidget(_gen_el_btn("Rb"), 4,0);
    _layout->addWidget(_gen_el_btn("Sr"), 4,1);
    _layout->addWidget(_gen_el_btn("Y"), 4,2);
    _layout->addWidget(_gen_el_btn("Zr"), 4,3);
    _layout->addWidget(_gen_el_btn("Nb"), 4,4);
    _layout->addWidget(_gen_el_btn("Mo"), 4,5);
    _layout->addWidget(_gen_el_btn("Tc"), 4,6);
    _layout->addWidget(_gen_el_btn("Ru"), 4,7);
    _layout->addWidget(_gen_el_btn("Rh"), 4,8);
    _layout->addWidget(_gen_el_btn("Pd"), 4,9);
    _layout->addWidget(_gen_el_btn("Ag"), 4,10);
    _layout->addWidget(_gen_el_btn("Cd"), 4,11);
    _layout->addWidget(_gen_el_btn("In"), 4,12);
    _layout->addWidget(_gen_el_btn("Sn"), 4,13);
    _layout->addWidget(_gen_el_btn("Sb"), 4,14);
    _layout->addWidget(_gen_el_btn("Te"), 4,15);
    _layout->addWidget(_gen_el_btn("I"), 4,16);
    _layout->addWidget(_gen_el_btn("Xe"), 4,17);

    _layout->addWidget(_gen_el_btn("Cs"), 5,0);
    _layout->addWidget(_gen_el_btn("Ba"), 5,1);
    _layout->addWidget(_gen_el_btn("La"), 5,2);
    _layout->addWidget(_gen_el_btn("Hf"), 5,3);
    _layout->addWidget(_gen_el_btn("Ta"), 5,4);
    _layout->addWidget(_gen_el_btn("W"), 5,5);
    _layout->addWidget(_gen_el_btn("Re"), 5,6);
    _layout->addWidget(_gen_el_btn("Os"), 5,7);
    _layout->addWidget(_gen_el_btn("Ir"), 5,8);
    _layout->addWidget(_gen_el_btn("Pt"), 5,9);
    _layout->addWidget(_gen_el_btn("Au"), 5,10);
    _layout->addWidget(_gen_el_btn("Hg"), 5,11);
    _layout->addWidget(_gen_el_btn("Tl"), 5,12);
    _layout->addWidget(_gen_el_btn("Pb"), 5,13);
    _layout->addWidget(_gen_el_btn("Bi"), 5,14);
    _layout->addWidget(_gen_el_btn("Po"), 5,15);
    _layout->addWidget(_gen_el_btn("At"), 5,16);
    _layout->addWidget(_gen_el_btn("Rn"), 5,17);

    _layout->addWidget(_gen_el_btn("Fr"), 6,0);
    _layout->addWidget(_gen_el_btn("Ra"), 6,1);
    _layout->addWidget(_gen_el_btn("Ac"), 6,2);
    _layout->addWidget(_gen_el_btn("Rf"), 6,3);
    _layout->addWidget(_gen_el_btn("Db"), 6,4);
    _layout->addWidget(_gen_el_btn("Sg"), 6,5);
    _layout->addWidget(_gen_el_btn("Bh"), 6,6);
    _layout->addWidget(_gen_el_btn("Hs"), 6,7);
    _layout->addWidget(_gen_el_btn("Mt"), 6,8);
    _layout->addWidget(_gen_el_btn("Ds"), 6,9);
    _layout->addWidget(_gen_el_btn("Rg"), 6,10);
    _layout->addWidget(_gen_el_btn("Cn"), 6,11);
    _layout->addWidget(_gen_el_btn("Nh"), 6,12);
    _layout->addWidget(_gen_el_btn("Fl"), 6,13);
    _layout->addWidget(_gen_el_btn("Mc"), 6,14);
    _layout->addWidget(_gen_el_btn("Lv"), 6,15);
    _layout->addWidget(_gen_el_btn("Ts"), 6,16);
    _layout->addWidget(_gen_el_btn("Og"), 6,17);

    _layout->addWidget(new QLabel(" "), 7,3);

    _layout->addWidget(_gen_el_btn("Ce"), 8,3);
    _layout->addWidget(_gen_el_btn("Pr"), 8,4);
    _layout->addWidget(_gen_el_btn("Nd"), 8,5);
    _layout->addWidget(_gen_el_btn("Pm"), 8,6);
    _layout->addWidget(_gen_el_btn("Sm"), 8,7);
    _layout->addWidget(_gen_el_btn("Eu"), 8,8);
    _layout->addWidget(_gen_el_btn("Gd"), 8,9);
    _layout->addWidget(_gen_el_btn("Tb"), 8,10);
    _layout->addWidget(_gen_el_btn("Dy"), 8,11);
    _layout->addWidget(_gen_el_btn("Ho"), 8,12);
    _layout->addWidget(_gen_el_btn("Er"), 8,13);
    _layout->addWidget(_gen_el_btn("Tm"), 8,14);
    _layout->addWidget(_gen_el_btn("Yb"), 8,15);
    _layout->addWidget(_gen_el_btn("Lu"), 8,16);

    _layout->addWidget(_gen_el_btn("Th"), 9,3);
    _layout->addWidget(_gen_el_btn("Pa"), 9,4);
    _layout->addWidget(_gen_el_btn("U"), 9,5);
    _layout->addWidget(_gen_el_btn("Np"), 9,6);
    _layout->addWidget(_gen_el_btn("Pu"), 9,7);
    _layout->addWidget(_gen_el_btn("Am"), 9,8);
    _layout->addWidget(_gen_el_btn("Cm"), 9,9);
    _layout->addWidget(_gen_el_btn("Bk"), 9,10);
    _layout->addWidget(_gen_el_btn("Cf"), 9,11);
    _layout->addWidget(_gen_el_btn("Es"), 9,12);
    _layout->addWidget(_gen_el_btn("Fm"), 9,13);
    _layout->addWidget(_gen_el_btn("Md"), 9,14);
    _layout->addWidget(_gen_el_btn("No"), 9,15);
    _layout->addWidget(_gen_el_btn("Lr"), 9,16);

    this->setLayout(_layout);
}

//---------------------------------------------------------------------------

PeriodicTableWidget::~PeriodicTableWidget()
{

}

//---------------------------------------------------------------------------

void PeriodicTableWidget::button_pressed()
{
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    emit onSelect(btn->text());
    this->hide();
}

//---------------------------------------------------------------------------

QPushButton* PeriodicTableWidget::_gen_el_btn(QString symbol)
{
    enum Element_Class ec = Element_Class_Map.at(symbol);
    QPushButton *btn;
    btn = new QPushButton();
    btn->setText(symbol);
    btn->setFixedWidth(36);
    switch(ec)
    {
        case Element_Class::Hydrogen:
            btn->setStyleSheet("QPushButton {background-color: #8ab4f8; color: white;}");
            break;
        case Element_Class::Alkaline_Metal:
            btn->setStyleSheet("QPushButton {background-color: #244d57; color: white;}");
            break;
        case Element_Class::Alkaline_Earth_Metal:
            btn->setStyleSheet("QPushButton {background-color: #622e39; color: white;}");
            break;
        case Element_Class::Transition_metals:
            btn->setStyleSheet("QPushButton {background-color: #433c65; color: white;}");
            break;
        case Element_Class::Post_Transition_Metals:
            btn->setStyleSheet("QPushButton {background-color: #2f4d47; color: white;}");
            break;
        case Element_Class::Metalloids:
            btn->setStyleSheet("QPushButton {background-color: #523e1b; color: white;}");
            break;
        case Element_Class::Reactive_nonmetals:
            btn->setStyleSheet("QPushButton {background-color: #2a4165; color: white;}");
            break;
        case Element_Class::Noble_gases:
            btn->setStyleSheet("QPushButton {background-color: #623842; color: white;}");
            break;
        case Element_Class::Lanthanides:
            btn->setStyleSheet("QPushButton {background-color: #004a77; color: white;}");
            break;
        case Element_Class::Actinides:
            btn->setStyleSheet("QPushButton {background-color: #613b28; color: white;}");
            break;
        case Element_Class::Unknown_properties:
        default:
            btn->setStyleSheet("QPushButton {background-color: #46474c; color: white;}");
            break;
    }

    connect(btn, &QPushButton::released, this, &PeriodicTableWidget::button_pressed);
    return btn;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------