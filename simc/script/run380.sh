#/bin/bash
echo "simulate RunGroup 380"
cd /group/c-csv/shuo/simc_gfortran
echo csv_380_H2_neg_exc_rad | ./simc
echo csv_380_H2_pos_exc_rad | ./simc
