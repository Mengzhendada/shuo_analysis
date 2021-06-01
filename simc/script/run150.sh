#/bin/bash
echo "simulate RunGroup 150"
cd /group/c-csv/shuo/simc_gfortran
echo csv_150_H2_neg_exc_rad | ./simc
echo csv_150_H2_pos_exc_rad | ./simc
