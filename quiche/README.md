# quiche

quiche는 BVC(빌리빌리 비디오 클라우드 팀)에서 진행 중인 QUIC 구현 작업입니다. 구글 퀴시(https://quiche.googlesource.com/quiche/)를 기반으로 합니다. BVC는 이를 사용하여 프로덕션 환경에서 gQUIC 및 iQUIC 서비스 기능을 사용할 수 있으며, 예를 들어 quic 프록시 서버와 nginx quic 모듈이 있습니다.

QUIC(빠른 UDP 인터넷 연결)은 TCP에 비해 지연 시간을 줄여주는 새로운 전송 방식입니다. QUIC을 10000피트 상공에서 보면, UDP에서 구현된 TCP+TLS+HTTP/2와 매우 유사합니다. TCP는 운영 체제 커널과 미들박스 펌웨어에 구현되어 있기 때문에 TCP를 크게 변경하는 것은 거의 불가능에 가깝습니다. 하지만 QUIC은 UDP를 기반으로 구축되었기 때문에 이러한 제약이 없습니다.

기존 TCP+TLS+HTTP2에 비해 QUIC+HTTP3의 주요 특징은 다음과 같습니다.
- 사용자 공간 구현
- 획기적으로 단축된 연결 설정 시간
- 향상된 혼잡 제어
- 헤드 오브 라인 차단 없는 멀티플렉싱
- 연결 마이그레이션

구글 키시는 Chromium(http://www.chromium.org/quic) 프로젝트에서 사용됩니다. 이 리파지토리는 구글 키시를 크롬 플랫폼과는 별개로 일반적으로 사용되는 일부 리파지토리와 통합한 것입니다.
- epoll 서버/클라이언트, http2 스택, quic 스택의 플랫폼 관련 구현
- 구글 키시 소스 파일에 대한 지시어 포함 재작성

### 특징
- cmake를 이용한 쉬운 빌드
- 리눅스 플랫폼만 지원
- 구글 키쉬 업그레이드에 쉽게 따라갈 수 있습니다.

### 소스 레이아웃
- 베이스`: 기본 플랫폼 기능 구현
- 구글URL`: 구글URL 소스 파일
- `gquiche`: 구글 키쉬 소스 파일 
- net`: 플랫폼 넷 관련 함수 구현
- 플랫폼/에폴_플랫폼_임플`: epoll 클라이언트 및 서버 함수 구현
- platform/http2_platform_impl`: http2 스택 함수 구현
- platform/quic_platform_impl`: quic 스택 함수 구현
- 플랫폼/퀴시_플랫폼_임플란트`: 구글 키시 플랫폼 함수 구현
- 플랫폼/spdy_플랫폼_임플`: spdy 스택 함수 구현
- `third_party`: 서드파티 리포지토리의 서브모듈
- utils`: 몇 가지 유용한 유틸리티의 스크립트


## Getting Started

### Prerequisite  

```bash
apt-get install cmake build-essential protobuf-compiler libprotobuf-dev golang-go libunwind-dev libicu-dev
git submodule update --init
```

### Build  

```bash
mkdir build && cd build  
cmake .. && make
cd -
```

| extra cmake options | values | default |
| ------ | ------ | ------ |
| ENABLE_LINK_TCMALLOC | on, off | on |

### Play examples
- A sample quic server and client implementation are provided in quiche. To use these you should build the binaries.

```bash
cd build
make simple_quic_server simple_quic_client
cd -
```

- Download a copy of www.example.org, which we will serve locally using the simple_quic_server binary.

```bash
mkdir -p /data/quic-root
wget -p --save-headers https://www.example.org -P /data/quic-root
```

- In order to run the simple_quic_server, you will need a valid certificate, and a private key is pkcs8 format. If you don't have one, there are scripts to generate them.

```bash
cd utils
./generate-certs.sh
mkdir -p /data/quic-cert
mv ./out/* /data/quic-cert/
cd -
```

- Run the quic server

```bash
./build/simple_quic_server \
  --quic_response_cache_dir=/data/quic-root/ \
  --certificate_file=/data/quic-cert/leaf_cert.pem \
  --key_file=/data/quic-cert/leaf_cert.pkcs8
```

- Request the file with quic client

```bash
./build/simple_quic_client \
  --disable_certificate_verification=true \
  --host=127.0.0.1 --port=6121 \
  "https://www.example.org/index.html"
```

You can also use chormium-based browsers to access simple_quic_server at `127.0.0.1:6121`, and check the request/response protocol by DevTools -> Network panel.
