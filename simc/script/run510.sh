#/bin/bash
echo "simulate RunGroup 510"
cd /group/c-csv/shuo/simc_gfortran
echo csv_510_Dummy_neg_inc_norad | ./simc
echo csv_510_Dummy_neg_inc_rad | ./simc
echo csv_510_Dummy_neg_exc_norad | ./simc
echo csv_510_Dummy_pos_inc_norad | ./simc
echo csv_510_Dummy_pos_inc_rad | ./simc
echo csv_510_Dummy_pos_exc_norad | ./simc
