#/bin/bash
echo "simulate RunGroup 510"
cd /group/c-csv/shuo/simc_gfortran
echo csv_510_H2_neg_exc_rad | ./simc
echo csv_510_H2_pos_exc_rad | ./simc
