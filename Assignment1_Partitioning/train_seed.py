import subprocess
import re

best_cut_num = 10000
seed = 0
for i in range(1, 10):
    run_command = "./Fiduccia_Mattheyses 10000.txt 20 " + str(i) + " 0"
    result = subprocess.run(run_command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
    if(result.returncode == 0):
        pattern = r'Best cut num\s*:\s*(\d+)'
        cut_num = re.search(pattern, result.stdout.decode("utf-8")).group(1)
        cut_num = int(cut_num)
        if cut_num < best_cut_num:
            seed = i
            best_cut_num = cut_num
            print(seed)
            print(best_cut_num)
            
# print(seed)
            
