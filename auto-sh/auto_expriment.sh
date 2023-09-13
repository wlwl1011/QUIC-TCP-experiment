
#QUIC - loss
for count in 0 1 2 3 4 5
do
    for cca in cubic bbr reno vegas
    do
        for trial in 1 2 3 4 5
        do
            ./waf --run "scratch/quic_main --topo=dumbbell --lo=${count} --cc1=${cca} --de=100 --i=10 --folder=loss/${count}/QUIC/${cca}/${trial}" | tee loss_${count}_QUIC_${cca}_${trial}.out
            cd /home/minzzl/networkCongestion/ns-allinone-3.33/ns-3.33/traces/loss/${count}/QUIC/${cca}/${trial}
            chmod 777 auto_cat_goodput.sh
            chmod 777 auto_cat_owd.sh
            chmod 777 auto_cat_inflight.sh
            chmod 777 auto_cat_lost.sh
            chmod 777 auto_cat_cwnd.sh
            ./auto_cat_goodput.sh | tee loss_${count}_QUIC_${cca}_${trial}_goodput.out
            ./auto_cat_owd.sh | tee loss_${count}_QUIC_${cca}_${trial}_owd.out
            ./auto_cat_inflight.sh | tee loss_${count}_QUIC_${cca}_${trial}_inflight.out
            ./auto_cat_lost.sh | tee loss_${count}_QUIC_${cca}_${trial}_lost.out
            ./auto_cat_cwnd.sh | tee loss_${count}_QUIC_${cca}_${trial}_cwnd.out
            cd ../../../../../../
        done
    done
done  


#TCP - loss
for count in 2 3 4 5
do
    for cca in cubic bbr reno vegas
    do
        for trial in 1 2 3 4 5 
        do
    

            ./waf --run "scratch/tcp-dumpbell --lo=${count} --cc1=${cca} --de=100 --i=10 --folder=loss/${count}/TCP/${cca}/${trial}" | tee loss_${count}_TCP_${cca}_${trial}.out
            
            cd /home/minzzl/networkCongestion/ns-allinone-3.33/ns-3.33/traces/loss/${count}/TCP/${cca}/${trial}
            chmod 777 auto_cat_goodput.sh
            chmod 777 auto_cat_owd.sh
            chmod 777 auto_cat_inflight.sh
            chmod 777 auto_cat_lost.sh
            chmod 777 auto_cat_cwnd.sh
            ./auto_cat_goodput.sh | tee loss_${count}_TCP_${cca}_${trial}_goodput.out
            ./auto_cat_owd.sh | tee loss_${count}_TCP_${cca}_${trial}_owd.out
            ./auto_cat_inflight.sh | tee loss_${count}_TCP_${cca}_${trial}_inflight.out
            ./auto_cat_lost.sh | tee loss_${count}_TCP_${cca}_${trial}_lost.out
            ./auto_cat_cwnd.sh | tee loss_${count}_TCP_${cca}_${trial}_cwnd.out
            cd ../../../../../../
        done
    done
done  


# #QUIC - delay
for count in 0 50 100 150 200 250
do
    for cca in cubic bbr reno vegas
    do
        for trial in 1 2 3 4 5
        do
            ./waf --run "scratch/quic_main --topo=dumbbell --i=10 --lo=2 --de=${count} --cc1=${cca} --folder=delay/${count}/QUIC/${cca}/${trial}" | tee delay_${count}_QUIC_${cca}_${trial}.out
            cd /home/minzzl/networkCongestion/ns-allinone-3.33/ns-3.33/traces/delay/${count}/QUIC/${cca}/${trial}
            chmod 777 auto_cat_goodput.sh
            chmod 777 auto_cat_owd.sh
            chmod 777 auto_cat_inflight.sh
            chmod 777 auto_cat_lost.sh
            chmod 777 auto_cat_cwnd.sh
            ./auto_cat_goodput.sh | tee delay_${count}_QUIC_${cca}_${trial}_goodput.out
            ./auto_cat_owd.sh | tee delay_${count}_QUIC_${cca}_${trial}_owd.out
            ./auto_cat_inflight.sh | tee delay_${count}_QUIC_${cca}_${trial}_inflight.out
            ./auto_cat_lost.sh | tee delay_${count}_QUIC_${cca}_${trial}_lost.out
            ./auto_cat_cwnd.sh | tee delay_${count}_QUIC_${cca}_${trial}_cwnd.out
            cd ../../../../../../
        done
    done
