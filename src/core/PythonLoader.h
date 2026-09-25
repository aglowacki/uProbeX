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

#ifndef PYTHON_LOADER_H
#define PYTHON_LOADER_H

//---------------------------------------------------------------------------

#include <pybind11/embed.h>
#include <stdexcept>
#include <memory>
#include <QString>
#include <QMap>
#include <QList>
#include <QStringList>
#include "core/defines.h"

//---------------------------------------------------------------------------

/**
 * @brief Singleton that embeds a Python interpreter (via pybind11) so uProbeX can
 * call user-supplied Python functions (VLM light-to-micro transforms, solvers,
 * region-box callbacks).
 */
class PythonLoader
{

public:

   /**
    * @brief Exception thrown for any load/call failure. Wraps either a plain
    * message or a pybind11::error_already_set (a Python-side exception/traceback).
    */
   class pyException : public std::runtime_error
   {
   public:
      explicit pyException(const QString& msg) : std::runtime_error(msg.toStdString()) {}

      explicit pyException(const pybind11::error_already_set& e) : std::runtime_error(e.what()) {}
   };

   /**
    * Destructor.
    */
   ~PythonLoader();

   /**
    * @brief inst
    * @return
    */
   static PythonLoader* inst();

   /**
    * @brief init: starts the embedded interpreter (once). Throws pyException on failure.
    * @return
    */
   bool init();

   /**
    * @brief isLoaded
    * @return
    */
   bool isLoaded() const;

   /**
    * @brief loadFunction: imports (or reloads, if already imported) moduleName from
    * path, and returns the named callable attribute. Throws pyException on failure.
    * @param path directory containing the .py file
    * @param moduleName basename of the .py file, without extension
    * @param functionName name of the function to look up in the module
    * @return
    */
   pybind11::function loadFunction(const QString& path, const QString& moduleName, const QString& functionName);

   /**
    * @brief getFunctionList: enumerates top-level functions defined in a script.
    * @param path
    * @param moduleName
    * @return
    */
   QStringList getFunctionList(const QString& path, const QString& moduleName);

   /**
    * @brief toPyDict: convert a QMap<QString,double> to a python dict.
    */
   static pybind11::dict toPyDict(const QMap<QString, double>& m);

   /**
    * @brief toPyTupleOfTuples: convert a list of coordinate rows into a python
    * tuple-of-tuples of floats.
    */
   static pybind11::list toPyTupleOfTuples(const QList<QList<double>>& ll);

   /**
    * @brief toQMapDouble: convert a python dict (string keys, numeric values) back
    * into a QMap<QString,double>.
    */
   static QMap<QString, double> toQMapDouble(const pybind11::dict& d);

private:

   /**
    * Constructor.
    */
   PythonLoader();

   PythonLoader(PythonLoader const&) = delete;
   PythonLoader& operator=(PythonLoader const&) = delete;

   static PythonLoader* m_inst;

   std::unique_ptr<pybind11::scoped_interpreter> m_interpreter;

   // Keyed by "path\x1Fmodule" so two different directories with the same script
   // basename never collide.
   QMap<QString, pybind11::module_> m_modules;

   bool m_loaded;

};

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
