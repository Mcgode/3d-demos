/**
 * @file
 * @author Max Godefroy
 * @date 27/02/2021.
 */

#include "kryne-engine/Dispatch/MainPool.h"

void MainPool::synchronize(SynchronizablePool *pool)
{
    pool->overrideSynchronizeWait(&this->waitCondition);

    for (;;)
    {
        function <void()> task;
        {
            Utils::double_lock lock(*this->mainMutex, *pool->getMutex());

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

        task();
    }

    pool->overrideSynchronizeWait(nullptr, true);
}


void MainPool::swapQueues(queue<function<void()>> &swapQueue, bool allowNonEmpty)
{
    {
        scoped_lock<mutex> lock(*this->mainMutex);

        if (!allowNonEmpty && !this->tasks.empty())
            throw runtime_error("Can't swap non-empty task queue");

        swap(swapQueue, this->tasks);
    }

    this->waitCondition.notify_all();
}
