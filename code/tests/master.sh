# FILE="1sH2.sh
# 1sH3.sh
# 1sH.sh
# 1s.sh
# frontI.sh
# front.sh
# joel7H2.sh
# joel7H3.sh
# joel7H.sh
# joel7.sh
# joelH.sh
# joel.sh
# sintec.sh
# texjoelNoise.sh
# texjoel.sh
# tex.sh"

# FILE="1sH2.sh
# 1sH3.sh
# 1sH.sh
# 1s.sh
# frontI.sh
# joel7H2.sh
# joel7H3.sh
# joel7H.sh
# joelH.sh
# sintec.sh
# sintecInv.sh
# texjoelNoise.sh
# texjoel.sh
# tex.sh"

# FILE="sintecInv.sh"
# FILE="frontIbaixares.sh"
# FILE="texjoelNoise.sh
# texjoel.sh"
# FILE="joelHnotI.sh"

# FILE="joel7Hlast.sh"
# FILE="frontInoconvergence.sh"
FILE="frontI.sh"

while read -r line; do
  # echo "... $line ..."
  bash $line

  tmp="$(basename "$line" .sh)"

  cd ..;
  mkdir ../results/others/${tmp}
  cp poseinput.txt ../results/others/${tmp}/${tmp}_prepose.txt;
  cp pose.txt ../results/others/${tmp}/${tmp}_pospose.txt
  cp out/newUV.png ../results/others/${tmp}/${tmp}_uv.png
  cp out/nor111.png ../results/others/${tmp}/${tmp}_nor_pre.png
  cp out/nor888.png ../results/others/${tmp}/${tmp}_nor_pos.png
  cp out/tmp111.png ../results/others/${tmp}/${tmp}_res_pre.png
  cp out/tmp888.png ../results/others/${tmp}/${tmp}_res_pos.png
  cp disOUTPUT.txt ../results/others/${tmp}/${tmp}_dis.txt
  cd tests;

done <<< "$FILE"
