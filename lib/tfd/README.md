# TinyFileDialogs
[Vareille's](https://sourceforge.net/u/vareille) Cross-Platform [TinyFileDialog](https://sourceforge.net/projects/tinyfiledialogs/) Library

---

### Compiling

1. Include [`src/tinyfiledialogs.h`](src/tinyfiledialogs.h) in your code to get function declaration & other definitions.
2. To compile the [`src/tinyfiledialogs.c`](src/tinyfiledialogs.c) & on windows make sure to link with the following flags:

```bash
-lcomdlg32 -lole32 -luser32 -lshell32
```

Example:

- Linux/Darwin: `clang main.c src/tinyfiledialogs.c`
- Windows: `clang -lcomdlg32 -lole32 -luser32 -lshell32 main.c src/tinyfiledialogs.c`

---
### API

#### Global Variables

TinyFileDialog Version is stored & can be accessed via this Variable:

```c
char tinyfd_version[8];
```

On unix, prints the command line calls (default is 0, else can be 1)
```c
int tinyfd_verbose;
```

On unix, hide errors and warnings from called dialogs (default 1 else can be 0)
```c
int tinyfd_silent;
```

Curses dialogs are difficult to use, on windows they are only ascii and uses the unix backslah (default 0 else can be 1)
```c
int tinyfd_allowCursesDialogs;
```

Force to use console mode, default is 0 (false) else can be 1 (true). Tries to use Graphic mode if false and if fails tries to use console mode
```c
int tinyfd_forceConsole;
```

some systems don't set the environment variable DISPLAY even when a graphic display is present.
set this to 1 to tell tinyfiledialogs to assume the existence of a graphic display
```c
int tinyfd_assumeGraphicDisplay;
```

if you pass `"tinyfd_query"` as `aTitle`, the functions will not display the dialogs but will return 0 for console mode, 1 for graphic mode.
`tinyfd_response` is then filled with the retain solution, possible values for `tinyfd_response` are (all lowercase).

- Graphic mode: `windows_wchar`, `windows`, `applescript`, `kdialog`, `zenity`, `zenity3`, `matedialog`, `shellementary`, `qarma`, `yad`, `python2-tkinter`, `python3-tkinter`, `python-dbus`, `perl-dbus`, `gxmessage`, `gmessage`, `xmessage`, `xdialog` & `gdialog`.
- Console mode: `dialog`, `whiptail`, `basicinput`, `no_solution`.

```c
char tinyfd_response[1024];
```

---

## Thanks
