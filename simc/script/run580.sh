#/bin/bash
echo "simulate RunGroup 580"
cd /group/c-csv/shuo/simc_gfortran
echo csv_580_D2_neg_rho | ./simc
echo csv_580_D2_neg_delta | ./simc
echo csv_580_D2_pos_rho | ./simc
echo csv_580_D2_pos_delta | ./simc
