#/bin/bash
echo "simulate RunGroup 580"
cd /group/c-csv/shuo/simc_gfortran
echo csv_580_H2_neg_exc_rad | ./simc
echo csv_580_H2_pos_exc_rad | ./simc
