/*
* Copyright (c) 2016, UChicago Argonne, LLC. All rights reserved.
*
* Copyright 2016. UChicago Argonne, LLC. This software was produced
* under U.S. Government contract DE-AC02-06CH11357 for Argonne National
* Laboratory (ANL), which is operated by UChicago Argonne, LLC for the
* U.S. Department of Energy. The U.S. Government has rights to use,
* reproduce, and distribute this software.  NEITHER THE GOVERNMENT NOR
* UChicago Argonne, LLC MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR
* ASSUMES ANY LIABILITY FOR THE USE OF THIS SOFTWARE.  If software is
* modified to produce derivative works, such modified software should
* be clearly marked, so as not to confuse it with the version available
* from ANL.

* Additionally, redistribution and use in source and binary forms, with
* or without modification, are permitted provided that the following
* conditions are met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*
*   * Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in
*     the documentation and/or other materials provided with the
*     distribution.
*
*   * Neither the name of UChicago Argonne, LLC, Argonne National
*     Laboratory, ANL, the U.S. Government, nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.

* THIS SOFTWARE IS PROVIDED BY UChicago Argonne, LLC AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL UChicago
* Argonne, LLC OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*/

#include <core/PythonLoader.h>
#include "core/defines.h"

namespace py = pybind11;

//---------------------------------------------------------------------------

static QString make_key(const QString& path, const QString& moduleName)
{
    return path + QChar(0x1F) + moduleName;
}

//---------------------------------------------------------------------------

static void append_path_if_missing(py::list& sysPath, const QString& path)
{
    std::string stdPath = path.toStdString();
    for (auto item : sysPath)
    {
        if (py::isinstance<py::str>(item) && item.cast<std::string>() == stdPath)
        {
            return;
        }
    }
    sysPath.append(stdPath);
}

//---------------------------------------------------------------------------

PythonLoader* PythonLoader::m_inst = nullptr;

PythonLoader::PythonLoader()
{
    m_loaded = false;
}

//---------------------------------------------------------------------------

PythonLoader::~PythonLoader()
{

}

//---------------------------------------------------------------------------

PythonLoader* PythonLoader::inst()
{
    if (m_inst == nullptr)
    {
        m_inst = new PythonLoader();
    }
    return m_inst;
}

//---------------------------------------------------------------------------

bool PythonLoader::init()
{
    if (m_loaded)
    {
        return true;
    }

    try
    {
        m_interpreter = std::make_unique<py::scoped_interpreter>();
    }
    catch (const std::exception& e)
    {
        throw pyException(QString("Failed to initialize Python interpreter: ") + e.what());
    }

    logI << "Python interpreter initialized.\n";
    m_loaded = true;
    return true;
}

//---------------------------------------------------------------------------

bool PythonLoader::isLoaded() const
{
    return m_loaded;
}

//---------------------------------------------------------------------------

pybind11::function PythonLoader::loadFunction(const QString& path, const QString& moduleName, const QString& functionName)
{
    if (!m_loaded)
    {
        throw pyException("PythonLoader is not initialized");
    }

    QString key = make_key(path, moduleName);

    try
    {
        py::module_ mod;
        auto it = m_modules.find(key);
        if (it != m_modules.end())
        {
            mod = it.value();
            mod.reload();
            logI << "Reloaded python module: " << moduleName.toStdString() << "\n";
        }
        else
        {
            py::module_ sysMod = py::module_::import("sys");
            py::list sysPath = sysMod.attr("path");
            append_path_if_missing(sysPath, path);

            mod = py::module_::import(moduleName.toStdString().c_str());
            logI << "Loaded python module: " << moduleName.toStdString() << "\n";
        }

        m_modules[key] = mod;

        py::object attr = mod.attr(functionName.toStdString().c_str());
        if (!py::isinstance<py::function>(attr))
        {
            throw pyException(QString("Function not callable: ") + functionName);
        }

        return attr.cast<py::function>();
    }
    catch (py::error_already_set& e)
    {
        throw pyException(e);
    }
}

//---------------------------------------------------------------------------

QStringList PythonLoader::getFunctionList(const QString& path, const QString& moduleName)
{
    QStringList out;
    if (!m_loaded)
    {
        return out;
    }

    try
    {
        py::module_ sysMod = py::module_::import("sys");
        py::list sysPath = sysMod.attr("path");
        append_path_if_missing(sysPath, path);

        py::module_ inspect = py::module_::import("inspect");
        py::module_ mod = py::module_::import(moduleName.toStdString().c_str());

        py::list members = inspect.attr("getmembers")(mod, inspect.attr("isfunction")).cast<py::list>();
        for (auto item : members)
        {
            py::tuple pair = item.cast<py::tuple>();
            out.append(QString::fromStdString(pair[0].cast<std::string>()));
        }
    }
    catch (py::error_already_set& e)
    {
        logW << "PythonLoader::getFunctionList failed: " << e.what() << "\n";
    }

    return out;
}

//---------------------------------------------------------------------------

pybind11::dict PythonLoader::toPyDict(const QMap<QString, double>& m)
{
    py::dict d;
    for (auto it = m.constBegin(); it != m.constEnd(); ++it)
    {
        d[it.key().toStdString().c_str()] = it.value();
    }
    return d;
}

//---------------------------------------------------------------------------

pybind11::list PythonLoader::toPyTupleOfTuples(const QList<QList<double>>& ll)
{
    py::list outer;
    for (const QList<double>& row : ll)
    {
        py::list inner;
        for (double v : row)
        {
            inner.append(v);
        }
        outer.append(inner);
    }
    return outer;
}

//---------------------------------------------------------------------------

QMap<QString, double> PythonLoader::toQMapDouble(const pybind11::dict& d)
{
    QMap<QString, double> out;
    for (auto item : d)
    {
        QString key = QString::fromStdString(py::str(item.first).cast<std::string>());
        double val = item.second.cast<double>();
        out.insert(key, val);
    }
    return out;
}

//---------------------------------------------------------------------------
