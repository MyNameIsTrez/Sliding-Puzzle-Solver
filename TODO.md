* Running "C:/msys64/usr/bin/make.exe clean" manually in the terminal doesn't work, so neither does it in the Makefile.
* Use a nicer and shorter iterator that doesn't require a vague "it" pointer.
* Try to make things like "pieces" or a struct like "piece_pos" a reference for better performance.
* Consistently use "position" instead of "pos" in all the jsonc, Python and C++ code.
* Rename self.HEIGHT and self.WIDTH to self.grid_rows and self.grid_columns. Same for C++.

* Use range-based for loops whenever possible.

* Check whether using auto whenever possible results in the same performance.

* Use a 2D vector of booleans to keep track of occupied tiles for collision.
* A 2D vector supposedly is slow because it packs bits, so compare with the containers that don't: `std::array` and `bitset`.

* The pairs data type is supposedly slow, so store a struct inside of path_queue instead of pairs.

* Find a C++ formatting function that takes a number like 1100 and returns "1.1k".
