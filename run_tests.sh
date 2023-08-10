#!/bin/bash

for i in {1..111}
do
   ./main < tests/open_$i.txt > output.txt
   if diff -q output.txt tests/open_$i.output.txt > /dev/null; then
       echo "Test $i: Success"
   else
       echo "Test $i: Fail"
   fi
done

