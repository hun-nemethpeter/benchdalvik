
#include <sys/time.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <algorithm>

double DoubleTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec + (((double)tv.tv_usec) / 1000000);
}


struct TreeNode {
    TreeNode *left;
    TreeNode *right;
    int item;

    inline TreeNode(TreeNode *l, TreeNode *r, int i)
      : left(l), right(r), item(i)
    {}

    inline int itemCheck() {
        if (!left)
            return item;
        return item + left->itemCheck() - right->itemCheck();
    }
    void* operator new(size_t sz);
    void operator delete(void*) {}
};
                              
char nodes[sizeof(TreeNode) * 16319100];
static int count = 0;

void* TreeNode::operator new(size_t sz)
{
  return &nodes[(count++) * sizeof(TreeNode)];
}

inline void resetPool()
{
  count = 0;
}

TreeNode *bottomUpTree(int item, int depth) {
    if (depth > 0) {
        return new TreeNode(bottomUpTree(2*item-1, depth-1),
                            bottomUpTree(2*item, depth-1),
                            item);
    }
    return new TreeNode(NULL, NULL, item);
}


static int32_t ret = 0;
static int32_t check = 0;

void runBinaryTrees() {
    for ( int n = 4; n <= 7; n += 1 ) {
        int minDepth = 4;
        int maxDepth = std::max(minDepth + 2, n);
        int stretchDepth = maxDepth + 1;
        
        resetPool();
        TreeNode *tn = bottomUpTree(0,stretchDepth);
        check += tn->itemCheck();
        resetPool();
        
        TreeNode *longLivedTree = bottomUpTree(0,maxDepth);
        for (int depth=minDepth; depth<=maxDepth; depth+=2){
            int iterations = 1 << (maxDepth - depth + minDepth);

            for (int i=1; i<=iterations; i++){
                TreeNode *t1 = bottomUpTree(i,depth);
                check += t1->itemCheck();
                TreeNode *t2 = bottomUpTree(i,depth);
                check += t2->itemCheck();
            }
        }
        ret = longLivedTree->itemCheck();
    }
}


int main() {
    double d1 = DoubleTime();
    for (int i = 0; i < 1000; i++) {
        runBinaryTrees();
    }
    double d2 = DoubleTime();
    fprintf(stderr, "CXX Time: %lf  (count=%d)\n", (d2 - d1), check);
    return 0;
}
