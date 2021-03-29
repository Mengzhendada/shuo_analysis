#/bin/bash
echo "simulate RunGroup 440"
cd /group/c-csv/shuo/simc_gfortran
echo csv_440_D2_neg_rho | ./simc
echo csv_440_D2_neg_delta | ./simc
echo csv_440_D2_pos_rho | ./simc
echo csv_440_D2_pos_delta | ./simc
