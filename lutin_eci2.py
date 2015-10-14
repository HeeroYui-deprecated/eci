#!/usr/bin/python
import lutin.module as module
import lutin.tools as tools
import os

def get_desc():
	return "Ewol C Interpreter"

def create(target, module_name):
	myModule = module.Module(__file__, module_name, 'BINARY')
	myModule.add_extra_compile_flags()
	myModule.add_src_file([
		'eci/eci.cpp',
		'eci/Lexer.cpp',
		'eci/debug.cpp',
		'eci/Class.cpp',
		'eci/Enum.cpp',
		'eci/File.cpp',
		'eci/Function.cpp',
		'eci/Interpreter.cpp',
		'eci/Library.cpp',
		'eci/Type.cpp',
		'eci/Variable.cpp',
		'eci/Value.cpp',
		'eci/lang/ParserCpp.cpp',
		'eci/lang/ParserJS.cpp'
		])
	myModule.add_export_path(tools.get_current_path(__file__))
	myModule.add_module_depend('etk')
	return myModule

