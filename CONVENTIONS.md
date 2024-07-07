# Code Style

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

## Function Naming

### Shared Functions

```c
// File Path: `~/source/src/AAA/BBBB`
type csp_AAA_BBBB_name(args);
```

### Private Functions

```c
static type _name(args) {
  // ... impl ...
}
```

