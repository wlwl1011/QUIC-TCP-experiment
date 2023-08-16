import matplotlib.pyplot as plt
import pandas as pd
import os
import numpy as np
# Initialize an empty DataFrame to store the data
#combined_data = pd.DataFrame()

# Loop through the 40 files
np.set_printoptions(precision=3, suppress=True)  
defualt = '/home/minzzl/networkCongestion/ns-allinone-3.33/ns-3.33/traces'
st = ['delay']
#st = ['delay']
#count_loss = [0,1,2,3,4,5]
#count_delay = [0, 50, 100, 150, 200, 250]
count_node = [1, 10, 20, 30, 40, 50]
protocol = ['QUIC','TCP']
cca = ['bbr', 'cubic','reno','vegas']

congestion_window_arr_QUIC = [ [[[0 for _ in range(1000)] for _ in range(2)] for _ in range(4)]for _ in range(6)]  #20초동안인데 500 줄 적힌 애가 있었다. . . 일단 그렇게 하고 시간, 윈도우 순이다.. 
congestion_window_arr_TCP = [ [[[0 for _ in range(1000)] for _ in range(2)] for _ in range(4)]for _ in range(6)]  #20초동안인데 500 줄 적힌 애가 있었다. . . 일단 그렇게 하고 시간, 윈도우 순이다.. 
avg_cw_QUIC = [[[ [0 for _ in range(20)] for _ in range(2)] for _ in range(4)]  for _ in range(6)]
avg_cw_TCP = [[[ [0 for _ in range(20)] for _ in range(2)] for _ in range(4)]  for _ in range(6)]
for standard in st:
    for count in count_node:
        d_i = count_node.index(count)
        for pro in protocol:
            for cc in cca: 
                cc_i = cca.index(cc)
                for number in range(1,2):
                    initial_port = 49153 #20 20 임 
                    defualt = '/home/minzzl/networkCongestion/ns-allinone-3.33/ns-3.33/traces'
                    if pro == 'QUIC':
                        initial_port = 49153  
                        for i in range(10):
                            file_path = defualt + '/{}/{}/{}/{}/{}/1_10.1.1.1:{}_10.1.3.2:1234_cwnd.txt'.format(standard, count, pro, cc, number, initial_port)
                            if os.path.isfile(file_path): 
                                if os.stat(file_path).st_size == 0:
                                    print(name,'파일이 비어있습니다.')
                                else:    
                                    with open(file_path, 'r') as f:
                                        for line in f:
                                            cols = line.strip().split()
                                            if cols != '' and cols != ' ' and len(cols)==2: 
                                                if float(cols[0]) >=1 :
                                                    
                                                    congestion_window_arr_QUIC[d_i][cc_i].append([float(cols[0]),int(cols[1])]) #시간이고 4
                            else:
                                print(file_path,"파일이 없습니다.")
                            initial_port += 1    

                        initial_port = 49153    
                        for i in range(10,20):
                            file_path = defualt + '/{}/{}/{}/{}/{}/1_10.1.4.2:{}_10.1.5.2:1234_cwnd.txt'.format(standard, count, pro, cc, number, initial_port)
                            if os.path.isfile(file_path): 
                                if os.stat(file_path).st_size == 0:
                                    print(name,'파일이 비어있습니다.')
                                else:    
                                    with open(file_path, 'r') as f:
                                        for line in f:
                                            cols = line.strip().split()
                                            if cols != '' and cols != ' ' and len(cols)==2: 
                                                if float(cols[0]) >=1 :
                                                    congestion_window_arr_QUIC[d_i][cc_i].append([float(cols[0]),int(cols[1])]) #시간이고 4
                                               
                            else:
                                print(file_path,"파일이 없습니다.")   
                            initial_port += 1   
                        #20개 파일을 일단 다 받아왔다! 그럼 시간 별로 평균을 구해보자..
                        # Find the maximum time point across all files
                        max_time = 20
                        sum_cw = np.zeros(int(max_time) + 1)
                        count_cw = np.zeros(int(max_time) + 1)
                        for item in congestion_window_arr_QUIC[d_i][cc_i]:
                            #print(item)
                            time = int(item[0])
                            cw = int(item[1])
                            if time != 0 and time <= 20:
                                sum_cw[time] += cw
                                count_cw[time] += 1
                        avg_cw_QUIC[d_i][cc_i]= np.divide(sum_cw, count_cw, where=count_cw != 0)

                    else:
                        #TCP
                        initial_port = 49153  
                        for i in range(20,30):
                            file_path = defualt + '/{}/{}/{}/{}/{}/10.1.1.1_{}_10.1.3.2_5000_cwnd.txt'.format(standard, count, pro, cc, number, initial_port)
                            if os.path.isfile(file_path): 
                                if os.stat(file_path).st_size == 0:
                                    print(name,'파일이 비어있습니다.')
                                else:    
                                    with open(file_path, 'r') as f:
                                        for line in f:
                                            cols = line.strip().split()
                                            if cols != '' and cols != ' ' and len(cols)==2: 
                                                if float(cols[0]) >=1 :
                                                    congestion_window_arr_TCP[d_i][cc_i].append([float(cols[0]),int(cols[1])])
                            else:
                                print(file_path,"파일이 없습니다.")     
                            initial_port += 1            
       
                        initial_port =  49153     
                        for i in range(30,40):
                            file_path = defualt + '/{}/{}/{}/{}/{}/10.1.4.2_{}_10.1.5.2_5000_cwnd.txt'.format(standard, count, pro, cc, number, initial_port)
                            if os.path.isfile(file_path): 
                                if os.stat(file_path).st_size == 0:
                                    print(name,'파일이 비어있습니다.')
                                else:    
                                    with open(file_path, 'r') as f:
                                        for line in f:
                                            cols = line.strip().split()
                                            if cols != '' and cols != ' ' and len(cols)==2: 
                                                if float(cols[0]) >=1 :
                                                    congestion_window_arr_TCP[d_i][cc_i].append([float(cols[0]),int(cols[1])])
                            else:
                                print(file_path,"파일이 없습니다.")
                            initial_port += 1     
                        max_time = 20
                        sum_cw = np.zeros(int(max_time) + 1)
                        count_cw = np.zeros(int(max_time) + 1)
                        for item in congestion_window_arr_TCP[d_i][cc_i]:
                            #print(item)
                            time = int(item[0]//10)
                            cw = int(item[1])
                            if time != 0 and time <= 20:
                                while True:
                                    if cw > 1000:
                                        cw = cw//10
                                    else:
                                        break 
                                if cw < 1000:  
                                         
                                    sum_cw[time] += cw
                                    count_cw[time] += 1
                         
                        avg_cw_TCP[d_i][cc_i]= np.divide(sum_cw, count_cw, where=count_cw != 0)

# print('QUIC')
# for i in range(0,6):
#     for j in range(4):
#         print(i,cca[j])
#         print(len(avg_cw_QUIC[i][j]))
#         print(avg_cw_QUIC[i][j])

#print('TCP')
for i in range(0,6):
    for j in range(4):
         if np.any(avg_cw_TCP[i][j] > 1000):
            while True:
                #print(avg_cw_TCP[i][j])
                avg_cw_TCP[i][j] = avg_cw_TCP[i][j] // 10
                if  np.any(avg_cw_TCP[i][j] < 1000):
                    break
            
# Define the parameters
error_rates = [1, 10, 20, 30, 40, 50]
congestion_algorithms = ['bbr', 'cubic','reno','vegas']
protocols = ['QUIC', 'TCP']
  # Assuming you have data for 20 seconds

# Create a single figure and axis for the plot
plt.figure(figsize=(10, 6))
plt.title('Congestion Window')
plt.xlabel('Time (s)')
plt.ylabel('Congestion Window (cwnd)')
time_points = np.arange(21)
# Loop through all combinations and plot the data
for error_rate in error_rates:
    d_i = error_rates.index(error_rate)
    for cc_algorithm in congestion_algorithms:
        for protocol in protocols:
            # Determine the correct array to use based on the protocol and error rate
            label = f'{protocol}_node_{error_rate}_{cc_algorithm}'
            if protocol == 'QUIC':
                data = avg_cw_QUIC[d_i][congestion_algorithms.index(cc_algorithm)]
                plt.plot(time_points, data, label=label, marker='o')
                #print(data)
            else:
                #print('kdjdjd')
                #print(error_rate, cc_algorithm)
                data = avg_cw_TCP[d_i][congestion_algorithms.index(cc_algorithm)]
                plt.plot(time_points, data, label=label, marker='*')
                #print(data)
            
            # Create a label for the legend
            #label = f'{protocol}_loss_{error_rate}_{cc_algorithm}'
            
            # Plot the data
            #plt.plot(time_points, data, label=label, marker='o')

# Customize the plot appearance
plt.xticks(time_points)
plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
plt.grid(True)

# Save the plot as an image file (adjust the filename as needed)
filename = 'combined_cwnd_plot_node.png'
plt.savefig(filename, dpi=300, bbox_inches='tight')

# Display the plot
plt.show()
