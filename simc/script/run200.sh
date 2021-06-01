#/bin/bash
echo "simulate RunGroup 200"
cd /group/c-csv/shuo/simc_gfortran
echo csv_200_H2_neg_exc_rad | ./simc
echo csv_200_H2_pos_exc_rad | ./simc
