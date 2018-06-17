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
    ThreadPool(int num);

    ~ThreadPool();

    bool AddTask(const Task &task);

    bool Close();

private:
    static void *Routine(void *arg);

private:
    std::queue<Task> m_taskList;
    std::vector<pthread_t> m_tids;

    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
    bool m_isClose;
    int m_count;
};

#endif //CPPWEBSERVER_THREADPOOL_H
