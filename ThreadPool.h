#ifndef CPPWEBSERVER_THREADPOOL_H
#define CPPWEBSERVER_THREADPOOL_H

#include <queue>
#include <vector>
#include <pthread.h>

class Task
{
public:
    Task(void *(*process)(void *), void *arg)
        : m_process(process), m_arg(arg)
    {}

public:
    void *(*m_process)(void *);
    void *m_arg;
};

class ThreadPool
{
public:
    ThreadPool(int num)
        : m_tids(num), m_count(num), m_isClose(false)
    {
        pthread_mutex_init(&m_mutex, nullptr);
        pthread_cond_init(&m_cond, nullptr);

        for(int i=0; i<num; i++)
        {
            pthread_create(&m_tids[i], nullptr, Routine, this);
            pthread_detach(m_tids[i]);
        }
    }

    ~ThreadPool()
    {
        pthread_mutex_destroy(&m_mutex);
        pthread_cond_destroy(&m_cond);
    }

    bool AddTask(const Task &task)
    {
        pthread_mutex_lock(&m_mutex);

        m_taskList.push(task);

        pthread_mutex_unlock(&m_mutex);

        pthread_cond_signal(&m_cond);

        return true;
    }

    bool Close()
    {
        pthread_mutex_lock(&m_mutex);

        m_isClose = true;
        pthread_cond_broadcast(&m_cond);

        while(m_count)
        {
            pthread_cond_wait(&m_cond, &m_mutex);
        }

        pthread_mutex_unlock(&m_mutex);

        return true;
    }

private:
    static void *Routine(void *arg)
    {
        ThreadPool *pool = (ThreadPool *)arg;

        while(true)
        {
            pthread_mutex_lock(&pool->m_mutex);

            while(pool->m_taskList.size()==0 && pool->m_isClose==false)
            {
                pthread_cond_wait(&pool->m_cond, &pool->m_mutex);
            }

            if(pool->m_taskList.size())
            {
                Task newTask = pool->m_taskList.front();
                pool->m_taskList.pop();
                pthread_mutex_unlock(&pool->m_mutex);

                newTask.m_process(newTask.m_arg);
            }
            else if(pool->m_isClose)
            {
                pool->m_count --;
                pthread_mutex_unlock(&pool->m_mutex);

                if(pool->m_count == 0)
                {
                    pthread_cond_signal(&pool->m_cond);
                }

                return nullptr;
            }
        }

        return nullptr;
    }

private:
    std::queue<Task> m_taskList;
    std::vector<pthread_t> m_tids;

    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
    bool m_isClose;
    int m_count;
};

#endif //CPPWEBSERVER_THREADPOOL_H
