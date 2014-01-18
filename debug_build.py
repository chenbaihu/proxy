#!/usr/bin/python

import os

os.system('cmake -DCMAKE_BUILD_TYPE=Debug -D CMAKE_C_COMPILER=/usr/local/clang/bin/clang -D CMAKE_CXX_COMPILER=/usr/local/clang/bin/clang++ -D BUILD_CURL_TESTS=OFF -D CURL_STATICLIB=ON .')
