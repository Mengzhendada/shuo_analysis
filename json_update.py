import json
with open('../online_csv/db2/run_list_coin.json') as json_file:
     data = json.load(json_file)
     for runs in data:
         if(int(runs)>=7593):
             data[runs]["run_info"]["beam_energy"] = 10.214
         if(7593<=int(runs)<=7670):
             data[runs]["spectrometers"]["hms_angle"] = 17.65
             data[runs]["spectrometers"]["shms_angle"] = 19.93
         if(7671<=int(runs)<=7722):
             data[runs]["spectrometers"]["hms_angle"] = 20.24
             data[runs]["spectrometers"]["shms_angle"] = 13.81
         if(7723<=int(runs)<=7792):
             data[runs]["spectrometers"]["hms_angle"] = 18.51
             data[runs]["spectrometers"]["shms_angle"] = 17.75
         if(7793<=int(runs)<=7820):
             data[runs]["spectrometers"]["hms_angle"] = 19.1
             data[runs]["spectrometers"]["shms_angle"] = 15.94
         if(7793<=int(runs)<=7820):
             data[runs]["spectrometers"]["hms_angle"] = 19.1
             data[runs]["spectrometers"]["shms_angle"] = 15.94
         if(7822<=int(runs)<=7824):
             data[runs]["spectrometers"]["hms_angle"] = 23.22
             data[runs]["spectrometers"]["shms_angle"] = 22.25
         if(7826<=int(runs)<=7828):
             data[runs]["spectrometers"]["hms_angle"] = 21.93
             data[runs]["spectrometers"]["shms_angle"] = 23.45
         if(7830<=int(runs)<=7830):
             data[runs]["spectrometers"]["hms_angle"] = 20.71
             data[runs]["spectrometers"]["shms_angle"] = 24.71
         
     with open("run_list_update.json",'w') as outfile : 
        outfile.write(json.dumps(data, indent=2,sort_keys=True))
