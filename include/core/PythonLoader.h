/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef PYTHON_LOADER_H
#define PYTHON_LOADER_H

/*---------------------------------------------------------------------------*/

//#include <python2.7/Python.h>
#include <exception>
#include <QString>
#include <QMap>
#include <QStringList>
#include <QLibrary>
#include <QMessageBox>

/*---------------------------------------------------------------------------*/

/**
 * @brief Singleton class to call python scripts.
 *
 */
class PythonLoader 
{

public:

   /**
    * @brief The pyException class
    */
   class pyException: public std::exception
   {
   public:
      /**
       * @brief The pyError enum
       */
      enum pyError{ FUNC_NOT_FOUND
                   ,FUNC_PTR_NULL
                   ,CALL_FUNC_THROW_EXCP
                   ,RETURN_WRONG_LIST_SIZE
                   ,RETURN_WRONG_DICT_SIZE
                   ,NULL_RETURNED_FOR_LIST
                   ,NULL_RETURNED_FOR_DICT
                   ,VALUE_DOES_NOT_EXIST_FOR_KEY
                   ,VOID_CALL_FAILED
                   ,PYTHON_NOT_FOUND_ON_SYSTEM
                   ,COULD_NOT_FIND_MODULE
                   ,COULD_NOT_FIND_FUNCTION
                   ,COULD_NOT_APPEND_PATH_TO_PYPATH
      };

      /**
       * @brief pyException: Set the string representation of the exception
       * @param er
       */
      pyException(QString er)
      {
         m_strErr = er;
      }

      /**
       * @brief pyException: Set the exception by enum pyError
       * @param er
       */
      pyException(pyError er)
      {
         m_er = er;
         m_strErr = "";
      }

      ~pyException() throw (){}

      /**
       * @brief what: Return string reresentation of pyError enum
       * @return
       */
      virtual const char* what() const throw()
      {
         if(m_strErr.length() > 0)
            return m_strErr.toLatin1();

         switch (m_er)
         {
         case FUNC_NOT_FOUND:
            return "Function not found in map";
         case FUNC_PTR_NULL:
            return "Function pointer null";
         case CALL_FUNC_THROW_EXCP:
            return "Exception was catch in PythonLoader::callFunc";
         case RETURN_WRONG_LIST_SIZE:
            return "Function returned wrong list size";
         case RETURN_WRONG_DICT_SIZE:
            return "Function returned wrong dictionary size";
         case NULL_RETURNED_FOR_LIST:
            return "Function returned NULL for list, A variable may be mispelled?";
         case NULL_RETURNED_FOR_DICT:
            return "Function returned NULL for dictionary";
         case VALUE_DOES_NOT_EXIST_FOR_KEY:
            return "Value was not found for key in dictionary";
         case VOID_CALL_FAILED:
            return "Failed to call the python function";
         case PYTHON_NOT_FOUND_ON_SYSTEM:
            return "Python was not found on the system.";
         case COULD_NOT_FIND_MODULE:
            return "Could not find or load python module";
         case COULD_NOT_FIND_FUNCTION:
            return "Could not find or load python function";
         case COULD_NOT_APPEND_PATH_TO_PYPATH:
            return "Could not append path to Python lib path";
         default:
            break;
            //return "Unknown error";
         }
         return "";
      }

      /**
       * @brief whatCode: returns pyError enum exception code
       * @return
       */
      pyError whatCode() const throw()
      {
         return m_er;
      }

   private:
      /**
       * @brief m_er: Saved exception enum
       */
      pyError m_er;

      /**
       * @brief m_strErr: Saved exception string
       */
      QString m_strErr;
   };

   /**
    * @brief The pyReturnType enum: Used by callFunc()
    */
   enum pyReturnType{RET_NONE, RET_LIST, RET_DICT};

   /**
    * Destructor.
    */
   ~PythonLoader();

   /**
    * @brief callFunc
    * @param module
    * @param func
    * @return
    */
   void callFunc(QString module, QString func, pyReturnType returnType);

   /**
    * @brief inst
    * @return
    */
   static PythonLoader* inst();

   /**
    * @brief init
    * @return
    */
   bool init();

   /**
    * @brief init
    * @param sharedLibName
    * @return
    */
   bool init(QString sharedLibName);

   /**
    * @brief LoadPythonFunction
    * @param path
    * @param moduleName
    * @param functionnName
    * @return
    */
   bool loadFunction(QString path, QString moduleName, QString functionnName);

   /**
    * @brief getFunctionList
    * @param filename
    * @return
    */
   QStringList getFunctionList(QString path, QString moduleName);

   /**
    * @brief isLoaded
    * @return
    */
   bool isLoaded();

   /**
    * @brief getRetDict
    * @param module
    * @param func
    * @param val
    * @return
    */
   bool getRetDict(QString module, QString func, QMap<QString, double> *val);

   /**
    * @brief getRetDouble
    * @param module
    * @param func
    * @param idx
    * @param val
    * @return
    */
   bool getRetDouble(QString module, QString func, int idx, double *val);

   /**
    *
   */
   bool safeCheck();

   /**
    * @brief setArgs
    * @param module
    * @param func
    * @param globalVars
    * @return
    */
   bool setArgDict(QString module,
                   QString func,
                   int idx,
                   QMap<QString, double> globalVars);

   /**
    * @brief setArgTupleTuples
    * @param module
    * @param func
    * @param idx
    * @param globalTuple
    * @return
    */
   bool setArgTupleTuples(QString module,
                          QString func,
                          int idx,
                          QList < QList<double> > globalTuple);

