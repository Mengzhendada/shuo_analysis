#!/bin/bash
cd /home/jias/simc_gfortran/util/root_tree
#cd /group/c-csv/shuo/simc_gfortran/util/root_tree 
for i in {10..590..10}
do
  #echo csv_${i}_D2_neg_inc_rad | ./make_root_tree
  #echo csv_${i}_D2_pos_inc_rad | ./make_root_tree
  #echo csv_${i}_D2_neg_inc_norad | ./make_root_tree
  #echo csv_${i}_D2_pos_inc_norad | ./make_root_tree
  echo csv_${i}_D2_neg_exc_rad | ./make_root_tree
  echo csv_${i}_D2_pos_exc_rad | ./make_root_tree
  #echo csv_${i}_D2_neg_delta | ./make_root_tree
  #echo csv_${i}_D2_pos_delta | ./make_root_tree
  #echo csv_${i}_H2_neg_inc_rad | ./make_root_tree
  #echo csv_${i}_H2_pos_inc_rad | ./make_root_tree
  #echo csv_${i}_H2_neg_inc_norad | ./make_root_tree
  #echo csv_${i}_H2_pos_inc_norad | ./make_root_tree
  #echo csv_${i}_H2_pos_exc_rad | ./make_root_tree
  #echo csv_${i}_H2_neg_delta | ./make_root_tree
  #echo csv_${i}_H2_pos_delta | ./make_root_tree
  echo csv_${i}_D2_neg_rho | ./make_root_tree
  echo csv_${i}_D2_pos_rho | ./make_root_tree
  echo csv_${i}_H2_neg_rho | ./make_root_tree
  echo csv_${i}_H2_pos_rho | ./make_root_tree
done
