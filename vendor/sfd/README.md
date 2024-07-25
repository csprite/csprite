# Simple File Dialog
A small C library for opening a file dialog on Windows and Linux.

---

## Compiling
The library comes with `CMakeLists.txt` but you can just put
**[sfd.c](src/sfd.c?raw=1)** and **[sfd.h](src/sfd.h?raw=1)**
into an existing project and compiled along with it

### Build Options
```c
#define SFD_BACKEND_WIN32   // Only For Windows (Make Sure To Link With 'comdlg32')
#define SFD_BACKEND_ZENITY  // Zenity Backend (Requires zenity)
#define SFD_BACKEND_KDIALOG // KDialog Backend (Requires kdialog)
```

---

## Usage

```c
sfd_Options opt = {
	.title        = "Open Image File",
	.filter_name  = "Image File",
	.filter       = "*.png|*.jpg",
	.save         = 0
};

const char* filename = sfd_open_dialog(&opt);

if (filename) {
	printf("Got file: '%s'\n", filename);
} else {
	const char* LastError = sfd_get_error();
	if (LastError != NULL) {
		printf("Error: %s\n", LastError);
	} else {
		printf("Open canceled\n");
	}
}
```

All fields of the `sfd_Options` struct are *optional* except for `.save`.

| Field           | Description
|-----------------|-------------------------------------------------------------
| `.title`        | Title for the file dialog window
| `.path`         | Default directory
| `.filter_name`  | Name used to describe the file filter
| `.filter`       | File filters separated by <code>&#124;</code>
| `.save`         | Type of the dialog, `0` = Open & `1` = Save

If the file dialog is canceled or an error occurs `NULL` is returned.
`sfd_get_error()` will return an error string in the case of an error.

---

## License
This library is free software; you can redistribute it and/or modify it under
the terms of the MIT license. See [LICENSE](LICENSE) for details.

---

# Thanks
