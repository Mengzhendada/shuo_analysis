#/bin/bash
echo "simulate RunGroup 480"
cd /group/c-csv/shuo/simc_gfortran
echo csv_480_H2_neg_exc_rad | ./simc
echo csv_480_H2_pos_exc_rad | ./simc
