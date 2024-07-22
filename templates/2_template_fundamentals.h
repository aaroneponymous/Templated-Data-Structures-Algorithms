// Chapter 2: Template Fundamentals

/**
 * 1. How to define function templates, class templates, variable templates, and alias templates
 * 2. What kinds of template parameters exist?
 * 3. What is template instantiation?
 * 4. What is template specialization?
 * 5. How to use generic lambdas and lamda templates
 */


/**
 * @brief: Defining Function Templates
 */

template <typename T>
T add(T const a, T const b) {
    return a + b;
}

// Valid Calls
auto a = add(42, 21);
auto b = add<int>(42, 21);
auto c = add<>(42, 21);

// Option 1: Member Function
class foo
{
    int value_;

public:
    explicit foo(int const i) : value_(i)
    {}

    explicit operator int() const { return value_; }

    // Define operator+ as a member function
    foo operator+(foo const& b) const {
        return foo(value_ + b.value_);
    }
};

// Option 2: Non-member Function
class foo
{
    int value_;

public:
    explicit foo(int const i) : value_(i)
    {}

    explicit operator int() const { return value_; }

    // Friend function to access private members
    friend foo operator+(foo const& a, foo const& b) {
        return foo(a.value_ + b.value_);
    }
};

// Example of a count_if() function
template <typename Input, typename Output, typename Predicate>
int count_if(Input start, Output end, Predicate p) {

    int total = 0;
    for (Input i = start, i != end; i++) {
        
        if (p(*i)) {
            total++;
        }
    }

    return total;
}

int main() {
    
    int arr[] {1, 1, 2, 3, 5, 8, 11};

    int elements = count_if(std::begin(arr), std::end(arr), 
                            [] (int const n) { return n % 2 == 1; });
}


// New C++ Feature: Class Template Argument Deduction

// 1. Member function of class templates
// 2. Member function templates of a class
// 3. Member function templates of class templates