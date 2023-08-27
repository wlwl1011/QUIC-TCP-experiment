import matplotlib.pyplot as plt
import pandas as pd
import os
import numpy as np
# Initialize an empty DataFrame to store the data
#combined_data = pd.DataFrame()

# Loop through the 40 files
np.set_printoptions(precision=3, suppress=True)  
defualt = '/home/minzzl/networkCongestion/ns-allinone-3.33/ns-3.33/traces'
st = ['loss']
#st = ['delay']
count_loss = [0,1,2,3,4,5]
#count_delay = [0, 50, 100, 150, 200, 250]
#count_node = [1, 10, 20, 30, 40, 50]
protocol = ['QUIC','TCP']
cca = ['bbr', 'cubic','reno','vegas']

congestion_window_arr_QUIC = [ [[[] ] for _ in range(4)]for _ in range(6)]  #20초동안인데 500 줄 적힌 애가 있었다. . . 일단 그렇게 하고 시간, 윈도우 순이다.. 
congestion_window_arr_TCP = [ [[[] ] for _ in range(4)]for _ in range(6)]  #20초동안인데 500 줄 적힌 애가 있었다. . . 일단 그렇게 하고 시간, 윈도우 순이다.. 
avg_cw_QUIC = [[[ [0 for _ in range(20)] for _ in range(2)] for _ in range(4)]  for _ in range(6)]
avg_cw_TCP = [[[ [0 for _ in range(20)] for _ in range(2)] for _ in range(4)]  for _ in range(6)]
for standard in st:
    for count in count_loss:
        d_i = count_loss.index(count)
        for pro in protocol:
            for cc in cca: 
                cc_i = cca.index(cc)
                for number in range(1,2):
                    initial_port = 49153 #20 20 임 
                    defualt = '/home/minzzl/networkCongestion/ns-allinone-3.33/ns-3.33/traces'
                    if pro == 'QUIC':
                        initial_port = 49153
                        initial_server_port = 1234
                        for i in range(1):
                            file_path = defualt + '/{}/{}/{}/{}/{}/1_10.1.1.1:{}_10.1.3.2:{}_cwnd.txt'.format(standard, count, pro, cc, number, initial_port,initial_server_port)
                            if os.path.isfile(file_path): 
                                if os.stat(file_path).st_size == 0:
                                    print(name,'파일이 비어있습니다.')
                                else:    
                                    with open(file_path, 'r') as f:
                                        for line in f:
                                            cols = line.strip().split()
                                            if cols != '' and cols != ' ' and len(cols)==2: 
                                                if int(cols[1]) < 1000 :
                                                    
                                                    congestion_window_arr_QUIC[d_i][cc_i][0].append([float(cols[0]), int(cols[1])]) #시간이고 4
                            else:
                                print(file_path,"파일이 없습니다.")
                        
                    else:
                        #TCP
                        initial_port = 49153  
                        initial_server_port = 5000
                        for i in range(1):
                            file_path = defualt + '/{}/{}/{}/{}/{}/10.1.1.1_{}_10.1.3.2_{}_cwnd.txt'.format(standard, count, pro, cc, number, initial_port,initial_server_port)
                            if os.path.isfile(file_path): 
                                if os.stat(file_path).st_size == 0:
                                    print(name,'파일이 비어있습니다.')
                                else:    
                                    with open(file_path, 'r') as f:
                                        for line in f:
                                            cols = line.strip().split()
                                            if cols != '' and cols != ' ' and len(cols)==2: 
                                                if int(cols[1]) < 1000 :
                                                    # if cc == 'bbr':
                                                    #     print(cols[1])
                                                    #if float(cols[0]) <20:
                                                    congestion_window_arr_TCP[d_i][cc_i][0].append([float(cols[0]), int(cols[1])])

                            else:
                                print(file_path,"파일이 없습니다.")     
                        # if cc == "bbr":
                            #print(d_i)
                            #print(congestion_window_arr_TCP[d_i][cc_i])                
                           
       
                        
# Continue from your code
output_directory = "./plots/cwnd/"
if not os.path.exists(output_directory):
    os.makedirs(output_directory) 

# Define colors for QUIC and TCP algorithms
quic_colors = {
    'bbr': 'pink',
    'cubic': 'gray',
    'reno': 'skyblue',
    'vegas': '#ba52a1'
}

tcp_colors = {
    'bbr': '#e85d5d',
    'cubic': '#424242',
    'reno': '#4255a6',
    'vegas': '#961277'
}

# Function to plot data
def plot_graph(data_QUIC, data_TCP, loss_rate):
    # Plotting for QUIC
    plt.figure(figsize=(10, 6))
    loss_idx = count_loss.index(loss_rate)
    for cc_algorithm in cca:
        cc_idx = cca.index(cc_algorithm)
        
        if len(data_QUIC[loss_idx][cc_idx][0]) > 0:
            times = [item[0] for item in data_QUIC[loss_idx][cc_idx][0]]
            cwnd_values = [item[1] for item in data_QUIC[loss_idx][cc_idx][0]]
            plt.plot(times, cwnd_values, label=f"QUIC {cc_algorithm}", color=quic_colors[cc_algorithm])
        
    plt.title(f"QUIC - Congestion Window Over Time (Loss Rate = {loss_rate})")
    plt.xlabel("Time (seconds)")
    plt.ylabel("cwnd (packets)")
    plt.legend()
    plt.grid(True)
    plt.autoscale(axis='y', tight=True)
    plt.tight_layout()
    plt.savefig(f"{output_directory}/QUIC_loss_rate_{loss_rate}_cwnd.png")
    plt.show()

    # Plotting for TCP
    plt.figure(figsize=(10, 6))
    for cc_algorithm in cca:
        cc_idx = cca.index(cc_algorithm)
        
        if len(data_TCP[loss_idx][cc_idx][0]) > 0:
            times = [item[0] for item in data_TCP[loss_idx][cc_idx][0]]
            cwnd_values = [item[1] for item in data_TCP[loss_idx][cc_idx][0]]
            plt.plot(times, cwnd_values, label=f"TCP {cc_algorithm}", color=tcp_colors[cc_algorithm])

    plt.title(f"TCP - Congestion Window Over Time (Loss Rate = {loss_rate})")
    plt.xlabel("Time (seconds)")
    plt.ylabel("cwnd (packets)")
    plt.legend()
    plt.grid(True)
    plt.autoscale(axis='y', tight=True)
    plt.tight_layout()
    plt.savefig(f"{output_directory}/TCP_loss_rate_{loss_rate}_cwnd.png")
    plt.show()

# Plot graphs for each loss rate
for loss_rate in count_loss:
    plot_graph(congestion_window_arr_QUIC, congestion_window_arr_TCP, loss_rate)