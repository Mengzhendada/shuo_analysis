cd /group/c-csv/shuo/simc_gfortran/util/root_tree
for (( i = 10; i<590; i=i+10 ))
do
  echo csv_${i}_D2_neg_inc_norad | ./make_root_tree
  echo csv_${i}_D2_neg_inc_rad | ./make_root_tree
  echo csv_${i}_D2_neg_exc_rad | ./make_root_tree
  echo csv_${i}_D2_neg_rho | ./make_root_tree
  echo csv_${i}_D2_pos_inc_norad | ./make_root_tree
  echo csv_${i}_D2_pos_inc_rad | ./make_root_tree
  echo csv_${i}_D2_pos_exc_rad | ./make_root_tree
  echo csv_${i}_D2_pos_rho | ./make_root_tree
done
