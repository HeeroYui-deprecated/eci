#!/usr/bin/python
import lutin.module as module
import lutin.tools as tools
import os

def get_type():
	return "BINARY"

def get_name():
	return "ECI"

def get_desc():
	return "Ewol C Interpreter"

def get_licence():
	return "MPL-2"

def get_compagny_type():
	return "org"

def get_compagny_name():
	return "Edouard DUPIN"

def get_maintainer():
	return ["Mr DUPIN Edouard <yui.heero@gmail.com>"]

def get_version():
	return [0,1,"dev"]

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

