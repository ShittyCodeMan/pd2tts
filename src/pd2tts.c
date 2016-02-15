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
	
	//送信するデータの生成(文字列を除いた先頭の部分)
	len = (long)lstrlen(msg);
	*((short*)&buf[0])  = 0x0001; //[0-1]  (16Bit) コマンド          （ 0:メッセージ読み上げ）
	*((short*)&buf[2])  = -1;     //[2-3]  (16Bit) 速度              （-1:棒読みちゃん画面上の設定）
	*((short*)&buf[4])  = -1;     //[4-5]  (16Bit) 音程              （-1:棒読みちゃん画面上の設定）
	*((short*)&buf[6])  = -1;     //[6-7]  (16Bit) 音量              （-1:棒読みちゃん画面上の設定）
	*((short*)&buf[8])  = 0;      //[8-9]  (16Bit) 声質              （ 0:棒読みちゃん画面上の設定、1:女性1、2:女性2、3:男性1、4:男性2、5:中性、6:ロボット、7:機械1、8:機械2、10001～:SAPI5）
	*((char* )&buf[10]) = 2;      //[10]   ( 8Bit) 文字列の文字コード（ 0:UTF-8, 1:Unicode, 2:Shift-JIS）
	*((long* )&buf[11]) = len;    //[11-14](32Bit) 文字列の長さ
	
	//接続先指定用構造体の準備
	server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	server.sin_port             = htons(50001);
	server.sin_family           = AF_INET;
	
	//Winsock2初期化
	WSAStartup(MAKEWORD(1, 1), &wsadata);
	
	//ソケット作成
	sock = socket(AF_INET, SOCK_STREAM, 0);
	
	//サーバに接続
	connect(sock, (struct sockaddr *)&server, sizeof(server));
	
	//データ送信
	send(sock, buf, 15, 0);
	send(sock, msg, len, 0);
	
	//ソケット終了
	closesocket(sock);
	
	//Winsock2終了
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

