<Request>
  <Email email="shuojia@jlab.org" request="false" job="true"/>
  <Project name="c-comm2017"/>
  <Track  name="reconstruction"/>
  <Name   name="kingroupspeing"/>
  <OS     name="centos7"/>
  <DiskSpace space = "50" unit = "GB"/>
  <Memory space="3" unit="GB"/>
  <List name="runs">
  7593
  7597
  7608
  7611
  7625
  7671
  7679
  7684
  7723
  7729
  7741
  7753
  7796
  7798
  7805
  </List>
  <ForEach list="runs">
    <Job>
      <Input src="mss:/mss/hallc/spring17/raw/coin_all_0${runs}.dat" dest="coin_all_0${runs}.dat"/>
      <!-- Properties overridden here -->
      <Command><![CDATA[
/bin/bash <<EOF
source /home/shuojia/.bashrc
cd /group/c-csv/shuo/hallc_replay_sidis_fall18/


hcana -q "SCRIPTS/COIN/PRODUCTION/replay_production_coin_hElec_pProt.C(${runs},-1)" 

EOF
        ]]></Command>
    </Job>
  </ForEach> 

</Request>

