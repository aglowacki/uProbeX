/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef PERIODIC_TABLE_WIDGET_H
#define PERIODIC_TABLE_WIDGET_H

//---------------------------------------------------------------------------

#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <unordered_map>

enum class Element_Class { Hydrogen, Alkaline_Metal, Alkaline_Earth_Metal, Transition_metals, Post_Transition_Metals, Metalloids, Reactive_nonmetals, Noble_gases, Lanthanides, Actinides, Unknown_properties };

class PeriodicTableWidget : public QWidget
{

   Q_OBJECT

public:
    PeriodicTableWidget(QWidget *parent = nullptr);

    ~PeriodicTableWidget();

signals:

    void onSelect(QString);

public slots:

    void button_pressed();

private:

    QPushButton* _gen_el_btn(QString symbol);

    QGridLayout* _layout;

    const std::unordered_map<QString, enum Element_Class> Element_Class_Map = {
        {"H", Element_Class::Hydrogen},
        {"He", Element_Class::Noble_gases},
         {"Li", Element_Class::Alkaline_Metal},
         {"Be", Element_Class::Alkaline_Earth_Metal},
         {"B", Element_Class::Metalloids},
         {"C", Element_Class::Reactive_nonmetals},
         {"N", Element_Class::Reactive_nonmetals},
         {"O", Element_Class::Reactive_nonmetals},
         {"F", Element_Class::Reactive_nonmetals},
         {"Ne", Element_Class::Noble_gases},
         {"Na", Element_Class::Alkaline_Metal},
         {"Mg", Element_Class::Alkaline_Earth_Metal},
         {"Al", Element_Class::Post_Transition_Metals},
         {"Si", Element_Class::Metalloids},
         {"P", Element_Class::Reactive_nonmetals},
         {"S", Element_Class::Reactive_nonmetals},
         {"Cl", Element_Class::Reactive_nonmetals},
         {"Ar", Element_Class::Noble_gases},
         {"K", Element_Class::Alkaline_Metal},
         {"Ca", Element_Class::Alkaline_Earth_Metal},
         {"Sc", Element_Class::Transition_metals},
         {"Ti", Element_Class::Transition_metals},
         {"V", Element_Class::Transition_metals},
         {"Cr", Element_Class::Transition_metals},
         {"Mn", Element_Class::Transition_metals},
         {"Fe", Element_Class::Transition_metals},
         {"Co", Element_Class::Transition_metals},
         {"Ni", Element_Class::Transition_metals},
         {"Cu", Element_Class::Transition_metals},
         {"Zn", Element_Class::Transition_metals},
         {"Ga", Element_Class::Post_Transition_Metals},
         {"Ge", Element_Class::Metalloids},
         {"As", Element_Class::Metalloids},
         {"Se", Element_Class::Reactive_nonmetals},
         {"Br", Element_Class::Reactive_nonmetals},
         {"Kr", Element_Class::Noble_gases},
         {"Rb", Element_Class::Alkaline_Metal},
         {"Sr", Element_Class::Alkaline_Earth_Metal},
         {"Y", Element_Class::Transition_metals},
         {"Zr", Element_Class::Transition_metals},
         {"Nb", Element_Class::Transition_metals},
         {"Mo", Element_Class::Transition_metals},
         {"Tc", Element_Class::Transition_metals},
         {"Ru", Element_Class::Transition_metals},
         {"Rh", Element_Class::Transition_metals},
         {"Pd", Element_Class::Transition_metals},
         {"Ag", Element_Class::Transition_metals},
         {"Cd", Element_Class::Transition_metals},
         {"In", Element_Class::Post_Transition_Metals},
         {"Sn", Element_Class::Post_Transition_Metals},
         {"Sb", Element_Class::Metalloids},
         {"Te", Element_Class::Metalloids},
         {"I", Element_Class::Reactive_nonmetals},
         {"Xe", Element_Class::Noble_gases},
         {"Cs", Element_Class::Alkaline_Metal},
         {"Ba", Element_Class::Alkaline_Earth_Metal},
         {"La", Element_Class::Lanthanides},
         {"Ce", Element_Class::Lanthanides},
         {"Pr", Element_Class::Lanthanides},
         {"Nd", Element_Class::Lanthanides},
         {"Pm", Element_Class::Lanthanides},
         {"Sm", Element_Class::Lanthanides},
         {"Eu", Element_Class::Lanthanides},
         {"Gd", Element_Class::Lanthanides},
         {"Tb", Element_Class::Lanthanides},
         {"Dy", Element_Class::Lanthanides},
         {"Ho", Element_Class::Lanthanides},
         {"Er", Element_Class::Lanthanides},
         {"Tm", Element_Class::Lanthanides},
         {"Yb", Element_Class::Lanthanides},
         {"Lu", Element_Class::Lanthanides},
         {"Hf", Element_Class::Transition_metals},
         {"Ta", Element_Class::Transition_metals},
         {"W", Element_Class::Transition_metals},
         {"Re", Element_Class::Transition_metals},
         {"Os", Element_Class::Transition_metals},
         {"Ir", Element_Class::Transition_metals},
         {"Pt", Element_Class::Transition_metals},
         {"Au", Element_Class::Transition_metals},
         {"Hg", Element_Class::Transition_metals},
         {"Tl", Element_Class::Post_Transition_Metals},
         {"Pb", Element_Class::Post_Transition_Metals},
         {"Bi", Element_Class::Post_Transition_Metals},
         {"Po", Element_Class::Post_Transition_Metals},
         {"At", Element_Class::Metalloids},
         {"Rn", Element_Class::Noble_gases},
         {"Fr", Element_Class::Alkaline_Metal},
         {"Ra", Element_Class::Alkaline_Earth_Metal},
         {"Ac", Element_Class::Actinides},
         {"Th", Element_Class::Actinides},
         {"Pa", Element_Class::Actinides},
         {"U", Element_Class::Actinides},
         {"Np", Element_Class::Actinides},
         {"Pu", Element_Class::Actinides},
         {"Am", Element_Class::Actinides},
         {"Cm", Element_Class::Actinides},
         {"Bk", Element_Class::Actinides},
         {"Cf", Element_Class::Actinides},
         {"Es", Element_Class::Actinides},
         {"Fm", Element_Class::Actinides},
         {"Md", Element_Class::Actinides},
         {"No", Element_Class::Actinides},
         {"Lr", Element_Class::Actinides},
         {"Rf", Element_Class::Transition_metals},
         {"Db", Element_Class::Transition_metals},
         {"Sg", Element_Class::Transition_metals},
         {"Bh", Element_Class::Transition_metals},
         {"Hs", Element_Class::Transition_metals},
         {"Mt", Element_Class::Unknown_properties},
         {"Ds", Element_Class::Unknown_properties},
         {"Rg", Element_Class::Unknown_properties},
         {"Cn", Element_Class::Unknown_properties},
         {"Nh", Element_Class::Unknown_properties},
         {"Fl", Element_Class::Unknown_properties},
         {"Mc", Element_Class::Unknown_properties},
         {"Lv", Element_Class::Unknown_properties},
         {"Ts", Element_Class::Unknown_properties},
         {"Og", Element_Class::Unknown_properties},

    };
    
};

#endif