// ClientSock.cpp : implementation file
//

#include "stdafx.h"
#include "ChatSer.h"
#include "ClientSock.h"


// CClientSock

CClientSock::CClientSock()
{
}

CClientSock::~CClientSock()
{
}


// CClientSock member functions

void CClientSock::OnClose(int nErrorCode)
{
	CChatSerApp *pApp = (CChatSerApp*)AfxGetApp();
	pApp->CloseClient(this);

	CAsyncSocket::OnClose(nErrorCode);
}

void CClientSock::OnReceive(int nErrorCode)
{
	CChatSerApp *pApp = (CChatSerApp*)AfxGetApp();

	pApp->ReceiveData(this);

	CAsyncSocket::OnReceive(nErrorCode);
}
