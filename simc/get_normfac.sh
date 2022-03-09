#!/bin/bash
echo "Get normfac for all run groups"
#cd /group/c-csv/shuo/online_csv
cd /lcrc/project/jlab/csv/offline/online_csv
for i in {10..590..10} 
do
  root -q -b "shuo_analysis/simc/get_normfac.cxx+($i)"
done

