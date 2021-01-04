/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <preferences/Preferences.h>

#include <QSettings>
#include <QStringList>
#include <QFile>
#include <QMessageBox>

#define INI_FILENAME "uProbeX"
#define INI_DIR "UChicagoArgonneLLC"

Preferences* Preferences::_this_inst(nullptr);
std::mutex Preferences::_mutex;

/*---------------------------------------------------------------------------*/

Preferences::Preferences()
{
   load();
}

/*---------------------------------------------------------------------------*/

Preferences::~Preferences()
{
    save();
}

/*---------------------------------------------------------------------------*/

Preferences* Preferences::inst()
{
    std::lock_guard<std::mutex> lock(_mutex);

    if (_this_inst == nullptr)
    {
        _this_inst = new Preferences();
    }
    return _this_inst;
}

/*---------------------------------------------------------------------------*/

QVariant Preferences::getValue(QString key)
{

    if (_pref_map.count(key) > 0)
    {
        return _pref_map.at(key);
    }
    return QVariant();

}

/*---------------------------------------------------------------------------*/

void Preferences::setValue(QString key, QVariant value)
{

    _pref_map[key] = value;

}

/*---------------------------------------------------------------------------*/

void Preferences::load()
{
    QSettings s(QSettings::IniFormat,
        QSettings::UserScope,
        INI_DIR,
        INI_FILENAME);

    // Write value
    s.beginGroup("Values");
    for(const auto &itr : s.allKeys())
    {
        _pref_map[itr] = s.value(itr);
    }
    s.endGroup();
}

/*---------------------------------------------------------------------------*/

void Preferences::save()
{
    QSettings s(QSettings::IniFormat,
        QSettings::UserScope,
        INI_DIR,
        INI_FILENAME);

    // Write value
    s.beginGroup("Values");
    for (const auto& itr : _pref_map)
    {
        s.setValue(itr.first, itr.second);
    }
    s.endGroup();
}

/*---------------------------------------------------------------------------*/
