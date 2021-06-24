#/bin/bash
echo "simulate RunGroup 340"
cd /group/c-csv/shuo/simc_gfortran
echo csv_340_H2_neg_exc_rad | ./simc
echo csv_340_H2_pos_exc_rad | ./simc
