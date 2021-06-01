#/bin/bash
echo "simulate RunGroup 390"
cd /group/c-csv/shuo/simc_gfortran
echo csv_390_H2_neg_exc_rad | ./simc
echo csv_390_H2_pos_exc_rad | ./simc
