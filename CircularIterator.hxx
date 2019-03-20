#ifndef CIRCULARITERATOR_HXX
#define CIRCULARITERATOR_HXX

#include <iterator>

template <typename Iterator>
class circular_iterator {
public:
  circular_iterator(Iterator begin, Iterator end)
      : _begin(begin)
      , _current(_begin)
      , _end(end)
  {
  }

  using iterator_category = typename std::bidirectional_iterator_tag;
  using value_type        = typename Iterator::value_type;
  using difference_type   = typename Iterator::difference_type;
  using pointer           = typename Iterator::pointer;
  using reference         = typename Iterator::reference;

  reference operator*() { return _current.operator*(); }

  pointer operator->() { return _current.operator->(); }

  circular_iterator& operator++()
  {
    ++_current;
    if (_current == _end)
    {
      _current = _begin;
    }
    return *this;
  }

  circular_iterator operator++(int)
  {
    circular_iterator previous = *this;
    ++(*this);
    return previous;
  }

  circular_iterator operator--()
  {
    if (_current == _begin)
    {
      _current = _end;
    }
    --_current;
    return *this;
  }

  circular_iterator operator--(int)
  {
    circular_iterator previous = *this;
    --(*this);
    return previous;
  }

  bool operator==(const circular_iterator& other) const
  {
    return (_current == other._current) &&
           (_begin == other._begin) &&
           (_end == other._end);
  }

  bool operator!=(const circular_iterator& other) const
  {
    return !(*this == other);
  }

private:
  Iterator _begin;
  Iterator _current;
  Iterator _end;
};

#endif  // CIRCULARITERATOR_HXX
