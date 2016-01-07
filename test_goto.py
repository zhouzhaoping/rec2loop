#!/usr/bin/python  
# -*- coding: utf8 -*- 

import os

#os.system('make')

filename = ['add' , 'fac', 'fib', 'multicall', 'test0', 'add_fac_fib']
for f in filename:
	os.system('build/rec2goto test/' + f + '.cpp')
	os.system('g++ test/' + f + '.cpp -o before')
	os.system('g++ test/' + f + '_goto.cpp -o after')