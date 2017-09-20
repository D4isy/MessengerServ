
// ChatSer.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "ChatSer.h"
#include "ChatSerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChatSerApp

BEGIN_MESSAGE_MAP(CChatSerApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CChatSerApp ����

CChatSerApp::CChatSerApp()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	m_pServer = NULL;
	m_RoomIndex = 1;
}


// ������ CChatSerApp ��ü�Դϴ�.

CChatSerApp theApp;


// CChatSerApp �ʱ�ȭ

BOOL CChatSerApp::InitInstance()
{
	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControlsEx()�� �ʿ��մϴ�.
	// InitCommonControlsEx()�� ������� ������ â�� ���� �� �����ϴ�.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ���� ���α׷����� ����� ��� ���� ��Ʈ�� Ŭ������ �����ϵ���
	// �� �׸��� �����Ͻʽÿ�.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ
	// ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.
	SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));

	if (Sql_Initialize()) {
		AfxMessageBox(_T("SQL �ʱ�ȭ ����!"));
		return FALSE;
	}

	CChatSerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: ���⿡ [Ȯ��]�� Ŭ���Ͽ� ��ȭ ���ڰ� ������ �� ó����
		//  �ڵ带 ��ġ�մϴ�.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: ���⿡ [���]�� Ŭ���Ͽ� ��ȭ ���ڰ� ������ �� ó����
		//  �ڵ带 ��ġ�մϴ�.
	}

	Sql_Finish();

	// ��ȭ ���ڰ� �������Ƿ� ���� ���α׷��� �޽��� ������ �������� �ʰ�  ���� ���α׷��� ���� �� �ֵ��� FALSE��
	// ��ȯ�մϴ�.
	return FALSE;
}

void CChatSerApp::InitServer(void)
{
	m_pServer = new CServerSock;
	m_pServer->Create(2010);
	m_pServer->Listen();
}

void CChatSerApp::AcceptClient(void)
{
	CClientSock *pClient = new CClientSock;
	m_pServer->Accept(*pClient);
	m_UserList.AddTail(pClient);	// ��� ���� ����
}

/*
	* �α��� ����
		-. 0 / ���̵� / ��й�ȣ (clnt)
		-. 0 / ����Ȯ�� / ����Ű ���� (serv), ���Ű ���� ����
		-. �α��� ������ �ִ��� ������ Ȯ��
	* �κ� ����
		-. 1 / �� ��ȣ / (������ ��ȣȭ �� �� �� ����)
		-. ��� ���������� ���� ������ ������.
		-. ģ�� ��� �ҷ�����
	* �� ���� (����)
		-. 2 / �� ��ȣ / �Է� ���� / ������ ��ȣȭ �� (clnt)
		-. 2 / �� ��ȣ / �г��� / �Է� ���� + �ð� (serv)
*/

void CChatSerApp::DeleteUserList(CString name)
{
	CListBox *pList = (CListBox*)m_pMainWnd->GetDlgItem(IDC_LIST_MSG);		// ��� �޽���
	CListBox *pUser = (CListBox*)m_pMainWnd->GetDlgItem(IDC_LIST_USER);		// ��� ���� ����

	CString strt;
	CTime t = CTime::GetCurrentTime();
	if (name == _T("")) {
		return;
	}
	strt = name + _T("���� �����ϼ̽��ϴ�.") + t.Format(_T("(%Y.%m.%d %H:%M:%S)"));

	pList->AddString(strt);
	int index = pList->GetCount();
	pList->SetAnchorIndex(index - 1);

	int nFind = pUser->FindString(-1, name);
	pUser->DeleteString(nFind);
}

int LoginQuery(CString id, CString pass)
{
	if (Sql_Query("SELECT `id`,`pass` FROM `account`"))
		return -1;

	char *data;
	CString user_id, user_pass;
	while (! Sql_NextRow()) {
		Sql_GetData(0, &data), user_id = CA2T(data);
		Sql_GetData(1, &data), user_pass = CA2T(data);

		if (id == user_id) {
			if (pass == user_pass) {
				// ������ ����
				Sql_Free_Result();
				return 1;
			}
			else {
				// ������ ������ ��й�ȣ�� Ʋ��
				Sql_Free_Result();
				return 0;
			}
		}
	}
	// ������ ��й�ȣ�� ��
	Sql_Free_Result();
	return 0;
}

int PassHelpQuery(CString id, CString &pass)
{
	char char_id[128];
	strcpy_s(char_id, sizeof(char_id), CT2A(id));

	char query[256] = { 0, };
	sprintf_s(query, sizeof(query), "SELECT `pass` FROM `account` WHERE `id`='%s' LIMIT %d", char_id, 1);

	if (Sql_Query(query))
		return -1;

	char *data;
	CString user_pass;
	if (!Sql_NextRow()) {
		Sql_GetData(0, &data), user_pass = CA2T(data);

		// ��й�ȣ �ű��
		pass = user_pass;
	}
	else {
		Sql_Free_Result();
		return -1;
	}

	Sql_Free_Result();
	return 0;
}

int UpdateQuery(int type, CString id, int login)
{
	char char_id[128];
	strcpy_s(char_id, sizeof(char_id), CT2A(id));

	char query[256] = { 0, };

	if (type & 1) {	// �ð� ������Ʈ
		memset(query, 0, sizeof(query));
		DWORD tick = time(NULL);
		sprintf_s(query, sizeof(query), "UPDATE `account` SET `time`='%u' WHERE `id`='%s'", tick, char_id);

		if (Sql_UpdateQuery(query)) {
			return -1;
		}
	}
	if (type & 2) { // �α��� ������Ʈ
		memset(query, 0, sizeof(query));
		sprintf_s(query, sizeof(query), "UPDATE `account` SET `login`='%u' WHERE `id`='%s'", login, char_id);

		if (Sql_UpdateQuery(query)) {
			return -1;
		}
	}
	return 0;
}

int FriendQuery(CString userName, CString friendName)
{
	char char_userName[128];
	char char_friendName[128];
	strcpy_s(char_userName, sizeof(char_userName), CT2A(userName));
	strcpy_s(char_friendName, sizeof(char_friendName), CT2A(friendName));

	char query[256] = { 0, };
	sprintf_s(query, sizeof(query), "SELECT `name`,`friend` FROM `friend` WHERE `name`='%s' and `friend`='%s'", char_userName, char_friendName);

	if (Sql_Query(query))
		return -1;

	if (!Sql_NextRow()) {
		// �̹� ������ ������
		Sql_Free_Result();
		return -1;
	}

	Sql_Free_Result();

	sprintf_s(query, sizeof(query), "INSERT INTO `friend` (`name`,`friend`) VALUES ('%s','%s')", char_userName, char_friendName);

	if (Sql_UpdateQuery(query))
		return -1;
	return 0;
}

