#include <cstdio>

class Stack {
private:
	int top;
	static constexpr unsigned int MAX_STATES = 5;
	unsigned char *canvasStates[MAX_STATES] = { NULL };

public:
	Stack() {
		top = -1;
	}
	~Stack() {
		printf("Freeing Undo Redo Stack!\n");
		for (int i = 0; i < (MAX_STATES - 1); i++) {
			if (canvasStates[i] != NULL) {
				free(canvasStates[i]);
			}
		}
	}

	bool isEmpty() {
		return top == -1;
	}

	bool isFull() {
		return top == (MAX_STATES - 1);
	}

	bool push(unsigned char *CanvasData) {
		if (isFull()) {
			printf("Cannot Push Onto Stack, Stack is Full!\n");
			return false;
		} else {
			top++;
			canvasStates[top] = CanvasData;
			return true;
		}
	}

	unsigned char * pop() {
		if (isEmpty()) {
			printf("Cannot Pop from the stack, Stack is empty!\n");
			return NULL;
		} else {
			unsigned char *itemPopped = canvasStates[top];
			canvasStates[top] = NULL;
			top--;
			return itemPopped;
		}
	}

	int count() {
		return (top + 1);
	}

	int size() {
		return MAX_STATES;
	}

	unsigned char * peek(int position) {
		if (isEmpty()) {
			printf("Cannot Get Element from the stack, Stack is empty!\n");
			return NULL;
		} else {
			return canvasStates[position];
		}
	}

	void change(int position, unsigned char *CanvasData) {
		if (position > MAX_STATES - 1) {
			printf("Cannot access the element at index %d\n", position);
			return;
		}
		canvasStates[position] = CanvasData;
	}

	void display() {
		printf("----------------\n");
		for (int i = (MAX_STATES - 1); i >= 0; i--) {
			printf("> %p\n", (void*)canvasStates[i]);
		}
		printf("----------------\n");
	}
};
