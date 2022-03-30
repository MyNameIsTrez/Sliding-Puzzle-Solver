* Running "C:/msys64/usr/bin/make.exe clean" manually in the terminal doesn't work, so neither does it in the Makefile.
* Use a nicer and shorter iterator that doesn't require a vague "it" pointer.
* Try to make things like "pieces" or a struct like "piece_pos" a reference for better performance.
* Consistently use "position" instead of "pos" in all the jsonc, Python and C++ code.
* Rename self.HEIGHT and self.WIDTH to self.grid_rows and self.grid_columns. Same for C++.
