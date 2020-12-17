#!/bin/bash
tex_file_name=$1
DIR=test_dir
mkdir $DIR
grep -r "\\includegraphics" $tex_file_name >outfile.txt
while read p; do
  PIC=`echo "$p" | grep -oP '(?<={).*(?=})'`
  echo $PIC
  mkdir -p $DIR/${PIC%/*}
  cp -r $PIC $DIR/$PIC
done <outfile.txt

