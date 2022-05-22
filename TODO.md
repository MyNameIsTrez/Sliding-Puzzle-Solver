* Running "C:/msys64/usr/bin/make.exe clean" manually in the terminal doesn't work, so neither does it in the Makefile.
* Use a nicer and shorter iterator that doesn't require a vague "it" pointer.
* Try to make things like "pieces" or a struct like "piece_pos" a reference for better performance.
* Consistently use "position" instead of "pos" in all the jsonc, Python and C++ code.
* Rename self.HEIGHT and self.WIDTH to self.grid_rows and self.grid_columns. Same for C++.
* Use range-based for loops whenever possible.
* Check whether using auto whenever possible results in the same performance.
* The pairs data type is supposedly slow, so store a struct inside of path_queue instead of pairs.
* Use the Include What You Use extension.
* Find way around using json.hpp which doesn't seem to support C++20.
* Add a new JSON key for rectangular Pieces with `x, y, width, height` that substitutes having a gazillion 1x1 square Pieces. Minimizing the number of squares using rectangles should be done by a bin packing program. It may be even better to put the 1x1 square Pieces in the same vector as the rectangular Pieces by giving them a width and height of 1, but that should be profiled.
* Move methods like set_wall_cell_indices() which aren't integral to the algorithm to other classes.
* Make an iterator (generator?) for std::vector<T> (T can be pieces, walls, more?) that yields all x and y values.
* Is it faster to store a constant reference than it is to dereference twice? For example: "const Pos &piece_top_left = piece.top_left; const int x = piece_top_left.x; const int y = piece_top_left.y;"
* Maybe the number of collisions is so high and hashing is so cheap that it may be faster to just always check if the state is already in states before doing collision checking?
* Use < or != consistently in for-loops.
* Turn -O3 back on.
* Check if initializing all for and while loop conditions as consts before the loops is faster.
* Rename id to index everywhere.
* emplace_back() should be faster than push_back()
* Custom typedef for x, y, width and height so they aren't hardcoded as ints. uint8_t might even be faster?

# New algorithm
* Use IDDFS: https://en.wikipedia.org/wiki/Iterative_deepening_depth-first_search
