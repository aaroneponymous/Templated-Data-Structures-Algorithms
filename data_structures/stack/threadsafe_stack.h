#pragma once

#include <exception>
#include <memory>
#include <mutex>
#include <stack>


struct empty_stack: std::exception
{
    const char* what() const noexcept override
    {
        return "Stack is empty.";
    }

};


template<typename T>
class threadsafe_stack() {}
