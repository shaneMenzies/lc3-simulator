#!/bin/bash

kitty --detach --hold bash -c "time echo continue | ../lc3tools/lc3sim ./demo/performance_test.obj" ; kitty --detach --hold bash -c "time ./lc3-simulator ./demo/performance_test.obj" ; kitty --detach --hold bash -c "time ./demo/native_test.out"