#/bin/bash
echo "simulate RunGroup 250"
cd /group/c-csv/shuo/simc_gfortran
echo csv_250_D2_neg_rho | ./simc
echo csv_250_D2_neg_delta | ./simc
echo csv_250_D2_pos_rho | ./simc
echo csv_250_D2_pos_delta | ./simc
