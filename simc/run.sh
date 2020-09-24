#/bin/bash
echo "simulate RunGroup $1"
cd /home/shuojia/simc_gfortran
export SIMCIN="infiles/csv_110_D2_neg_exc_norad.inp"
echo csv_${1}_D2_neg_inc_norad | ./simc
echo csv_${1}_D2_neg_inc_rad | ./simc
echo csv_${1}_D2_neg_exc_norad | ./simc
echo csv_${1}_D2_pos_inc_norad | ./simc
echo csv_${1}_D2_pos_inc_rad | ./simc
echo csv_${1}_D2_pos_exc_norad | ./simc
