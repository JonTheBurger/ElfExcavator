#ifndef MAXSTORE_HXX
#define MAXSTORE_HXX

#include <algorithm>
#include <functional>
#include <vector>

template <typename T>
class MaxStore {
public:
  explicit MaxStore(size_t capacity)
      : _container(capacity)
      , _capacity(capacity)
  {
  }

  bool     empty() const { return _container.empty(); }
  size_t   size() const { return _container.size(); }
  size_t   capacity() const { return _capacity; }
  const T& front() const { return _container.front(); }

  void push(const T& v)
  {
    _container.push_back(v);
    std::push_heap(_container.begin(), _container.end(), std::greater<void>{});

    if (size() > capacity())
    {
      pop();
    }
  }

  void pop()
  {
    std::pop_heap(_container.begin(), _container.end(), std::greater<void>{});
    _container.pop_back();
  }

private:
  std::vector<T> _container;
  size_t         _capacity;
};

#endif  // MAXSTORE_HXX
