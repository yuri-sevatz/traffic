#ifndef FUNCTIONAL_HPP
#define FUNCTIONAL_HPP

#include <functional>
#include <utility>

namespace std {

template<typename T>
struct hash<std::pair<T,T>> : private std::hash<T> {
    std::size_t operator()(const std::pair<T,T> & value) const {
        return (
            std::hash<T>::operator()(value.first) ^
            std::hash<T>::operator()(value.second)
        );
    }
};

template<typename T, typename U>
struct hash<std::pair<T,U>> : private std::hash<T>, private std::hash<U> {
    std::size_t operator()(const std::pair<T,U> & value) const {
        return (
            std::hash<T>::operator()(value.first) ^
            std::hash<U>::operator()(value.second)
        );
    }
};

template<typename T>
struct equal_to<std::pair<T,T>> : private std::equal_to<T> {
    std::size_t operator()(const std::pair<T,T> & lhs, const std::pair<T,T> & rhs) const {
        return (
            std::equal_to<T>::operator()(lhs.first, rhs.first) &&
            std::equal_to<T>::operator()(lhs.second, rhs.second)
        );
    }
};

template<typename T, typename U>
struct equal_to<std::pair<T,U>> : private std::equal_to<T>, private std::equal_to<U> {
    std::size_t operator()(const std::pair<T,U> & lhs, const std::pair<T,U> & rhs) const {
        return (
            std::equal_to<T>::operator()(lhs.first, rhs.first) &&
            std::equal_to<U>::operator()(lhs.second, rhs.second)
        );
    }
};

} // namespace std

#endif // FUNCTIONAL_HPP
