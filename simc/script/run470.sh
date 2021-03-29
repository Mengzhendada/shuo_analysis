#/bin/bash
echo "simulate RunGroup 470"
cd /group/c-csv/shuo/simc_gfortran
echo csv_470_D2_neg_rho | ./simc
echo csv_470_D2_neg_delta | ./simc
echo csv_470_D2_pos_rho | ./simc
echo csv_470_D2_pos_delta | ./simc
