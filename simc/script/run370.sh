#/bin/bash
echo "simulate RunGroup 370"
cd /group/c-csv/shuo/simc_gfortran
echo csv_370_H2_neg_exc_rad | ./simc
echo csv_370_H2_pos_exc_rad | ./simc
