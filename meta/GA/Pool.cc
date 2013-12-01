#include "Pool.h"
#ifdef _DEBUG
#include <cassert>
#endif

struct node
{
    node* next;
};

struct Pool::impl
{
    const int elem_size;
    const int max_elems;
    char* data;
    node* head;

    impl (int _elem_size, int _max_elems)
        : elem_size(_elem_size),
          max_elems(_max_elems),
          data(new char[elem_size*max_elems]) {}

    ~impl () {
        delete[] data;
    }
};

Pool::Pool (int elem_size, int max_elems)
    : my(new impl(elem_size, max_elems))
{
    clear();
}

Pool::~Pool ()
{
    delete my;
}

void Pool::clear ()
{
    my->head = (node*) my->data;
    node* n = my->head;
    char* p = my->data + my->elem_size;
    for (int i = 1; i < my->max_elems; ++i) {
        n->next = (node*) p;
        n = n->next;
        p += my->elem_size;
    }
    n->next = 0;
}

void* Pool::alloc ()
{
#ifdef _DEBUG
    assert(my->head != 0);
#endif
    node* n = my->head;
    my->head = my->head->next;
    return (void*) n;
}

void Pool::free (void* p)
{
    node* n = (node*) p;
    n->next = my->head;
    my->head = n;
}

int Pool::elemSize () const
{
    return my->elem_size;
}

int Pool::maxElems () const
{
    return my->max_elems;
}
