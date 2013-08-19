
/* The Computer Language Benchmarks Game
 * http://benchmarksgame.alioth.debian.org/
 *
 * contributed by Jon Harrop
 * modified by Alex Mizrahi
 * modified by Andreas Schäfer
 * very minor omp tweak by The Anh Tran
 */

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include <sys/time.h>
#include <stddef.h>

double DoubleTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec + (((double)tv.tv_usec) / 1000000);
}

#include <boost/pool/object_pool.hpp>


static int32_t check = 0;

struct Node 
{
    Node *l, *r;
    int i;
    
    Node(int i2) : l(0), r(0), i(i2) 
    {}
    Node(Node *l2, int i2, Node *r2) : l(l2), r(r2), i(i2) 
    {}

    int check() const 
    {
      if (l)
        return l->check() + i - r->check();
      else
        return i;
    }
};

typedef boost::object_pool<Node> NodePool;


Node *make(int i, int d, NodePool &store) 
{
    if (d > 0) 
        return store.construct( make(2*i-1, d-1, store), 
                                i, 
                                make(2*i, d-1, store)   );
    return store.construct(i);
}

int GetThreadCount()
{
    cpu_set_t cs;
    CPU_ZERO(&cs);
    sched_getaffinity(0, sizeof(cs), &cs);

    int count = 0;
    for (int i = 0; i < 8; i++)
    {
        if (CPU_ISSET(i, &cs))
            count++;
    }
    return count;
}

int runBinaryTrees() 
{
  for ( int n = 4; n <= 7; n += 1 )
  {
    int min_depth = 4;
    int max_depth = std::max(min_depth+2, n);
    int stretch_depth = max_depth+1;

    // Alloc then dealloc stretchdepth tree
    {
      NodePool store;
      Node *c = make(0, stretch_depth, store);
      check += c->check();
    }

    NodePool long_lived_store;
    Node *long_lived_tree = make(0, max_depth, long_lived_store);

    for (int d = min_depth; d <= max_depth; d += 2) 
    {
      int iterations = 1 << (max_depth - d + min_depth);

      for (int i = 1; i <= iterations; ++i) 
      {
        NodePool store;
        Node* a = make(i, d, store);
        Node* b = make(i, d, store);
        check += a->check() + b->check();
      }
    }
  }

  return 0;
}

int main(int argc, char *argv[]) {
    double d1 = DoubleTime();
    for (int i = 0; i < 1000; i++) {
        runBinaryTrees();
    }
    double d2 = DoubleTime();
    fprintf(stderr, "CXX Time: %lf  (count=%d)\n", (d2 - d1), check);
    return 0;
}
