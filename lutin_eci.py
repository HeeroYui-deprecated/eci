#!/usr/bin/python
import lutinModule as module
import lutinTools as tools
import os
import lutinMultiprocess

def get_desc():
	return "Ewol C Interpreter"

def create(target):
	# module name is 'edn' and type binary.
	myModule = module.Module(__file__, 'eci', 'BINARY')
	# add extra compilation flags :
	myModule.add_extra_compile_flags()
	# add the file to compile:
	myModule.add_src_file([
		'eci/clibrary.c',
		'eci/expression.c',
		'eci/heap.c',
		'eci/include.c',
		'eci/lex.c',
		'eci/parse.c',
		'eci/picoc.c',
		'eci/platform.c',
		'eci/table.c',
		'eci/type.c',
		'eci/variable.c',
		'eci/platform/platform_unix.c',
		'eci/platform/library_unix.c',
		'eci/cstdlib/ctype.c',
		'eci/cstdlib/errno.c',
		'eci/cstdlib/math.c',
		'eci/cstdlib/stdbool.c',
		'eci/cstdlib/stdio.c',
		'eci/cstdlib/stdlib.c',
		'eci/cstdlib/string.c',
		'eci/cstdlib/time.c',
		'eci/cstdlib/unistd.c'
		])
	myModule.add_export_path(tools.get_current_path(__file__))
	# add the currrent module at the 
	return myModule

