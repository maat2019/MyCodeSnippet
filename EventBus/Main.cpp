

#include "EventBus.h"

#include <cstdio>
#include <string>
#include <cstdlib>
#include <iostream>
class Event1 :public Event<Event1>
{

};

class Event2 :public Event<Event2>
{

};

class Sender1
{
public:
    void OnEvent(const Event1* e)
    {
        std::cout << (" onEvent in Sender1") << std::endl;
    }
};

class Sender2  {
public:
    void OnEvent(const Event2* e)
    {
        std::cout << (" onEvent in Sender2") << std::endl;
    }
};

class EventBusDemo 
{
public:
    void Demo()
    {
        EventBus bus;

        Sender1 sender1;
        Sender2 sender2;

        bus.Listen<Event1>(&sender1, &Sender1::OnEvent);
        bus.Listen<Event2>(&sender2, &Sender2::OnEvent);

        Event1 e1;
        bus.notify(&e1);

        Event2 e2;
        bus.notify(&e2);

        bus.Unlisten<Event1>(&sender1, &Sender1::OnEvent);
        bus.notify(&e1);

    }
};
int main()
{
    std::cout << "* * * EventBus Demo Program * * * " << std::endl;

	try
	{
		EventBusDemo demo;
		demo.Demo();
	}
	catch (std::runtime_error & e)
	{
        std::cout << "Runtime exception: " << e.what() << std::endl;
	}
}


