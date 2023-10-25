# SortTape

Application simulating external sorting of a big data array.

* Input: file big enough not to fit into RAM consisted of unsorted integer values
* Output: file with sorted values

## How to build:

Build jsoncpp header and source files:
```
cd lib/third_party/jsoncpp
python amalgamate.py
```
Build and run application:
```
cmake .
make
./sort_tape MaxMemory(in Kilobytes) InputFileName OutputFileName [latency_config]
```

## Hot to run unit-tests:
Build tests_runner:
```
make unit_tests_run
```
Run tests:
```
(cd lib ; ./unit_tests/unit_tests_run )
```