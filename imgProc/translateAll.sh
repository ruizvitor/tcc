#!/bin/bash
cd ../material/rotation/pos/;
for file in *.png; do
  # mv "$file" "$(basename "$file" .jpgb).jpg"
  ../../../imgProc/./translate "$file" $1 $2
done
