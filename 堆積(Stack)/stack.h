// header proect

#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int *arr;
    int top;
    int capacity;
} stack;

int pop(stack *s);
int push(stack *s, int item);
bool isFull(stack *s);
bool isEmpty(stack *s);
stack *createStack(int capacity);
void deleteStack(stack *s);
void printStack(stack *s);

stack *createStack(int capacity)
{
    stack *s = (stack *)malloc(sizeof(stack));
    s->capacity = capacity;
    s->top = -1;
    s->arr = (int *)malloc(s->capacity * sizeof(int));
    return s;
}
bool isFull(stack *s)
{
    return s->top == s->capacity - 1;
}

bool isEmpty(stack *s)
{
    return s->top == -1;
}
void deleteStack(stack *s)
{
    if (!isEmpty(s))
    {
        printf("Stack is not empty, deletion failed\n");
    }
    else
    {
        free(s->arr);
        free(s);
        printf("Stack deleted successfully\n");
    }
}

int pop(stack *s)
{
}