#ifndef DIRECTCONNECTIONMESSAGEHANDLER_H
#define DIRECTCONNECTIONMESSAGEHANDLER_H
#include <QDebug>

class CDirectSubscriber : public SubscribingObject
{
  public:
    CDirectSubscriber(QObject *parent = 0) {
    }

    virtual ~CDirectSubscriber(){
    }

    virtual void HandleMessage(boost::function<void()> fn)
    {
        fn();
    }
    // We don't actually process messages here
    virtual void ProcessMessages(){}

};

#endif // DIRECTCONNECTIONMESSAGEHANDLER_H

