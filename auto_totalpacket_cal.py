import pandas as pd
import glob
import os
import openpyxl
from xlsxwriter import Workbook


st = ['loss']
count_loss = [0, 1, 2, 3, 4, 5]
protocol = ['QUIC', 'TCP']
cca = ['bbr']
txt_name_list = {}



for standard in st:
    for count in count_loss:
        for pro in protocol:
            for cc in cca:
                name = '{}_{}_{}_{}'.format( pro,standard,  cc, count)
                txt_name_list[name] = [] 


for standard in st:
    for count in count_loss:
        for pro in protocol:
            for cc in cca:
                for number in range(1,6):
                    defualt = '/home/minzzl/networkCongestion/ns-allinone-3.33/ns-3.33'
                    file_path = defualt + '/{}_{}_{}_{}_{}.out'.format(pro, standard, cc, count, number)
                    name = '{}_{}_{}_{}'.format(pro, standard, cc, count)
                    if os.path.isfile(file_path): 
                        if os.stat(file_path).st_size == 0:
                            print(name,'파일이 비어있습니다.')
                        else:   
                            with open(file_path, 'r') as f:
                                temp = []
                                for line in f:
                                    cols = line.strip().split()
                                    if cols != '' and cols != ' ' and len(cols)==1: 
                                        temp.append(cols[0])
                                if len(temp) == 0:
                                        print(file_path,'파일 데이터를 저장한 리스트가 비었습니다.')    
                                else:
                                    data = [int(x.strip()) for x in temp[14:]]
                                    #print(name,"length",len(temp))
                                    total_packet = max(data) 
                                    txt_name_list[name].append(total_packet)
                    else:
                            print(file_path,"파일이 없습니다.")

                                     


# 그전에 Min Max 를 기록 하고,! 그 들을 평균도 있어야합니당

for key, values in txt_name_list.items():
    
    min_value = 0
    max_value = 0
    avg_value = 0

    min_value = min(values)
    max_value = max(values)
    avg_value = sum(values) / len(values)
    
    txt_name_list[key].append(min_value)
    txt_name_list[key].append(max_value)
    txt_name_list[key].append(avg_value)
    print(key, values, end="\n")


#loss, delay, node 가지 파일로 ! 각각을 나누겠습니다람쥐
# 그전에 Min Max 를 기록 하고,! 그 들을 평균도 있어야합니당

header_list=["name", "1", "2", "3","4","5","min","max","average"]

wb = Workbook("bbr_total_packet_amout.xlsx")
ws1 = wb.add_worksheet("packet_amout")


row_loss = 1


for header in header_list:
    col=header_list.index(header)
    ws1.write(0,col,header)
    


for name, txt_list in txt_name_list.items():
    if name.find("TCP") != -1:
        
        ws1.write(row_loss, 0, name)
        for item in txt_list:
            col=txt_list.index(item)
            col += 1
            ws1.write(row_loss, col, item)
        row_loss += 1    
        
for name, txt_list in txt_name_list.items():
    if name.find("QUIC") != -1:
        
        ws1.write(row_loss, 0, name)
        for item in txt_list:
            col=txt_list.index(item)
            col += 1
            ws1.write(row_loss, col, item)
        row_loss += 1    
        





wb.close()
   


