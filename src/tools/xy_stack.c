#include <stdlib.h>
#include <string.h>
#include "xy_stack.h"

xy_stack_t* s_init(uint32_t len) {
	if (len < 1) return NULL;
	xy_stack_t* s = (xy_stack_t*) malloc(sizeof(xy_stack_t));
	s->top = -1;
	s->length = len;
	s->stack = malloc(sizeof(uint32_t) * len * 2);
	return s;
}

void s_free(xy_stack_t* s) {
	if (s != NULL) {
		if (s->stack != NULL) free(s->stack);
		free(s);
	}
}

void s_clear(xy_stack_t* s) {
	s->top = -1;
}

bool s_push(xy_stack_t* s, int x, int y) {
	if (s == NULL || s->stack == NULL || s->top >= s->length - 1) return false;
	s->top++;
	int32_t* stack_ptr = &s->stack[s->top * 2];
	*(stack_ptr + 0) = x;
	*(stack_ptr + 1) = y;
	return true;
}

bool s_pop(xy_stack_t* s, int* x, int* y) {
	if (s == NULL || s->stack == NULL || s->top == -1) return false;
	int32_t* stack_ptr = &s->stack[s->top * 2];
	*x = *(stack_ptr + 0);
	*y = *(stack_ptr + 1);
	s->top--;
	return true;
}

int s_getcount(xy_stack_t* s) {
	return s->top + 1;
}

bool s_isEmpty(xy_stack_t* s) {
	return s->top == -1;
}

