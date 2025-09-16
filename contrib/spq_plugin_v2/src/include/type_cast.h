#ifndef TYPE_CAST_H
#define TYPE_CAST_H

template <typename T>
inline T* __cast_cast_var(const T* v)
{
    return const_cast<T*>(v);
}

template <typename T>
inline T* __cast_cast_var(T* v)
{
    return v;
}

template <typename T>
inline void* void_cast(T* v)
{
    return reinterpret_cast<void*>(__cast_cast_var(v));
}

template <typename T>
inline auto walker_cast0(T f) -> bool (*)()
{
    return reinterpret_cast<bool (*)()>(f);
}

#endif