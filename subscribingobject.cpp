#include "subscribingobject.h"

std::mutex  SubscribingObject::m_Mutex;
int SubscribingObject::m_Id = 0;
