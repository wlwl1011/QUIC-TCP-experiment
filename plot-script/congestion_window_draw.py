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
                                                    
                                                    congestion_window_arr_QUIC[d_i][cc_i].append([float(cols[0]),int(cols[1])]) #시간이고 4
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
                                                    
                                                    congestion_window_arr_TCP[d_i][cc_i].append([float(cols[0]),int(cols[1])])
                            else:
                                print(file_path,"파일이 없습니다.")     
                        # if cc == "bbr":
                            #print(d_i)
                            #print(congestion_window_arr_TCP[d_i][cc_i])                
                           
       
                        
# Continue from your code
output_directory = "./plots/cwnd/"
if not os.path.exists(output_directory):
    os.makedirs(output_directory)

def plot_graph_for_cca(data, protocol, cca_name):
    plt.figure(figsize=(10, 6))
    for loss_rate in count_loss:
        cc_i = cca.index(cca_name)
        d_i = count_loss.index(loss_rate)

        # Debug prints to check the data
        #print(f"Debugging Information for {protocol} {cca_name} with loss rate = {loss_rate}:")
        #print(f"data[d_i][cc_i] max = {max(data[d_i][cc_i])}")
        
        # Check for empty lists or invalid data
        # if len(data[d_i][cc_i]) == 0:
        #     print(f"No data available for {protocol} {cca_name} with loss rate = {loss_rate}")
        #     continue

        # Extract times and cwnd_values
        times = [item[0] for item in data[d_i][cc_i] if len(item) > 1]
        cwnd_values = [item[1] for item in data[d_i][cc_i] if len(item) > 1]
        
        # if protocol == 'TCP' and cca[cc_i] == 'vegas' and loss_rate == 1:
        #     print(loss_rate)
        #     print(times)
        #     print(cwnd_values)
     
        plt.plot(times, cwnd_values, label=f'Loss Rate = {loss_rate}')
    
    # Plotting details
    plt.title(f'{protocol} {cca_name} Congestion Window Over Time')
    plt.xlabel('Time (seconds)')
    plt.ylabel('cwnd (packets)')
    plt.legend()
    plt.grid(True)
    plt.autoscale(axis='y', tight=True)
    plt.tight_layout()

    # Save and show plot
    file_name = f'{protocol}_{cca_name}_loss_cwnd.png'
    #file_name = f'bbr.png'
    plt.savefig(f"{output_directory}/{file_name}")
    plt.show()
# Plotting the graphs
for cc in cca:
    plot_graph_for_cca(congestion_window_arr_QUIC, 'QUIC', cc)
    plot_graph_for_cca(congestion_window_arr_TCP, 'TCP', cc)
# plot_graph_for_cca(congestion_window_arr_TCP, 'TCP', 'bbr')    
