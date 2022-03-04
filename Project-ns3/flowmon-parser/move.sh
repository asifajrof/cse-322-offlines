#!/bin/bash

# move from output folder with extension passed from cmd

taskcount=$1
extension=$2

outputfolder="/mnt/e/Study/Level-3/L3T2/CSE321-322/322/github_backup/cse-322-offlines/ns3/ns-allinone-3.35/ns-3.35/output-files/"
movefolder="/mnt/e/Study/Level-3/L3T2/CSE321-322/322/ns3/project/3-final/flowmon-parser/"

# check the files in output folder
for file in `ls $outputfolder`
do
    # check taskcount
    if [[ $file =~ $taskcount ]]
    then
        # check extension
        if [[ $file =~ xml ]]
        then
            # move file to movefolder renamed with extension
            xmlfile=`echo $file | sed 's/.xml/'-$extension.xml'/'`
            mv $outputfolder$file $movefolder$xmlfile
        fi
    fi
done
python3 flowmon-parse-ipv6.py $xmlfile