* Running "C:/msys64/usr/bin/make.exe clean" manually in the terminal doesn't work, so neither does it in the Makefile.
* Use a nicer and shorter iterator that doesn't require a vague "it" pointer.
* Try to make things like "pieces" or a struct like "piece_pos" a reference for better performance.
* Consistently use "position" instead of "pos" in all the jsonc, Python and C++ code.
* Rename self.HEIGHT and self.WIDTH to self.grid_rows and self.grid_columns. Same for C++.

* Add `bool end;` to `StartingPieceInfo` and update `set_ending_pieces()` so it sets that `end` to `false` by default.

* Use a vector everywhere instead of a map.

* Remove `print_board_every_path` and instead if `pieces_queue.size() > 0` in timed_print() then use `std::cout << ", Path length: " << pieces_queue[0].size();`.

* Add "empty_areas" as a key in the puzzle's JSON holding an array.
  The array contains {"top_left_x": 5, ...}
* Add the struct empty_area to sliding_puzzle_solver.h
* Add the field `vec<empty_area> empty_area;` as a field and initialize it.

* Use a 2D vector of booleans to keep track of occupied tiles for collision.
* A 2D vector supposedly is slow because it packs bits, so compare with the containers that don't: `std::array` and `bitset`.
