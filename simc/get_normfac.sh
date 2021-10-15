#/bin/bash
echo "Get normfac for all run groups"
#cd /group/c-csv/shuo/online_csv
cd /lcrc/project/jlab/csv/offline/online_csv
for (( i = 1; i<59; i++ )) 
do
  root -q -b "shuo_analysis/simc/get_normfac.cxx+(10*$i)"
done

