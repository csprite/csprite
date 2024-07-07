# Code Style

based off of <ftp://ftp.idsoftware.com/idstuff/doom3/source/CodeStyleConventions.doc>

## General

Use Tab For Indentation, How many characters the tab is equal to is upto you.

Use typically trailing braces everywhere (if, else, functions, structures, typedefs, etc.)
```c
if (x) {
}
````

The else statement starts on the same line as the last closing brace.
```c
if ( x ) {
} else {
}
```

Pad parenthesized expressions with spaces
```c
if ( x ) {
}
```
Instead of
```c
if (x) {
}
```
And `x = ( y * 0.5f );` Instead of `x = (y * 0.5f);`

Use precision specification for floating point values unless there is an explicit need for a double.
`float f = 0.5f;` Instead of `float f = 0.5;` And `float f = 1.0f;` Instead of `float f = 1.f;`

Functions names start with an upper case:
```c
void Function(void);
```
In multi-word function names each word starts with an upper case:
```c
void ThisFunctionDoesSomething(void);
```

Functions used only in a single translation unit shall be marked `static` and should start with an underscore:
```c
static void _FunctionName(void) {
  // ... impl ...
}
```

Variable names start with a lower case character.
```c
float x;
```

In multi-word variable names the first word starts with a lower case character and each successive word starts with an upper case.
```c
float maxDistanceFromPlane;
```

Typedef names use the same naming convention as variables, however they always end with "_t".
```c
typedef int fileHandle_t;
```

Struct names use the same naming convention as variables, however they always end with "_t".
```c
struct renderEntity_t;
```

Enum names use the same naming convention as variables, however they always end with "_t". The enum constants use all upper case characters. Multiple words are separated with an underscore.
```c
enum contact_t {
  CONTACT_NONE,
  CONTACT_EDGE,
  CONTACT_MODELVERTEX,
  CONTACT_TRMVERTEX
};
```

Names of recursive functions end with "_r"
```c
void WalkBSP_r(int node);
```

Defined names use all upper case characters. Multiple words are separated with an underscore.
```c
#define SIDE_FRONT 0
```

Use ‘const’ as much as possible.

Use:
```c
const int *p; // pointer to const int
int * const p; // const pointer to int
const int * const p; // const pointer to const int
```

Don't use:
```c
int const *p;
```


