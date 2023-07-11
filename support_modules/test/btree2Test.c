#include <stdio.h>

#include "btree2.c"

BTree2Box testBox[10] = {
    { 0, 0,  5, 5, 'a'},
    {10, 0, 15, 5, 'b'},
    {20, 0, 25, 5, 'c'},
    {30, 0, 35, 5, 'd'},
    {40, 0, 45, 5, 'e'},
    { 0,10,  5,15, 'f'},
    {10,10, 15,15, 'g'},
    {20,10, 25,15, 'h'},
    {30,10, 35,15, 'i'},
    {40,10, 45,15, 'j'}
};

void pick(const BTree2* bt, int x, int y)
{
    const BTree2Box* hit = btree2_pick(bt, testBox, x, y);
    printf("    %2d,%2d %c", x, y, hit ? hit->data : '-');
}

int main(int argc, char** argv)
{
    BTree2Gen gen;
    BTree2* tree;
    int i;
    (void) argc;
    (void) argv;


    tree = btree2_generate(&gen, testBox, 10);

    for (i = 0; i < 10; ++i) {
        BTree2Box* tb = testBox + i;
        pick(tree, tb->x - 1, tb->y - 1);
        pick(tree, tb->x, tb->y);
        pick(tree, tb->x2 - 1, tb->y2 - 1);
        pick(tree, tb->x2, tb->y2);
        printf("\n");
    }

    free(tree);
    return 0;
}