int FriendDelQuery(CString userName, CString friendName)
{
	char char_userName[128];
	char char_friendName[128];
	strcpy_s(char_userName, sizeof(char_userName), CT2A(userName));
	strcpy_s(char_friendName, sizeof(char_friendName), CT2A(friendName));

	char query[256] = { 0, };
	sprintf_s(query, sizeof(query), "SELECT `name`,`friend` FROM `friend` WHERE `name`='%s' and `friend`='%s'", char_userName, char_friendName);

	if (Sql_Query(query))
		return -1;

	if (!Sql_NextRow()) {
		// �̹� ������ ������
		Sql_Free_Result();
		return -1;
	}

	Sql_Free_Result();

	sprintf_s(query, sizeof(query), "DELETE from `friend` (`name`,`friend`) VALUES ('%s','%s')", char_userName, char_friendName);

	if (Sql_UpdateQuery(query))
		return -1;
	return 0;
}

int FriendLoadQuery(CString userName, CString &totalUserName)
{
	char char_userName[128];
	strcpy_s(char_userName, sizeof(char_userName), CT2A(userName));

	char query[256] = { 0, };
	sprintf_s(query, sizeof(query), "SELECT `friend` FROM `friend` WHERE `name`='%s'", char_userName);

	if (Sql_Query(query))
		return -1;

	char *data;
	CString user_id;
	while (!Sql_NextRow()) {
		Sql_GetData(0, &data), user_id = CA2T(data);
		totalUserName.Format(_T("%s%s,"), totalUserName, user_id);
	}

	totalUserName = totalUserName.Mid(0, totalUserName.GetLength() - 1);
	Sql_Free_Result();
	return 0;
}

int RegisterQuery(CString id, CString pass)
{
	char char_id[128];
	char char_pass[128];

	strcpy_s(char_id, sizeof(char_id), CT2A(id));
	strcpy_s(char_pass, sizeof(char_pass), CT2A(pass));

	char query[256] = { 0, };
	sprintf_s(query, sizeof(query), "SELECT `id`,`pass` FROM `account` WHERE `id`='%s' LIMIT %d", char_id, 1);

	if (Sql_Query(query))
		return -1;

	char *data;
	CString user_id;
	if (!Sql_NextRow()) {
		// �̹� ������ ������
		Sql_Free_Result();
		return -1;
	}

	Sql_Free_Result();

	sprintf_s(query, sizeof(query), "INSERT INTO `account` (`id`,`pass`,`login`,`time`,`root`) VALUES ('%s','%s','0','%d','0')", char_id, char_pass, CTime::GetCurrentTime());

	if (Sql_UpdateQuery(query))
		return -1;
	return 0;
}

void CChatSerApp::LoginData(CClientSock* pSock, CString Data)
{
	// 0 |/| �α���:0 / �α׾ƿ�:1 |/| �� �� �� |/| ��й�ȣ
	// 0 |/| ȸ������:2            |/| �� �� �� |/| ��й�ȣ
	// 0 |/| ���̵� ã��:3         |/| �� �� ��
	// 0 |/| ����:0   / ����:1     |/| ����Ű ����(serv), ���Ű ���� ���� |/| �Ҽ�
	CString token[4];
	int nPos = 0;

	nPos = strsplit(Data, CString("|/|"), token, 4);
	//for (int i = 0; i < nPos; i++) {
	//	str.Format(_T("[%d] %s"), i + 1, token[i]);
	//	MessageBox(NULL, str, _T(""), 0);
	//}

	CListBox *pList = (CListBox*)m_pMainWnd->GetDlgItem(IDC_LIST_MSG);		// ��� �޽���
	CListBox *pUser = (CListBox*)m_pMainWnd->GetDlgItem(IDC_LIST_USER);		// ��� ���� ����

	// MessageBox(NULL, token[1] + _T(", ") + token[2], _T("�˸�"), 0);

	CString sendData;
	if (token[1] == _T("0")) {	// �α���
		// token[2] : ���̵�
		// token[3] : ��й�ȣ
		if (LoginQuery(token[2], token[3]) == 1) {
			// �α��� ����

			// �α����� ����� �ִ��� �˻�
			CString connectUserName;
			std::map<CClientSock *, CString>::iterator iter;
			for (iter = m_UserName.begin(); iter != m_UserName.end(); iter++) {
				connectUserName = iter->second;
				if (token[2] == (CString)connectUserName) {
					CloseClient(iter->first);
					break;
				}
			}

			// �α���, �ð� ���� ������Ʈ
			UpdateQuery(3, token[2], 1);

			CString strt;
			CTime t = CTime::GetCurrentTime();
			strt = token[2] + _T("���� �����ϼ̽��ϴ�.") + t.Format(_T("(%Y.%m.%d %H:%M:%S)"));

			pList->AddString(strt);
			int index = pList->GetCount();
			pList->SetAnchorIndex(index - 1);

			pUser->AddString(token[2]);
			m_UserName[pSock] = token[2];

			// ���� Ű �����ϱ�
			crypt crypt_info;
			crypt_info.m_primeNumber = Make_Random_Prime_Number();
			crypt_info.m_publicKey = Make_Public_Key(crypt_info.m_primeNumber);
			crypt_info.m_privateKey = Make_Private_Key(crypt_info.m_publicKey, crypt_info.m_primeNumber);
			crypt_info.m_prime = GetPrime();

			//CString str;
			//str.Format(_T("�Ҽ�: %d\n����Ű: %d"), m_primeNumber, m_publicKey);
			//MessageBox(NULL, str, _T(""), 0);

			// �ش� ���� idx �ο��ϱ�
			sendData.Format(_T("\\x80x0|/|0|/|%u|/|%u"), crypt_info.m_publicKey, crypt_info.m_prime);
			SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);

			// Ű ����
			m_CryptInfo[pSock] = crypt_info;
			return;
		}

		//if (token[2] == _T("test") || token[2] == _T("q")) {
		//	if (token[3] == _T("test")) {
		//		// �α��� ����

		//		CString strt;
		//		CTime t = CTime::GetCurrentTime();
		//		strt = token[2] + _T("���� �����ϼ̽��ϴ�.") + t.Format(_T("(%Y.%m.%d %H:%M:%S)"));

		//		pList->AddString(strt);
		//		int index = pList->GetCount();
		//		pList->SetAnchorIndex(index-1);	

		//		pUser->AddString(token[2]);
		//		m_UserName[pSock] = token[2];

		//		// �ش� ���� idx �ο��ϱ�
		//		sendData.Format(_T("0|/|0|/|%s"), _T("����Ű"));
		//		pSock->Send(sendData, sendData.GetLength() * 2);

		//		SendRoomList();
		//		return;
		//	}
		//}
		// ����
		sendData = _T("\\x80x0|/|1");
		SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);
	}
	else if (token[1] == _T("2")) {	// ȸ������
		// token[2] : ���̵�
		// token[3] : ��й�ȣ
		int idx = RegisterQuery(token[2], token[3]);
		if (idx == 0) {
			// ���
			sendData = _T("\\x80x0|/|2|/|0");
			SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);
		}
		else {
			// �����
			sendData = _T("\\x80x0|/|2|/|1");
			SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);
		}
	}
	else if (token[1] == _T("3")) {	// ��й�ȣ ã��
		CString pass;
		int idx = PassHelpQuery(token[2], pass);
		if (idx == 0) {
			// ���
			sendData.Format(_T("\\x80x0|/|3|/|0|/|%s"), pass);
			SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);
		}
		else {
			// �����
			sendData = _T("\\x80x0|/|3|/|1");
			SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);
		}
	}
	else {						// �α׾ƿ�
		// ��� �� ��忡�� �ִ��� Ȯ���ϱ�
		// ��ü ��忡�� ���� (CloseClient())

		// DeleteUserList(token[2]);
	}
}

