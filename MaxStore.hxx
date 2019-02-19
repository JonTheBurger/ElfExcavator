#ifndef MAXSTORE_HXX
#define MAXSTORE_HXX

#include <algorithm>
#include <functional>
#include <vector>

template <typename T, size_t Capacity>
class MaxStore {
public:
  bool empty() const { return _container.empty(); }

  size_t size() const { return _container.size(); }

  const T& front() const { return _container.front(); }

  void push(const T& v)
  {
    _container.push_back(v);
    std::push_heap(_container.begin(), _container.end(), std::greater{});

    if (size() > Capacity)
    {
      pop();
    }
  }

  void pop()
  {
    std::pop_heap(_container.begin(), _container.end(), std::greater{});
    _container.pop_back();
  }

private:
  std::vector<T> _container;
};

#endif  // MAXSTORE_HXX
