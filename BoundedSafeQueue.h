//
// Created by Rabia on 21/12/2018.
//

#ifndef SAFE_QUEUE_SAFEQUEUE_H
#define SAFE_QUEUE_SAFEQUEUE_H

#include <condition_variable>
#include <queue>
#include <pthread.h>


template<typename T>
class BoundedSafeQueue
{

public:

    explicit BoundedSafeQueue (size_t bound);

    ~BoundedSafeQueue ();

    void push (const T &item);

    void pop (T &item);

    bool empty () const;

private:

    size_t              m_bound;
    size_t              m_size;

    std::queue<T>       m_queue;

    pthread_mutex_t     m_mutex;
    pthread_cond_t      m_nonempty;
    pthread_cond_t      m_nonfull;

    // prevent copying
    BoundedSafeQueue(const BoundedSafeQueue& safeQueue);
    BoundedSafeQueue& operator=(const BoundedSafeQueue& safeQueue);

    bool isFull() const;
    bool isEmpty() const;

};


template <typename T>
inline bool BoundedSafeQueue<T>::isFull () const
{ return m_size == m_bound; }


template <typename T>
inline bool BoundedSafeQueue<T>::isEmpty () const
{ return m_size == 0; }


template<typename T>
BoundedSafeQueue<T>::
BoundedSafeQueue (size_t bound)
        :m_bound (bound), m_size (0)
{
    pthread_mutex_init (&m_mutex, NULL);
    pthread_cond_init (&m_nonempty, NULL);
    pthread_cond_init (&m_nonfull, NULL);
}


template<typename T>
BoundedSafeQueue<T>::
~BoundedSafeQueue ()
{
    pthread_mutex_destroy (&m_mutex);
    pthread_cond_destroy (&m_nonfull);
    pthread_cond_destroy (&m_nonempty);
}


template<typename T>
void BoundedSafeQueue<T>::
push (const T &item)
{
    // acquire lock and while full: wait
    // at the nonfull queue till consumer takes
    // an item
    pthread_mutex_lock (&m_mutex);
    while (isFull ()) pthread_cond_wait (&m_nonfull, &m_mutex);

    m_queue.push (item);
    ++m_size;

    // signal to waiting consumers queue
    // that an item was produced
    pthread_cond_signal (&m_nonempty);
    pthread_mutex_unlock (&m_mutex);
}


template<typename T>
void BoundedSafeQueue<T>::
pop (T &item)
{
    // acquire lock, then while empty:
    // wait at the nonempty queue till
    // a producer adds an item
    pthread_mutex_lock (&m_mutex);
    while (isEmpty ()) pthread_cond_wait (&m_nonempty, &m_mutex);

    item = m_queue.front ();
    m_queue.pop ();
    --m_size;

    // signal to waiting producers that
    // an item was consumed
    pthread_cond_signal (&m_nonfull);
    pthread_mutex_unlock (&m_mutex);
}


template<typename T>
bool BoundedSafeQueue<T>::
empty () const
{
    return m_queue.empty ();
}


#endif //SAFE_QUEUE_SAFEQUEUE_H
