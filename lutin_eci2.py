#!/usr/bin/python
import lutin.module as module
import lutin.tools as tools
import os

def get_type():
	return "LIBRARY"

def get_desc():
	return "Ewol C Interpreter"

def get_licence():
	return "APACHE-2"

def get_compagny_type():
	return "com"

def get_compagny_name():
	return "atria-soft"

def configure(target, my_module):
	my_module.add_extra_flags()
	my_module.add_src_file([
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
	my_module.add_path(".", export=True)
	my_module.add_depend('etk')
	return True

