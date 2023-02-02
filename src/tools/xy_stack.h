/* Simple Stack Implementation For X, Y Coordinates */

#ifndef XY_STACK_H
#define XY_STACK_H 1

#include <stdint.h>
#include <stdbool.h>

typedef struct {
	int32_t    top;
	int32_t*   stack;
	int32_t    length;
} xy_stack_t;

xy_stack_t* s_init(uint32_t len);
void s_free(xy_stack_t* s);
void s_clear(xy_stack_t* s);
bool s_push(xy_stack_t* s, int x, int y);
bool s_pop(xy_stack_t* s, int* x, int* y);
int  s_getcount(xy_stack_t* s);
bool s_isEmpty(xy_stack_t* s);

#endif // XY_STACK_H

