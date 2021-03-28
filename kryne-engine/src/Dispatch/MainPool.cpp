/**
 * @file
 * @author Max Godefroy
 * @date 27/02/2021.
 */

#include "kryne-engine/Dispatch/MainPool.h"

void MainPool::synchronize(SynchronizablePool *pool)
{
    pool->overrideSynchronizeWait(&this->waitCondition);

    {
        unique_lock<mutex> l(*this->mainMutex);
        this->mainMutex = pool->getMutex();
    }

    for (;;)
    {
        function <void()> task;
        {
            unique_lock<mutex> lock(*this->mainMutex);

            bool synchronize;
            this->waitCondition.wait(lock, [this, pool, &synchronize]
            {
                synchronize = pool->trySynchronize();
                return synchronize || !this->tasks.empty();
            });

            if (synchronize && this->tasks.empty())
                break;

            task = move(this->tasks.front());
            this->tasks.pop();
        }

        try
        {
            task();
        }
        catch (bad_function_call &e)
        {
            cerr << "Function error: " << e.what() << endl;
        }
    }

    {
        unique_lock<mutex> l(*this->mainMutex);
        this->mainMutex = &this->_mainMutex;
    }

    pool->overrideSynchronizeWait(nullptr, true);
}


void MainPool::swapQueues(queue<function<void()>> &swapQueue, bool allowNonEmpty)
{
    {
        unique_lock<mutex> lock(*this->mainMutex);

        if (!allowNonEmpty && !this->tasks.empty())
            throw runtime_error("Can't swap non-empty task queue");

        swap(swapQueue, this->tasks);
    }

    this->waitCondition.notify_all();
}
