#!/usr/bin/python
# Requires Python v3 Interpreter

import os
import sys
import glob
import json

Files = []

for f in glob.glob("assets/languages/*.json"):
	if os.path.isfile(f):
		Files.append(f)

for f in Files:
	try:
		fp = open(f, "rb")
		j = json.load(fp)
		if "UnicodeRange" in j:
			ranges = j["UnicodeRange"]
			for r in ranges:
				if not r.startswith("0x"):
					raise Exception(f"Invalid hexadecimal representation: \"{r}\"")
				num = int(r, 16)

		print(f"{f} - ok")
	except Exception as e:
		print(f"{f} - {e}")
		sys.exit(1)

