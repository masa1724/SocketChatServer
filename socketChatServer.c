#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "log.h"
#include <WInerror.h>
#include <tchar.h>

/*
 * http://wisdom.sakura.ne.jp/system/winapi/winsock/winSock2.html
 */

// ポート番号
#define PORT 12345

// 最大クライアント接続数
#define MAX_CLIENT 5

// ソケットオプションOFF
#define SOCK_OPT_OFF 0

// ソケットオプションON
#define SOCK_OPT_ON 1


/**/
#define CLIENT_PORT 12346
#define CLIENT_IP_ADDR "127.0.0.1"


//
#define RESPONSE_KIND_MSG TEXT("100")
#define RESPONSE_KIND_USER TEXT("200")


int watchClientRequest(void);
int sendSocketToClient(TCHAR *sendMsg, size_t msgLen, TCHAR *cliIpAddr, int cliPort);
TCHAR* getErrorMessage(int errcd);

int main(void) {
	infolog(TEXT("Socketサーバーを起動します。"));

	int retCd = watchClientRequest();
	if(retCd != 0) {
		errorlog(TEXT("Socketサーバーでエラーが発生しました。"));
	}

	infolog(TEXT("Socketサーバーを終了します。"));
	return 0;
}


/**
 * クライアントからのリクエストを監視します。
 * リクエストの種類によって処理を振り分けます。
 *
 * @return 正常終了(0) 異常終了(0以外)
 *
 */
int watchClientRequest(void) {
	WSADATA wsaData;
	SOCKET listenSock;
	struct sockaddr_in listenAddr;
	struct sockaddr_in clientAddr;
	int clientAddrlen;
	SOCKET acceptSock;
	TCHAR recvmsg[1024];
	BOOL sockOptVal;
	int errCode;
	TCHAR *errMsg;

	// winsock2の初期化
	if (WSAStartup(MAKEWORD(2,0), &wsaData) != 0) {
		errorlog(TEXT("winsock2の初期化に失敗しました。"));
		return -1;
	}

	// 受付用のSocketを作成
	listenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSock == INVALID_SOCKET) {
		errCode = WSAGetLastError();
		errMsg = getErrorMessage(errCode);
		errorlog(TEXT("待ち受けSocketの生成に失敗しました。エラー詳細:[%s] エラーコード:[%d]"), errMsg, errCode);
		return -1;
	}

	/*
	 * サーバー側で先にTCPセッションをクローズすると、プロセスで使用していたポートがTIME_WAIT状態となり、
	 * 次、別プロセスが同一ポートを利用しようと場合、使用中と判断されてしまいエラーとなってしまう。
	 * 処理が終了したプロセスで使っていたポート番号をすぐに再利用出来るようにSO_REUSEADDRをONにし、
	 * TIME_WAIT状態のポート番号にSocketをbind出来るようにする
	 */
	sockOptVal = SOCK_OPT_ON;
	setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR, (LPCTSTR *)&sockOptVal, sizeof(sockOptVal));

	/** 受付を許可するIP情報を設定 **/
	memset(&listenAddr, 0, sizeof(listenAddr));
	// ファミリー
	listenAddr.sin_family = AF_INET;
	// ポート番号
	listenAddr.sin_port = htons(PORT);
	// IPアドレス
	listenAddr.sin_addr.S_un.S_addr = htons(INADDR_ANY);

	// 受付用のSocketへ受付を許可するIP情報を紐付ける
	if (bind(listenSock, (struct sockaddr *)&listenAddr, sizeof(listenAddr)) != 0) {
		int errCode = WSAGetLastError();
		errCode = WSAGetLastError();
		errMsg = getErrorMessage(errCode);
		errorlog(TEXT("受付用のSocketへ受付を許可するIP情報を紐付けに失敗しました。エラー詳細:[%s] エラーコード:[%d]"), errMsg, errCode);
		return -1;
	}

	// TCPクライアントからの接続要求を待てる状態にする
	if (listen(listenSock, MAX_CLIENT) != 0) {
		errCode = WSAGetLastError();
		errMsg = getErrorMessage(errCode);
		errorlog(TEXT("クライアントからの接続要求待ちの準備中にエラーが発生しました。。エラー詳細:[%s] エラーコード:[%d]"), errMsg, errCode);
		return -1;
	}

	while(1) {
		 // TCPクライアントからの接続要求を受け付ける
		 clientAddrlen = sizeof(clientAddr);
		 memset(&clientAddr, 0, clientAddrlen);
		 acceptSock = accept(listenSock, (struct sockaddr *)&clientAddr, &clientAddrlen);

		 if (acceptSock == INVALID_SOCKET) {
			 errCode = WSAGetLastError();
			 errMsg = getErrorMessage(errCode);
			 errorlog(TEXT("受付Socketの生成に失敗しました。エラー詳細:[%s] エラーコード:[%d]"), errMsg, errCode);
			 break;
		 }

		 //CreateThread( ClientHandler, arg );

		// infolog(TEXT("クライアントからの要求を受け付けました。IPAddress:[%s] Port:[%d]"),
			//	 inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

		 /*

		 // クライアントへメッセージを送信
		 if (send(acceptSock, sendMsg, sizeof(sendMsg), 0) <= 0) {
			 errCode = WSAGetLastError();
			 errMsg = getErrorMessage(errCode);
			 errorlog(TEXT("クライアントへのメッセージ送信に失敗しました。エラー詳細:[%s] エラーコード:[%d]"), errMsg, errCode);
			 break;
		 }

		 infolog(TEXT("クライアントへメッセージを送信しました。Message:[%s] Length:[%d] IPAddress:[%s] Port:[%d]"),
				 sendMsg , _tcslen(sendMsg), inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

	*/
		 memset(recvmsg, 0, sizeof(recvmsg));
		 if (recv(acceptSock, recvmsg, sizeof(recvmsg), 0) <= 0) {
			 errCode = WSAGetLastError();
			 errMsg = getErrorMessage(errCode);
			 errorlog(TEXT("クライアントからのメッセージ受信に失敗しました。エラー詳細:[%s] エラーコード:[%d]"), errMsg, errCode);
			 break;
		 }

		 infolog(TEXT("クライアントからメッセージを受信しました。Message:[%s] Length:[%d] IPAddress:[%s] Port:[%d]"),
				 recvmsg, _tcslen(recvmsg), inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));


		 // 5文字送信
		 TCHAR sendMsg[1024] = "server       ";
		 _tcscat(sendMsg, recvmsg);
		 sendSocketToClient(sendMsg, sizeof(sendMsg), CLIENT_IP_ADDR, CLIENT_PORT);

		 // TCPセッションの終了
		 closesocket(acceptSock);
	}

	// winsock2の終了処理
	WSACleanup();

	return 0;
}

