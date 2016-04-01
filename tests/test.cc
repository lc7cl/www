#include <iostream>

using namespace std;

class A {
private:
    const int m_a;
    int m_b;
public:
    A(): m_a(0) { m_b = 0;};
    A(int b);
    ~A() {};

    void print();
};

A::A(int b) : m_a(1)
{
    m_b = b;
    cout<<b<<endl;
}

void A::print()
{
    cout<<(m_a + m_b)<<endl;
}

int main(int argc, const char ** argv)
{
    A a(1);
    a.print();

    return 0;
}
