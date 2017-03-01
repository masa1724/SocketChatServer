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

// �|�[�g�ԍ�
#define PORT 12345

// �ő�N���C�A���g�ڑ���
#define MAX_CLIENT 5

// �\�P�b�g�I�v�V����OFF
#define SOCK_OPT_OFF 0

// �\�P�b�g�I�v�V����ON
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
	infolog(TEXT("Socket�T�[�o�[���N�����܂��B"));

	int retCd = watchClientRequest();
	if(retCd != 0) {
		errorlog(TEXT("Socket�T�[�o�[�ŃG���[���������܂����B"));
	}

	infolog(TEXT("Socket�T�[�o�[���I�����܂��B"));
	return 0;
}


/**
 * �N���C�A���g����̃��N�G�X�g���Ď����܂��B
 * ���N�G�X�g�̎�ނɂ���ď�����U�蕪���܂��B
 *
 * @return ����I��(0) �ُ�I��(0�ȊO)
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

	// winsock2�̏�����
	if (WSAStartup(MAKEWORD(2,0), &wsaData) != 0) {
		errorlog(TEXT("winsock2�̏������Ɏ��s���܂����B"));
		return -1;
	}

	// ��t�p��Socket���쐬
	listenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSock == INVALID_SOCKET) {
		errCode = WSAGetLastError();
		errMsg = getErrorMessage(errCode);
		errorlog(TEXT("�҂���Socket�̐����Ɏ��s���܂����B�G���[�ڍ�:[%s] �G���[�R�[�h:[%d]"), errMsg, errCode);
		return -1;
	}

	/*
	 * �T�[�o�[���Ő��TCP�Z�b�V�������N���[�Y����ƁA�v���Z�X�Ŏg�p���Ă����|�[�g��TIME_WAIT��ԂƂȂ�A
	 * ���A�ʃv���Z�X������|�[�g�𗘗p���悤�Əꍇ�A�g�p���Ɣ��f����Ă��܂��G���[�ƂȂ��Ă��܂��B
	 * �������I�������v���Z�X�Ŏg���Ă����|�[�g�ԍ��������ɍė��p�o����悤��SO_REUSEADDR��ON�ɂ��A
	 * TIME_WAIT��Ԃ̃|�[�g�ԍ���Socket��bind�o����悤�ɂ���
	 */
	sockOptVal = SOCK_OPT_ON;
	setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR, (LPCTSTR *)&sockOptVal, sizeof(sockOptVal));

	/** ��t��������IP����ݒ� **/
	memset(&listenAddr, 0, sizeof(listenAddr));
	// �t�@�~���[
	listenAddr.sin_family = AF_INET;
	// �|�[�g�ԍ�
	listenAddr.sin_port = htons(PORT);
	// IP�A�h���X
	listenAddr.sin_addr.S_un.S_addr = htons(INADDR_ANY);

	// ��t�p��Socket�֎�t��������IP����R�t����
	if (bind(listenSock, (struct sockaddr *)&listenAddr, sizeof(listenAddr)) != 0) {
		int errCode = WSAGetLastError();
		errCode = WSAGetLastError();
		errMsg = getErrorMessage(errCode);
		errorlog(TEXT("��t�p��Socket�֎�t��������IP����R�t���Ɏ��s���܂����B�G���[�ڍ�:[%s] �G���[�R�[�h:[%d]"), errMsg, errCode);
		return -1;
	}

	// TCP�N���C�A���g����̐ڑ��v����҂Ă��Ԃɂ���
	if (listen(listenSock, MAX_CLIENT) != 0) {
		errCode = WSAGetLastError();
		errMsg = getErrorMessage(errCode);
		errorlog(TEXT("�N���C�A���g����̐ڑ��v���҂��̏������ɃG���[���������܂����B�B�G���[�ڍ�:[%s] �G���[�R�[�h:[%d]"), errMsg, errCode);
		return -1;
	}

	while(1) {
		 // TCP�N���C�A���g����̐ڑ��v�����󂯕t����
		 clientAddrlen = sizeof(clientAddr);
		 memset(&clientAddr, 0, clientAddrlen);
		 acceptSock = accept(listenSock, (struct sockaddr *)&clientAddr, &clientAddrlen);

		 if (acceptSock == INVALID_SOCKET) {
			 errCode = WSAGetLastError();
			 errMsg = getErrorMessage(errCode);
			 errorlog(TEXT("��tSocket�̐����Ɏ��s���܂����B�G���[�ڍ�:[%s] �G���[�R�[�h:[%d]"), errMsg, errCode);
			 break;
		 }

		 //CreateThread( ClientHandler, arg );

		// infolog(TEXT("�N���C�A���g����̗v�����󂯕t���܂����BIPAddress:[%s] Port:[%d]"),
			//	 inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

		 /*

		 // �N���C�A���g�փ��b�Z�[�W�𑗐M
		 if (send(acceptSock, sendMsg, sizeof(sendMsg), 0) <= 0) {
			 errCode = WSAGetLastError();
			 errMsg = getErrorMessage(errCode);
			 errorlog(TEXT("�N���C�A���g�ւ̃��b�Z�[�W���M�Ɏ��s���܂����B�G���[�ڍ�:[%s] �G���[�R�[�h:[%d]"), errMsg, errCode);
			 break;
		 }

		 infolog(TEXT("�N���C�A���g�փ��b�Z�[�W�𑗐M���܂����BMessage:[%s] Length:[%d] IPAddress:[%s] Port:[%d]"),
				 sendMsg , _tcslen(sendMsg), inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

	*/
		 memset(recvmsg, 0, sizeof(recvmsg));
		 if (recv(acceptSock, recvmsg, sizeof(recvmsg), 0) <= 0) {
			 errCode = WSAGetLastError();
			 errMsg = getErrorMessage(errCode);
			 errorlog(TEXT("�N���C�A���g����̃��b�Z�[�W��M�Ɏ��s���܂����B�G���[�ڍ�:[%s] �G���[�R�[�h:[%d]"), errMsg, errCode);
			 break;
		 }

		 infolog(TEXT("�N���C�A���g���烁�b�Z�[�W����M���܂����BMessage:[%s] Length:[%d] IPAddress:[%s] Port:[%d]"),
				 recvmsg, _tcslen(recvmsg), inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));


		 // 5�������M
		 TCHAR sendMsg[1024] = "server       ";
		 _tcscat(sendMsg, recvmsg);
		 sendSocketToClient(sendMsg, sizeof(sendMsg), CLIENT_IP_ADDR, CLIENT_PORT);

		 // TCP�Z�b�V�����̏I��
		 closesocket(acceptSock);
	}

	// winsock2�̏I������
	WSACleanup();

	return 0;
}

