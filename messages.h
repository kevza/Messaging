#ifndef C_MESSAGES
#define C_MESSAGES

#include <queue>
#include <map>
#include <mutex>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/functional/value_factory.hpp>
#include <boost/bind.hpp>

#include "singleton.h"
#include "subscribingobject.h"

/**
 * @brief Contains a pointer to the 
 * subscribing object and a boost function
 * that will eventually be loaded with the message.
 * The boost::function points back to the correct
 * messages processor function on the subsribing object.
 */
template <class T>
struct MessageContainer
{
  SubscribingObject *m_Obj;
  boost::function<void(boost::shared_ptr<const T>)> m_Fn;
};

template <class T>
class CMessage
{
  public:
    /**
     * @brief Publishes the message to the message handlers
     * of all subscribing objects.
     * @param ptr A pointer to the message item
     */
    static void Publish(T *ptr)    
    {
      m_Mutex.lock();
      boost::shared_ptr<const T> messagePointer(ptr);

      typename std::map<int, MessageContainer<T> >::iterator it = m_Subscribers.begin();
      for (;it != m_Subscribers.end(); ++it)
      {
        //it->second->HandleMessage(theMessage);
        MessageContainer<T> handler = it->second;

        // Create Reference to the call back function
        boost::function<void()> fn;
        fn =  boost::bind(handler.m_Fn, messagePointer);

        // Trigger handle message on the object
        handler.m_Obj->HandleMessage(fn);
      }
      m_Mutex.unlock();
    }

    // Register a subscriber with this message type
    template <class OBJ>
    static void AddSubscriber(OBJ *ptr, int id)
    {
      m_Mutex.lock();
      MessageContainer<T> msgObj;
      msgObj.m_Obj = ptr;
      msgObj.m_Fn = boost::bind(static_cast<void (OBJ::*)(boost::shared_ptr<const T>)> (&OBJ::ProcessMessage), ptr, _1);

      m_Subscribers[id] = msgObj;
      m_Mutex.unlock();
    }

    // Remove a subscriber from this message type
    static void RemoveSubscriber(int id)
    {
      m_Mutex.lock();
      m_Subscribers.erase(id);
      m_Mutex.unlock();
    }

  private:
    static std::map<int, MessageContainer<T> > m_Subscribers;
    static std::mutex m_Mutex;
};

template <class T>
std::map<int, MessageContainer<T> > CMessage<T>::m_Subscribers;

template <class T>
std::mutex CMessage<T>::m_Mutex;


// Terminal Case for type list
struct EmptyList {};

// Subscribe at rate
template<typename Head, int messageRate, typename Tail = EmptyList>
struct AtRate
{
  typedef Head head;
  int rate = messageRate;
  typedef Tail tail;
};

template <typename AtRate>
class Subscriber {};


template <typename H, int R, typename T>
class Subscriber<AtRate<H, R, T> > : public Subscriber<T>
{
  public:
    /**
     * @brief Connects the subscriber to
     * the messages
     * @param ptr
     * A pointer to the subscribing object
     */
    template <class OBJ>
    void Subscribe(OBJ *ptr)
    {
      m_Subscriber = ptr;
      CMessage<H>::AddSubscriber(ptr, ptr->Uid());
      Subscriber<T>::Subscribe(ptr);
    }

    /**
     * @brief Unsubscribes this from the messages
     */
    void Unsubscribe()
    {
      CMessage<H>::RemoveSubscriber(m_Subscriber->Uid());
      Subscriber<T>::Unsubscribe();
    }

  private:
    SubscribingObject *m_Subscriber;
};

template <>
class Subscriber<EmptyList>
{
  public:
    void Subscribe(SubscribingObject */*ptr*/)
    {}
    void Unsubscribe(){}
};

#endif




