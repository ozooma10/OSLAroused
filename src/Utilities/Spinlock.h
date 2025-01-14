#pragma once

#include <atomic>

namespace
{
    class Spinlock
    {
    public:
        void Lock()
        {
            while (_lock.exchange(true))
            {
                ; // do nothing
            }
        }
        void Unlock()
        {
            _lock.store(false);
        }
    private:
        mutable std::atomic<bool> _lock   = false;
    };

    class UniqueLock
    {
    public:
        UniqueLock(Spinlock& a_lock)
        {
            _lock = &a_lock;
            _lock->Lock();
        }
        ~UniqueLock()
        {
            _lock->Unlock();
        }
    private:
        mutable Spinlock* _lock;
    };
}