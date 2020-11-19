#/bin/bash
echo "Make all bash file"
cd /group/c-csv/shuo
root -b -q shuo_analysis/simc/make_runshfile.cxx
cd shuo_analysis/simc/script
chmod +x run*.sh