int CChatSerApp::CreateRoom(CClientSock* pSock, INT32 status, CString title, CString pass)
{
	if (m_RoomInfo[m_RoomIndex]) {
		return -1;	// error!
	}
	else {
		RoomInfo *pRoomInfo = new RoomInfo;
		pRoomInfo->m_Status = status;
		pRoomInfo->m_Title = title;
		pRoomInfo->m_Pass = pass;
		pRoomInfo->m_RoomIndex = m_RoomIndex;
		pRoomInfo->m_MasterAccount = pSock;
		pRoomInfo->m_Account.push_back(pSock);
		pRoomInfo->m_banAccount.clear();
		m_RoomInfo[m_RoomIndex] = pRoomInfo;
	}

	return 0;
}
void CChatSerApp::LobbyData(CClientSock* pSock, CString Data)
{
	// From: Ŭ���̾�Ʈ
	// 1 |/| ����:0 |/| ����:0,�����:1,�Ӹ�:2 |/| Ÿ��Ʋ |/| ��й�ȣ |/| (������ ��ȣȭ �� �� �� ����)
	// 1 |/| ����:1 |/| �� ��ȣ |/| (������ ��ȣȭ �� �� �� ����)
	// 1 |/| ����:2 |/| �� ��ȣ |/| (������ ��ȣȭ �� �� �� ����)
	// 1 |/| �븮��Ʈ:3 |/| ��ȣȭ

	// To: Ŭ���̾�Ʈ
	// 1 |/| ����:1,����:0 |/| �� ��ȣ |/| ����:1,���:0 |/| �г��� |/| (������ ��ȣȭ �� �� �� ����)
	CString token[6];
	int nPos = 0;

	nPos = strsplit(Data, CString("|/|"), token, 6);
	//for (int i = 0; i < nPos; i++) {
	//	str.Format(_T("[%d] %s"), i + 1, token[i]);
	//	MessageBox(NULL, str, _T(""), 0);
	//}

	CListBox *pList = (CListBox*)m_pMainWnd->GetDlgItem(IDC_LIST_MSG);		// ��� �޽���

	CString sendData;
	if (token[1] == _T("0")) {	// ����
		// token[2] : ����:0,�����:1
		// token[3] : Ÿ��Ʋ
		// token[4] : ��й�ȣ
		// token[5] : (������ ��ȣȭ �� �� �� ����)
		if (CreateRoom(pSock, _ttoi(token[2]), token[3], token[4]) == -1)
			return;

		sendData.Format(_T("\\x80x1|/|0|/|%u|/|0|/|%s|/|(������ ��ȣȭ �� �� �� ����)"), m_RoomIndex, m_UserName[pSock]);
		SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);

		// ����!
		m_RoomIndex++;

		SendRoomList();
	}
	else if (token[1] == _T("1")) {	// ����
		// token[2] : ��ȭ��û:1, ����:0
		// token[3] : �� ��ȣ
		// token[4] : �г���
		// token[5] : ��ȣȭ
		if (token[2] == _T("0")) {
			UINT32 nIndex = _ttoi(token[3]);
			RoomInfo *pRoomInfo = m_RoomInfo[nIndex];
			if (pRoomInfo) {
				std::vector<CString>::iterator iter;
				iter = find(pRoomInfo->m_banAccount.begin(), pRoomInfo->m_banAccount.end(), m_UserName[pSock]);
				if (iter == pRoomInfo->m_banAccount.end()) {
					pRoomInfo->m_Account.push_back(pSock);
					// 1 |/| ����:1 |/| ��ȭ:1,���:0 |/| �� ��ȣ |/| (������ ��ȣȭ �� �� �� ����)
					sendData.Format(_T("\\x80x1|/|1|/|0|/|%u|/|%s|/|(������ ��ȣȭ �� �� �� ����)"), pRoomInfo->m_RoomIndex, m_UserName[pSock]);
					SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);
				}
				else {
					sendData.Format(_T("\\x80x1|/|1|/|3"));
					SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);
				}
			}
		}
		else if (token[2] == _T("1")) {
			CClientSock *pt;
			POSITION fpos, pos = m_UserList.GetHeadPosition();
			sendData.Format(_T("\\x80x1|/|1|/|1|/|%s|/|%s|/|(������ ��ȣȭ �� �� �� ����)"), token[3], m_UserName[pSock]);
			while (pos)
			{
				fpos = pos;
				pt = (CClientSock*)m_UserList.GetNext(pos);
				if (pt != pSock) {
					SendData(pt, sendData);	// pt->Send(sendData, sendData.GetLength() * 2);
				}
			}
		}
	}
	else if (token[1] == _T("2")) {	// ����
		//token[2] : �� ��ȣ
		//token[3] : (������ ��ȣȭ �� �� �� ����)
		CloseRoom(pSock, _ttoi(token[2]));
	}
	else if (token[1] == _T("3")) { // �� ����Ʈ ��û
		SendRoomList();
	}
}

