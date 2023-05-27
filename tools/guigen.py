"""
 <MainMenuBar>                                -> if (ImGui::BeginMainMenuBar()) {
  <Menu label="File">                         ->   if (ImGui::BeginMenu($label)) {
   <MenuItem label="New" shortcut="Ctrl+N" /> ->     if (ImGui::MenuItem($label, $shortcut)) {}
  </Menu>                                     ->     ImGui::EndMenu();
	                                          ->   }
 </MainMenuBar>                               ->   ImGui::EndMainMenuBar();
                                              -> }
"""

# Python 3.10 Or Higher Required

import xml.sax as sax
import subprocess
import os

scriptDir = os.path.dirname(__file__)

class MyXML_Handler(sax.ContentHandler):
	def __init__(self):
		self.finalOutput = ""

	def startElement(self, name, attrs):
		self.current = name
		match name:
			case "MainMenuBar":
				self.finalOutput += f"if (ImGui::BeginMainMenuBar()) {{\n"
			case "Menu":
				label = attrs.get('label', "NULL")
				self.finalOutput += f"if (ImGui::BeginMenu(\"{label}\")) {{\n"
			case "MenuItem":
				label = attrs.get('label', "NULL")
				shortcut = attrs.get('shortcut', "")
				self.finalOutput += f"if (ImGui::MenuItem(\"{label}\", \"{shortcut}\")) {{\n"
			case "Include":
				file = attrs.get('file', None)
				if file:
					self.finalOutput += f"#include \"{file}\"\n"

	# Content Of The XML Tag Itself
	def characters(self, content):
		match self.current:
			case "MenuItem":
				self.finalOutput += f"{content}\n"

	def endElement(self, name):
		match name:
			case "MainMenuBar":
				self.finalOutput += "ImGui::EndMainMenuBar();\n}\n"
			case "Menu":
				self.finalOutput += f"ImGui::EndMenu();\n}}\n"
			case "MenuItem":
				self.finalOutput += f"}}\n"
			case "Include":
				pass

		self.current = ""

Files = [
	{
		f"{scriptDir}/../data/gui/menu.xml": f"{scriptDir}/../src/gui/menu.hpp",
	}
]

for file in Files:
	inputFile, outputFile = file.popitem()
	handler = MyXML_Handler()
	parser = sax.make_parser()
	parser.setContentHandler(handler)
	parser.parse(inputFile)

	if not os.path.exists(os.path.dirname(outputFile)):
		os.makedirs(os.path.dirname(outputFile))

	with open(outputFile, "w") as f:
		f.write(handler.finalOutput)

	formattedCode = subprocess.check_output(['clang-format', '--style=Google', outputFile])

	with open(outputFile, "wb") as f:
		f.write(formattedCode)
