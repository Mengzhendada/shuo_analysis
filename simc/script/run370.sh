#/bin/bash
echo "simulate RunGroup 370"
cd /group/c-csv/shuo/simc_gfortran
echo csv_370_D2_neg_rho | ./simc
echo csv_370_D2_neg_delta | ./simc
echo csv_370_D2_pos_rho | ./simc
echo csv_370_D2_pos_delta | ./simc
