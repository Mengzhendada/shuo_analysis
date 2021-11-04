#/bin/bash
echo "make simc input files for all run groups"
cd /lcrc/project/jlab/csv/offline/online_csv
for (( i = 1; i<59; i++ )) 
do
  root -q -b "shuo_analysis/simc/make_simc_input.cxx+(10*$i)"
  #root -q -b "/group/c-csv/shuo/shuo_analysis/simc/make_simc_input_Dummy.cxx+(10*$i)"
  root -q -b "shuo_analysis/simc/make_simc_input_H2.cxx+(10*$i)"
done

