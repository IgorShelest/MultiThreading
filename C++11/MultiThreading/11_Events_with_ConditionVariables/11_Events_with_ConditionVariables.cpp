// 11_Events_with_ConditionVariables.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

//-----------------------------------------------------------------------------

#include <iostream>
#include <windows.h>
#include <iomanip>
#include <string>

//-----------------------------------------------------------------------------

#include <thread>
#include <mutex>
#include <condition_variable>

//-----------------------------------------------------------------------------

class SomeObject
{
public:
    SomeObject() :m_is_data_loaded(false) {};

    bool isDataLoaded()
    {
        return m_is_data_loaded;
    };

    void load_data(int counter)
    {
        for (unsigned int iter = 0; iter < counter; ++iter)
        {
            std::lock_guard<std::mutex> lock_guard(m_mutex);
            
            if (!m_is_data_loaded)
            {
                std::cout << "\nLoading... " << "iter: " << iter << " data: " << m_data;
                
                m_data = std::to_string(iter);
                m_is_data_loaded = true;
                m_cond_variable.notify_one();
            }                
        }
    }

    void process_data(int counter)
    {
        for (unsigned int iter = 0; iter < counter; ++iter)
        {
            std::unique_lock<std::mutex> unique_lock(m_mutex);
            m_cond_variable.wait(unique_lock, std::bind(&SomeObject::isDataLoaded, this));

            std::cout << "\nProcessing... " << "iter: " << iter << " data: " << m_data;
            m_is_data_loaded = false;            
        }        
    }


private:
    std::mutex              m_mutex;
    std::condition_variable m_cond_variable;
    bool                    m_is_data_loaded;
    std::string             m_data;
};

//-----------------------------------------------------------------------------

class SomeAnotherObject
{
public:
    SomeAnotherObject() :m_is_data_loaded(false) {};

    void load_data(const bool* is_termnate_requeted)
    {
        unsigned iter = 0;

        while (!(*is_termnate_requeted))
        {
            std::unique_lock<std::mutex> unique_lock(m_mutex);
            m_data_not_loaded_cond_variable.wait(unique_lock, std::bind(&SomeAnotherObject::isDataNotLoaded, this));

            std::cout << "\nLoading... " << "iter: " << iter++ << " data: " << m_data;

            m_data = std::to_string(iter);
            m_is_data_loaded = true;
            m_data_loaded_cond_variable.notify_one();
        }
    }

    void process_data(const bool* is_termnate_requeted)
    {
        unsigned iter = 0;

        while (!(*is_termnate_requeted))
        {
            std::unique_lock<std::mutex> unique_lock(m_mutex);
            m_data_loaded_cond_variable.wait(unique_lock, std::bind(&SomeAnotherObject::isDataLoaded, this));

            std::cout << "\nProcessing... " << "iter: " << iter++ << " data: " << m_data;
            m_is_data_loaded = false;
            m_data_not_loaded_cond_variable.notify_one();
        }
    }

private:
    std::mutex              m_mutex;
    std::condition_variable m_data_loaded_cond_variable;
    std::condition_variable m_data_not_loaded_cond_variable;
    bool                    m_is_data_loaded;
    std::string             m_data;
    
    bool isDataLoaded()
    {
        return m_is_data_loaded;
    };

    bool isDataNotLoaded()
    {
        return !isDataLoaded();
    };
};

//-----------------------------------------------------------------------------

int main()
{
    std::cout << "\nMain thread START";

    // thread_1 notifies thread_2 via one condition_variable
    {
        SomeObject object;
        const unsigned counter = 1000;

        std::thread thread_1(&SomeObject::load_data, &object, counter);
        std::thread thread_2(&SomeObject::process_data, &object, counter);

        // Detaching first thread
        if (thread_1.joinable())
            thread_1.join();

        // Waiting for the first thread to finish
        if (thread_2.joinable())
            thread_2.join();
    }

    // thread_1 and thread_2 notify each other via two condition_variables
    {
        SomeAnotherObject object;
        bool is_termnate_requeted = false;
        
        std::thread thread_1(&SomeAnotherObject::load_data, &object, &is_termnate_requeted);
        std::thread thread_2(&SomeAnotherObject::process_data, &object, &is_termnate_requeted);

        // Let the thread do some work
        Sleep(1000);

        // Terminate threads
        is_termnate_requeted = true;

        // Detaching first thread
        if (thread_1.joinable())
            thread_1.join();

        // Waiting for the first thread to finish
        if (thread_2.joinable())
            thread_2.join();
    }

    std::cout << "\nMain thread END\n";
 
    std::system("pause");
    return 0;
}
