#include <QWidget>

template <typename T, typename... Args>
T* MakeQChild(Args... args, QWidget* parent)
{
  return new T(std::forward<Args>(args)..., parent);
}

template <typename T>
T* MakeQChild(QWidget* parent)
{
  return new T(parent);
}

template <typename T, typename... Args>
T* MakeQChild(Args... args, QObject* parent)
{
  return new T(std::forward<Args>(args)..., parent);
}

template <typename T>
T* MakeQChild(QObject* parent)
{
  return new T(parent);
}
