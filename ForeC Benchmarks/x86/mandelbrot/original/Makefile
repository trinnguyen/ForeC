#
# Laboratory for Scientific Computing
# http://www.lam-mpi.org/tutorials/
# University of Notre Dame
#
# MPI Tutorial
# Sample Makefile
#

ifeq ($(shell uname), Linux)
	CC			= clang
else ifeq ($(shell uname), Darwin)
	CC			= clang
	LDFLAGS		= -L/usr/X11R6/lib
else
	CC			= gcc
endif

c:
	$(CC) $(CFLAGS) -Wall Mandelbrot.c -o Mandelbrot

visual.c:
	$(CC) $(CFLAGS) -Wall Mandelbrot.visual.c -o Mandelbrot.visual $(LDFLAGS) -lX11

clean:
	rm -f Mandelbrot Mandelbrot.visual *.exe

