
template <class Type>
class ManagedClass;

template <class Type>
class Manager
{
public:
    class CreateKey
    {
    private:
        // this key can only be created by Manager
        CreateKey() {};
        friend class Manager;
    };

    Type* createInstance()
    {
        return new Type(CreateKey());
    }
};

template <class Type>
class ManagedClass
{
public:
    typedef typename Manager<Type>::CreateKey CreateKey;

    // a ManagedClass can only be constructed with a key
    explicit ManagedClass(CreateKey) 
    {
    }
};

// every user defined class must derive from ManagedClass<T>
class MyManagedClass : public ManagedClass<MyManagedClass>
{
public:
    // it must take a key, otherwise we cannot construct our base
    explicit MyManagedClass(CreateKey key) : ManagedClass(key)
    {
    }
};

void Demo()
{
    // we cannot create a instance of MyManagedClass without the manager, because we cannot create a key for it
    Manager<MyManagedClass> manager;
    manager.createInstance();
}
