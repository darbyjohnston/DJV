#!/bin/sh

find bin cmake doc etc lib tests examples experiments -name "*.h" -or -name "*.cpp" | xargs wc -l
find bin cmake doc etc lib tests examples experiments -name "*.h" -or -name "*.cpp" | wc -l

