#pragma once

// CServerSock command target

class CServerSock : public CAsyncSocket
{
public:
	CServerSock();
	virtual ~CServerSock();
	virtual void OnAccept(int nErrorCode);
};


