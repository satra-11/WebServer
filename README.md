# 仕様
- HTTP/1.1 
    - [RFC 7230](http://www.rfc-editor.org/info/rfc7230)
    - [RFC 7231](http://www.rfc-editor.org/info/rfc7231)
    - [RFC 7232](http://www.rfc-editor.org/info/rfc7232)
    - [RFC 7233](http://www.rfc-editor.org/info/rfc7233)
    - [RFC 7234](http://www.rfc-editor.org/info/rfc7234)
    - [RFC 7235](http://www.rfc-editor.org/info/rfc7235)
- HTTP/2
    - [RFC 7540](http://www.rfc-editor.org/info/rfc7540)
    - [RFC 7541](http://www.rfc-editor.org/info/rfc7541)
- ライブラリ
    - [Winsock2.h](https://learn.microsoft.com/ja-jp/windows/win32/api/winsock2/nf-winsock2-socket)

# 実装
HTTP通信を行うためには、まず基盤となるトランスポート層のプロトコルであるTCPをsocketで実装する。
socketは、ほとんどのオペレーティングシステムが提供する機能で、プログラムがネットワークにアクセスできるようにする仕組み。
TCP/IPソケットプログラミングでは以下のステップに従う：

1. socketの生成：通信のエンドポイントを作成
2. socketの識別：socketを特定のポート番号に関連付け
3. 接続の待機：サーバーがクライアントからの接続を受け入れるまで待機
4. データの送受信：接続が確立されたら、メッセージのやり取りを行う
5. ソケットのクローズ：通信が終了したら、ソケットを閉じてリソースを解放

### ステップ1: ソケットの作成
```c
int server_fd = socket(domain, type, protocol);
```
※引数および返り値はいずれもint型

#### `domain`:
アドレスファミリには、`AF_INET (IP)`, `AF_INET6 (IPv6)`, `AF_UNIX (ローカルチャネル、パイプに似ている)`, `AF_ISO (ISOプロトコル)`, `AF_NS (Xerox Network Systemsプロトコル)`などがある。

#### `type`:
サービスタイプには、アプリケーションの要求に応じて変わるプロパティがある。`SOCK_STREAM (仮想回線サービス)`, `SOCK_DGRAM (データグラムサービス)`, `SOCK_RAW (ダイレクトIPサービス)`などが挙げられる。アドレスファミリと共に利用可能なサービスを把握しておくと良い。

#### `protocol`:
ソケットを扱う上で使用するプロトコルを指す。ファミリによっては複数のプロトコルが使用可能。このケースでは使用可能なプロトコルが一つのみなので0になる。

```c
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
...
server_fd = socket(AF_INET, SOCK_STREAM, 0);
if (server_fd == 0) {
        perror("cannot create socket");
        exit(EXIT_FAILURE);
    }
```
### ステップ2: ソケットの識別（アドレスのバインド）
ソケットにトランスポートアドレス（ポート番号）を割り当てる操作を、bindシステムコールを用いて行う。アドレスは、汎用的な`sockaddr`構造体を用いて設定する。

```c
int bind(int socket, const struct sockaddr *address, socklen_t address_len);
```
sockaddr構造体はOSがアドレスファミリを識別できるようにする汎用コンテナ。IPネットワーキングでは、sockaddr_in構造体を使用する。ソケットのアドレスをポート番号で受け取るのではなくsockaddr構造として受け取る。

```c
typedef struct sockaddr_in {
  short          sin_family;
  u_short        sin_port;
  struct in_addr sin_addr;
  char           sin_zero[8];
} SOCKADDR_IN, *PSOCKADDR_IN, *LPSOCKADDR_IN;
```

バインドする前に、この構造体に必要な情報を設定する。sin_familyにはAF_INET、sin_portにはネットワークバイトオーダーで変換されたポート番号、sin_addr.s_addrにはhtonl(INADDR_ANY)を設定する。

#### `sin_family`
このフィールドは、ソケットの設定時に使用するアドレスファミリを指定する。本ケースではAF_INETが使用される。

#### `sin_port`
このフィールドにはポート番号を指定する。ポート番号は、OSに自動で割り当ててもらうか、明示的に指定することも可能。クライアント側ではポート番号を0に設定することで、使用可能なポートをOSに割り当ててもらうことが一般的。一方サーバー側では、クライアントが接続するためのポート番号を特定する必要がある。

#### `sin_addr`
このフィールドには、ソケットがバインドされるIPアドレスを指定する。一般的に、サーバーは複数のネットワークインターフェース（例：Wi-Fiやイーサネット）が存在する場合でも、特定のインターフェースを指定する必要はなく、INADDR_ANY (0.0.0.0) を使用してすべてのインターフェースで受け入れるように設定されている。

#### パラメータのサイズ
アドレス構造体のタイプはトランスポートのタイプによって異なるが、sockaddr_in構造体のサイズはsizeof(struct sockaddr_in)に設定する。このサイズは、関数に正しいメモリサイズを提供し、エラーを避けるために必要。

最終的なコードは以下のようになる:
```c
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
...
#define PORT 8080
struct sockaddr_in address;

address.sin_family = AF_INET;
address.sin_addr.s_addr = INADDR_ANY;
address.sin_port = htons(PORT);

if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
}
```