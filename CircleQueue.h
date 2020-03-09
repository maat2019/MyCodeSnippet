
template<typename T , unsigned int size>
class TCircleQueue
{
public:
    TCircleQueue() :_head(0),_tail(0)
    {
        ::InitializeCriticalSection(&_enqueuelock);
        ::InitializeCriticalSection(&_dequeuelock);
        _enqueuesize=CreateSemaphore(NULL,size,size,NULL);
        _dequeuesize=CreateSemaphore(NULL,0,size,NULL);
    }
    ~TCircleQueue()
    {
        CloseHandle(_enqueuesize);
        CloseHandle(_dequeuesize);
        ::DeleteCriticalSection(&_enqueuelock);
        ::DeleteCriticalSection(&_dequeuelock);

    }
    void re_init()
    {
        _head = 0;
        _tail = 0;
        ReleaseSemaphore(_enqueuesize,size,0);

            for (int k=1;k<5;k++)
            {

                while(1){
                    DWORD i=WaitForSingleObject(_dequeuesize,0);
                    if(i!=0)
                        break;
                }
            }
    }
    bool full()
    {
        return (_tail + 1) % size == _head;
    }

    bool empty()
    {
        return _head == _tail;
    }

    bool enqueue(T &p,DWORD  waittime = INFINITE) 
    {
        int rc = ::WaitForSingleObject(_enqueuesize, waittime );
        if (rc != 0)			
            return false;
        ::EnterCriticalSection(&_enqueuelock);
        _data[_tail] = p;
        _tail = (_tail + 1) % size;
        ::LeaveCriticalSection(&_enqueuelock);
        ReleaseSemaphore(_dequeuesize,1,0);
        return true;
    }

    bool dequeue(T &o, int waittime = INFINITE) 
    {
        int rc = ::WaitForSingleObject(_dequeuesize, waittime );
        if (rc != 0) return false;
      ::EnterCriticalSection(&_dequeuelock);
        o = _data[_head];
        _head = (_head + 1) % size;
        ::LeaveCriticalSection(&_dequeuelock);
        ReleaseSemaphore(_enqueuesize,1,0);
        return true;
    }
    
    HANDLE & get_dequeueSem()
    {
        return _dequeuesize;
    }

    bool dequeue_unsem(T &o) 
    {
  
       ::EnterCriticalSection(&_dequeuelock);
       if (empty())
       {
           ::LeaveCriticalSection(&_dequeuelock);
           return false;
       }
        o = _data[_head];
        _head = (_head + 1) % size;
        ::LeaveCriticalSection(&_dequeuelock);
        ReleaseSemaphore(_enqueuesize,1,0);
        return true;
    }

private:
    T                       _data[size];
    unsigned int            _head;	
    unsigned int            _tail;	
    CRITICAL_SECTION        _enqueuelock;
    CRITICAL_SECTION        _dequeuelock;
    HANDLE                  _enqueuesize;
    HANDLE                  _dequeuesize;
};


#endif