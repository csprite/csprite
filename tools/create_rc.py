#!/usr/bin/python

import os
import sys

DirPath = os.path.dirname(os.path.realpath(__file__))
FileData = ''

with open(f'{DirPath}/../data/windows.rc.template', 'r') as rcTemplate:
	FileData = rcTemplate.read()
	FileData = FileData.replace("__PRODUCT_VERSION1__", f"1,0,0,0")
	FileData = FileData.replace("__PRODUCT_VERSION2__", f"1.0.0")

with open('windows.rc', 'w') as rcOutput:
	rcOutput.write(FileData)
