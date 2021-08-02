#/bin/bash
echo "simulate RunGroup 250"
cd /group/c-csv/shuo/simc_gfortran
echo csv_250_D2_neg_inc_norad | ./simc
echo csv_250_D2_neg_inc_rad | ./simc
echo csv_250_D2_neg_exc_norad | ./simc
echo csv_250_D2_neg_rho | ./simc
echo csv_250_D2_neg_delta | ./simc
echo csv_250_D2_pos_inc_norad | ./simc
echo csv_250_D2_pos_inc_rad | ./simc
echo csv_250_D2_pos_exc_norad | ./simc
echo csv_250_D2_pos_rho | ./simc
echo csv_250_D2_pos_delta | ./simc
echo csv_250_Dummy_neg_inc_norad | ./simc
echo csv_250_Dummy_neg_inc_rad | ./simc
echo csv_250_Dummy_neg_exc_rad | ./simc
echo csv_250_Dummy_neg_rho | ./simc
echo csv_250_Dummy_pos_inc_norad | ./simc
echo csv_250_Dummy_pos_inc_rad | ./simc
echo csv_250_Dummy_pos_exc_rad | ./simc
echo csv_250_Dummy_pos_rho | ./simc
echo csv_250_H2_neg_inc_norad | ./simc
echo csv_250_H2_neg_inc_rad | ./simc
echo csv_250_H2_neg_rho | ./simc
echo csv_250_H2_neg_delta | ./simc
echo csv_250_H2_pos_inc_norad | ./simc
echo csv_250_H2_pos_inc_rad | ./simc
echo csv_250_H2_pos_exc_rad | ./simc
echo csv_250_H2_pos_rho | ./simc
echo csv_250_H2_pos_delta | ./simc
