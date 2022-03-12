CC=gcc

ifeq ($(CC),gcc)
  CFLAGS=-I/usr/include/bsd -DLIBBSD_OVERLAY -lbsd
endif

convert_to_labeled_directories: convert_to_labeled_directories.c
	$(CC) $(CFLAGS) convert_to_labeled_directories.c -o convert_to_labeled_directories
