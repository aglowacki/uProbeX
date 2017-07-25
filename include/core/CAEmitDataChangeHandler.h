/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef TXM_CA_EMIT_DATA_CHANGE_HANDLER_H
#define TXM_CA_EMIT_DATA_CHANGE_HANDLER_H

/*---------------------------------------------------------------------------*/

#include <handlers/base/CAEventHandler.h>

#include <cadef.h>

#include <QString>
#include <qdebug.h>

#include <iostream>

/*---------------------------------------------------------------------------*/

/**
 * @brief Template class for data change handler.
 */
template <class T, class K>
class CAEmitDataChangeHandler
: public CAEventHandler
{

public:

   /**
    * Constructor
    *
    * @param model
    * @param funcPtr
    *
    */
   CAEmitDataChangeHandler(K* model, void (K::*funcPtr)(T));

   /**
    * On failure function
    */
   virtual void onFailure();

   /**
    * On success function
    */
   virtual void onSuccess();

private:

   /**
    * Motor model
    */
   K* m_model;

   /**
    * Function pointer
    */
   void (K::*m_funcPtr)(T);

};

/*---------------------------------------------------------------------------*/

/**
 * Partial Template specialization
 */
template <class K>
class CAEmitDataChangeHandler<QString, K>
: public CAEventHandler
{

public:

   /**
    * Constructor
    *
    * @param model
    * @param funcPtr
    *
    */
   CAEmitDataChangeHandler(K* model, void (K::*funcPtr)(QString));

   /**
    * On failure function
    */
   virtual void onFailure();

   /**
    * On success function
    */
   virtual void onSuccess();

private:

   /**
    * Motor model
    */
   K* m_model;

   /**
    * Function pointer
    */
   void (K::*m_funcPtr)(QString);

};

/*---------------------------------------------------------------------------*/

template <class T, class K>
CAEmitDataChangeHandler<T,K>::CAEmitDataChangeHandler(K* model,
                            void (K::*funcPtr)(T))
: CAEventHandler(), m_model(model), m_funcPtr(funcPtr)
{

}

/*---------------------------------------------------------------------------*/

template <class T, class K>
void CAEmitDataChangeHandler<T,K>::onFailure()
{

   std::cout << "FAILURE" << std::endl;

}

/*---------------------------------------------------------------------------*/

template <class T, class K>
void CAEmitDataChangeHandler<T,K>::onSuccess()
{

   T* val = (T*) m_args.dbr;
   (m_model->*m_funcPtr)((T) val[0]);

}

/*---------------------------------------------------------------------------*/

template <class K>
CAEmitDataChangeHandler<QString, K>::CAEmitDataChangeHandler(K* model,
                            void (K::*funcPtr)(QString))
: CAEventHandler(), m_model(model), m_funcPtr(funcPtr)
{

}

/*---------------------------------------------------------------------------*/

template <class K>
void CAEmitDataChangeHandler<QString, K>::onFailure()
{

   std::cout << "FAILURE" << std::endl;

}

/*---------------------------------------------------------------------------*/

template <class K>
void CAEmitDataChangeHandler<QString, K>::onSuccess()
{

   char* val = (char*) m_args.dbr;

   if (m_args.count < strlen(val)){ 
      (m_model->*m_funcPtr)(QString::fromLatin1(val, m_args.count)); 
   } 
   else { 
      (m_model->*m_funcPtr)(QString::fromLatin1(val, strlen(val))); 
   } 

}

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
