// ServerSock.cpp : implementation file
//

#include "stdafx.h"
#include "ChatSer.h"
#include "ServerSock.h"


// CServerSock

CServerSock::CServerSock()
{
}

CServerSock::~CServerSock()
{
}


// CServerSock member functions

void CServerSock::OnAccept(int nErrorCode)
{
	CChatSerApp *pApp = (CChatSerApp*)AfxGetApp();
	pApp->AcceptClient();

	CAsyncSocket::OnAccept(nErrorCode);
}
