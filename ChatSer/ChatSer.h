
// ChatSer.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CChatSerApp:
// �� Ŭ������ ������ ���ؼ��� ChatSer.cpp�� �����Ͻʽÿ�.
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
	CClientSock *m_MasterAccount;			// ����
	std::vector<CString> m_WhisperAccount;	// �Ӹ� ����Ʈ (���� ������� ������ ������)
	std::vector<CClientSock *> m_Account;	// ���� ����Ʈ
	std::vector<CString> m_banAccount;		// ���� ����Ʈ
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

// �������Դϴ�.
	public:
	virtual BOOL InitInstance();

// �����Դϴ�.

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