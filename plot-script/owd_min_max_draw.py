import matplotlib.pyplot as plt
import numpy as np
import os

# 1. Set default style
plt.style.use('default')
plt.rcParams['figure.figsize'] = (10, 6)
plt.rcParams['font.size'] = 12

# 2. Prepare data
np.random.seed(0)
np.set_printoptions(precision=3, suppress=True)
defualt = '/home/minzzl/networkCongestion/ns-allinone-3.33/ns-3.33/traces'
st = ['loss']
count_loss = [0, 1, 2, 3, 4, 5]
protocol = ['QUIC', 'TCP']
cca = ['bbr', 'cubic', 'reno', 'vegas']

owd_arr_QUIC = [ [[[0 for _ in range(10000)] for _ in range(5)] for _ in range(4)] for _ in range(6)]
owd_arr_TCP = [ [[[0 for _ in range(10000)] for _ in range(5)] for _ in range(4)] for _ in range(6)]

avg_owd_QUIC = np.zeros((4, 6))  # 4 algorithms x 6 loss rates
avg_owd_TCP = np.zeros((4, 6))  # 4 algorithms x 6 loss rates

for standard in st:
    for count in count_loss:
        d_i = count_loss.index(count)
        for pro in protocol:
            for cc in cca:
                cc_i = cca.index(cc)
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

                    else:
                        # TCP
                        initial_port = 49153
                        initial_server_port = 1234
                        for i in range(20, 30):
                            file_path = defualt + '/{}/{}/{}/{}/{}/10.1.1.1_{}_10.1.3.2_{}_rtt.txt'.format(standard, count, pro, cc, number, initial_port, initial_server_port)
                            if os.path.isfile(file_path):
                                if os.stat(file_path).st_size == 0:
                                    print(name, '파일이 비어있습니다.')
                                else:
                                    with open(file_path, 'r') as f:
                                        for line in f:
                                            cols = line.strip().split()
                                            if cols and len(cols) == 2:
                                                data.append(int(cols[1]))
                            else:
                                print(file_path, "파일이 없습니다.")
                            initial_port += 1
                            initial_server_port += 1

                    avg_delay = np.mean(data)
                    if pro == 'QUIC':
                        avg_owd_QUIC[cc_i][d_i] = avg_delay
                    else:
                        avg_owd_TCP[cc_i][d_i] = avg_delay

# 3. Plot average one-way delay for QUIC and TCP together
for cc_idx, cc_label in enumerate(cca):
    plt.figure(figsize=(10, 6))
    plt.plot(count_loss, avg_owd_QUIC[cc_idx], marker='o', label='QUIC')
    plt.plot(count_loss, avg_owd_TCP[cc_idx], marker='x', label='TCP')
    plt.xlabel('Loss Rate')
    plt.ylabel('Average One-Way Delay')
    plt.title(f'Average One-Way Delay for {cc_label}')
    plt.xticks(count_loss)
    plt.legend()
    plt.grid()
    plt.show()
