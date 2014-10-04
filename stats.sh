#!/bin/sh

find bin cmake doc etc lib plugins tests -name "*.h" -or -name "*.cpp" | xargs wc -l

find bin cmake doc etc lib plugins tests -name "*.h" -or -name "*.cpp" | wc -l
