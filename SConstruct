import glob
import os
import sys

vars = Variables('settings.py')
vars.AddVariables(
	EnumVariable('mode', 'Build mode', 'debug', allowed_values=('debug', 'release')),
	BoolVariable('werror', 'Warnings as error', True),
	BoolVariable('yocto', 'Enable yocto renderer', True),
)

target_os = str(Platform())

env = Environment(variables = vars, ENV = os.environ)
conf = env.Configure()

if os.environ.get('CC') == 'clang' or target_os == 'darwin':
	env.Replace(CC='clang', CXX='clang++')
else:
	env.Replace(CC='gcc', CXX='g++')

# Asan & Ubsan (need to come first).
if env['mode'] == 'debug' and target_os == 'posix':
	env.Append(
		CCFLAGS=['-fsanitize=address', '-fsanitize=undefined'],
		LINKFLAGS=['-fsanitize=address', '-fsanitize=undefined'],
		LIBS=['asan', 'ubsan']
	)

# Global compilation flags.
# CCFLAGS   : C and C++
# CFLAGS    : only C
# CXXFLAGS  : only C++
env.Append(
	CFLAGS=['-std=c99'],
	CXXFLAGS=['-std=c++17']
)

if env['mode'] == 'release':
	env.Append(
		CCFLAGS='-Os',
		CDEFINES='ENABLE_WIN_ICON',
		CPPDEFINES='ENABLE_WIN_ICON'
	)

if env['mode'] == 'debug':
	env.Append(
		CCFLAGS=['-O0', '-g'],
		CDEFINES=['IS_DEBUG', 'SHOW_FRAME_TIME', 'SHOW_HISTORY_LOGS'],
		CPPDEFINES=['IS_DEBUG', 'SHOW_FRAME_TIME', 'SHOW_HISTORY_LOGS']
	)

def GatherFiles(Directories):
	if type(Directories) is not list:
		Directories = [Directories];

	files = [];
	for directory in Directories:
		for root, dirnames, filenames in os.walk(directory):
			for filename in filenames:
				if filename.endswith('.c') or filename.endswith('.cpp'):
					files.append(os.path.join(root, filename))

	return files

# Get all the c and c++ files in src, recursively.
sources = GatherFiles(['src', 'lib/imgui'])
sources += glob.glob('lib/glad.c')
sources += glob.glob('lib/tinyfiledialogs.c')

# Header Directories.
env.Append(
	CPATH=['src/', 'include/', 'lib/'],
	CPPPATH=['src/', 'include/', 'lib/']
)

# Windows compilation support.
if target_os == 'msys':
	env.Append(
		LIBS=['opengl32', 'gdi32', 'comdlg32', 'ole32', 'shell32'],
		LINKFLAGS="-mwindows" # Fix Console From Popping-Up
	)
else:
	env.Append(
		LIBS=['glfw', 'dl', 'm'],
		CXXFLAGS=['-Wall', '-Wno-narrowing'],
		CFLAGS=['-Wall', '-Wno-unknown-pragma']
	)

# OSX Compilation support.
if target_os == 'darwin':
	env.Append(FRAMEWORKS=['OpenGL', 'Cocoa'])
	env.Append(LIBS=['m', 'glfw', 'objc'])

# Append external environment flags
env.Append(
	CFLAGS=os.environ.get("CFLAGS", "").split(),
	CXXFLAGS=os.environ.get("CXXFLAGS", "").split(),
	LINKFLAGS=os.environ.get("LDFLAGS", "").split()
)

env.Program(target='csprite', source=sorted(sources))
