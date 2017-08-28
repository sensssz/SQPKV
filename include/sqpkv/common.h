#ifndef INCLUDE_SQPKV_COMMON_H_
#define INCLUDE_SQPKV_COMMON_H_

#include <memory>

template<typename T, typename ...Args>
std::unique_ptr<T> make_unique( Args&& ...args )
{
    return std::unique_ptr<T>( new T( std::forward<Args>(args)... ) );
}

#endif // INCLUDE_SQPKV_COMMON_H_
