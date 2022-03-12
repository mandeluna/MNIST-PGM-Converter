# MNIST to PGM Converter

This utility will convert raw MNIST data files to PGM image files in labeled directories.

## Motivation

MNIST is a database of handwritten digits that are commonly used for machine learning tutorials.

PGM is a [very simple image format](http://netpbm.sourceforge.net/doc/pgm.html) for grayscale images.

Apple's Create ML framework will import images data from these directories, and it may be useful
to view the images directly from the file system.

If you are using Python you are unlikely to need this.

## Alternatives

Most people learning the MNIST tutorial use Keras
[mnist.load_data()](https://github.com/keras-team/keras/blob/v2.8.0/keras/datasets/mnist.py) function
from a Python workspace to obtain their data.

The Keras function does not return the data directly from the [MNIST Homepage](http://yann.lecun.com/exdb/mnist/)
(fortunately, because automated scripts should not do that), but it takes a "pickled" or serialized
Python binary object file from the Google servers.

## Usage

The Keras Python binary is not suitable for this, and neither is the raw data from the MNIST page.

To use this utility, download labels and images from the MNIST data, uncompress them, and run the following command
to put them into labeled directories:

`mkdir train`
`mkdir test`
`convert_to_labeled_directories train-labels-idx1-ubyte train-images-idx3-ubyte train`
`convert_to_labeled_directories t10k-labels-idx1-ubyte t10k-images-idx3-ubyte test`

## Compiling

To compile on Mac OS X: `clang convert_to_labeled_directories.c -o convert_to_labeled_directories`

To compile on Linux: `gcc -I/usr/include/bsd -DLIBBSD_OVERLAY -lbsd  -o convert_to_labeled_directories`

