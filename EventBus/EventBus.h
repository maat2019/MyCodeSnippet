#pragma once
#include <map>
#include <list>

struct CounterTypeInfo
{
    static int Type()
    {
        static int count = 0;
        return count++;
    }
};

struct BaseEvent
{
    virtual ~BaseEvent() {}
};

template<typename type, typename TypeInfo = CounterTypeInfo>
class Event : public BaseEvent
{
public:
    virtual ~Event() {}

public:
    static int Type()
    {
        static int type = TypeInfo::Type();
        return type;
    }
};

class IEventHandler
{
public:
    virtual ~IEventHandler() {};

public:
    virtual void Dispatch(const BaseEvent* event) = 0;
};

template <typename E, typename C, typename F>
class EventHandler : public IEventHandler
{
    typedef void(C::*Handler)(const E*);

    C* _owner;
    Handler _handler;

public:
    EventHandler(C* owner, F memberFun) : _owner(owner), _handler(memberFun) {}
    virtual ~EventHandler() {}

public:
    virtual void Dispatch(const BaseEvent* e) 
    {
        if (!e)
            return;

        invoke(dynamic_cast<const E*>(e));
    }

    bool Compare(C* owner, F memberFun)
    {
        return owner == _owner && memberFun == _handler;
    }

protected:
    void invoke(const E* e)
    {
        if (!e)
            return;
        
        (_owner->*_handler)(e);
    }
};

class EventBus
{
    std::map<int, std::list<IEventHandler*> > _eventHandlers;

public:
    EventBus(void) {};
    ~EventBus(void) {};

private:
    EventBus(const EventBus&) {}
    EventBus operator=(const EventBus&)  {}

public:
    template <typename E, typename C, typename F>
    void Listen(C* listener, F method)
    {
        IEventHandler* handler = new EventHandler<E, C, F>(listener, method);
        _eventHandlers[Event<E>::Type()].push_back(handler);
    }

    template <typename E, typename C, typename F>
    void Unlisten(C* listener, F method)
    {
        std::list<IEventHandler*>& handlers = _eventHandlers[Event<E>::Type()];
        std::list<IEventHandler*>::iterator it = handlers.begin();
        for ( ; it != handlers.end(); )
        {
           EventHandler<E, C, F>* handler = dynamic_cast<EventHandler<E, C, F>*>(*it);
           if (handler->Compare(listener, method))
           {
               delete *it;
               it = handlers.erase(it);
           }
           else
           {
               ++it;
           }
        }
    }

    template <typename E>
    void notify(const E* event)
    {
        std::list<IEventHandler*>& handlers = _eventHandlers[Event<E>::Type()];
        std::list<IEventHandler*>::iterator it = handlers.begin();
        for ( ; it != handlers.end(); ++it)
            (*it)->Dispatch(event);
    }

};

