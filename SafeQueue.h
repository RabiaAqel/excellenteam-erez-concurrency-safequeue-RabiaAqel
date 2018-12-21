//
// Created by Rabia on 21/12/2018.
//

#ifndef SAFE_QUEUE_SAFEQUEUE_H
#define SAFE_QUEUE_SAFEQUEUE_H

#include <condition_variable>
#include <queue>
#include <pthread.h>


template<typename T>
class SafeQueue
{

public:

    explicit SafeQueue (size_t capacity);

    ~SafeQueue ();

    void push (const T &item);

    void pop (T &item);

    bool empty () const;

private:

    size_t              m_capacity;
    size_t              m_size;

    std::queue<T>       m_queue;

    pthread_mutex_t     m_mutex;
    pthread_cond_t      m_nonempty;
    pthread_cond_t      m_nonfull;

    // prevent copying
    SafeQueue(const SafeQueue& safeQueue);
    SafeQueue& operator=(const SafeQueue& safeQueue);

};


template<typename T>
SafeQueue<T>::
SafeQueue (size_t capacity)
        :m_capacity (capacity), m_size (0)
{
    pthread_mutex_init (&m_mutex, NULL);
    pthread_cond_init (&m_nonempty, NULL);
    pthread_cond_init (&m_nonfull, NULL);
}


template<typename T>
SafeQueue<T>::
~SafeQueue ()
{
    pthread_mutex_destroy (&m_mutex);
    pthread_cond_destroy (&m_nonfull);
    pthread_cond_destroy (&m_nonempty);
}


template<typename T>
void SafeQueue<T>::
push (const T &item)
{
    pthread_mutex_lock (&m_mutex);
    while (m_size == m_capacity) pthread_cond_wait (&m_nonfull, &m_mutex);

    m_queue.push (item);
    ++m_size;

    pthread_cond_signal (&m_nonempty);
    pthread_mutex_unlock (&m_mutex);
}


template<typename T>
void SafeQueue<T>::
pop (T &item)
{
    pthread_mutex_lock (&m_mutex);
    while (m_size == 0) pthread_cond_wait (&m_nonempty, &m_mutex);

    item = m_queue.front ();
    m_queue.pop ();
    --m_size;

    pthread_cond_signal (&m_nonfull);
    pthread_mutex_unlock (&m_mutex);
}


template<typename T>
bool SafeQueue<T>::
empty () const
{
    return m_queue.empty ();
}


#endif //SAFE_QUEUE_SAFEQUEUE_H
