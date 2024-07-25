#pragma once

#include <exception>
#include <memory>
#include <mutex>
#include <stack>

struct empty_stack : std::exception
{
    const char *what() const noexcept override
    {
        return "Stack is empty.";
    }
};

template <typename T>
class threadsafe_stack
{
private:
    std::stack<T> data_;
    mutable std::mutex m_;

public:
    threadsafe_stack() {}
    threadsafe_stack(const threadsafe_stack &other)
    {
        std::lock_guard<std::mutex> lock(other.m_);
        data = other.data_;                             // Copy performed in constructor body
    }

    // Copy Operator Deleted: To Maintain Thread Safety
    threadsafe_stack &operator=(const threadsafe_stack &) = delete;
    
    // push method
    void push(T new_value)
    {
        std::lock_guard<std::mutex> lock(m_);
        data_.push(std::move(new_value));                  // Use move semantics
    }


    // pop method - Combined Top() and Pop() functions
    // return value: shared_ptr<T> 
    std::shared_ptr<T> pop()
    {
        std::lock_guard<std::mutex> lock(m_);
        if (data_.empty() throw empty_stack());                                 // Check for empty before trying to pop value
        // Allocate return value before modifying stack
        std::shared_ptr<T> const res(std::make_shared<T>(data_.top()));         // Variable Uniform Initalization with Move Semantics
        data_.pop();
        return res;
    }

    void pop(T& value) {
        std::lock_guard<std::mutex> lock(m_);
        if (data_.empty()) throw empty_stack();
        value = data_.top();
        data.pop();
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lock(m_);
        return data_.empty();
    }

};