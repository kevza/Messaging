#ifndef C_QT_MESSAGE_HANDLER
#define C_QT_MESSAGE_HANDLER

#include "subscribingobject.h"
#include "singleton.h"

#include <queue>
#include <QCoreApplication>
#include <QObject>
#include <QEvent>
#include <QDebug>
#include <boost/function.hpp>

#define MsgEvent QEvent::User + 1

// Handles messages using QTs event system
class CQtHandler : public QObject
{
  Q_OBJECT
  public:
    CQtHandler(QObject *parent = 0) : QObject(parent){
        connect(this, SIGNAL(ProcessMsg()), this, SLOT(ProcessMessages()));
    }

    virtual ~CQtHandler(){}

    void HandleMessage(boost::function<void()> fn)
    {
      m_Mutex.lock();
      m_Queue.push(fn);
      m_Mutex.unlock();

      // Post event to process messages
      emit ProcessMsg();
      //QCoreApplication::postEvent(this, new QEvent(static_cast<QEvent::Type>(MsgEvent)));
    }

  public slots:
    void ProcessMessages()
    {
      while (!m_Queue.empty())
      {
        m_Mutex.lock();
        boost::function<void()> fn = m_Queue.front();
        m_Queue.pop();
        fn();
        m_Mutex.unlock();
      }

    }
  signals :
    void ProcessMsg();

  private:
    std::queue<boost::function<void()> > m_Queue;
    std::mutex m_Mutex;
};


//static CQtHandler &msgHandler = CSingleton<CQtHandler>::Instance();
// Qt Message Subscriber
class CQtMessageSubscriber : public SubscribingObject
{
  public:
    CQtMessageSubscriber(){}
    virtual ~CQtMessageSubscriber(){
    }

    virtual void HandleMessage(boost::function<void()> fn)
    {
        CSingleton<CQtHandler>::Instance().HandleMessage(fn);
    }
    // We don't actually process messages here
    virtual void ProcessMessages(){}

};

#endif
