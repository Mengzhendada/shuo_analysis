#These are my scripts. 

tool scripts: 
readroot*.c: plot variable that are interested in and save in .pdf to download
creat_txt  : creat some annoying repeated txt
makelatexfromttxt.C: write repeated latex format txt

#Calorimeter gain analysis calorimeter_gain_analysis
cal_gain:plot for each block
cal_gain_json:calibration gain for each run in json
csvfallcalib: calibration gain for fall runs in txt
csvspringcalib: calibration gain for spring runs in txt
calorimeter_gain_modifyc: plot gain constant for each block
csvelas.C, csvelasfall.C: compare elastic data runs with simc result
SHMS_CAL_Read_Calparam.h: read txt calgain into json
shms_cal_param_to_json.cxx, read txt calgain into json for shms 
calorimeter_gain_analysis_statistic.cxx:count how mant blocks fires in calorimeter
creat_cal/rdataset: failed try

#elastic runs 
modifyelas.C: compare elastic data runs with some recalculation with simc data

#refertime 
csvreftimecut.c: make plots that are related to reference time cut

pre steering scripts:
string compare.C: test if string1 is included in string2
testrange.c: test x and y axis range in TGraph
txt_json: unfinished(forgot why)
calorimeter_gain_analysis: to_json.c: convert calorimeter output txt file to json format

help scripts:
json_update.cxx, json_update.py: update json file
print_good_run_number: read all run info from one json file, filter with comments
runnumclassify.c: classify runnumbers by momentum or angle
selected_info: as above

