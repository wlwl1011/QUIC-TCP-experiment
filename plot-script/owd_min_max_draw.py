import matplotlib.pyplot as plt
import numpy as np
import os

# 1. Set the default style
plt.style.use('default')
plt.rcParams['figure.figsize'] = (12, 8)  # Adjusted for a larger graph
plt.rcParams['font.size'] = 12

# 2. Prepare data
np.random.seed(0)
np.set_printoptions(precision=3, suppress=True)
defualt = '/home/minzzl/networkCongestion/ns-allinone-3.33/ns-3.33/traces'
st = ['loss']
count_loss = [0, 1, 2, 3, 4, 5]
protocol = ['QUIC']
cca = ['bbr', 'cubic', 'reno', 'vegas']

owd_arr_QUIC = [ [[[0 for _ in range(10000)] for _ in range(5)] for _ in range(4)] for _ in range(6)]
owd_arr_TCP = [ [[[0 for _ in range(10000)] for _ in range(5)] for _ in range(4)] for _ in range(6)]

min_owd_QUIC = np.zeros((4, 6))
max_owd_QUIC = np.zeros((4, 6))

data_points_QUIC = {algorithm: [] for algorithm in cca}

for standard in st:
    for count in count_loss:
        d_i = count_loss.index(count)
        for pro in protocol:
            for cc in cca:
                cc_i = cca.index(cc)
                mins, maxs = [], []  # Temporary storage for the 5 min and max values
                
                for number in range(1, 5 + 1):  # 5번 실험한다
                    data = []
                    initial_port = 49153  # 10 임
                    initial_server_port = 1234
                    defualt = '/home/minzzl/networkCongestion/ns-allinone-3.33/ns-3.33/traces'
                    if pro == 'QUIC':
                        initial_port = 49153
                        initial_server_port = 1234
                        for i in range(10):
                            file_path = defualt + '/{}/{}/{}/{}/{}/1_10.1.1.1:{}_10.1.3.2:{}_owd.txt'.format(standard, count, pro, cc, number, initial_port, initial_server_port)
                            if os.path.isfile(file_path):
                                if os.stat(file_path).st_size == 0:
                                    print(name, '파일이 비어있습니다.')
                                else:
                                    with open(file_path, 'r') as f:
                                        for line in f:
                                            cols = line.strip().split()
                                            if cols and len(cols) == 4:
                                                data.append(int(cols[2]))  # 시간이고 4
                            else:
                                print(file_path, "파일이 없습니다.")
                            initial_port += 1
                            initial_server_port += 1
                             
                    if data:
                        #print(data)
                        mins.append(min(data))
                        maxs.append(max(data))
                    else:
                        print('oh no! There isnt data!',d_i, cc,number)    
                    
                    # else:
                    #     # TCP
                    #     initial_port = 49153
                    #     initial_server_port = 1234
                    #     for i in range(20, 30):
                    #         file_path = defualt + '/{}/{}/{}/{}/{}/10.1.1.1_{}_10.1.3.2_{}_rtt.txt'.format(standard, count, pro, cc, number, initial_port, initial_server_port)
                    #         if os.path.isfile(file_path):
                    #             if os.stat(file_path).st_size == 0:
                    #                 print(name, '파일이 비어있습니다.')
                    #             else:
                    #                 with open(file_path, 'r') as f:
                    #                     for line in f:
                    #                         cols = line.strip().split()
                    #                         if cols and len(cols) == 2:
                    #                             data.append(int(cols[1]))
                    #         else:
                    #             print(file_path, "파일이 없습니다.")
                    #         initial_port += 1
                    #         initial_server_port += 1
                 
                 # Storing the average of the 5 min values and 5 max values        
                if pro == 'QUIC':
                    min_owd_QUIC[cc_i][d_i] = sum(mins) / len(mins) if mins else 0
                    max_owd_QUIC[cc_i][d_i] = sum(maxs) / len(maxs) if maxs else 0   
                    # print(cc,d_i,min_owd_QUIC[cc_i][d_i])     
                    # print(cc,d_i,max_owd_QUIC[cc_i][d_i])    
                        

# 3. Draw a graph representing the range of OWDs for all congestion control algorithms according to the error rate
fig, ax = plt.subplots(figsize=(12, 8))

# We will set up data for each congestion control algorithm
box_data_by_cc = {algorithm: [] for algorithm in cca}

for cc_idx, cc_label in enumerate(cca):
    for error_rate_idx in range(len(count_loss)):
        min_val = min_owd_QUIC[cc_idx][error_rate_idx]
        max_val = max_owd_QUIC[cc_idx][error_rate_idx]
        box_data_by_cc[cc_label].append([min_val, max_val])

# We will plot each congestion control algorithm's data side-by-side for each error rate
positions = []
all_data = []

for idx, error_rate in enumerate(count_loss):
    current_data_group = []
    for cc_idx, cc_label in enumerate(cca):
        box_data = box_data_by_cc[cc_label][idx]
        current_data_group.append(box_data)
    all_data.append(current_data_group)

# Define colors
colors = {
    'bbr': 'pink',
    'cubic': 'gray',
    'vegas': 'brown',
    'reno': 'skyblue'
}

# Define positions for each box
group_width = len(cca)
positions = [i * (group_width + 1) + j for i in range(len(count_loss)) for j in range(group_width)]
bp = ax.boxplot([item for sublist in box_data_by_cc.values() for item in sublist], positions=positions, patch_artist=True)

# Set colors
for i, box in enumerate(bp['boxes']):
    cc_label = cca[i % len(cca)]
    box.set_facecolor(colors[cc_label])

# Adjust x-ticks' positions to be in the center of each group of four boxes
ax.set_xticks([(group_width + 1) * i + group_width / 2 for i in range(len(count_loss))])
ax.set_xticklabels(count_loss)
ax.set_xlabel('Loss Count')
ax.set_ylabel('One-Way Delay')
ax.set_title('OWD Range for Each Congestion Control Algorithm in QUIC Grouped by Loss Count')

# Legend based on colors
from matplotlib.patches import Patch
legend_elements = [Patch(facecolor=colors[cc], label=cc) for cc in cca]
ax.legend(handles=legend_elements, loc='upper left', bbox_to_anchor=(1, 1))


ax.grid(axis='y', linestyle='--', linewidth=0.7, alpha=0.6)

# Saving the plot to the designated output directory
output_directory = "./plots/"
if not os.path.exists(output_directory):
    os.makedirs(output_directory)
plt.savefig(f"{output_directory}/all_cc_QUIC_owd_range_boxplot_colored.png", dpi=300)
plt.show()