void CChatSerApp::RoomData(CClientSock* pSock, CString Data)
{
	// 2 |/| �� ��ȣ |/| �Է� ���� |/| ������ ��ȣȭ �� (clnt)
	// 2 |/| �� ��ȣ |/| �� �� ��  |/| �Է� ���� + �ð�(serv)
	CString token[6];
	int nPos = 0;
	BOOL bAllSay = TRUE;

	nPos = strsplit(Data, CString("|/|"), token, 6);
	//for (int i = 0; i < nPos; i++) {
	//	str.Format(_T("[%d] %s"), i + 1, token[i]);
	//	MessageBox(NULL, str, _T(""), 0);
	//}

	if (! m_RoomInfo[_ttoi(token[1])]) {
		return;
	}

	CString Command;
	Command = token[2].GetAt(0);
	
	// �Ӹ��� �ƾ� ��ɾ� �ʿ� ����.
	if (m_RoomInfo[_ttoi(token[1])]->m_Status != ROOM_WHISPER) {
		if (Command == _T("/")) {
			CString cmd_token[2];
			int idx = strsplit(token[2], CString(" "), cmd_token, 2);

			//cmd_token[0] : ��ɾ�
			//cmd_token[1] : �г���
			if (cmd_token[0] == _T("/h")) {
				// ����
				CString sendData;
				token[2] = "/h : ����\n/w �г��� : �Ӹ�\n/k �г��� : �߹�\n/b �г��� : ��\n/bl : �� ����Ʈ\n/c �г��� : �� ����\n/l: �� ���� ����Ʈ\n/ll : �� ���� �ο� ��";
				sendData.Format(_T("\\x80x2|/|%s|/|[�˸�]|/|%s"), token[1], token[2]);
				SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);
			}
			else if (cmd_token[0] == _T("/w")) {
				// ���濡�� �Ӹ�
				// ���� �濡 ��밡 �ִ��� Ȯ��
				// ������ ���ٰ� �˸���
				// ������ �״�� ��  ��

				RoomInfo *pRoomInfo = m_RoomInfo[_ttoi(token[1])];

				if (pRoomInfo) {
					BOOL bInvalidUser = TRUE;
					CClientSock *pTarget = NULL;
					std::vector<CClientSock *>::iterator iter;
					CString sendData;

					for (iter = pRoomInfo->m_Account.begin(); iter != pRoomInfo->m_Account.end(); iter++) {
						pTarget = (*iter);
						if (m_UserName[pTarget] == cmd_token[1]) {
							// send: 3 |/| 0: ���� |/| �� �� �� |/| ��ȣȭ
							bInvalidUser = FALSE;

							if (m_UserName[pSock] == m_UserName[pTarget]) {
								break;
							}
							// �� �����ϱ�
							//    - �� ������, �Ϲ� ��� �Ӹ� ���� �����Ͽ� ����Ʈ ���۽� ����! (�Ϸ�)
							//    - �Ӹ� ���� ��ɾ� ��� ���ϰ� �ϱ�. (�Ϸ�)
							//    - �����Ͱ� �������� ������, ���� 2�� ���� ���� (�Ϸ�)
							//    - �Ӹ��� �� ��û�� �����ϴ� ���� �ִµ� �������� ���� ����� ������
							//      �ش� ����� ��û.

							sendData = m_UserName[pSock] + m_UserName[pTarget];

							if (m_WhispRoomInfo[sendData]) {	// �Ӹ����� ������
								// ������� �ʴ��ϱ�
								switch (m_WhispRoomInfo[sendData]->m_Account.size()) {
									//case 2: // �ʴ��� �ʿ䵵 ���� ������ ���� �ʿ䵵 ����.
										//break;
								case 1: // 1�� �ʴ��� ����.
									if (m_UserName[m_WhispRoomInfo[sendData]->m_Account.at(0)] == m_UserName[pSock]) {
										// ��뿡��

										// ����� �ֱ�
										m_RoomInfo[m_WhispRoomInfo[sendData]->m_RoomIndex]->m_Account.push_back(pTarget);

										sendData.Format(_T("\\x80x3|/|0|/|%u|/|%s|/|��ȣȭ"), m_WhispRoomInfo[sendData]->m_RoomIndex, m_UserName[pSock]);
										SendData(pTarget, sendData);	// pTarget->Send(sendData, sendData.GetLength() * 2);
									}
									else {	// �̰͵� ���ٵ�...? ��밡 �� ���� �ʴ�����..?
										// ������

										// ����� �ֱ�
										m_RoomInfo[m_WhispRoomInfo[sendData]->m_RoomIndex]->m_Account.push_back(pSock);

										sendData.Format(_T("\\x80x3|/|0|/|%u|/|%s|/|��ȣȭ"), m_WhispRoomInfo[sendData]->m_RoomIndex, m_UserName[pTarget]);
										SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);
									}
									break;
								case 0:	// ����? ���ٵ� �Ѥ�..
									m_WhispRoomInfo.erase(sendData);	// ������ ����
									break;
								}
							}
							else {								// �Ӹ����� �������� ����
								if (CreateRoom(pSock, ROOM_WHISPER, _T("Ÿ��Ʋ"), _T("��й�ȣ")) == -1)
									return;

								// �Ӹ��濡 ������ �ֱ�
								m_WhispRoomInfo[sendData] = m_RoomInfo[m_RoomIndex];

								// ��� ����� �ֱ�
								m_RoomInfo[m_RoomIndex]->m_Account.push_back(pTarget);

								// �г��� �ֱ�
								m_RoomInfo[m_RoomIndex]->m_WhisperAccount.push_back(m_UserName[pSock]);
								m_RoomInfo[m_RoomIndex]->m_WhisperAccount.push_back(m_UserName[pTarget]);

								// ��ɾ� ��� ����!
								m_RoomInfo[m_RoomIndex]->m_MasterAccount = NULL;

								// ������
								sendData.Format(_T("\\x80x3|/|0|/|%u|/|%s|/|��ȣȭ"), m_RoomIndex, m_UserName[pTarget]);
								SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);

								// ��뿡��
								sendData.Format(_T("\\x80x3|/|0|/|%u|/|%s|/|��ȣȭ"), m_RoomIndex, m_UserName[pSock]);
								SendData(pTarget, sendData);	// pTarget->Send(sendData, sendData.GetLength() * 2);

								// ����!
								m_RoomIndex++;
							}
							break;
						}
					}

					if (bInvalidUser) {
						sendData.Format(_T("\\x80x2|/|%s|/|[�˸�]|/|������ �������� �ʽ��ϴ�."), token[1]);
						SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);
					}
				}
				bAllSay = FALSE;
			}
			else if (cmd_token[0] == _T("/k")) {
				// ���� �濡 ��밡 �ִ��� Ȯ��
				// ������ ���ٰ� �˸���
				// ������ �״�� �������� �˸���
				bAllSay = FALSE;
				RoomInfo *pRoomInfo = m_RoomInfo[_ttoi(token[1])];

				if (pRoomInfo) {
					BOOL bInvalidUser = TRUE;
					CClientSock *pTarget = NULL;
					std::vector<CClientSock *>::iterator iter;
					CString sendData;

					if (pRoomInfo->m_MasterAccount != pSock) {
						return;
					}

					for (iter = pRoomInfo->m_Account.begin(); iter != pRoomInfo->m_Account.end(); iter++) {
						pTarget = (*iter);
						if (m_UserName[pTarget] == cmd_token[1]) {
							// pTarget �� ��¥�� ������ ��������. (�Ϸ�)
							sendData.Format(_T("\\x80x2|/|-1|/|%s"), token[1]);
							SendData(pTarget, sendData);	// pTarget->Send(sendData, sendData.GetLength() * 2);

							// CloseRoom(pTarget, pRoomInfo->m_RoomIndex);
							break;
						}
					}
				}
			}
			else if (cmd_token[0] == _T("/b")) {
				// ���� �濡 ��밡 �ִ��� Ȯ��
				// ������ ���ٰ� �˸���
				// ������ �г��� ��� �� �˸���
				bAllSay = FALSE;
				RoomInfo *pRoomInfo = m_RoomInfo[_ttoi(token[1])];

				if (pRoomInfo) {
					BOOL bInvalidUser = TRUE;
					CClientSock *pTarget = NULL;
					std::vector<CClientSock *>::iterator iter;
					CString sendData;

					if (pRoomInfo->m_MasterAccount != pSock) {
						return;
					}

					for (iter = pRoomInfo->m_Account.begin(); iter != pRoomInfo->m_Account.end(); iter++) {
						pTarget = (*iter);
						if (m_UserName[pTarget] == cmd_token[1]) {
							std::vector<CString>::iterator v_iter;
							v_iter = find(pRoomInfo->m_banAccount.begin(), pRoomInfo->m_banAccount.end(), cmd_token[1]);
							if (v_iter == pRoomInfo->m_banAccount.end()) {
								bInvalidUser = FALSE;
								pRoomInfo->m_banAccount.push_back(cmd_token[1]);
							}

							sendData.Format(_T("\\x80x2|/|%s|/|[�˸�]|/|�� ��ϵǾ����ϴ�."), token[1]);
							SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);
							break;
						}
					}

					if (bInvalidUser) {
						sendData.Format(_T("\\x80x2|/|%s|/|[�˸�]|/|�ش� ������ �������� �ʽ��ϴ�."), token[1]);
						SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);
					}
				}
			}
			else if (cmd_token[0] == _T("/bl")) {
				// ���� ��ϵ� ��� �˷��ֱ�
				bAllSay = FALSE;
				RoomInfo *pRoomInfo = m_RoomInfo[_ttoi(token[1])];

				if (pRoomInfo->m_MasterAccount != pSock) {
					return;
				}

				if (pRoomInfo) {
					CString pTargetString;
					std::vector<CString>::iterator iter;
					CString sendData;
					CString userNameList;
					for (iter = pRoomInfo->m_banAccount.begin(); iter != pRoomInfo->m_banAccount.end(); iter++) {
						pTargetString = (*iter);
						userNameList = userNameList + pTargetString + _T("\n");
					}

					sendData.Format(_T("\\x80x2|/|%s|/|[�� �ο�]|/|%s"), token[1], userNameList);
					SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);
				}
			}
			else if (cmd_token[0] == _T("/c")) {
				// �� ��Ͽ� ��밡 �ִ��� Ȯ��
				// ������ ���ٰ� �˸���
				// ������ �г��� ���� �� �˸���
				bAllSay = FALSE;
				RoomInfo *pRoomInfo = m_RoomInfo[_ttoi(token[1])];

				if (pRoomInfo->m_MasterAccount != pSock) {
					return;
				}

				if (pRoomInfo) {
					BOOL bInvalidUser = TRUE;
					CString pTargetString;
					std::vector<CString>::iterator iter;
					CString sendData;

					for (iter = pRoomInfo->m_banAccount.begin(); iter != pRoomInfo->m_banAccount.end(); iter++) {
						pTargetString = (*iter);
						if (pTargetString == cmd_token[1]) {
							bInvalidUser = FALSE;
							pRoomInfo->m_banAccount.erase(iter);
							sendData.Format(_T("\\x80x2|/|%s|/|[�˸�]|/|���� �Ǿ����ϴ�."), token[1]);
							SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);
							break;
						}
					}

					if (bInvalidUser) {
						sendData.Format(_T("\\x80x2|/|%s|/|[�˸�]|/|�ش� ������ ��ϵ��� �ʾҽ��ϴ�."), token[1]);
						SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);
					}
				}
			}
			else if (cmd_token[0] == _T("/l")) {
				// ���� �濡 �ִ� �ο� �˷��ֱ�
				bAllSay = FALSE;
				RoomInfo *pRoomInfo = m_RoomInfo[_ttoi(token[1])];

				if (pRoomInfo) {
					CClientSock *pTarget = NULL;
					std::vector<CClientSock *>::iterator iter;
					CString sendData;
					CString userNameList;
					for (iter = pRoomInfo->m_Account.begin(); iter != pRoomInfo->m_Account.end(); iter++) {
						pTarget = (*iter);
						userNameList = userNameList + m_UserName[pTarget] + _T("\n");
					}

					sendData.Format(_T("\\x80x2|/|%s|/|[���� �ο�]|/|%s"), token[1], userNameList);
					SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);
				}
			}
			else if (cmd_token[0] == _T("/ll")) {
				// ���� �濡 �ִ� �ο� �� �˷��ֱ�
				bAllSay = FALSE;
				RoomInfo *pRoomInfo = m_RoomInfo[_ttoi(token[1])];

				if (pRoomInfo) {
					std::vector<CClientSock *>::iterator iter;
					CString sendData;

					sendData.Format(_T("\\x80x2|/|%s|/|[�˸�]|/|���� %u�� �ֽ��ϴ�."), token[1], pRoomInfo->m_Account.size());
					SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);
				}
			}
		}
	}

	if (bAllSay) {
		RoomInfo *pRoomInfo = m_RoomInfo[_ttoi(token[1])];
		if (pRoomInfo) {
			//CString strt;
			//CTime t = CTime::GetCurrentTime();
			//strt = t.Format(_T("(%Y.%m.%d %H:%M:%S)"));

			CString sendData;
			sendData.Format(_T("\\x80x2|/|%s|/|%s|/|%s"), token[1], m_UserName[pSock], token[2]);
			CClientSock *pClntSock = NULL;
			std::vector<CClientSock *>::iterator iter;
			for (iter = pRoomInfo->m_Account.begin(); iter != pRoomInfo->m_Account.end(); iter++) {
				pClntSock = (*iter);
				if (pClntSock && pSock != pClntSock) {
					SendData(pClntSock, sendData);	// pClntSock->Send(sendData, sendData.GetLength() * 2);
				}
			}
		}
	}
}

