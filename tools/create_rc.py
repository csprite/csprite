#!/usr/bin/python

import os
import sys

ARCH = ''
MAJVER = 0
MINVER = 0
PATVER = 0

for item in sys.argv:
	if (item.startswith("--arch=")):
		item = item.replace("--arch=", '')
		if not (item == 'x86_64' or item == 'i686'):
			print(f"Invalid '--arch' value: '{item}', accepted values are 'x86_64' & 'i686'.")
			sys.exit(-1)
		else:
			ARCH = item
	elif (item.startswith("--majver=")):
		item = item.replace("--majver=", '')
		if not item.isdigit():
			print(f"Invalid '--majver' value: '{item}'.")
			sys.exit(-1)
		else:
			MAJVER = int(item)
	elif (item.startswith("--minver=")):
		item = item.replace("--minver=", '')
		if not item.isdigit():
			print(f"Invalid '--minver' value: '{item}'.")
			sys.exit(-1)
		else:
			MINVER = int(item)
	elif (item.startswith("--patver=")):
		item = item.replace("--patver=", '')
		if not item.isdigit():
			print(f"Invalid '--patver' value: '{item}'.")
			sys.exit(-1)
		else:
			PATVER = int(item)

DirPath = os.path.dirname(os.path.realpath(__file__))
FileData = ''

with open(f'{DirPath}/../data/windows.rc.template', 'r') as rcTemplate:
	FileData = rcTemplate.read()
	FileData = FileData.replace("__PRODUCT_VERSION1__", f"{MAJVER},{MINVER},{PATVER},0")
	FileData = FileData.replace("__PRODUCT_VERSION2__", f"{MAJVER}.{MINVER}.{PATVER}")

with open('windows.rc', 'w') as rcOutput:
	rcOutput.write(FileData)


