#/bin/bash
echo "simulate RunGroup 160"
cd /group/c-csv/shuo/simc_gfortran
echo csv_160_D2_neg_inc_norad | ./simc
echo csv_160_D2_neg_inc_rad | ./simc
echo csv_160_D2_neg_exc_rad | ./simc
echo csv_160_D2_neg_rho | ./simc
echo csv_160_D2_pos_inc_norad | ./simc
echo csv_160_D2_pos_inc_rad | ./simc
echo csv_160_D2_pos_exc_rad | ./simc
echo csv_160_D2_pos_rho | ./simc
