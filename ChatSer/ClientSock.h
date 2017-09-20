#pragma once

// CClientSock command target

class CClientSock : public CAsyncSocket
{
public:
	CClientSock();
	virtual ~CClientSock();
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
};


