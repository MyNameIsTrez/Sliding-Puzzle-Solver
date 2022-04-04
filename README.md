# Sliding-Puzzle-Solver

See misc/klotski.pdf in this repository for more information on the Klotski puzzle.

There's a launch.json so you can run whatever implementation you want with ease in VS Code.

## C++

The Python implementation finds ~15000 new states/second and takes 12 minutes and 54 seconds (774 seconds) to find the shortest path of 116 moves.

### Profiling

`sudo perf record --call-graph dwarf ./puzzle && sudo chmod 777 perf.data && flamegraph --perfdata ./perf.data`

`sudo perf record --call-graph dwarf ./puzzle && sudo perf script | sudo /root/.cargo/bin/inferno-collapse-perf | sudo /root/.cargo/bin/inferno-flamegraph > flamegraph.svg`
See https://github.com/jonhoo/inferno

#### Individual profiling commands

`sudo perf record --call-graph dwarf ./puzzle`

`sudo chmod 777 perf.data`
See https://github.com/jonhoo/inferno/issues/226#issuecomment-1009460934

`flamegraph --perfdata ./perf.data`

Setting up flamegraph:
`code /etc/environment`
`. /etc/environment`
See https://askubuntu.com/a/170240

## Python

`pip install -r code/python/requirements.txt`

`python code/python/main.py`

The Python implementation finds ~5300 new states/second and takes 37 minutes and 46 seconds (2266 seconds) to find the shortest path of 116 moves.
