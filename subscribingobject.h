#ifndef C_SUBSCRIBING_OBJECT
#define C_SUBSCRIBING_OBJECT

#include <queue>
#include <mutex>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <QDebug>

// Lock for Subscribing object id generation
// Parent of subscribing class
class SubscribingObject
{
  public:
    SubscribingObject()
    {
      m_Mutex.lock();
      ++m_Id;
      m_MyId = m_Id;
      m_Mutex.unlock();
    }

    virtual ~SubscribingObject(){}

    // Handle an incoming messages
    // this is on the publishing thread
    virtual void HandleMessage(boost::function<void()> fn)
    {
      m_MessageQueue.push(fn);
    }

    // Process all the messages
    // this should be on the recieving thread
    virtual void ProcessMessages()
    {
      while (!m_MessageQueue.empty())
      {
        boost::function<void()> fn;
        fn = m_MessageQueue.front();
        m_MessageQueue.pop();

        // Process the message on the correct thread
        fn();
      }
    }

    int Uid()
    {
      return m_MyId;
    }

  private:
    //static int m_Id;

    static std::mutex m_Mutex;
    static int m_Id;

    int m_MyId;
    std::queue<boost::function<void()> > m_MessageQueue;
};

#endif // C_SUBSCRIBING_OBJECT
