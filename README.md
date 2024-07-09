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
```
int server_fd = socket(domain, type, protocol);
```
※引数および返り値はいずれもint型

#### `domain`:
アドレスファミリには、`AF_INET (IP)`, `AF_INET6 (IPv6)`, `AF_UNIX (ローカルチャネル、パイプに似ている)`, `AF_ISO (ISOプロトコル)`, `AF_NS (Xerox Network Systemsプロトコル)`などがあります。

#### `type`:
サービスタイプには、アプリケーションの要求に応じて変わるプロパティがあります。`SOCK_STREAM (仮想回線サービス)`, `SOCK_DGRAM (データグラムサービス)`, `SOCK_RAW (ダイレクトIPサービス)`などが挙げられます。アドレスファミリと共に利用可能なサービスを把握しておくと良いでしょう。

#### `protocol`:
ソケットを扱う上で使用するプロトコルを指します。ファミリによっては複数のプロトコルが使用可能です。最後の引数は、このケースでは使用可能なプロトコルが一つのみなので0になります。

```
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