done  


#TCP - delay
for count in 100 150 200 250
do
    for cca in cubic bbr reno vegas
    do
        for trial in 1 2 3 4 5
        do
            ./waf --run "scratch/tcp-dumpbell --de=${count} --lo=2 --cc1=${cca} --i=10 --folder=delay/${count}/TCP/${cca}/${trial}" | tee delay_${count}_TCP_${cca}_${trial}.out
            cd /home/minzzl/networkCongestion/ns-allinone-3.33/ns-3.33/traces/delay/${count}/TCP/${cca}/${trial}
            chmod 777 auto_cat_goodput.sh 
            chmod 777 auto_cat_owd.sh
            chmod 777 auto_cat_inflight.sh
            chmod 777 auto_cat_lost.sh
            chmod 777 auto_cat_cwnd.sh
            ./auto_cat_goodput.sh | tee delay_${count}_TCP_${cca}_${trial}_goodput.out
            ./auto_cat_owd.sh | tee delay_${count}_TCP_${cca}_${trial}_owd.out
            ./auto_cat_inflight.sh | tee delay_${count}_TCP_${cca}_${trial}_inflight.out
            ./auto_cat_lost.sh | tee delay_${count}_TCP_${cca}_${trial}_lost.out
            ./auto_cat_cwnd.sh | tee delay_${count}_TCP_${cca}_${trial}_cwnd.out
            cd ../../../../../../
        done
    done
done  


#QUIC - client number
for count in 1 10 20 30 40 50 60 70 80 90 100
do
    for cca in cubic bbr reno vegas
    do
        for trial in 1 2 3 4 5
        do
            ./waf --run "scratch/quic_main --topo=dumbbell --de=100 --lo=2 --i=${count} --cc1=${cca} --folder=node/${count}/QUIC/${cca}/${trial}" | tee node_${count}_QUIC_${cca}_${trial}.out
            cd /home/minzzl/networkCongestion/ns-allinone-3.33/ns-3.33/traces/node/${count}/QUIC/${cca}/${trial}
            chmod 777 auto_cat_goodput.sh
            chmod 777 auto_cat_owd.sh
            chmod 777 auto_cat_inflight.sh
            chmod 777 auto_cat_lost.sh
            chmod 777 auto_cat_cwnd.sh
            ./auto_cat_goodput.sh | tee node_${count}_QUIC_${cca}_${trial}_goodput.out
            ./auto_cat_owd.sh | tee node_${count}_QUIC_${cca}_${trial}_owd.out
            ./auto_cat_inflight.sh | tee node_${count}_QUIC_${cca}_${trial}_inflight.out
            ./auto_cat_lost.sh | tee node_${count}_QUIC_${cca}_${trial}_lost.out
            ./auto_cat_cwnd.sh | tee node_${count}_QUIC_${cca}_${trial}_cwnd.out
            cd ../../../../../../ 
        done
    done
done  




#TCP - client number
for count in 1 10 20 30 40 50 60 70 80 90 100
do
    for cca in cubic bbr reno vegas
    do
        for trial in 1 2 3 4 5
        do
            ./waf --run "scratch/tcp-dumpbell --i=${count} --cc1=${cca} --de=100 --lo=2 --folder=node/${count}/TCP/${cca}/${trial}" | tee node_${count}_TCP_${cca}_${trial}.out
            cd /home/minzzl/networkCongestion/ns-allinone-3.33/ns-3.33/traces/node/${count}/TCP/${cca}/${trial}
            chmod 777 auto_cat_goodput.sh
            chmod 777 auto_cat_owd.sh
            chmod 777 auto_cat_inflight.sh
            chmod 777 auto_cat_lost.sh
            chmod 777 auto_cat_cwnd.sh
            ./auto_cat_goodput.sh | tee node_${count}_TCP_${cca}_${trial}_goodput.out
            ./auto_cat_owd.sh | tee node_${count}_TCP_${cca}_${trial}_owd.out
            ./auto_cat_inflight.sh | tee node_${count}_TCP_${cca}_${trial}_inflight.out
            ./auto_cat_lost.sh | tee node_${count}_TCP_${cca}_${trial}_lost.out
            ./auto_cat_cwnd.sh | tee node_${count}_TCP_${cca}_${trial}_cwnd.out
            cd ../../../../../../
        done
    done
done  
