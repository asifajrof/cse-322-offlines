echo "waf run"
cd "/mnt/e/Study/Level-3/L3T2/CSE321-322/322/github_backup/cse-322-offlines/ns3/ns-allinone-3.35/ns-3.35"
./waf --run scratch/task-a-wireless-low-static
echo "parsing now"
cd "/mnt/e/Study/Level-3/L3T2/CSE321-322/322/ns3/project/3-final/flowmon-parser"
./move.sh task-a-2 txrange-6
echo "done"