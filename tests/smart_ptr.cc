#include <iostream>
#include <memory>
#include <atomic>

class MyClass {
  
};

int main(int argc, char** argv)
{
  auto ptr = std::make_shared<MyClass>();
  printf("1. use_count=%d\n", ptr.use_count());

  {
      auto p = std::atomic_load(&ptr);
      printf("3a. use_count=%d\n", ptr.use_count());
  }

  printf("3b. use_count=%d\n", ptr.use_count());

  {
      auto p = std::atomic_load(&ptr);
      printf("3a. use_count=%d\n", ptr.use_count());
  }

  printf("4b. use_count=%d\n", ptr.use_count());
  return 0;
}

