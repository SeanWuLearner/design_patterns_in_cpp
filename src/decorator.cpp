#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include <functional>

using namespace std;

struct Shape
{
    virtual string str() const = 0;
};

struct Circle : public Shape
{
    float radius_;

    Circle(float radius):radius_(radius){}
    void resize(float ratio){ radius_ *= ratio;}
    string str() const override
    {
        ostringstream oss;
        oss << "Circle with radius " << radius_;
        return oss.str();
    }
};

struct ColoredShape : Shape
{
    Shape& shape_;
    string color_;

    ColoredShape(Shape& shape, string color):shape_(shape), color_(color){}
    string str() const override
    {
        ostringstream oss;
        oss <<  shape_.str() << ", has color " << color_;
        return oss.str();
    }
};

template<typename T>
struct ColoredShape2 : public T
{
    static_assert(is_base_of<Shape, T>::value, "Template Arg must be a Shape");

    string color_;

    template <typename...Args>
    ColoredShape2(string color, Args ...args):T(std::forward<Args>(args)...), color_(color){}
    string str() const override
    {
        ostringstream oss;
        oss <<  T::str() << ", has color " << color_;
        return oss.str();
    }
};


int add(int a, int b){
    cout << a << "+" << b << "=" << a+b << endl;
    return a + b;
}

//need partial specialization for this to work
template<typename> struct Logger;

template<typename R, typename... Args>
struct Logger<R(Args...)>
{
    function<R(Args ...)> func_;
    string name_;

    Logger(function<R(Args...)> func, const string& name): func_(func), name_(name){}

    R operator()(Args ...args)
    {
        cout << "+++ Entering " << name_ << " +++" << endl;
        R result = func_(args ...);
        cout << "--- Leaving " << name_ << " ---" << endl;
        return result;
    }
};

template<typename R, typename... Args>
auto make_logger(R (*func)(Args ...), const string& name)
{
    return Logger<R(Args...)>(function<R(Args...)>(func), name);
}


int main ()
{
// Dynamic Decorator : Con, cannot use Circle.resize() unless you modify the baseclass
Circle circle{5.0};
ColoredShape cs1{circle, "Green"};
cout << cs1.str() << endl;

// Mixin Decorator
ColoredShape2<Circle> cs2{"Red", 7};
cout << cs2.str() << endl;

// Functional Decorator
auto logged_add = make_logger(add, "Add()");
logged_add(3, 6);
}