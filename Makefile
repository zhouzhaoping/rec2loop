#-------------------------------------------------------------------------------
# Sample makefile for building the code. Read inline comments for
# documentation.
#
# Zhaoping Zhou (zhouzhaoping11@gmail.com)
#
#-------------------------------------------------------------------------------

# The following variables will likely need to be customized, depending on where
# and how you built LLVM & Clang. They can be overridden by setting them on the
# make command line: "make VARNAME=VALUE", etc.

# LLVM_SRC_PATH is the path to the root of the checked out source code. This
# directory should contain the configure script, the include/ and lib/
# directories of LLVM, Clang in tools/clang/, etc.
#
# Alternatively, if you're building vs. a binary distribution of LLVM
# (downloaded from llvm.org), then LLVM_SRC_PATH can point to the main untarred
# directory of the binary download (the directory that has bin/, lib/, include/
# and other directories inside).
LLVM_SRC_PATH := /home/work/llvm3.7/llvm

# LLVM_BUILD_PATH is the directory in which you built LLVM - where you ran
# configure or cmake.
# For linking vs. a binary build of LLVM, point to the main untarred directory.
# LLVM_BIN_PATH is the directory where binaries are placed by the LLVM build
# process. It should contain the tools like opt, llc and clang. The default
# reflects a debug build with autotools (configure & make), and needs to be
# changed when a Ninja build is used (see below for example). For linking vs. a
# binary build of LLVM, point it to the bin/ directory.
LLVM_BUILD_PATH := /home/work/llvm3.7/build
LLVM_BIN_PATH := $(LLVM_BUILD_PATH)/Release+Asserts/bin

# DETECT_CYCLE detector find cycle or linear recursion from callgraph
MYDEF=-DDETECT_CYCLE

# Run make BUILD_NINJA=1 to enable these paths
ifdef BUILD_NINJA
	LLVM_BUILD_PATH := $$HOME/llvm/build/svn-ninja-release
	LLVM_BIN_PATH 	:= $(LLVM_BUILD_PATH)/bin
endif

$(info -----------------------------------------------)
$(info Using LLVM_SRC_PATH = $(LLVM_SRC_PATH))
$(info Using LLVM_BUILD_PATH = $(LLVM_BUILD_PATH))
$(info Using LLVM_BIN_PATH = $(LLVM_BIN_PATH))
$(info -----------------------------------------------)

# CXX has to be a fairly modern C++ compiler that supports C++11. gcc 4.8 and
# higher or Clang 3.2 and higher are recommended. Best of all, if you build LLVM
# from sources, use the same compiler you built LLVM with.
# Note: starting with release 3.7, llvm-config will inject flags that gcc may
# not support (for example '-Wcovered-switch-default'). If you run into this
# problem, build with CXX set to a modern clang++ binary instead of g++.
CXX := g++
CXXFLAGS := -fno-rtti -O0 -g
PLUGIN_CXXFLAGS := -fpic

LLVM_CXXFLAGS := `$(LLVM_BIN_PATH)/llvm-config --cxxflags`
LLVM_LDFLAGS := `$(LLVM_BIN_PATH)/llvm-config --ldflags --libs --system-libs`

# Plugins shouldn't link LLVM and Clang libs statically, because they are
# already linked into the main executable (opt or clang). LLVM doesn't like its
# libs to be linked more than once because it uses globals for configuration
# and plugin registration, and these trample over each other.
LLVM_LDFLAGS_NOLIBS := `$(LLVM_BIN_PATH)/llvm-config --ldflags`
PLUGIN_LDFLAGS := -shared

# These are required when compiling vs. a source distribution of Clang. For
# binary distributions llvm-config --cxxflags gives the right path.
CLANG_INCLUDES := \
	-I$(LLVM_SRC_PATH)/tools/clang/include \
	-I$(LLVM_BUILD_PATH)/tools/clang/include

# List of Clang libraries to link. The proper -L will be provided by the
# call to llvm-config
# Note that I'm using -Wl,--{start|end}-group around the Clang libs; this is
# because there are circular dependencies that make the correct order difficult
# to specify and maintain. The linker group options make the linking somewhat
# slower, but IMHO they're still perfectly fine for tools that link with Clang.
CLANG_LIBS := \
	-Wl,--start-group \
	-lclangAST \
	-lclangAnalysis \
	-lclangBasic \
	-lclangDriver \
	-lclangEdit \
	-lclangFrontend \
	-lclangFrontendTool \
	-lclangLex \
	-lclangParse \
	-lclangSema \
	-lclangEdit \
	-lclangASTMatchers \
	-lclangRewrite \
	-lclangRewriteFrontend \
	-lclangStaticAnalyzerFrontend \
	-lclangStaticAnalyzerCheckers \
	-lclangStaticAnalyzerCore \
	-lclangSerialization \
	-lclangToolingCore \
	-lclangTooling \
	-Wl,--end-group

# Internal paths in this project: where to find sources, and where to put
# build artifacts.
SRC_DIR := src
BUILDDIR := build

.PHONY: all
all: make_builddir \
	emit_build_config \
	$(BUILDDIR)/recdetector_linear \
	$(BUILDDIR)/recdetector_cycle \
	$(BUILDDIR)/rec2loop \
	$(BUILDDIR)/rec2goto

.PHONY: emit_build_config
emit_build_config: make_builddir
	@echo $(LLVM_BIN_PATH) > $(BUILDDIR)/_build_config

.PHONY: make_builddir
make_builddir:
	@test -d $(BUILDDIR) || mkdir $(BUILDDIR)

$(BUILDDIR)/rec2loop: $(SRC_DIR)/rec2loop.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $^ \
		$(CLANG_LIBS) $(LLVM_LDFLAGS) -o $@

$(BUILDDIR)/rec2goto: $(SRC_DIR)/rec2goto.cpp $(SRC_DIR)/FinderASTComsumer.cpp $(SRC_DIR)/Funcinfo.cpp $(SRC_DIR)/CodeInsert.cpp $(SRC_DIR)/CodeChangeVisitor.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $^ \
		$(CLANG_LIBS) $(LLVM_LDFLAGS) -o $@

$(BUILDDIR)/recdetector_linear: $(SRC_DIR)/recdetector.cpp $(SRC_DIR)/FinderASTComsumer.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $^ \
		$(CLANG_LIBS) $(LLVM_LDFLAGS) -o $@

$(BUILDDIR)/recdetector_cycle: $(SRC_DIR)/recdetector.cpp $(SRC_DIR)/FinderASTComsumer.cpp
	$(CXX) $(MYDEF) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $^ \
		$(CLANG_LIBS) $(LLVM_LDFLAGS) -o $@

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)/*
