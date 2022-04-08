* Running "C:/msys64/usr/bin/make.exe clean" manually in the terminal doesn't work, so neither does it in the Makefile.
* Use a nicer and shorter iterator that doesn't require a vague "it" pointer.
* Try to make things like "pieces" or a struct like "piece_pos" a reference for better performance.
* Consistently use "position" instead of "pos" in all the jsonc, Python and C++ code.
* Rename self.HEIGHT and self.WIDTH to self.grid_rows and self.grid_columns. Same for C++.

* `typedef int CellIndex;`
* `typedef int PieceID;`

* `std::unordered_map<CellIndex, PieceID> cells;`
* `std::unordered_map<CellIndex, PieceID> active_cells;`

* `std::vector<int> movable_directions;` re-initialized to 0, 1, 2, 3 for every Piece and will only ever shrink.

* Use range-based for loops whenever possible.

* Check whether using auto whenever possible results in the same performance.

* The pairs data type is supposedly slow, so store a struct inside of path_queue instead of pairs.

* Use the Include What You Use extension.

* Find way around using json.hpp which doesn't seem to support C++20.
