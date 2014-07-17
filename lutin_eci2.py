#!/usr/bin/python
import lutinModule as module
import lutinTools as tools
import os
import lutinMultiprocess

def get_desc():
	return "Ewol C Interpreter"

def create(target):
	# module name is 'edn' and type binary.
	myModule = module.Module(__file__, 'eci2', 'BINARY')
	# add extra compilation flags :
	myModule.add_extra_compile_flags()
	# add the file to compile:
	myModule.add_src_file([
		'eci/eci.cpp',
		'eci/Lexer.cpp',
		'eci/debug.cpp',
		'eci/lang/ParserCpp.cpp'
		])
	myModule.add_export_path(tools.get_current_path(__file__))
	myModule.add_module_depend('etk')
	return myModule

