# MonaLena
**MonaLena** is a comprehensive C++ image-library for converting color images to grayscale and from grayscale to halftone aka dithering. 
The library implements a full conversion pipeline. There are several color to grayscale conversions available. The grayscale image can be 
preprocessed to enhence edges and/or remove noise before one of several dither algorithmus are applied. In the last stage of the pipeline various effects can be created with postprocessing filters. 
One of the more fancy postprocessing steps is GameOfLife by John Conway.

The name is a hommage to Lena Forsen-Soederberg, the Mona-Lisa of the internet age.
The initial idea for this project is by Thomas Krenn, https://www.timitoosystems.com/

A detailed description can be found in ./docs/MonaLenaHandbook.odt or ./docs/MonaLenaHandbook.pdf. The *.pdf file is the exported result of the former. 
This document serves also as a gentle introduction to the field. 
Additionally the code is relative well documented.

The library is self-contained with no external dependencies. The test-driver can read and write *.jpg images.
The ./image subdirectory contains a collection of test-images. 

MonaLena can be freely used for private or educational projects.

Commercial users must ask the Author Dr. Chrilly Donninger at c.donninger@wavenet.at for explicit permission.