void CChatSerApp::FriendData(CClientSock* pSock, CString Data)
{
	// 4 |/| 0: �߰���û   |/| �߰��� �г���
	// 4 |/| 1: ��    ��   |/| �߰��� �г���
	// 4 |/| 2: ��ȭ��û   |/| �г���
	// 4 |/| 3: ������û   |/| ������ �г���
	// 4 |/| 9: ģ�����   |/| �г���1, �г���2, ...
	CString token[6];
	int nPos = 0;

	nPos = strsplit(Data, CString("|/|"), token, 6);
	//for (int i = 0; i < nPos; i++) {
	//	str.Format(_T("[%d] %s"), i + 1, token[i]);
	//	MessageBox(NULL, str, _T(""), 0);
	//}

	if (token[1] == _T("0")) {
		if (token[2] == m_UserName[pSock])
			return;

		std::map<CClientSock *, CString>::iterator iter;
		CString targetName;
		for (iter = m_UserName.begin(); iter != m_UserName.end(); iter++) {
			targetName = (*iter).second;

			if (targetName == token[2]) {
				// �߰�
				if (FriendQuery(m_UserName[pSock], token[2]) == 0) {
					FriendQuery(token[2], m_UserName[pSock]);

					CString sendData;
					// ������
					sendData.Format(_T("\\x80x4|/|1|/|%s"), token[2]);
					SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);

					// ��뿡��
					CClientSock *pTarget = (*iter).first;
					sendData.Format(_T("\\x80x4|/|1|/|%s"), m_UserName[pSock]);
					SendData(pTarget, sendData);	// pTarget->Send(sendData, sendData.GetLength() * 2);
				}
				break;
			}
		}
	}
	else if (token[1] == _T("2")) {
		// token[2] : �г���

		if (m_UserName[pSock] == token[2]) {
			return;
		}

		CClientSock *pTarget;
		POSITION fpos, pos = m_UserList.GetHeadPosition();

		CString sendData;
		while (pos)
		{
			fpos = pos;
			pTarget = (CClientSock*)m_UserList.GetNext(pos);
			if (m_UserName[pTarget] == token[2])
			{
				// �� �����ϱ�
				//    - �� ������, �Ϲ� ��� �Ӹ� ���� �����Ͽ� ����Ʈ ���۽� ����! (�Ϸ�)
				//    - �Ӹ� ���� ��ɾ� ��� ���ϰ� �ϱ�. (�Ϸ�)
				//    - �����Ͱ� �������� ������, ���� 2�� ���� ���� (�Ϸ�)
				//    - �Ӹ��� �� ��û�� �����ϴ� ���� �ִµ� �������� ���� ����� ������
				//      �ش� ����� ��û.

				sendData = m_UserName[pSock] + m_UserName[pTarget];
				if (m_WhispRoomInfo[sendData] != NULL) {	// �Ӹ����� ������
													// ������� �ʴ��ϱ�
					int num = m_WhispRoomInfo[sendData]->m_Account.size();
					switch (num) {
						//case 2: // �ʴ��� �ʿ䵵 ���� ������ ���� �ʿ䵵 ����.
						//break;
					case 1: // 1�� �ʴ��� ����.
						if (m_UserName[m_WhispRoomInfo[sendData]->m_Account.at(0)] == m_UserName[pSock]) {
							// ��뿡��

							// ����� �ֱ�
							m_RoomInfo[m_WhispRoomInfo[sendData]->m_RoomIndex]->m_Account.push_back(pTarget);

							sendData.Format(_T("\\x80x3|/|0|/|%u|/|%s|/|��ȣȭ"), m_WhispRoomInfo[sendData]->m_RoomIndex, m_UserName[pSock]);
							SendData(pTarget, sendData);	// pTarget->Send(sendData, sendData.GetLength() * 2);
						}
						else {	// �̰͵� ���ٵ�...? ��밡 �� ���� �ʴ�����..?
								// ������

								// ����� �ֱ�
							m_RoomInfo[m_WhispRoomInfo[sendData]->m_RoomIndex]->m_Account.push_back(pSock);

							sendData.Format(_T("\\x80x3|/|0|/|%u|/|%s|/|��ȣȭ"), m_WhispRoomInfo[sendData]->m_RoomIndex, m_UserName[pTarget]);
							SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);
						}
						break;
					case 0:	// ����? ���ٵ� �Ѥ�..
						m_WhispRoomInfo.erase(sendData);	// ������ ����
						break;
					}
				}
				else {								// �Ӹ����� �������� ����
					if (CreateRoom(pSock, ROOM_WHISPER, _T("Ÿ��Ʋ"), _T("��й�ȣ")) == -1)
						return;

					// �Ӹ��濡 ������ �ֱ�
					m_WhispRoomInfo[sendData] = m_RoomInfo[m_RoomIndex];

					// ��� ����� �ֱ�
					m_RoomInfo[m_RoomIndex]->m_Account.push_back(pTarget);

					// �г��� �ֱ�
					m_RoomInfo[m_RoomIndex]->m_WhisperAccount.push_back(m_UserName[pSock]);
					m_RoomInfo[m_RoomIndex]->m_WhisperAccount.push_back(m_UserName[pTarget]);

					// ��ɾ� ��� ����!
					m_RoomInfo[m_RoomIndex]->m_MasterAccount = NULL;

					// ������
					sendData.Format(_T("\\x80x3|/|0|/|%u|/|%s|/|��ȣȭ"), m_RoomIndex, m_UserName[pTarget]);
					SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);

					// ��뿡��
					sendData.Format(_T("\\x80x3|/|0|/|%u|/|%s|/|��ȣȭ"), m_RoomIndex, m_UserName[pSock]);
					SendData(pTarget, sendData);	// pTarget->Send(sendData, sendData.GetLength() * 2);

					// ����!
					m_RoomIndex++;
				}
				break;
			}
		}
	}
	else if (token[1] == _T("3")) {
		// ģ�� �����ϱ�
		if (FriendDelQuery(m_UserName[pSock], token[2]) == 0) {
			FriendDelQuery(token[2], m_UserName[pSock]);

			CString sendData;
			sendData.Format(_T("\\x80x3|/|3|/|%s"), token[2]);
			SendData(pSock, sendData);

			std::map<CClientSock *, CString>::iterator iter;
			CClientSock *pTarget = NULL;
			for (iter = m_UserName.begin(); iter != m_UserName.end(); iter++) {
				if ((*iter).second == token[2]) {
					pTarget = (*iter).first;
					sendData.Format(_T("\\x80x3|/|3|/|%s"), m_UserName[pSock]);
					SendData(pTarget, sendData);
					break;
				}
			}
		}
	}
	else if (token[1] == _T("9")) {
		// ģ�� ��� �ҷ�����
		CString totalUserName;
		FriendLoadQuery(m_UserName[pSock], totalUserName);

		if (!totalUserName.GetLength()) {
			return;
		}

		// ģ�� ��� ������
		CString sendData;
		sendData.Format(_T("\\x80x4|/|9|/|%s"), totalUserName);
		SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);
	}
}

