Embryo 3 version optimizing structure and color pattern.

CMA-ES (Covariance Matrix Adaptation Evolution Strategy) is used as the optimizer: http://en.wikipedia.org/wiki/CMA-ES

see video of an (already optimized) embryo evolving into the learned pattern:
https://www.youtube.com/watch?v=cB1l2mf_OwI

fontconfig, cairo, SDL, gsl, openmp headers and libraries required

in debian based distros:

sudo aptitude install 'desired library'

where the desired libraries are:
libfontconfig-dev
libcairo-dev
libsdl-dev
libgsl0-dev
libopenmpi-dev

set your LD_LIBRARY_PATH and PKG_CONFIG_PATH environment variables (in ~/.bashvrc)
modify GENERAL_INSTALL_DIR in the Makefile 
make
