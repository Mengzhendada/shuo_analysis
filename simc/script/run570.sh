#/bin/bash
echo "simulate RunGroup 570"
cd /group/c-csv/shuo/simc_gfortran
echo csv_570_D2_neg_inc_norad | ./simc
echo csv_570_D2_neg_inc_rad | ./simc
echo csv_570_D2_neg_exc_rad | ./simc
echo csv_570_D2_neg_rho | ./simc
echo csv_570_D2_pos_inc_norad | ./simc
echo csv_570_D2_pos_inc_rad | ./simc
echo csv_570_D2_pos_exc_rad | ./simc
echo csv_570_D2_pos_rho | ./simc
