DIRS = libembryo tools/view tools/optim

export PKG_CONFIG_PATH=$(shell pwd)/libembryo/lib/pkgconfig
export LD_LIBRARY_PATH=$(shell pwd)/libembryo

export CFLAGS="-fno-stack-protector"
#export CFLAGS= -fno-stack-protector-all

export VERSION = 3.0

#export CXX = gcc -fno-stack-protector -U_FORTIFY_SOURCE
export CXX = g++
export CXXFLAGS = -g -Wall -ansi -fopenmp
#export OPTIM_FLAGS = -O2 -march=pentium4
#export OPTIM_FLAGS = -O2 -march=i686 -mtune=pentium4
#export CHOST="i686-pc-linux-gnu"
#export OPTIM_FLAGS = -march=prescott -O2 -pipe -fomit-frame-pointer
export OPTIM_FLAGS = -march=core2 -mtune=core2 -O2 -pipe -openmp
#export OPTIM_FLAGS = -march=pentium4 -O2 -pipe -fomit-frame-pointer
#CFLAGS="-march=pentium4 -O2 -pipe -fomit-frame-pointer"

#export CXX = g++

export TOP_DIRECTORY = $(shell pwd)
export LOCAL_DIRECTORY = $(TOP_DIRECTORY)/libembryo

export GENERAL_INSTALL_DIR = /usr/lib/gcc/x86_64-linux-gnu/4.8

export INCLUDES = \
-I. \
-I./include \
-I$(GENERAL_INSTALL_DIR)/include \
$(shell pkg-config gsl --cflags) \
$(shell pkg-config sdl --cflags) \
$(shell pkg-config cairo --cflags) \
$(shell sdl-config --cflags) \

export LDFLAGS = \
-lrt \
-lpthread \
-ldl \
-L$(GENERAL_INSTALL_DIR) -lgomp \
$(shell pkg-config fontconfig --libs) \
$(shell pkg-config cairo --libs) \
$(shell pkg-config gsl --libs) \
$(shell pkg-config sdl --libs) \
$(shell sdl-config --libs)

export DIST_FILES_LIST = $(TOP_DIRECTORY)/.dist
export LIB_EMBRYO_PATH = $(TOP_DIRECTORY)/libembryo

#export INSTALL_DIRECTORY = /usr/local
export INSTALL_DIRECTORY = $(TOP_DIRECTORY)/libembryo

ARCHIVE_FILE = $(shell basename `pwd`).tar.gz

all: $(DIRS)

# --- Project clean up ---

clean: $(DIRS)

cleaner: clean $(DIRS)
	rm -f $(ARCHIVE_FILE)

compile: $(OBJS)


# --- Project tarball generation ---

dist_files: $(DIRS)
	ls -1 $(TOP_DIRECTORY)/Makefile $(TOP_DIRECTORY)/how-to.txt $(TOP_DIRECTORY)/changelog $(TOP_DIRECTORY)/test/*.* $(TOP_DIRECTORY)/test/targets/*.pgm  >> $(DIST_FILES_LIST)

dist:
	rm -f $(DIST_FILES_LIST)
	$(MAKE) dist_files
	sed -i -e "s#${TOP_DIRECTORY}#../$(shell basename `pwd`)#g" $(DIST_FILES_LIST)
	tar czf $(ARCHIVE_FILE) `cat $(DIST_FILES_LIST)`
	rm -f $(DIST_FILES_LIST)



# --- Project installation ---

install: $(DIRS)

uninstall: $(DIRS)



# --- GNUMake oddities ---

.PHONY: $(DIRS)
$(DIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)
