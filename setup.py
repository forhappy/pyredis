#!/usr/bin/python

# Copyright (c) 2011 Fu Haiping.
# See LICENSE for details.

import sys
from distutils.core import setup, Extension

extra_compile_args = ['-pedantic','-I./hiredis/', 
'-shared', '-std=c99', '-fPIC', '-Wall', '-g', '-D_GNU_SOURCE']
extra_link_args = ['-L./hiredis', '-Wl,-Bstatic', '-lhiredis', '-Wl,-Bdynamic']

setup(
	name = 'pyredis',
	version = '0.1',
	maintainer = 'Fu Haiping',
	maintainer_email = 'haipingf@gmail.com',
	url = 'http://code.google.com/p/py-redis/',

	classifiers = [
		'Development Status :: 4 - Beta',
		'Environment :: Other Environment',
		'Intended Audience :: Developers',
		'License :: OSI Approved :: GNU Library or Lesser General Public License (LGPL)',
		'Operating System :: POSIX',
		'Programming Language :: C',
		'Programming Language :: Python',
		'Programming Language :: Python :: 2.4',
		'Programming Language :: Python :: 2.5',
		'Programming Language :: Python :: 2.6',
		'Programming Language :: Python :: 2.7',
		'Topic :: Database',
		'Topic :: Software Development :: Libraries'
	],

	description = 'Python bindings for redis database library using hiredis c api',
	# long_description = 

	packages = ['pyredis'],
	package_dir = {'pyredis': ''},

	ext_modules = [
		Extension('pyredis',
			sources = [
				# python stuff
				'py-redis.c'
			],
			extra_compile_args = extra_compile_args,
			extra_link_args = extra_link_args
		)
	]
)
