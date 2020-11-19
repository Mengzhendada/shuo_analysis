#/bin/bash
echo "Get normfac for all run groups"
cd /group/c-csv/shuo/online_csv

for (( i = 1; i<59; i++ )) 
do
  root -q -b "/group/c-csv/shuo/shuo_analysis/simc/get_normfac.cxx+(10*$i)"
done

root -q -b "/group/c-csv/shuo/shuo_analysis/simc/combine_json.cxx
