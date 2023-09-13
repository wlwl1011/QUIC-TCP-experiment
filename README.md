# QUIC-TCP-experiment

## 1. 소개

TCP 및 QUIC의 혼잡제어 알고리즘 별 성능 분석도구 (NS3.33 버전)

<br>

## 2. 사용 방법

## 전제 조건

```
apt-get install cmake build-essential protobuf-compiler libprotobuf-dev golang-go libunwind-dev libicu-dev
```

## 빌드

Download quiche library first. And I unzip the quiche.zip under "/home/xxx/"

```
cd quiche
mkdir build && cd build
cmake ..
make
```

## 인증서 생성

```
cd quiche/util
chmod 777 generate-certs.sh
./generate-certs.sh
mkdir -p data/quic-cert
mv ./out/*  data/quic-cert/


```

# ns3에 quic 모듈 구축

ns3.33 환경에 적합합니다.

## 환경 변수 추가

```
sudo gedit /etc/profile
export QUICHE_SRC_DIR=/home/xxx/quiche/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$QUICHE_SRC_DIR/build/
```

## ns3 빌드

1 c++17 flag (ns3.33/wscript)

```
opt.add_option('--cxx-standard',
               help=('Compile NS-3 with the given C++ standard'),
               type='string', default='-std=c++17', dest='cxx_standard')
```

2 quic folder to ns3.33/src/

```
cd ns3.33
source /etc/profile
./waf configure
./waf build
```

# 실행 예제

1 ns3.33 다운로드 후, scratch 파일 아래에 quic 모듈 및 tcp 모듈 삽입.  
2 실행( 예시 - BBR, loss rate 0.01 , client number =10 일 때)

```
source /etc/profile
./waf --run "scratch/quic_topology --cc1=bbr --lo=1 --i=10"
```

3 auto-sh 안의 auto_experiment 파일을 이용하여 여러 변수를 적용한 실험 자동화 가능.

4 데이터 값에 따른 표 생성시 plot-script 활용
