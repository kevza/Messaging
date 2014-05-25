#ifndef C_SINGLETON
#define C_SINGLETON

#include <mutex>


template <class T>
class CSingleton 
{
  public:
    static T &Instance()
    {
      if (!m_Instance)
      {
        m_Mutex.lock();
        if (!m_Instance)
        {
          m_Instance = new T();
        }
        m_Mutex.unlock();
      }
      return *m_Instance;
    }

    void Delete()
    {
      delete m_Instance;
      m_Instance = NULL;
    }


  private:
    static std::mutex m_Mutex;
    static T *m_Instance;


};

template <class T>
std::mutex CSingleton<T>::m_Mutex;

template <class T>
T *CSingleton<T>::m_Instance = NULL;

#endif
