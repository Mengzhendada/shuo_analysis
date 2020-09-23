#/bin/bash
echo "simulate RunGroup 70"
cd /group/c-csv/shuo/simc_gfortran
echo csv_70_D2_neg_inc_norad | ./simc
echo csv_70_D2_neg_inc_rad | ./simc
echo csv_70_D2_neg_exc_norad | ./simc
echo csv_70_D2_pos_inc_norad | ./simc
echo csv_70_D2_pos_inc_rad | ./simc
echo csv_70_D2_pos_exc_norad | ./simc
