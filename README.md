# Sliding-Puzzle-Solver

`python main.py`

40 path length to 50 path length takes `` seconds

Benchmarking w/ list for states:
10k -> 20k states: 20s
20k -> 30k states: 45s + 25s
30k -> 40k states: 75s + 30s

Benchmarking w/ deque for states:
10k -> 20k states: 21s
20k -> 30k states: 48s + 27s
30k -> 40k states: 79s + 29s