# Webserv

with [Iwoo](https://github.com/humblEgo) & [Sanam](https://github.com/simian114)


## 1. 소켓 이해하기

전화를 걸기 위해서는 '단연코' 전화기가 필요하다. 전화기는 멀리 떨어져 있는 두 사람이 대화할 수 있도록 만들어주는 매개체이다. 소켓은 전화기처럼 멀리 떨어져 있는 두 개의 호스트(host)를 연결해주는 매개체 역할을 한다.

```C
#include <sys/types.h>
#include <sys/socket.h>

int socket(int domain, int type, int protocol);
```

우리는 누군가에게 걸려오는 전화를 받게 된다. 그리고 전화를 받으면 '여보세요'라는 응답을 주게 된다. 그렇다면, 전화기(소켓)을 만든 이후에 무엇이 더 필요할까? 당연히 전화번호가 필요하다. 그래야 누가 나한테 전화를 할 수 있기 때문이다. 그렇다면, 전화번호를 우리의 전화기에 할당시켜야 하는 것처럼, 소켓에도 일종의 '전화번호'가 필요하다. 소켓에서 전화번호의 역할을 하는 것은 바로 IP 주소이며, bind 함수를 이용하여 소켓에 주소를 할당할 수 있다.

```C
#include <sys/socket.h>

int bind(int sockfd, struct sockaddr *myaddr, int addrlen);
```

이제 전화를 받을 준비가 끝났다. 하지만, 전화가 케이블에 연결되어 있지 않으면 소용이 없다. 즉, 전화선을 연결시키고 전화를 받을 상태를 만들어주어야 한다. 마찬가지로 소켓도 연결 가능한 상태가 되도록 대기를 시켜주어야 하는데, listen 함수를 이용하여 연결 요청이 가능하도록 만들어줄 수 있다.

```C
#include <sys/socket.h>

int listen(int sockfd, int backlog);
```

자, 이제 전화벨이 울린다. 통화를 하기 위해서는 통화버튼을 눌러야 한다. 즉 누군가 대화를 요청했을 때 이것을 수락해야 한다는 뜻인데 소켓 역시 누군가가 데이터를 주고받기 위해 대화(연결)를 요청했을 때 그 요청을 수락할 수 있어야 한다. 이것을 accept 함수가 수행하게 된다.

```C
#include <sys/socket.h>

int accept(int sockfd, struct sockaddr *addr, int *addrlen);
```

지금까지의 내용을 정리해보면, 네트워크 프로그래밍의 기본 과정은
1. 전화기를 구한다. -> socket()
2. 전화번호를 할당한다. -> bind()
3. 전화를 케이블에 연결한다. -> listen()
4. 전화를 수락한다. -> accept()
위 과정을 수행하게 된다. 그리고 이 과정을 거친 프로그램이 바로 '서버' 프로그램이 되는 것이다.

자 그러면 이제 전화를 받을 수 있는 상황은 다 만들어졌다. 이제 남은 것은 전화를 거는 사람!을 구하는 것이다. 네트워크의 개념 속에서 이 사람은 '클라이언트'가 되고 클라이언트 역시 위와 비슷한 과정을 거쳐야 한다. 하지만 여기서 달라지는 것은 서버에게 먼저 전화를 걸어야 한다는 것. 이 기능을 하는 함수가 connect 이다.

클라이언트 소켓 구현
1. 전화를 걸기 위한 전화기를 구한다. socket()
2. 전화를 건다. (connect)
3. 연결이 완료되면 데이터를 주고받는다. (read)

* 자 지금까지의 내용을 전부 이해했다면 의아한 부분이 있을 것이다. 바로 전화를 걸기위한 '전화번호'를 부여받지 않는다는 것이다. 이 궁금증은 추후에 풀어갈 것이다.

---

* 파일 조작하기

리눅스에서는 모든 것(파일, 소켓, 콘솔)을 '파일'로 간주한다. 즉 파일 입・출력 함수를 소켓의 입・출력에서 사용할 수 있다.

42 과정을 잘 진행했다면, 파일 입・출력에 관한 것은 도사가 되어있을 것이니 파일 디스크립터에 대한 내용은 생략하도록 한다!

#### File Open Mode
|Mode|의미|
|:---:|:---:|
|O_CREATE|필요한 경우 파일을 생성함|
|O_TRUNC|존재하던 데이터를 모두 삭제|
|O_APPEND|존재하던 데이터를 보존하고 뒤에 이어서 저장|
|O_RDONLY|읽기 전용 모드로 파일을 연다.|
|O_WRONLY|쓰기 전용 모드로 파일을 연다.|
|O_RDWR|읽기 쓰기 전용 모드로 파일을 연다.|

socket() 으로 소켓을 생성하면 소켓 파일 디스크립터가 생성된다. 소켓 파일 디스크립터 역시 3번부터 넘버링된다.

---

## 2. 소켓 생성과 프로토콜 설정

```C
#include <sys/types.h>
#include <sys/socket.h>

int socket(int domain, int type, int protocol);
/*
** domain: 생성할 소켓의 통신을 위해 프로토콜 체계(Protocol Family)를 설정한다.
** type: 생성할 소켓의 데이터 전송을 위해 전송 타입을 설정한다.
** protocol: 두 호스트 간 통신을 위해 특정 프로토콜을 지정한다.
*/
```

socket() 함수의 인자들을 제대로 이해하기 위해서는 프로토콜 체계를 이해할 필요가 있다.

|프로토콜 체계|정의|
|:---:|:---:|
|PF_INET|IPv4 인터넷 프로토콜|
|PF_INET6|IPv6 인터넷 프로토콜|
|PF_LOCAL|Local 통신을 위한 UNIX 프로토콜|
|PF_PACKET|Low Level socket을 위한 인터페이스|
|PF_IPX|IPX 노벨 프로토콜|

PF_INET을 소켓 함수의 첫 번째 인자로 전달하는 경우, 생성되는 소켓은 IPv4 기반으로 하는 인터넷 프로토콜에 적합한 소켓이 생성될 것이다. 이렇듯 데이터를 주고 받는 환경에 따라 그에 맞는 적합한 프로토콜을 지정해주어야 한다. 이 말은 즉 소켓은 모든 프로토콜을 수용할 수 있다는 뜻이며, "소켓은 프로토콜에 독립적이다"라고 표현한다.

소켓 함수의 두 번째 인자인 Type은 데이터 전송 타입을 말한다. 주의해야 할 것은 domain(프로토콜 체계)이 정해졌다고 해서 데이터 전송 방법까지 결정된 것은 아니라는 것이다. 즉, 하나의 프로토콜 체계 안에서도 데이터를 전송하는 방법이 여러 개 있을 수 있다는 뜻이다.

가장 대표적으로 사용되는 두 가지의 데이터 전송방식은 다음과 같다.
1. SOCK_STREAM
- SOCK_STREAM 타입의 소켓은 연결 지향형 소켓이다. 연결 지향형 소켓을 사용하게 되면 두 사람이 하나의 라인을 통해 물건을 주고받을 수 있게 된다.
- 에러나 데이터의 손실이 없다. - 독립된 전송 라인을 통하여 데이터를 전달하기 때문에 라인의 문제가 아니라면 데이터는 반드시 전달될 수 있다.
- 전송하는 순서대로 데이터가 전달된다.
- 전송되는 데이터의 경계가 존재하지 않는다. - 이 말이 좀 애매할 수 있는데, 용량이 큰 데이터를 보낼 때 여러 개로 나누어서 보낼 수 있고, 하나의 큰 데이터를 여러 개로 나누어 옮길 수 있다는 뜻으로 이해하면 될 것 같다.
- 정리하면, `신뢰성이 있는 순차적 바이트 기반의 연결 지향 전송 타입`이다.

2. SOCK_DGRAM
- 비연결 지향형 소켓이다.
- 전송 순서에 상관없이 가장 빠른 전송을 지향한다.
- 전송 데이터는 손실될 수 있고 에러가 발생할 수 있다.
- 전송 데이터의 경계가 존재한다.
- 한 번에 전송되는 데이터의 크기가 제한된다.

소켓 함수의 세 번째 인자인 protocol은 호스트 대 호스트가 사용할 프로토콜을 설정하기 위해 사용된다.

프로토콜 체계가 PF_INET인 경우,
1. IPPROTO_TCP: TCP 기반의 소켓 생성
2. IPPROTO_UDP: UDP 기반의 소켓 생성
두 가지 프로토콜이 올 수 있다.

근데 사실, 첫 번째와 두 번째 인자만 잘 정해졌어도, 세 번째 인자에 0이 들어와도 적합한 소켓이 생성될 수 있다.

그렇다면, 이 세 번째 인자는 언제 사용되느냐?

바로 하나의 프로토콜 체계 안에서 최종적으로 통신하는 형태가 다른 여러 개의 프로토콜이 존재하는 경우를 위해서다. RAW_SOCKET을 생성하는 경우가 유용한 경우라고 하는데, 일단은 스킵하도록 한다.

대체로, socket(PF_INET, SOCK_STREAM, ) 인 경우에는 세 번째 인자로 TCP 타입이, socket(PF_INET, SOCK_DGRAM, )인 경우에는 세 번째 인자로 UDP 타입이 들어간다.

---

## 3. 주소 체계와 데이터 정렬

일반적으로 하나의 컴퓨터 안에서 여러 프로그램을 동시에 실행시킨다. 인강을 들으면서 카톡을 한다던가 하는 경우... 이러한 경우 두 개 이상의 프로그램이 인터넷을 통해 데이터를 주고 받고 있다고 할 수 있다. 컴퓨터는 하나의 IP 주소를 가지고 있으며, 인터넷을 통해 데이터를 주고 받는 프로그램이 여러 개가 실행되고 있다고 하더라도 데이터를 송・수신 하는 길은 하나밖에 존재하지 않을텐데, 어떻게 수신한 데이터를 구분하여 각각의 프로그램에 전달해 줄 수 있는 것일까?

이것을 위하여 Port가 사용된다. 포트는 short int (0 ~ 65535)의 범위를 가지며, 포트를 사용하여 수신한 데이터 패킷(네트워크 상에서 이동하는 데이터 블록)을 어떤 프로그램에 전달할 것인지 결정한다.

* TCP 소켓과 UDP 소켓은 하나의 포트를 공유할 수 없으며, 데이터 전송 시에는 IP 주소 뿐만 아니라 Port 정보도 포함시켜야 한다.

자 그러면 이제 IPv4의 주소 체계를 알아보자.

```C
struct sockaddr_in {
	sa_family_t		sin_family; // 주소 체계 (address family)
	uint16_t		sin_port; // 16비트 TCP / UDP port
	struct in_addr	sin_addr; // 32비트 IPv4 주소
	char			sin_zero[8]; // 사용되지 않음
};
```

생소한 데이터 타입이 등장하는데, 이 타입들은 POSIX(Portable Operating System Interface)에서 근거를 찾을 수 있다.
|Data Type|Description|Header|
|:---:|:---:|:---:|
|sa_family_t|address family|<sys/socket.h>|
|socklen_t|length of struct|<sys/socket.h>|

구조체의 sin_family 변수에는 주소 체계 정보를 대입한다. 아래 표의 값들이 사용될 수 있다.

|Address Family|정의|
|:---:|:---:|
|AF_INET|IPv4 인터넷 프로토콜|
|AF_INET6|IPv6 인터넷 프로토콜|
|AF_LOCAL|Local 통신을 위한 UNIX 프로토콜|

sockaddr_in 구조체 변수에 값을 대입할 경우에는 반드시 네트워크 바이트 순서로 값을 변경한 다음에 대입해야 한다.

1. Big-Endian 방식
- 상위 바이트의 값이 메모리 상에 먼저 표시되는 방법이다.
2. Little-Endian 방식
- 하위 바이트의 값이 메모리 상에 먼저 표시되는 방법이다.

위 두 방식 중 어떤 방식을 택할것인지는 사용자의 CPU에 따라 달라진다. 이것을 '호스트 바이트 순서'라고 한다.

하지만 호스트 바이트 순서는 일정치 않다는 문제점이 있기 때문에, 네트워크 바이트 순서로 `Big-Endian` 방식만을 사용하기로 약속되어 있다.

Big-Endian 방식으로 데이터를 변경하기 위해서 일종의 함수들을 사용해야 하는데, 함수 이름만 보면 그 기능을 충분히 파악할 수 있다.

```
h: host byte order
n: network byte order
s: short (16bit)
l: long (32bit)

unsigned short htons(unsigned short); // host to network short : 16비트 데이터를 host byte 순서에서 network byte 순서로 바꾸어라!
unsigned short ntohs(unsigned short); // network to host short : 16비트 데이터를 network byte 순서에서 host byte 순서로 바꾸어라!
unsigned long htonl(unsigned long); // host to network long : 32비트 데이터를 host byte 순서에서 network byte 순서로 바꾸어라!
unsigned long ntohl(unsigned long); // network to host long : 32비트 데이터를 network byte 순서에서 host byte 순서로 바꾸어라!
```

* 일반적으로 short 타입은 Port 정보의 바이트 순서를, long 타입은 IP 주소의 바이트 순서를 변경하는데 사용된다.

sockaddr_in 안에서 주소를 나타내기 위해 선언된 멤버의 자료형은 unsgined long 이었다. 따라서 IP 주소를 할당하기 위해서는 unsigned long 타입의 IP 주소를 표현해야 한다.

```C
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

unsigned long inet_addr(const char *string);
```

inet_addr() 함수를 사용하면 해당 unsigned long 타입의 데이터 값을 반환해준다. 실패 시 `INADDR_NONE` 을 리턴한다.

하지만 inet_addr()을 사용하게 되면 반환값을 sockaddr_in->in_addr 구조체 안에 대입해야 한다.

이 번거로운 과정을 `inet_aton` 함수가 한 번에 해결해준다. 반대로 32비트 값을 Dotted-Decimal Notation(IP주소)으로 변환하기 위해서는 `inet_ntoa` 함수를 사용하면 된다.

```C
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int inet_aton(const char *string, struct in_addr *addr);
char *inet_ntoa(struct in_addr addr);
```

주소 정보 구조체를 생성하고 초기화 하는 방법을 살펴보았다. 이제는 소켓에 주소를 할당하는 일만 남았다.

```C
#include <sys/types.h>
#include <sys/socket.h>

int bind(int sockfd, struct sockaddr *myaddr, int addrlen);
/*
** sockfd: 소켓의 파일 디스크립터를 인자로 전달한다.
** myaddr: sockaddr_in 구조체 변수의 포인터를 인자로 전달한다.
** addrlen: 인자로 전달된 주소 정보 구조체의 길이를 전달한다.

** success: return 0
** fail: return -1
*/
```

bind() 함수 호출이 성공하면 sockfd가 가리키는 소켓에 myaddr이 가리키는 주소 정보가 할당된다.

---

## 3. TCP 기반 서버/클라이언트

bind() 함수를 이용하여 소켓에 주소까지 할당했으면, 이제는 listen 함수를 통해서 '연결 요청 대기 상태'로 들어갈 차례이다. 여기까지 진행이 되어야 클라이언트가 연결 요청을 할 수 있겠지. 즉, 여기까지 진행이 되어야 클라이언트가 연결을 요청하는 함수 connect()를 호출해도 오류가 발생되지 않는다!

```C
#include <sys/types.h>

int listen(int s, int backlog);
/*
** s: 클라이언트의 연결 요청을 받아들이는 역할을 하게 될 소켓의 파일 디스크립터를 인자로 전달한다. 이 소켓을 서버 소켓 이라고 함.
** backlog: 연결 요청 대기 큐(Queue)의 크기를 나타낸다. 보통 인자로 최소 15이상의 값이 들어오고, 큐의 크기가 5가 되어 클라이언트의 연결 요청을 5개까지 대기시킬 수 있게 된다.
*/
```

클라이언트의 연결 요청 역시 인터넷을 통해 들어오는 일종의 '데이터 전송'이다. 따라서 당연히 받아들이기 위해서는 소켓이 하나 있어야 한다. listen 함수의 첫 번째 인자로 전달된 소켓(서버 소켓)이 하는 일이 바로 이것이다. 일종의 수문장 역할인 셈이다.

클라이언트: "저기여,, 제가 혹시 감히 연결해도 될까요..?"
서버소켓: "아 그래도 되긴합니다 ㅋ. 근데 시스템이 바쁘니까 번호표 받고 대기실 가서 기다리세여 ㅋ."

listen() 함수가 호출되면 첫 번째 인자로 들어온 소켓을 서버 소켓으로 만들어주고, 두 번째 전달되는 backlog 값을 참조하여 대기실을 만든다. 이 대기실이 바로 Queue의 크기가 되는 것이고, 클라이언트의 연결 요청을 대기시키게 한다.

자, 연결 대기 상태까지 만들어 놨으면 당연히 연결 요청을 수락해야 한다. 그리고 당연히 수락하는 과정에도 소켓이 필요하다. 하지만, 서버 소켓은 이미 하는 일이 있으니... 소켓을 하나 더 만들어야 하는데... 다행히 accept 함수를 이용하면 직접 만드는 헛수고를 하지 않아도 된다.

```C
#include <sys/types.h>
#include <sys/socket.h>

int accept(int s, struct sockaddr *addr, int *addrlen);
/*
** s: 서버 소켓의 파일 디스크립터를 인자로 전달한다.
** addr: 연결 요청을 수락할 클라이언트의 주소 정보를 저장한다. 호출 성공시 클라이언트의 주소 정보로 채워진다.
** addrlen: 인자로 전달된 addr의 크기를 저장한다. 호출 성공시 리턴받은 클라이언트의 주소 길이가 바이트 단위로 채워진다.
*/
```

accpet() 함수는 대기실에서 클라이언트의 연결 요청을 수락해 주는 함수이다. 따라서 호출 성공시 내부적으로 클라이언트와의 데이터 입・출력을 위한 소켓을 생성하고, 그 소켓의 파일 디스크립터를 리턴한다. 중요한 것은 소켓은 알아서 만들어지기 때문에, 직접 소켓을 만들지 않아도 된다.

---

자 그렇다면 이제는 TCP 기반 클라이언트를 살펴보자.

앞서 이야기했듯이, 서버보다 간단하게 구현할 수 있다. 차이가 났던 부분은 '연결 요청'과정(connect)과 전화번호를 할당(bind)받지 않는다는 것이었다.

서버가 listen() 함수를 호출하게 되면 클라이언트의 연결 요청이 대기실에 들어갈 수 있다고 했다. 그렇다면 클라이언트의 연결 요청은 어느 시점에 발생할까? 눈치챘겠지만 바로 connect() 함수 호출 시점이다.

```C
#include <sys/types.h>
#include <sys/socket.h>

int connect(int sockfd, struct sockaddr *serv_addr, int addrlen);
/*
** sockfd: 미리 생성해 놓은 소켓의 파일 디스크립터이다.
** serv_addr: 연결 요청을 보낼 서버의 주소 정보를 지닌 구조체 변수의 포인터이다.
** addrlen: serv_addr 포인터가 가리키는 구조체 변수의 크기이다.
*/
```

connect 함수가 반환되는 시점은 연결 요청이 서버에 의해 수락(accept)되거나 오류가 발생하여 연결 요청이 중단되는 경우다. 만약 연결 요청이 바로 이루어지지 않고 서버의 대기 큐에서 대기하고 있는 상태라면 connect 함수는 리턴되지 않고 블로킹(blocking) 상태에 머무르게 된다.

자, 그렇다면 왜 클라이언트 영역에서는 소켓의 '전화번호'를 할당하지 않았을까? 전화번호인 IP 주소가 필요없다는 것인가?! 물론 아니다. 그렇다면 언제 주소를 할당받는 다는 것인가?
- Connect 함수가 호출할 때 주소가 자동으로 할당된다.
- 운영체제, 보다 정확하게 이야기하자면 커널이 생성해준다...
- 스스로 알아서 생성해준다...
- 커널은 호스트에 할당된 IP 주소를 찾아서 할당시키고, 남아돌고 있는 포트 중 하나를 골라서 할당시킨다.

* 즉, connect 함수 호출 시 자동적으로 소켓에 주소와 포트가 할당된다! bind 함수를 사용할 필요가 없다!

자, 여기까지 이해한 내용을 바탕으로 코딩을 진행하면, 우리의 서버는 한 클라이언트의 요청에만 응답하고 더이상 다른 어떤 요청도 무시하고 바로 종료된다. 이건 우리가 상상한 서버의 모습이 아니지 않은가?

따라서 클라이언트의 요청을 항시 대기할 수 있도록 해야한다. 간단하다. 클라이언트 연결이 종료되는 close() 함수 호출이 끝나고, 다시 listen() 상태로 돌아가게 하면 된다.

```C
while (1)
{
	client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_size);
	if (client_socket == -1)
	{
		error_handling("accept() error");
		break ;
	}
	write(client_socket, message, sizeof(message));
	close(client_socket);
}
```

얼마나 간단한가!

---

지금까지 과정으로 간단한 에코 서버와 에코 클라이언트를 구현할 수 있게 되었다.

하지만 에코 클라이언트에서 생각할 문제가 있다.

TCP는 연결 지향 프로토콜로서 데이터의 경계가 없다고 했다. 그렇다 보니 한 번의 write 함수 호출을 통해 "ABCD"라는 문자열을 전송한다고 했을 때 이 데이터들이 반드시 하나의 패킷으로 구성되어서 전송된다고 보장할 수 없다. 상황에 따라서 "AB" 문자열이 먼저 하나의 패킷으로 전송되고, 그 다음에 "C"가 전송되고, 마지막으로 "D"가 전송될 수 있다. 그렇다면, 이러한 데이터 전송 특징에 문제가 되는 것은 무엇일까?

정리하면, 지금까지는 한 번의 write 함수로 데이터를 전송하고 한 번의 read 함수를 통해 데이터를 수신했다. 이 때 한 번의 read 함수로는 일부 데이터 패킷만을 받았을 때 문제가 생긴다. 그렇다면, 반드시 하나의 패킷으로 구성되어야 온전한 데이터 송・수신을 보장할 수 있는데, TCP는 이것을 절대로 보장해주지 않는다.

이 때

```C
while (1)
{
	...

	for (recv_len = 0; recv_len < str_len; )
	{
		recv_num = read(sock, &message[recv_len], str_len - recv_len);
		if (recv_num == -1)
			error_handling("read() error");
		recv_len += recv_num;
	}

	...

}
```

데이터를 전송하려는 과정에서 반복문을 통해 read 함수를 계속해서 호출하며 배열에 저장시킨다면, 정확히 전송한 바이트 크기만큼의 데이터를 수신할 수 있다.

TCP 기반의 에코 클라이언트가 구현하기 쉬운 이유가, 수신할 데이터의 크기를 미리 알고 있다는 것에 있다. 다른 소켓 프로그램의 경우에서는 데이터를 수신하는 측에서 몇 바이트를 수신해야 하는지 알 수 없다. 이러한 경우엔 프로그램을 구현하기가 조금 더 어려워질 것이다. 하지만, EOF를 사용하여 전송한 데이터의 끝을 알리게 한다면 해결할 수 잇을 것이나, 제한적일 수밖에 없다... 경험과 시행착오를 통해서 항상 최선의 적합한 정답을 찾도록 하자.........

그리고 에코 서버에서

```C
...
accept()

sleep(5);
str_len = read(client_socket, message, BUFFER_SIZE);
write(client_socket, message, str_len);
```

5초간 sleep을 걸어주면서 write 함수 호출이 여러번 들어왔을 때를 대비할 수 있다.

---

### 4. I/O 멀티플렉싱

소켓 프로그래밍의 기본을 알았다면, 이제는 서브젝트에 해당하는 내용인 멀티플렉싱에 대해서 살펴볼 차례이다.

Webserv 과제는 select() 함수를 이용한 단일 프로세스의 멀티플렉싱(multiplexing) 서버를 구현하는 것이다.

보통 다중 접속 서버를 구현하기 위해서 fork와 pipe를 이용하여 다중 프로세스로 처리한다. 하지만 프로세스의 생성은 상당히 많은 연산과정을 거치며, 생성 후에도 많은 자원을 차지하게 된다. 따라서 모든 프로세스들은 서로 독립적인 메모리 공간을 할당받아서 사용하기 때문에 프로세스간 통신을 하기 위해서는 다소 복잡한 방법을 선택할 수 밖에 없다. 하지만, 만약 하나의 프로세스로 여러 클라이언트들과 데이터를 주고받을 수만 있다면 보다 좋은 서버의 모델이 될 것이다. 이것이 바로 I/O 멀티플렉싱 방법이 등장한 배경이다.

자, 그렇다면 멀티플렉싱 서버란 어떠한 서버를 의미하는 것인가? 전화기지국과 다른 전화기지국을 연결하는 하나의 중계선이 있다고 가정해보자. 이 때 비록 선은 하나이지만, 이 선을 통해서 여러 사람이 통화를 할 수 있다. 즉 하나의 선을 여러 사람이 공유하며 통화를 위해 사용한다는 것인데, 이것이 바로 멀티플렉싱이다. 이렇듯 멀티플렉싱이란 데이터를 전송하는데 있어서 장치의 효율성을 높이기 위해 최소한의 물리적인 요소만 사용하는 경제학적인 방법이다.

서버에 멀티플렉싱 개념을 도입해보자. 서버에서의 멀티플렉싱은 '여러 개'를 묶어서 '하나'로 만드는 것(그 하나를 공유한다)이라고 생각하면 이해하기 쉽다. 멀티프로세스 기반에서는 하나의 서버(부모프로세스)를 토대로 여러 개의 자식 프로세스를 만들고 그 자식 프로세스들을 통해 클라이언트와 소통하게 한다. 하지만 멀티플렉싱 기반의 서버는 여러 개의 자식 프로세스를 생성하는 것 대신에, 클라이언트의 입・출력을 하나로 묶어서 하나의 서버로 여러 클라이언트와 통신할 수 있도록 하며, 클라이언트의 연결 요청 엮시 하나로 묶어서 처리한다.

이해가 쉽게 되지 않을 수도 있다. 다른 예를 들어보자. 한 반에 학생이 열 명이 있고, 한 명의 선생님에게 열 명의 아이들이 동시에 질문한다고 생각해보자. 학교 측에서는 이 질문세례를 막기 위해 선생님 9명을 추가로 고용한다. 이렇게 `일 대 일` 로 교사와 학생을 연결시키는 것을 멀티 프로세스라고 생각하면 된다. 하지만, 아이들이 공부를 하면서 점점 지식이 쌓이고, 질문의 수가 갑자기 확 줄어버렸다. 따라서 교사 한 명으로 커버가 될 것이라 예상하고 나머지 교사 9명을 해고했다고 해보자. 이 때 동시에 질문이 들어오는 것을 막기 위해 질문하고 싶은 학생에게는 손을 들게 하고, 교사는 손을 든 학생이 있는지 확인하다가 손을 든 학생의 질문을 받기로 했다. 이것이 바로 멀티플렉싱 방법이다. 서버는 주기적으로 데이터를 전송하는 클라이언트가 있는지 `확인`하며, 발견한 경우에 그 클라이언트로부터 데이터를 받아온다. 이렇게 생각하면 이해가 된다!

그렇다면, 어떠한 방식으로 멀티플렉싱 서버를 구현할 수 있을까? 대표적으로 select() 함수를 통해 멀티플렉싱 서버를 구현한다. select() 함수를 사용하면 한 곳에 모아놓은 여러 개의 파일 디스크립터(소켓)를 동시에 관찰할 수 있다. 데이터 전송 시 파일 디스크립터를 블로킹 처리하지 않고 바로 전달 가능한 파일 디스크립터가 어떤 것인지, 예외가 발생한 파일 디스크립터가 어떤 것인지 지켜보게 된다.

```C
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int select(int n, fd_set *readfds, fd_set *writefds, ft_set *exceptfds, struct timeval *timeout);
/*
** n: 검사 대상이 되는 파일 디스크립터의 개수
** readfds: "입력 스트림에 변화가 발생했는지" 확인하고자 하는 소켓들의 정보. 입력 스트림에 변화가 발생했다는 것은 수신할 데이터가 존재한다는 뜻이다.
** writefds: "데이터 전송 시, 블로킹되지 않고 바로 전송이 가능한지" 확인하고자 하는 소켓들의 정보.
** exceptfds: "예외가 발생했는지" 확인하고자 하는 소켓들의 정보.
** timeout: 함수 호출 후, 무한대기 상태에 빠지지 않도록 설정할 수 있는 변수

** return value: -1(에러), 0(타임아웃), 0보다 큰 경우: 변경된 파일 디스크립터의 수
*/
```

* select 함수는 호출했을 때 관찰 대상들에게서 변화가 발생해야 반환되며, 그렇지 않으면 변화가 발생할 때까지 무한정으로 블로킹 상태에 빠지게 된다.
* fd_set 은 관찰하려고 하는 파일 디스크립터 정보를 모아놓은 비트 단위 배열이다.

|함수 선언|기능|
|:---:|:---:|
|FD_ZERO(fd_set *fdset);|fdset 포인터가 가리키는 변수의 모든 비트를 0으로 초기화|
|FD_SET(int fd, fd_set *fdset);|fdset 포인터가 가리키는 변수에 fd로 전달되는 파일 디스크립터 정보를 설정|
|FD_CLR(int fd, fd_set *fdset);|fdset 포인터가 가리키는 변수에서 fd로 전달되는 파일 디스크립터 정보를 삭제|
|FD_ISSET(int fd, fd_set *fdset);|fdset 포인터가 가리키는 변수가 fd로 전달되는 파일 디스크립터 정보를 지니고 있는지 확인|

select() 함수는 아래의 상황에서 반환된다.
- 수신할 데이터가 있을 때 - 읽어들일 데이터(read() 함수의 입력 버퍼)가 존재하는 경우(+ 클라이언트의 연결이 요청되었을 때).
- 파일 디스크립터를 통해 데이터를 전송할 경우 블로킹되지 않았을 때 - write() 함수의 출력 버퍼에 전송하지 못한 데이터가 남아있어서 곧바로 데이터 전송을 할 수 없는 경우 블로킹된다.
- 파일 디스크립터가 가리키는 소켓에서 에러가 발생하지 않았을 때.

##### select() 함수 호출 예제
```C
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>

#define BUFSIZE 30

int main(int argc, char **argv)
{
	fd_set reads, temps;
	int result;
	char msg[BUFSIZE];
	int str_len;
	struct timeval timeout;

	FD_ZERO(&reads);
	FD_SET(0, &reads); /* standard input 설정 */

	while (1)
	{
		temps = reads;
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;

		result = select(1, &temps, 0, 0, &timeout);
		if (result == -1) /* select 함수 오류 발생 */
		{
			puts("select error");
			exit(EXIT_FAILURE);
		}
		else if (result == 0) /* timeout */
		{
			puts("time over");
		}
		else
		{
			if (FD_ISSET(0, &temps))
			{
				str_len = read(0, msg, BUFSIZE);
				msg[str_len] = '\0';
				fputs(msg, stdout);
			}
		}
	}
}
```

select() 함수의 사용법까지 알았느니 실제 멀티플렉싱 서버를 구현해보자. /study/tutorial/ 디렉토리에 있는 multiplexing_server 를 임의의 포트와 연결시켜 실행하고, 여러 개의 터미널을 통해 multiplexing_client를 실행시켜보자.
```bash
# multiplexing_server
./multiplexing_server 9130

# multiplexing_client
./multiplexing_client 127.0.0.1 9130
```

---

### 5. Webserv 과제 진행하기

본격적으로 공부한 것을 토대로 과제에 진입해보자.

웹서비스는 서버와 클라이언트 모델을 따른다. 이 때 두 가지 모델에서 정보를 제대로 주고받고 해석하기 위해서는 두 가지 프로토콜이 필요하다. 하나는 데이터요청과 응답에 관한 프로토콜이고, 또 다른 하나는 데이터의 형식을 정의하기 위한 프로토콜이다. 전자가 HTTP, 후자가 HTML이다. 클라이언트가 형식에 맞게 웹서버에 무엇인가를 요청하면, 웹서버는 이 요청을 분석하여 알맞은 형태로 응답한다. 응답을 받은 클라이언트는 정보를 해석해서 사용자에게 보여준다.

이번 과제에서는 HTTP 프로토콜만 신경쓰면 되는 것 같다.

자 그럼 HTTP 프로토콜을 살펴보자.

1. 요청방법 (GET/POST/HEAD/PUT/DELETE ...)
2. 요청 페이지 (index.html)
3. 프로토콜 (HTTP/1.1)

GET 요청 방법부터 구현해보자. 웹브라우저로 웹페이지를 요청할 때 사용하는 일반적인 방법이다.
- 요청할 URL: GET이면 요청할 페이지, POST라면 클라이언트가 보낸 자료를 처리할 서버의 url이다.
- 프로토콜 버전: 책에 나오는 시점에서 최신은 1.1이라고 했는데 아직도 1.1인지는 모르겠다. (나중에.. 확인해보자.)

요청을 한 이후에는 적절한 응답을 보내주어야 하는데, 이것 역시 HTTP 형식을 따라야 한다. (최소 아래 모든 헤더를 구현해야 한다.)

```bash
◦ Accept-Charsets # 클라이언트가 이해할 수 있는 문자
◦ Accept-Language # 서버가 돌려주기로 예상된 언어
◦ Allow # HTTP 요청 방법
◦ Authorization # 서버와 함께 유저 에이전트를 인증하기 위한 자격증명
◦ Content-Language # 사용자 언어
◦ Content-Length # 문서의 크기
◦ Content-Location # 반환된 데이터의 대체 위치
◦ Content-Type # 이미지나 동영상 등 다양한 형식이 나타날 수 있기 때문에 형태를 알려주기 위해 꼭 필요하다
◦ Date # 문서를 전송한 시간
◦ Host # 서버가 listening 중인 TCP 포트 번호
◦ Last-Modified # 마지막 수정 날짜
◦ Location # 페이지를 리다이렉트할 url
◦ Referer # 현재 페이지로 연결되는 링크가 있던 이전 웹 페이지의 주소
◦ Retry-After # 클라이언트가 다음 요청을 생성하기 전에 대기해야 하는 시간
◦ Server # 웹서버 프로그램의 정보
◦ Transfer-Encoding # 데이터 전송을 위한 인코딩 형식
◦ User-Agent # 사용자 에이전트의 어플리케이션 타입
◦ WWW-Authenticate # 리소스에 접근할 때 사용되어야 하는 인증메소드
```

임의 페이지에 GET 요청을 보내보자.

```bash
# Telnet을 사용할 때는 반드시 끝에 개행문자가 두 개 와야한다.

% telnet www.joinc.co.kr 80
GET /modules/moniwiki/wiki.php/FrontPage HTTP/1.0
\n
\n
```

<br>

웹서버 프로그램을 만들기 위해 HTTP 프로토콜과 응답방식 등에 대해서 알아보았다. 자 그러면 구조를 설계해보자.

클라이언트의 요청을 받은 웹서버는 파일 시스템에서 파일 정보를 읽어서 클라이언트에 전송한다. 혹은 외부의 다른 프로그램을 실행해서 그 결과를 클라이언트에 전송하기도 한다. 외부 프로그램으로의 정보 입출력은 `CGI 규격`을 사용한다.

CGI 너는 도대체 무엇이냐..! 끝도 없이 새로운 게 나타난다... PHP와 같이 이해해보자.
- 웹서버는 웹 서비스를 위한 하부구조 역할을 한다. 이 때 웹서버는 다양한 서비스를 개발할 수 있도록 외부의 프로그램을 실행시켜 데이터를 대신 처리한다. 이 때 웹 서버와 외부 프로그램 간의 통신 방식을 CGI(Common Gateway Interface)라고 한다. CGI 프로그램은 공통규격만 따르면 되기 때문에 어떤 언어를 사용해도 관계없다. 하지만, 우리는 C++로 만들어야 하지 않은가... 뭐 이러한 특징 때문에 서로 다른 언어들로 디버깅을 하게 되더니 개발과 유지보수가 매우 어려워졌다고 한다. 이렇듯 여튼저튼 이유로 웹페이지 문서에 통합되는 언어로 개발할 필요가 생겼는데, 이렇게 해서 만들어진 언어가 바로 PHP라고 한다. PHP 코드는 HTML 문서에 포함되어 있는데, 덕분에 개발과 디버깅 과정이 웹 문서에 통합된다.

자 그렇다면 이제 웹서버의 구조를 설계할 수 있다.

> 반드시 C++로 HTTP 서버를 구현해야 한다.
>> 서버는 반드시 RFC 7230 ~ 7535 (http 1.1) 을 준수하도록 한다.
>>
>> 서버와 클라이언트 사이의 통신은 non-blocking으로, 단 하나의 select()와 listen()을 사용하도록 한다.
>>
>> 서버는 정지되면 안되고 클라이언트는 적절하게 연결을 끊을 수 있도록 한다.
>>
>> 서버로 온 Request가 영원히 머물러서는 안된다.
>>
>> CGI를 제외하고는 fork()를 사용하지 말자.

1. 서버 소켓을 생성한다.
2. bind() 함수를 사용하여 주소를 할당한다.
3. listen() 함수를 사용하여 사용자의 요청을 대기하도록 한다.
4. select() 함수를 이용하여 멀티플렉싱으로 사용자의 요청을 accept() 할 수 있게 한다. - 이 때 select 함수는 동시에 read와 write를 동시에 확인하도록 한다.
5. read() 로 클라이언트의 요청을 수신하고 요청방식에 따라 처리할 수 있도록 구조화한다.
6. 요청방식에 따라 응답할 HTTP 형태를 구현한다.
7. write() 로 클라이언트에게 응답한다.
8. 소켓을 닫는다.