#/bin/bash
echo "simulate RunGroup 160"
cd /group/c-csv/shuo/simc_gfortran
echo csv_160_H2_neg_exc_rad | ./simc
echo csv_160_H2_pos_exc_rad | ./simc
