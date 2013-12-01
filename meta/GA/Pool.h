#pragma once

#include <cstddef> // size_t

class Pool
{
    struct impl;
    impl* my;

public:

    Pool (int elem_size, int max_elems);

    ~Pool ();

    void clear ();

    void* alloc ();

    void free (void* p);

    int elemSize () const;

    int maxElems () const;
};

inline void* operator new (size_t, Pool& pool) {
    return pool.alloc();
}

template <class T>
void destroy (T* p, Pool& pool) {
    p->~T();
    pool.free(p);
}
