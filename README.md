rec2loop
====
automatic tool for replacing recursive functions using stack and while-loop to avoid the stack-overflow  

clang-based  
source-to-source transformation  

##  Require list
1. llvm3.7
2. gcc/g++4.8

What is where
-------------

src/  
	source code.  

test/  
	some sample.  

Building rec2loop
-----------------
1. change Makefile
2. make  

Running
-------
#### rec2loop \<filepath\>  
replacing recursive functions using stack and while-loop  
#### recdetector_linear  \<filepath\>  
only detect linear recursion functions  
#### recdetector_cycle  \<filepath\>  
detect recursion functions using graph algorithm and output callgraph circle  