void CChatSerApp::SendData(CClientSock *pSock, CString& strData)
{
	if (m_CryptInfo[pSock].m_publicKey > 0) {

		char sendDataConv[4096];
		long sendDataConv2[4096];

		strcpy_s(sendDataConv, sizeof(sendDataConv), CT2A(strData));

		SetPrime(m_CryptInfo[pSock].m_prime);
		Make_Cyper_text(sendDataConv, sendDataConv2, m_CryptInfo[pSock].m_privateKey);

		CString str;
		for (int i = 0; i < strlen(sendDataConv); i++) {
			if ((i + 1) == strlen(sendDataConv))
				str.Format(_T("%s%d"), str, sendDataConv2[i]);
			else
				str.Format(_T("%s%d "), str, sendDataConv2[i]);
		}

		pSock->Send(str, str.GetLength() * 2);
	}
	else {
		pSock->Send(strData, strData.GetLength() * 2);
	}
}

void CChatSerApp::ReceiveData(CClientSock* pSock)
{
	TCHAR temp[10240];
	memset(temp, 0, 10240);
	pSock->Receive(temp, sizeof temp);
	CString str;
	str = temp;

	crypt crypt_info = m_CryptInfo[pSock];
	if (crypt_info.m_publicKey > 0) {
		CString token[4096];
		int nPos = 0;

		nPos = strsplit(str, CString(" "), token, 4096);

		char sendDataConv[4096];
		long sendDataConv2[4096];

		for (int i = 0; i < nPos; i++) {
			sendDataConv2[i] = _ttoi(token[i]);
		}

		SetPrime(crypt_info.m_prime);
		Make_Plain_text(sendDataConv2, nPos, sendDataConv, crypt_info.m_privateKey);
		str = CA2T(sendDataConv); // .Format(_T("%s"), sendDataConv); // = CA2T(sendDataConv);
	}
	str.Trim();

	CString token[20];
	int nPos = 0;

	nPos = strsplit(str, CString(_T("\\x80x")), token, 20);
	//CListBox *pList = (CListBox*)m_pMainWnd->GetDlgItem(IDC_LIST_MSG);		// ��� �޽���
	//CListBox *pUser = (CListBox*)m_pMainWnd->GetDlgItem(IDC_LIST_USER);		// ��� ���� ����

	for (int i = 0; i < nPos; i++) {
		str = token[i];
		CString type = str.Left(1);
		if (type == _T("0")) {
			LoginData(pSock, str);
		}
		else if (type == _T("1")) {
			LobbyData(pSock, str);
		}
		else if (type == _T("2")) {
			RoomData(pSock, str);
		}
		else if (type == _T("3")) {
			// WhipserData(pSock, str);
		}
		else if (type == _T("4")) {
			FriendData(pSock, str);
		}
	}
	//pList->AddString(str);
	//int index = pList->GetCount();
	//pList->SetAnchorIndex(index-1);	

	//if(str.Left(1) == "<")
	//{
	//	int n = str.Find('>', 0);
	//	CString strt;
	//	CTime t= CTime::GetCurrentTime();
	//	strt = str.Mid(1, n-1) + t.Format(_T("(%d %H:%M)"));
	//	pUser->AddString(strt);

	//	CString ss = _T(""), tt;
	//	for(int i=0; i<pUser->GetCount(); i++)
	//	{
	//		pUser->GetText(i, tt);
	//		ss += tt + _T(",");
	//	}
	//	ss += _T("������");
	//	pSock->Send(ss, ss.GetLength()*2);
	//}
	//if(str.Left(1) == "(")
	//{
	//	int n = str.Find(')', 0); 
	//	CString s;
	//	s= str.Mid(1, n-1);
	//	int nFind = pUser->FindString(-1, s);
	//	pUser->DeleteString(nFind);
	//}

	//POSITION pos = m_list.GetHeadPosition();
	//CClientSock *pt;
	//while(pos)
	//{
	//	pt = (CClientSock*)m_list.GetNext(pos);
	//	if(pt != pSock)
	//	{
	//		pt->Send(str, str.GetLength()*2);
	//	}
	//}
	//m_pMainWnd->SetDlgItemInt(IDC_COUNT, pUser->GetCount());

}