/**
 * 指定したクライアントへメッセージを送信します.
 *
 * @param *sendMsg   送信するメッセージ
 * @param *cliIpAddr WSAGetLastError()で取得したエラーコード
 * @param cliPort    WSAGetLastError()で取得したエラーコード
 * @return 正常終了(0) 異常終了(0以外)
 *
 */
int
sendSocketToClient(TCHAR *sendMsg, size_t msgLen, TCHAR *cliIpAddr, int cliPort) {
	//WSADATA wsaData;
	SOCKET consock;
	struct sockaddr_in cliAddr;
	int errCode;
	TCHAR *errmsg;

	/*
	// winsock2の初期化
	if (WSAStartup(MAKEWORD(2,0), &wsaData) != 0) {
		errorlog(TEXT("winsock2の初期化に失敗しました。"));
		return -1;
	}
	*/

	// コネクションソケットの作成
	consock = socket(AF_INET, SOCK_STREAM, 0);
	if (consock == INVALID_SOCKET) {
		errCode = WSAGetLastError();
		errmsg = getErrorMessage(errCode);
		errorlog(TEXT("コネクションSocketの生成に失敗しました。エラー詳細:[%s] エラーコード:[%d]"), errmsg, errCode);
		return -1;
	}

	/*** 送信先のアドレス情報を設定 ***/
	memset(&cliAddr, 0, sizeof(cliAddr));
	// ファミリー
	cliAddr.sin_family = AF_INET;
	// ポート番号
	cliAddr.sin_port = htons(cliPort);
	// IPアドレス
	cliAddr.sin_addr.S_un.S_addr = inet_addr(cliIpAddr);

	// クライアントに接続
	if (connect(consock, (struct sockaddr *)&cliAddr, sizeof(cliAddr)) != 0) {
		int errCode = WSAGetLastError();
		errCode = WSAGetLastError();
		errmsg = getErrorMessage(errCode);
		errorlog(TEXT("クライアントへの接続に失敗しました。エラー詳細:[%s] エラーコード:[%d] IPAddress:[%s] Port:[%d]"),
				errmsg, errCode, inet_ntoa(cliAddr.sin_addr), ntohs(cliAddr.sin_port));
		return -1;
	}

	/*
	// サーバからデータを受信
	 memset(recvmsg, 0, sizeof(recvmsg));
	 if (recv(consock, recvmsg, sizeof(recvmsg), 0) <= 0) {
		 errCode = WSAGetLastError();
		 errmsg = getErrorMessage(errCode);
		 errorlog(TEXT("サーバーからのメッセージ受信に失敗しました。エラー詳細:[%s] エラーコード:[%d]"),
				 errmsg, errCode);
		 return -1;
	 }

	 infolog(TEXT("サーバーからメッセージを受信しました。Message:[%s] Length:[%d]"),
			 recvmsg , _tcslen(recvmsg));

	 Sleep(1);
	*/

	 // サーバーへメッセージを送信
	 if (send(consock, sendMsg, msgLen, 0) <= 0) {
		 errCode = WSAGetLastError();
		 errmsg = getErrorMessage(errCode);
		 errorlog(TEXT("クライアントへのメッセージ送信に失敗しました。エラー詳細:[%s] エラーコード:[%d]"), errmsg, errCode);
		 return -1;
	 }

	 infolog(TEXT("クライアントへメッセージを送信しました。Message:[%s] Length:[%d]"),
			 sendMsg , _tcslen(sendMsg));

	// winsock2の終了処理
	//WSACleanup();

	return 0;
}

/**
 * WSAGetLastError()で取得したエラーコードからエラーメッセージを取得し、返却します.
 *
 * @param errcd WSAGetLastError()で取得したエラーコード
 * @return エラーメッセージを格納したメモリのポインタ
 *
 */
TCHAR*
getErrorMessage(int errcd){
	LPVOID msg;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
	    FORMAT_MESSAGE_FROM_SYSTEM |
	    FORMAT_MESSAGE_IGNORE_INSERTS,
	    NULL,
		errcd,
	    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // 既定の言語
	    (TCHAR*)&msg,
	    0,
	    NULL
	);

	return msg;
}
