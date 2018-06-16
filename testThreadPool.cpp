// test threadpool

#include "ThreadPool.h"
#include <iostream>
#include <unistd.h>

pthread_mutex_t mutexStdout;

void *test(void *arg)
{
    int *test2 = (int *)arg;

    pthread_mutex_lock(&mutexStdout);
    std::cout << pthread_self() << " "<< *test2 << std::endl;
    pthread_mutex_unlock(&mutexStdout);

    sleep(2);

    delete test2;

    return nullptr;
}

int main()
{
    pthread_mutex_init(&mutexStdout, nullptr);

    ThreadPool pool(8);

    for (int i = 0; i < 100; i++)
    {
        int *arg = new int;
        *arg = i;
        pool.AddTask(Task(test, arg));
    }

    pool.Close();

    pthread_mutex_destroy(&mutexStdout);

    return 0;
}