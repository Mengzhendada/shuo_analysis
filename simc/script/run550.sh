#/bin/bash
echo "simulate RunGroup 550"
cd /group/c-csv/shuo/simc_gfortran
echo csv_550_H2_neg_exc_rad | ./simc
echo csv_550_H2_pos_exc_rad | ./simc
