#/bin/bash
echo "simulate RunGroup 220"
cd /group/c-csv/shuo/simc_gfortran
echo csv_220_H2_neg_exc_rad | ./simc
echo csv_220_H2_pos_exc_rad | ./simc
