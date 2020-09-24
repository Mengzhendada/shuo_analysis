#/bin/bash
echo "Make all bash file"
cd /group/c-csv/shuo/shuo_analysis/simc
root -b -q make_runshfile.cxx
cd script
chmod +x run*.sh