void CChatSerApp::CloseRoom(CClientSock* pSock, UINT32 Index)
{
	// �� �� �˻�
	BOOL bMessage = FALSE;
	RoomInfo* pRoomInfo = NULL;
	std::vector<UINT32> dellist;
	pRoomInfo = m_RoomInfo[Index];
	if (pRoomInfo) {
		// ���� �濡 �� �ִ��� üũ
		std::vector<CClientSock *>::iterator v_iter;
		for (v_iter = pRoomInfo->m_Account.begin(); v_iter != pRoomInfo->m_Account.end(); v_iter++) {
			if (pSock == (*v_iter)) {
				pRoomInfo->m_Account.erase(v_iter);
				bMessage = TRUE;
				break;
			}
		}

		if (pRoomInfo->m_Account.size() > 0) {
			// �濡 ���µ� �����Ͱ� ������ ���� ������� Turn �ѱ��
			if (pRoomInfo->m_MasterAccount == pSock) {
				pRoomInfo->m_MasterAccount = pRoomInfo->m_Account.at(0);
			}

			if (bMessage) {
				CString sendData;
				sendData.Format(_T("\\x80x1|/|1|/|2|/|%u|/|%s|/|��ȣȭ"), pRoomInfo->m_RoomIndex, m_UserName[pSock]);
				for (v_iter = pRoomInfo->m_Account.begin(); v_iter != pRoomInfo->m_Account.end(); v_iter++) {
					SendData((*v_iter), sendData);	// (*v_iter)->Send(sendData, sendData.GetLength() * 2);
				}
			}
		}
		else {
			// ���� �ƹ� ����� ������ �� �����ϱ�
			dellist.push_back(Index);
		}
	}

	INT32 index;
	CString str;
	for (int i = 0; i < dellist.size(); i++) {
		index = dellist.at(i);	// m_RoomInfo �� Key
		if (m_RoomInfo[index]->m_Status == ROOM_WHISPER) {
			// m_WhispRoomInfo �� Key
			str = m_RoomInfo[index]->m_WhisperAccount.at(0) + m_RoomInfo[index]->m_WhisperAccount.at(1);

			m_RoomInfo[index]->m_WhisperAccount.clear();
			m_WhispRoomInfo.erase(str);
		}
		m_RoomInfo.erase(index);
	}
	if (dellist.size() > 0) {
		SendRoomList();
	}
}

