/*---------------------------------------------------------------------------*/

#include <proc/Thread.h>

using boost::bind;
using boost::shared_ptr;
using boost::thread;

/*---------------------------------------------------------------------------*/

Thread::Thread()
{

   m_isRunning = false;

}

/*---------------------------------------------------------------------------*/

Thread::~Thread()
{
}

/*---------------------------------------------------------------------------*/

void Thread::interrupt()
{
   
   m_thread->interrupt();

}

/*---------------------------------------------------------------------------*/

bool Thread::isRunning()
{

   return m_isRunning;

}

/*---------------------------------------------------------------------------*/

void Thread::interuptionPoint()
{

   // Check for interruption
   boost::this_thread::interruption_point();

}

/*---------------------------------------------------------------------------*/

void Thread::start()
{

   // Create thread and start it
   m_thread = shared_ptr<thread>(new thread(bind(&Thread::run, this)));

   // Set to running
   m_isRunning = true;

}

/*---------------------------------------------------------------------------*/

void Thread::stop()
{

   // Don't do anything if it's not running
   if (m_isRunning == false) {
      return;
   }

   // Interrupt thread
   m_thread -> interrupt();

   if(m_thread->joinable())
   {
      //wait 5 seconds for join
     boost::posix_time::time_duration timeout =
           boost::posix_time::milliseconds(5000);
      // Wait for thread to finish
      m_thread -> timed_join(timeout);
   }

   // Set to not running
   m_isRunning = false;

}

/*---------------------------------------------------------------------------*/
