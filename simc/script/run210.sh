#/bin/bash
echo "simulate RunGroup 210"
cd /group/c-csv/shuo/simc_gfortran
echo csv_210_Dummy_neg_inc_norad | ./simc
echo csv_210_Dummy_neg_inc_rad | ./simc
echo csv_210_Dummy_neg_exc_norad | ./simc
echo csv_210_Dummy_pos_inc_norad | ./simc
echo csv_210_Dummy_pos_inc_rad | ./simc
echo csv_210_Dummy_pos_exc_norad | ./simc