void CChatSerApp::CloseClient(CClientSock* pSock)
{
	// �� �� �˻�
	BOOL bMessage = FALSE;
	RoomInfo* pRoomInfo = NULL;
	std::vector<UINT32> dellist;
	std::map<UINT32, RoomInfo*>::iterator iter;
	for (iter = m_RoomInfo.begin(); iter != m_RoomInfo.end(); iter++) {
		pRoomInfo = (*iter).second;
		bMessage = FALSE;

		if (pRoomInfo) {
			// ���� �濡 �� �ִ��� üũ
			std::vector<CClientSock *>::iterator v_iter;
			for (v_iter = pRoomInfo->m_Account.begin(); v_iter != pRoomInfo->m_Account.end(); v_iter++) {
				if (pSock == (*v_iter)) {
					pRoomInfo->m_Account.erase(v_iter);
					bMessage = TRUE;
					break;
				}
			}
			
			if (pRoomInfo->m_Account.size() > 0) {
				// �濡 ���µ� �����Ͱ� ������ ���� ������� Turn �ѱ��
				if (pRoomInfo->m_MasterAccount == pSock) {
					pRoomInfo->m_MasterAccount = pRoomInfo->m_Account.at(0);
				}

				if (bMessage) {
					CString sendData;
					sendData.Format(_T("\\x80x1|/|1|/|2|/|%u|/|%s|/|��ȣȭ"), pRoomInfo->m_RoomIndex, m_UserName[pSock]);
					for (v_iter = pRoomInfo->m_Account.begin(); v_iter != pRoomInfo->m_Account.end(); v_iter++) {
						SendData((*v_iter), sendData);	// (*v_iter)->Send(sendData, sendData.GetLength() * 2);
					}
				}
			}
			else {
				// ���� �ƹ� ����� ������ �� �����ϱ�
				dellist.push_back((*iter).first);
			}
		}
	}

	INT32 index;
	CString str;
	for (int i = 0; i < dellist.size(); i++) {
		index = dellist.at(i);	// m_RoomInfo �� Key
		if (m_RoomInfo[index]->m_Status == ROOM_WHISPER) {
			// m_WhispRoomInfo �� Key
			str = m_RoomInfo[index]->m_WhisperAccount.at(0) + m_RoomInfo[index]->m_WhisperAccount.at(1);

			m_RoomInfo[index]->m_WhisperAccount.clear();
			m_WhispRoomInfo.erase(str);
		}
		m_RoomInfo.erase(index);
	}
	if (dellist.size() > 0) {
		SendRoomList();
	}

	// ������ �˻�
	if (m_UserName[pSock]) {
		// �α׾ƿ� ���� ����
		UpdateQuery(2, m_UserName[pSock], 0);
		// �̸� �����
		DeleteUserList(m_UserName[pSock]);
		m_UserName.erase(pSock);
	}

	// ���� Ű ����
	m_CryptInfo.erase(pSock);

	CClientSock *pt;
	POSITION fpos, pos = m_UserList.GetHeadPosition();  // ��
	while(pos)
	{
		fpos = pos;  //  ��
		pt = (CClientSock*)m_UserList.GetNext(pos);  //  ��, ��
		if(pt == pSock)
		{
			pSock->Close();
			delete pSock;  // ��
			m_UserList.RemoveAt(fpos); // ��
			break;
		}
	}
}

void CChatSerApp::SendRoomList(void)
{
	// Socket Data Head
	if (m_RoomInfo.size() > 0) {
		CString sendData, Data;
		sendData.Format(_T("\\x80x1|/|2|/|%d|/|"), m_RoomInfo.size());

		RoomInfo *pRoomInfo = NULL;
		int cnt = 0;
		std::map<UINT32, RoomInfo*>::iterator iter;
		for (iter = m_RoomInfo.begin(); iter != m_RoomInfo.end(); iter++, cnt++) {
			pRoomInfo = (*iter).second;
			
			// �Ӹ� ���̸� ����Ʈ ��Ͽ��� ����
			if (pRoomInfo->m_Status == ROOM_WHISPER)
				continue;

			if ((cnt + 1) == m_RoomInfo.size())
				Data.Format(_T("%d,%s,%s,|/|"), pRoomInfo->m_RoomIndex, pRoomInfo->m_Title, m_UserName[pRoomInfo->m_MasterAccount]);
			else
				Data.Format(_T("%d,%s,%s,"), pRoomInfo->m_RoomIndex, pRoomInfo->m_Title, m_UserName[pRoomInfo->m_MasterAccount]);
			sendData.Append(Data);
		}

		CClientSock *pt;
		POSITION fpos, pos = m_UserList.GetHeadPosition();
		while (pos)
		{
			fpos = pos;
			pt = (CClientSock*)m_UserList.GetNext(pos);
			// �ʹ� ���� ������ �ν��� ����!
			Sleep(1);
			SendData(pt, sendData);	// pt->Send(sendData, sendData.GetLength() * 2);
		}
	}
}