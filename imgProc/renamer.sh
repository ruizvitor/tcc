cd ../material/rotation/pos/;
for file in *.jpgb; do
  # mv "$file" "$(basename "$file" .jpgb).jpg"
  cp "$file" "${file%.jpgb}.png"
done
