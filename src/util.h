#pragma once

#include <cstdint>
#include <cstddef>

void panic(const char* message);

template <typename _Tp>
struct remove_reference {
    typedef _Tp type;
};

template <typename _Tp>
struct remove_reference<_Tp&> {
    typedef _Tp type;
};

template <typename _Tp>
struct remove_reference<_Tp&&> {
    typedef _Tp type;
};

template <class T>
constexpr typename remove_reference<T>::type&& move(T&& t) noexcept {
    return static_cast<typename remove_reference<T>::type&&>(t);
}