   /**
    * @brief setArgDouble
    * @param module
    * @param func
    * @param idx
    * @param val
    * @return
    */
   bool setArgDouble(QString module, QString func, int idx, double val);

   /**
    * @brief setArgString
    * @param module
    * @param func
    * @param idx
    * @param val
    * @return
    */
   bool setArgString(QString module, QString func, int idx, QString val);

   /**
    * @brief setArgInt
    * @param module
    * @param func
    * @param idx
    * @param val
    * @return
    */
   bool setArgLong(QString module, QString func, int idx, long val);

   /**
    * @brief setNumArgs
    * @param module
    * @param func
    * @param amt
    * @return
    */
   bool setNumArgs(QString module, QString func, int amt);

   /**
    * @brief setRetCnt
    * @param module
    * @param func
    * @param amt
    * @return
    */
   bool setRetCnt(QString module, QString func, int amt);

private:

   /**
    * Constructor.
    */
   PythonLoader();

   /**
    * @brief PythonLoader
    */
   PythonLoader(PythonLoader const&){};

   static PythonLoader* m_inst;

    typedef long int Py_ssize_t;
    typedef struct _object
    {
       Py_ssize_t ob_refcnt; struct _typeobject *ob_type;
    } PyObject;

    typedef void (*Py_InitializeDef)();
    typedef void (*Py_FinalizeDef)();
    typedef void (*PyRun_SimpleStringDef)(const char*);
    typedef PyObject* (*PySys_GetObjectDef)(const char*);
    typedef PyObject* (*PyString_FromStringDef)(const char*);
    typedef int (*PyList_AppendDef)(PyObject*,PyObject*);
    typedef PyObject* (*PyImport_ImportDef)(PyObject*);
    typedef PyObject* (*PyObject_GetAttrStringDef)(PyObject*, const char*);
    typedef int (*PyCallable_CheckDef)(PyObject*);
    typedef PyObject* (*PyDict_NewDef)();
    typedef PyObject* (*PyTuple_NewDef)(int);
    typedef PyObject* (*PyLong_FromLongDef)(long);
    typedef PyObject* (*PyFloat_FromDoubleDef)(double);
    typedef int (*PyDict_SetItemDef)(PyObject*,PyObject*,PyObject*);
    typedef void (*Py_DecRefDef)(PyObject*);
    typedef void (*Py_IncRefDef)(PyObject*);
    typedef int (*PyTuple_SetItemDef)(PyObject*, long, PyObject*);
    typedef PyObject* (*PyObject_CallObjectDef)(PyObject*,PyObject*);
    typedef long (*PyList_SizeDef)(PyObject*);
    typedef PyObject* (*PyList_GetItemDef)(PyObject*, long);
    typedef PyObject* (*PyTuple_GetItemDef)(PyObject*, long);
    typedef double (*PyFloat_AsDoubleDef)(PyObject*);
    typedef char* (*PyString_AsStringDef)(PyObject*);
    typedef PyObject* (*PyDict_KeysDef)(PyObject*);
    typedef int (*PyDict_SizeDef)(PyObject*);
    typedef PyObject* (*PyDict_GetItemDef)(PyObject*, PyObject*);

    Py_InitializeDef Py_Initialize;
    Py_FinalizeDef Py_Finalize;
    PyRun_SimpleStringDef PyRun_SimpleString;
    PySys_GetObjectDef PySys_GetObject;
    PyString_FromStringDef PyString_FromString;
    PyList_AppendDef PyList_Append;
    PyImport_ImportDef PyImport_Import;
    PyObject_GetAttrStringDef PyObject_GetAttrString;
    PyCallable_CheckDef PyCallable_Check;
    PyDict_NewDef PyDict_New;
    PyTuple_NewDef PyTuple_New;
    PyLong_FromLongDef PyLong_FromLong;
    PyFloat_FromDoubleDef PyFloat_FromDouble;
    PyDict_SetItemDef PyDict_SetItem;
    Py_IncRefDef Py_IncRef;
    Py_DecRefDef Py_DecRef;
    PyTuple_SetItemDef PyTuple_SetItem;
    PyObject_CallObjectDef PyObject_CallObject;
    PyList_SizeDef PyList_Size;
    PyList_GetItemDef PyList_GetItem;
    PyTuple_GetItemDef PyTuple_GetItem;
    PyFloat_AsDoubleDef PyFloat_AsDouble;
    PyString_AsStringDef PyString_AsString;
    PyDict_KeysDef PyDict_Keys;
    PyDict_SizeDef PyDict_Size;
    PyDict_GetItemDef PyDict_GetItem;

    /**
    * @brief The pyFunc struct
    */
   struct pyFunc
   {
      /**
       * @brief pModule
       */
      PyObject *pModule;

      /**
       * @brief pFunc
       */
      PyObject *pFunc;

      /**
       * @brief pArgs
       */
      PyObject* pArgs;

      /**
       * @brief pDict
       */
      PyObject *pDict;

      /**
       * @brief pTuple
       */
      PyObject *pTuple;

      /**
       * @brief pTupleTuple
       */
      PyObject *pTupleTuple;

      /**
       * @brief argCnt
       */
      int argCnt;

      /**
       * @brief retCnt
       */
      int retCnt;

      /**
       * @brief retVals
       */
      double* retVals;

      /**
       * @brief retDict
       */
      QMap<QString, double> retDict;

   };

   /**
    * @brief m_functionMap
    */
   QMap<QString, pyFunc*> m_functionMap;

   /**
    * @brief m_foundFuncs
    */
   bool m_foundFuncs;


   bool m_safeCheckFailed;

   /**
    * @brief myLib
    */
   QLibrary myLib;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
