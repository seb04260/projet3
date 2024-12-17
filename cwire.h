#ifndef CWIRE_HEADER
# define CWIRE_HEADER

# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# define MAX_LINE_LENGTH 1024

typedef struct AVLNode {
    int station_id;
    long capacity;
    long consumption;
    int height;
    struct AVLNode *left;
    struct AVLNode *right;
} AVLNode;

#endif