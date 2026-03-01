#!/bin/bash

echo "Running clang-format on all files..."
find . -type f '(' -name '*.cpp' -o -name '*.h' ')' -print0 | xargs -0 -r /bin/clang-format-14 -i
echo "Done."