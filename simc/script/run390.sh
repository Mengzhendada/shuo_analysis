#/bin/bash
echo "simulate RunGroup 390"
cd /group/c-csv/shuo/simc_gfortran
echo csv_390_D2_neg_rho | ./simc
echo csv_390_D2_neg_delta | ./simc
echo csv_390_D2_pos_rho | ./simc
echo csv_390_D2_pos_delta | ./simc
