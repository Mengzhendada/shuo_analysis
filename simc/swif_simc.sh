#/bin/bash
cd /group/c-csv/shuo/shuo_analysis/simc
swif cancel csv_simc -delete
swif import -file csv_simc.json
swif run csv_simc
