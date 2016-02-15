#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

char ModulePath[MAX_PATH];

void _STSpeech(const char *msg)
{
	SOCKADDR_IN server;
	WSADATA     wsadata;
	SOCKET      sock;
	
	long   len;
	char   buf[15];
	
	//���M����f�[�^�̐���(��������������擪�̕���)
	len = (long)lstrlen(msg);
	*((short*)&buf[0])  = 0x0001; //[0-1]  (16Bit) �R�}���h          �i 0:���b�Z�[�W�ǂݏグ�j
	*((short*)&buf[2])  = -1;     //[2-3]  (16Bit) ���x              �i-1:�_�ǂ݂�����ʏ�̐ݒ�j
	*((short*)&buf[4])  = -1;     //[4-5]  (16Bit) ����              �i-1:�_�ǂ݂�����ʏ�̐ݒ�j
	*((short*)&buf[6])  = -1;     //[6-7]  (16Bit) ����              �i-1:�_�ǂ݂�����ʏ�̐ݒ�j
	*((short*)&buf[8])  = 0;      //[8-9]  (16Bit) ����              �i 0:�_�ǂ݂�����ʏ�̐ݒ�A1:����1�A2:����2�A3:�j��1�A4:�j��2�A5:�����A6:���{�b�g�A7:�@�B1�A8:�@�B2�A10001�`:SAPI5�j
	*((char* )&buf[10]) = 2;      //[10]   ( 8Bit) ������̕����R�[�h�i 0:UTF-8, 1:Unicode, 2:Shift-JIS�j
	*((long* )&buf[11]) = len;    //[11-14](32Bit) ������̒���
	
	//�ڑ���w��p�\���̂̏���
	server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	server.sin_port             = htons(50001);
	server.sin_family           = AF_INET;
	
	//Winsock2������
	WSAStartup(MAKEWORD(1, 1), &wsadata);
	
	//�\�P�b�g�쐬
	sock = socket(AF_INET, SOCK_STREAM, 0);
	
	//�T�[�o�ɐڑ�
	connect(sock, (struct sockaddr *)&server, sizeof(server));
	
	//�f�[�^���M
	send(sock, buf, 15, 0);
	send(sock, msg, len, 0);
	
	//�\�P�b�g�I��
	closesocket(sock);
	
	//Winsock2�I��
	WSACleanup();
}

__declspec(dllexport) int luaSTSpeech(lua_State *L) // lua_State is broken. I don't know how to fix :p
{
	HANDLE hFile;
	char FileName[MAX_PATH];
	char str[1024];
	DWORD numberOfBytesRead;
	
	lstrcpy(FileName, ModulePath);
	lstrcat(FileName, "msg.dat");
	hFile = CreateFile(
		FileName,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (INVALID_HANDLE_VALUE == hFile) {
		return 0;
	}
	ReadFile(hFile, str, sizeof(str), &numberOfBytesRead, NULL);
	str[numberOfBytesRead] = '\0';
	CloseHandle(hFile);
	
	_STSpeech(str);
	
	return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
					   DWORD  ul_reason_for_call,
					   LPVOID lpReserved
					 )
{
	int i;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		GetModuleFileName(hModule, ModulePath, sizeof(ModulePath));
		for(i = sizeof(ModulePath) - 1; i > 0 && ModulePath[i] != '\\'; i--) ModulePath[i] = '\0';
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

