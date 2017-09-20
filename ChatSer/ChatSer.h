
// ChatSer.h : PROJECT_NAME 응용 프로그램에 대한 주 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"		// 주 기호입니다.


// CChatSerApp:
// 이 클래스의 구현에 대해서는 ChatSer.cpp을 참조하십시오.
//
#include "ServerSock.h"
#include "ClientSock.h"

enum { ROOM_PUBLIC, ROOM_PRIAVTE, ROOM_WHISPER };
class RoomInfo{
public:
	int m_Status;
	CString m_Title;
	CString m_Pass;
	UINT32 m_RoomIndex;
	CClientSock *m_MasterAccount;			// 방장
	std::vector<CString> m_WhisperAccount;	// 귓말 리스트 (방이 사라지기 전까진 존재함)
	std::vector<CClientSock *> m_Account;	// 접속 리스트
	std::vector<CString> m_banAccount;		// 차단 리스트
};

class crypt {
public:
	crypt() : m_prime(0), m_primeNumber(0), m_publicKey(0), m_privateKey(0) {};

	LONG m_prime;
	LONG m_primeNumber;
	LONG m_publicKey, m_privateKey;
};

class CChatSerApp : public CWinAppEx
{
public:
	CChatSerApp();
	CServerSock  *m_pServer;
	CObList  m_UserList;
	std::map<CClientSock *, CString> m_UserName;
	UINT32 m_RoomIndex;
	std::map<UINT32, RoomInfo*> m_RoomInfo;
	std::map<CString, RoomInfo*> m_WhispRoomInfo;
	std::map<CClientSock *, crypt> m_CryptInfo;

// 재정의입니다.
	public:
	virtual BOOL InitInstance();

// 구현입니다.

	DECLARE_MESSAGE_MAP()
public:
	void InitServer(void);
	void AcceptClient(void);
	void SendData(CClientSock *pSock, CString& strData);
	void ReceiveData(CClientSock* pSock);
	void DeleteUserList(CString name);
	int CreateRoom(CClientSock* pSock, INT32 status, CString title, CString pass);
	void LoginData(CClientSock* pSock, CString Data);
	void LobbyData(CClientSock* pSock, CString Data);
	void RoomData(CClientSock* pSock, CString Data);
	void FriendData(CClientSock* pSock, CString Data);
	void CloseRoom(CClientSock* pSock, UINT32 Index);
	void CloseClient(CClientSock* pSock);
	void SendRoomList(void);
};

extern CChatSerApp theApp;