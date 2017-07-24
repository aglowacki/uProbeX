/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

/*---------------------------------------------------------------------------*/

#include <boost/thread.hpp>
#include <vector>

/*---------------------------------------------------------------------------*/

/**
 * @brief Circular buffer...
 */
template <class T>
class CircularBuffer
{

public:

   /**
    * Constructor. Creates a circular buffer with 100 elements.
    */
   CircularBuffer();

   /**
    * Destructor
    */
   ~CircularBuffer();

   /**
    * Get next element from buffer. Blocks waiting for a signal.
    *
    * return reference to next element in buffer.
    */
   void pop(T& item);

   /**
    * Push an item onto the buffer.
    *
    * @param item - item to push onto buffer.
    */
   void push(T& item);

   /**
    * Resize buffer.
    *
    * @param size - new size.
    */
   void resize(int size);

private:

   /**
    * Mutex variable
    */
   boost::mutex m_mutex;

   /**
    * Condition variable
    */
   boost::condition_variable m_cond;

   /**
    * Circular buffer
    */
   std::vector<T> m_buffer;

   /**
    * Position of next insert in circular buffer
    */
   int m_insertPosition;

};

/*---------------------------------------------------------------------------*/

template <class T>
CircularBuffer<T>::CircularBuffer()
{

   // Set size
   m_buffer.resize(100);

   // Set initial insert position
   m_insertPosition = 0;

}

/*---------------------------------------------------------------------------*/

template <class T>
CircularBuffer<T>::~CircularBuffer()
{
}

/*---------------------------------------------------------------------------*/

template <class T>
void CircularBuffer<T>::pop(T& item)
{

   // Lock mutex and wait for signal
   boost::unique_lock<boost::mutex> lock(m_mutex);
   m_cond.wait(lock);

   // Calculate read position
   int readPos = -1;
   if (m_insertPosition - 1 < 0) {
      readPos = m_buffer.capacity() - 1;
   }
   else {
      readPos = (m_insertPosition - 1) % m_buffer.capacity();
   }

   // Return reference to item
   item = m_buffer[readPos];

}

/*---------------------------------------------------------------------------*/

template <class T>
void CircularBuffer<T>::push(T& item)
{

   // Lock mutex
   boost::lock_guard<boost::mutex> guard(m_mutex);

   // Assign item to buffer
   m_buffer[m_insertPosition] = item;

   // Update insert position
   m_insertPosition = (m_insertPosition + 1) % m_buffer.capacity();

   // Notify waiting thread
   m_cond.notify_all();

}

/*---------------------------------------------------------------------------*/

template <class T>
void CircularBuffer<T>::resize(int size)
{

   // Check for invalid size
   if (size <= 0) {
      return;
   }

   // Lock mutex
   boost::lock_guard<boost::mutex> guard(m_mutex);

   // Resize buffer
   m_buffer.resize(size);

   // Reset insert position
   if (m_insertPosition >= size) {
      m_insertPosition = 0;
   }

}

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
