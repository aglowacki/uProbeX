/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef THREAD_H
#define THREAD_H

/*---------------------------------------------------------------------------*/

#include <boost/thread.hpp>

/*---------------------------------------------------------------------------*/

/**
 * @brief Thread class includes run start stop.
 */
class Thread
{

public:

   /**
    * Constructor.
    */
   Thread();

   /**
    * Destructor.
    */
   virtual ~Thread();

   void interrupt();

   /**
    * Check if thread is running
    *
    * @return true if thread is running, false otherwise.
    */
   bool isRunning();

   /**
    * Start thread
    */
   void start();

   /**
    * Stop thread
    */
   void stop();

protected:

   /**
    * The run method executed by the thread. This must be implemented in
    * a subclass and usually contains a loop.
    */
   virtual void run() = 0;

   /**
    * Signal an interruption point.
    *
    * @throws boost::thread_interrupted
    */
   void interuptionPoint();

private:

   /**
    * The thread
    */
   boost::shared_ptr<boost::thread> m_thread;

   /**
    * Thread running flag
    */
   bool m_isRunning;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
