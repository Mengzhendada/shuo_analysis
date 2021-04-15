cd /group/c-csv/shuo/simc_gfortran/util/root_tree
for (( i = 10; i<590; i=i+10 ))
do
  echo csv_${i}_D2_pos_delta | ./make_root_tree
done
