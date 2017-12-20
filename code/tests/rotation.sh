cd ..;
# primusrun ./runme -f initial/rotation/frame153.txt -p ~/tg-1/material/rotation/pos/frame153.png -i -h 1;
# cp pose.txt initial/rotation/frame154.txt
# cp out/newUV.png ../results/153_uv.png
# cp out/nor111.png ../results/153_nor_pre.png
# cp out/nor888.png ../results/153_nor_pos.png
# cp out/tmp111.png ../results/153_res_pre.png
# cp out/tmp888.png ../results/153_res_pos.png
# cp disOUTPUT.txt ../results/153_dis.txt
for file in ~/tg-1/material/rotation/pos/*.png; do
  DIR=$(dirname "$file")
  tmp="$(basename "$file" .png)"
  tmp="${tmp#frame}"
  number=${tmp}

  # tmp=$(printf "frame%s-%02d.txt" ${tmp} $((10#${tmp}+1)))
  tmp=$(printf "frame%02d.txt" $((10#${tmp}+1)))

  # primusrun ./runme -f initial/rotation/"$(basename "$file" .png).txt" -p ~/tg-1/material/rotation/pos/"$file" -i -h 1;
  primusrun ./runme -f initial/rotation/"$(basename "$file" .png).txt" -p "$file" -i -h 1;

  cp pose.txt initial/rotation/$tmp
  cp out/newUV.png ../results/${number}_uv.png
  cp out/nor111.png ../results/${number}_nor_pre.png
  cp out/nor888.png ../results/${number}_nor_pos.png
  cp out/tmp111.png ../results/${number}_res_pre.png
  cp out/tmp888.png ../results/${number}_res_pos.png
  cp disOUTPUT.txt ../results/${number}_dis.txt
  # echo ----------------
done
cd tests;
