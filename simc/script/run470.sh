#/bin/bash
echo "simulate RunGroup 470"
cd /group/c-csv/shuo/simc_gfortran
echo csv_470_H2_neg_exc_rad | ./simc
echo csv_470_H2_pos_exc_rad | ./simc