/**
 * �w�肵���N���C�A���g�փ��b�Z�[�W�𑗐M���܂�.
 *
 * @param *sendMsg   ���M���郁�b�Z�[�W
 * @param *cliIpAddr WSAGetLastError()�Ŏ擾�����G���[�R�[�h
 * @param cliPort    WSAGetLastError()�Ŏ擾�����G���[�R�[�h
 * @return ����I��(0) �ُ�I��(0�ȊO)
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
	// winsock2�̏�����
	if (WSAStartup(MAKEWORD(2,0), &wsaData) != 0) {
		errorlog(TEXT("winsock2�̏������Ɏ��s���܂����B"));
		return -1;
	}
	*/

	// �R�l�N�V�����\�P�b�g�̍쐬
	consock = socket(AF_INET, SOCK_STREAM, 0);
	if (consock == INVALID_SOCKET) {
		errCode = WSAGetLastError();
		errmsg = getErrorMessage(errCode);
		errorlog(TEXT("�R�l�N�V����Socket�̐����Ɏ��s���܂����B�G���[�ڍ�:[%s] �G���[�R�[�h:[%d]"), errmsg, errCode);
		return -1;
	}

	/*** ���M��̃A�h���X����ݒ� ***/
	memset(&cliAddr, 0, sizeof(cliAddr));
	// �t�@�~���[
	cliAddr.sin_family = AF_INET;
	// �|�[�g�ԍ�
	cliAddr.sin_port = htons(cliPort);
	// IP�A�h���X
	cliAddr.sin_addr.S_un.S_addr = inet_addr(cliIpAddr);

	// �N���C�A���g�ɐڑ�
	if (connect(consock, (struct sockaddr *)&cliAddr, sizeof(cliAddr)) != 0) {
		int errCode = WSAGetLastError();
		errCode = WSAGetLastError();
		errmsg = getErrorMessage(errCode);
		errorlog(TEXT("�N���C�A���g�ւ̐ڑ��Ɏ��s���܂����B�G���[�ڍ�:[%s] �G���[�R�[�h:[%d] IPAddress:[%s] Port:[%d]"),
				errmsg, errCode, inet_ntoa(cliAddr.sin_addr), ntohs(cliAddr.sin_port));
		return -1;
	}

	/*
	// �T�[�o����f�[�^����M
	 memset(recvmsg, 0, sizeof(recvmsg));
	 if (recv(consock, recvmsg, sizeof(recvmsg), 0) <= 0) {
		 errCode = WSAGetLastError();
		 errmsg = getErrorMessage(errCode);
		 errorlog(TEXT("�T�[�o�[����̃��b�Z�[�W��M�Ɏ��s���܂����B�G���[�ڍ�:[%s] �G���[�R�[�h:[%d]"),
				 errmsg, errCode);
		 return -1;
	 }

	 infolog(TEXT("�T�[�o�[���烁�b�Z�[�W����M���܂����BMessage:[%s] Length:[%d]"),
			 recvmsg , _tcslen(recvmsg));

	 Sleep(1);
	*/

	 // �T�[�o�[�փ��b�Z�[�W�𑗐M
	 if (send(consock, sendMsg, msgLen, 0) <= 0) {
		 errCode = WSAGetLastError();
		 errmsg = getErrorMessage(errCode);
		 errorlog(TEXT("�N���C�A���g�ւ̃��b�Z�[�W���M�Ɏ��s���܂����B�G���[�ڍ�:[%s] �G���[�R�[�h:[%d]"), errmsg, errCode);
		 return -1;
	 }

	 infolog(TEXT("�N���C�A���g�փ��b�Z�[�W�𑗐M���܂����BMessage:[%s] Length:[%d]"),
			 sendMsg , _tcslen(sendMsg));

	// winsock2�̏I������
	//WSACleanup();

	return 0;
}

/**
 * WSAGetLastError()�Ŏ擾�����G���[�R�[�h����G���[���b�Z�[�W���擾���A�ԋp���܂�.
 *
 * @param errcd WSAGetLastError()�Ŏ擾�����G���[�R�[�h
 * @return �G���[���b�Z�[�W���i�[�����������̃|�C���^
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
	    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // ����̌���
	    (TCHAR*)&msg,
	    0,
	    NULL
	);

	return msg;
}
