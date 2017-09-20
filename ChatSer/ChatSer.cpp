
// ChatSer.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
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


// CChatSerApp 생성

CChatSerApp::CChatSerApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	m_pServer = NULL;
	m_RoomIndex = 1;
}


// 유일한 CChatSerApp 개체입니다.

CChatSerApp theApp;


// CChatSerApp 초기화

BOOL CChatSerApp::InitInstance()
{
	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다.
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));

	if (Sql_Initialize()) {
		AfxMessageBox(_T("SQL 초기화 에러!"));
		return FALSE;
	}

	CChatSerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 여기에 [확인]을 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 여기에 [취소]를 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}

	Sql_Finish();

	// 대화 상자가 닫혔으므로 응용 프로그램의 메시지 펌프를 시작하지 않고  응용 프로그램을 끝낼 수 있도록 FALSE를
	// 반환합니다.
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
	m_UserList.AddTail(pClient);	// 모든 접속 유저
}

/*
	* 로그인 서버
		-. 0 / 아이디 / 비밀번호 (clnt)
		-. 0 / 계정확인 / 공개키 생성 (serv), 비밀키 서버 저장
		-. 로그인 계정이 있는지 없는지 확인
	* 로비 서버
		-. 1 / 룸 번호 / (원본과 암호화 값 두 개 전송)
		-. 어디를 누르는지에 따라 룸으로 보내기.
		-. 친구 목록 불러오기
	* 룸 서버 (따로)
		-. 2 / 룸 번호 / 입력 내용 / 원본과 암호화 값 (clnt)
		-. 2 / 룸 번호 / 닉네임 / 입력 내용 + 시간 (serv)
*/

void CChatSerApp::DeleteUserList(CString name)
{
	CListBox *pList = (CListBox*)m_pMainWnd->GetDlgItem(IDC_LIST_MSG);		// 모든 메시지
	CListBox *pUser = (CListBox*)m_pMainWnd->GetDlgItem(IDC_LIST_USER);		// 모든 접속 유저

	CString strt;
	CTime t = CTime::GetCurrentTime();
	if (name == _T("")) {
		return;
	}
	strt = name + _T("님이 종료하셨습니다.") + t.Format(_T("(%Y.%m.%d %H:%M:%S)"));

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
				// 계정이 맞음
				Sql_Free_Result();
				return 1;
			}
			else {
				// 계정은 맞으나 비밀번호가 틀림
				Sql_Free_Result();
				return 0;
			}
		}
	}
	// 계정도 비밀번호도 모름
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

		// 비밀번호 옮기기
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

	if (type & 1) {	// 시간 업데이트
		memset(query, 0, sizeof(query));
		DWORD tick = time(NULL);
		sprintf_s(query, sizeof(query), "UPDATE `account` SET `time`='%u' WHERE `id`='%s'", tick, char_id);

		if (Sql_UpdateQuery(query)) {
			return -1;
		}
	}
	if (type & 2) { // 로그인 업데이트
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
		// 이미 계정이 존재함
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
		// 이미 계정이 존재함
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
		// 이미 계정이 존재함
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
	// 0 |/| 로그인:0 / 로그아웃:1 |/| 아 이 디 |/| 비밀번호
	// 0 |/| 회원가입:2            |/| 아 이 디 |/| 비밀번호
	// 0 |/| 아이디 찾기:3         |/| 아 이 디
	// 0 |/| 성공:0   / 실패:1     |/| 공개키 생성(serv), 비밀키 서버 저장 |/| 소수
	CString token[4];
	int nPos = 0;

	nPos = strsplit(Data, CString("|/|"), token, 4);
	//for (int i = 0; i < nPos; i++) {
	//	str.Format(_T("[%d] %s"), i + 1, token[i]);
	//	MessageBox(NULL, str, _T(""), 0);
	//}

	CListBox *pList = (CListBox*)m_pMainWnd->GetDlgItem(IDC_LIST_MSG);		// 모든 메시지
	CListBox *pUser = (CListBox*)m_pMainWnd->GetDlgItem(IDC_LIST_USER);		// 모든 접속 유저

	// MessageBox(NULL, token[1] + _T(", ") + token[2], _T("알림"), 0);

	CString sendData;
	if (token[1] == _T("0")) {	// 로그인
		// token[2] : 아이디
		// token[3] : 비밀번호
		if (LoginQuery(token[2], token[3]) == 1) {
			// 로그인 성공

			// 로그인한 사람이 있는지 검사
			CString connectUserName;
			std::map<CClientSock *, CString>::iterator iter;
			for (iter = m_UserName.begin(); iter != m_UserName.end(); iter++) {
				connectUserName = iter->second;
				if (token[2] == (CString)connectUserName) {
					CloseClient(iter->first);
					break;
				}
			}

			// 로그인, 시간 쿼리 업데이트
			UpdateQuery(3, token[2], 1);

			CString strt;
			CTime t = CTime::GetCurrentTime();
			strt = token[2] + _T("님이 접속하셨습니다.") + t.Format(_T("(%Y.%m.%d %H:%M:%S)"));

			pList->AddString(strt);
			int index = pList->GetCount();
			pList->SetAnchorIndex(index - 1);

			pUser->AddString(token[2]);
			m_UserName[pSock] = token[2];

			// 공개 키 생성하기
			crypt crypt_info;
			crypt_info.m_primeNumber = Make_Random_Prime_Number();
			crypt_info.m_publicKey = Make_Public_Key(crypt_info.m_primeNumber);
			crypt_info.m_privateKey = Make_Private_Key(crypt_info.m_publicKey, crypt_info.m_primeNumber);
			crypt_info.m_prime = GetPrime();

			//CString str;
			//str.Format(_T("소수: %d\n공개키: %d"), m_primeNumber, m_publicKey);
			//MessageBox(NULL, str, _T(""), 0);

			// 해당 소켓 idx 부여하기
			sendData.Format(_T("\\x80x0|/|0|/|%u|/|%u"), crypt_info.m_publicKey, crypt_info.m_prime);
			SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);

			// 키 삽입
			m_CryptInfo[pSock] = crypt_info;
			return;
		}

		//if (token[2] == _T("test") || token[2] == _T("q")) {
		//	if (token[3] == _T("test")) {
		//		// 로그인 성공

		//		CString strt;
		//		CTime t = CTime::GetCurrentTime();
		//		strt = token[2] + _T("님이 접속하셨습니다.") + t.Format(_T("(%Y.%m.%d %H:%M:%S)"));

		//		pList->AddString(strt);
		//		int index = pList->GetCount();
		//		pList->SetAnchorIndex(index-1);	

		//		pUser->AddString(token[2]);
		//		m_UserName[pSock] = token[2];

		//		// 해당 소켓 idx 부여하기
		//		sendData.Format(_T("0|/|0|/|%s"), _T("공개키"));
		//		pSock->Send(sendData, sendData.GetLength() * 2);

		//		SendRoomList();
		//		return;
		//	}
		//}
		// 실패
		sendData = _T("\\x80x0|/|1");
		SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);
	}
	else if (token[1] == _T("2")) {	// 회원가입
		// token[2] : 아이디
		// token[3] : 비밀번호
		int idx = RegisterQuery(token[2], token[3]);
		if (idx == 0) {
			// 허용
			sendData = _T("\\x80x0|/|2|/|0");
			SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);
		}
		else {
			// 비허용
			sendData = _T("\\x80x0|/|2|/|1");
			SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);
		}
	}
	else if (token[1] == _T("3")) {	// 비밀번호 찾기
		CString pass;
		int idx = PassHelpQuery(token[2], pass);
		if (idx == 0) {
			// 허용
			sendData.Format(_T("\\x80x0|/|3|/|0|/|%s"), pass);
			SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);
		}
		else {
			// 비허용
			sendData = _T("\\x80x0|/|3|/|1");
			SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);
		}
	}
	else {						// 로그아웃
		// 모든 룸 노드에서 있는지 확인하기
		// 전체 노드에서 제거 (CloseClient())

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
	// From: 클라이언트
	// 1 |/| 생성:0 |/| 공개:0,비공개:1,귓말:2 |/| 타이틀 |/| 비밀번호 |/| (원본과 암호화 값 두 개 전송)
	// 1 |/| 입장:1 |/| 룸 번호 |/| (원본과 암호화 값 두 개 전송)
	// 1 |/| 퇴장:2 |/| 룸 번호 |/| (원본과 암호화 값 두 개 전송)
	// 1 |/| 룸리스트:3 |/| 암호화

	// To: 클라이언트
	// 1 |/| 입장:1,생성:0 |/| 룸 번호 |/| 입장:1,허용:0 |/| 닉네임 |/| (원본과 암호화 값 두 개 전송)
	CString token[6];
	int nPos = 0;

	nPos = strsplit(Data, CString("|/|"), token, 6);
	//for (int i = 0; i < nPos; i++) {
	//	str.Format(_T("[%d] %s"), i + 1, token[i]);
	//	MessageBox(NULL, str, _T(""), 0);
	//}

	CListBox *pList = (CListBox*)m_pMainWnd->GetDlgItem(IDC_LIST_MSG);		// 모든 메시지

	CString sendData;
	if (token[1] == _T("0")) {	// 생성
		// token[2] : 공개:0,비공개:1
		// token[3] : 타이틀
		// token[4] : 비밀번호
		// token[5] : (원본과 암호화 값 두 개 전송)
		if (CreateRoom(pSock, _ttoi(token[2]), token[3], token[4]) == -1)
			return;

		sendData.Format(_T("\\x80x1|/|0|/|%u|/|0|/|%s|/|(원본과 암호화 값 두 개 전송)"), m_RoomIndex, m_UserName[pSock]);
		SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);

		// 주의!
		m_RoomIndex++;

		SendRoomList();
	}
	else if (token[1] == _T("1")) {	// 입장
		// token[2] : 대화요청:1, 입장:0
		// token[3] : 룸 번호
		// token[4] : 닉네임
		// token[5] : 암호화
		if (token[2] == _T("0")) {
			UINT32 nIndex = _ttoi(token[3]);
			RoomInfo *pRoomInfo = m_RoomInfo[nIndex];
			if (pRoomInfo) {
				std::vector<CString>::iterator iter;
				iter = find(pRoomInfo->m_banAccount.begin(), pRoomInfo->m_banAccount.end(), m_UserName[pSock]);
				if (iter == pRoomInfo->m_banAccount.end()) {
					pRoomInfo->m_Account.push_back(pSock);
					// 1 |/| 입장:1 |/| 대화:1,허용:0 |/| 룸 번호 |/| (원본과 암호화 값 두 개 전송)
					sendData.Format(_T("\\x80x1|/|1|/|0|/|%u|/|%s|/|(원본과 암호화 값 두 개 전송)"), pRoomInfo->m_RoomIndex, m_UserName[pSock]);
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
			sendData.Format(_T("\\x80x1|/|1|/|1|/|%s|/|%s|/|(원본과 암호화 값 두 개 전송)"), token[3], m_UserName[pSock]);
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
	else if (token[1] == _T("2")) {	// 퇴장
		//token[2] : 룸 번호
		//token[3] : (원본과 암호화 값 두 개 전송)
		CloseRoom(pSock, _ttoi(token[2]));
	}
	else if (token[1] == _T("3")) { // 룸 리스트 요청
		SendRoomList();
	}
}

void CChatSerApp::RoomData(CClientSock* pSock, CString Data)
{
	// 2 |/| 룸 번호 |/| 입력 내용 |/| 원본과 암호화 값 (clnt)
	// 2 |/| 룸 번호 |/| 닉 네 임  |/| 입력 내용 + 시간(serv)
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
	
	// 귓말은 아얘 명령어 필요 없음.
	if (m_RoomInfo[_ttoi(token[1])]->m_Status != ROOM_WHISPER) {
		if (Command == _T("/")) {
			CString cmd_token[2];
			int idx = strsplit(token[2], CString(" "), cmd_token, 2);

			//cmd_token[0] : 명령어
			//cmd_token[1] : 닉네임
			if (cmd_token[0] == _T("/h")) {
				// 도움말
				CString sendData;
				token[2] = "/h : 도움말\n/w 닉네임 : 귓말\n/k 닉네임 : 추방\n/b 닉네임 : 벤\n/bl : 벤 리스트\n/c 닉네임 : 벤 해제\n/l: 방 접속 리스트\n/ll : 방 접속 인원 수";
				sendData.Format(_T("\\x80x2|/|%s|/|[알림]|/|%s"), token[1], token[2]);
				SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);
			}
			else if (cmd_token[0] == _T("/w")) {
				// 상대방에게 귓말
				// 현재 방에 상대가 있는지 확인
				// 없으면 없다고 알리기
				// 있으면 그대로 전  달

				RoomInfo *pRoomInfo = m_RoomInfo[_ttoi(token[1])];

				if (pRoomInfo) {
					BOOL bInvalidUser = TRUE;
					CClientSock *pTarget = NULL;
					std::vector<CClientSock *>::iterator iter;
					CString sendData;

					for (iter = pRoomInfo->m_Account.begin(); iter != pRoomInfo->m_Account.end(); iter++) {
						pTarget = (*iter);
						if (m_UserName[pTarget] == cmd_token[1]) {
							// send: 3 |/| 0: 승인 |/| 닉 네 임 |/| 암호화
							bInvalidUser = FALSE;

							if (m_UserName[pSock] == m_UserName[pTarget]) {
								break;
							}
							// 방 생성하기
							//    - 방 생성시, 일반 방과 귓말 방을 구분하여 리스트 전송시 주의! (완료)
							//    - 귓말 방은 명령어 사용 못하게 하기. (완료)
							//    - 마스터가 존재하지 않으며, 유저 2명 만이 존재 (완료)
							//    - 귓말을 재 요청시 존재하는 방이 있는데 접속하지 않은 사람이 있으면
							//      해당 사람만 초청.

							sendData = m_UserName[pSock] + m_UserName[pTarget];

							if (m_WhispRoomInfo[sendData]) {	// 귓말방이 존재함
								// 나간사람 초대하기
								switch (m_WhispRoomInfo[sendData]->m_Account.size()) {
									//case 2: // 초대할 필요도 없고 데이터 쌓을 필요도 없다.
										//break;
								case 1: // 1명만 초대해 주자.
									if (m_UserName[m_WhispRoomInfo[sendData]->m_Account.at(0)] == m_UserName[pSock]) {
										// 상대에게

										// 사용자 넣기
										m_RoomInfo[m_WhispRoomInfo[sendData]->m_RoomIndex]->m_Account.push_back(pTarget);

										sendData.Format(_T("\\x80x3|/|0|/|%u|/|%s|/|암호화"), m_WhispRoomInfo[sendData]->m_RoomIndex, m_UserName[pSock]);
										SendData(pTarget, sendData);	// pTarget->Send(sendData, sendData.GetLength() * 2);
									}
									else {	// 이것도 버근데...? 상대가 왜 나를 초대하지..?
										// 나에게

										// 사용자 넣기
										m_RoomInfo[m_WhispRoomInfo[sendData]->m_RoomIndex]->m_Account.push_back(pSock);

										sendData.Format(_T("\\x80x3|/|0|/|%u|/|%s|/|암호화"), m_WhispRoomInfo[sendData]->m_RoomIndex, m_UserName[pTarget]);
										SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);
									}
									break;
								case 0:	// 뭐지? 버근데 ㅡㅡ..
									m_WhispRoomInfo.erase(sendData);	// 데이터 삭제
									break;
								}
							}
							else {								// 귓말방이 존재하지 않음
								if (CreateRoom(pSock, ROOM_WHISPER, _T("타이틀"), _T("비밀번호")) == -1)
									return;

								// 귓말방에 데이터 넣기
								m_WhispRoomInfo[sendData] = m_RoomInfo[m_RoomIndex];

								// 상대 사용자 넣기
								m_RoomInfo[m_RoomIndex]->m_Account.push_back(pTarget);

								// 닉네임 넣기
								m_RoomInfo[m_RoomIndex]->m_WhisperAccount.push_back(m_UserName[pSock]);
								m_RoomInfo[m_RoomIndex]->m_WhisperAccount.push_back(m_UserName[pTarget]);

								// 명령어 사용 금지!
								m_RoomInfo[m_RoomIndex]->m_MasterAccount = NULL;

								// 나에게
								sendData.Format(_T("\\x80x3|/|0|/|%u|/|%s|/|암호화"), m_RoomIndex, m_UserName[pTarget]);
								SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);

								// 상대에게
								sendData.Format(_T("\\x80x3|/|0|/|%u|/|%s|/|암호화"), m_RoomIndex, m_UserName[pSock]);
								SendData(pTarget, sendData);	// pTarget->Send(sendData, sendData.GetLength() * 2);

								// 주의!
								m_RoomIndex++;
							}
							break;
						}
					}

					if (bInvalidUser) {
						sendData.Format(_T("\\x80x2|/|%s|/|[알림]|/|상대방이 존재하지 않습니다."), token[1]);
						SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);
					}
				}
				bAllSay = FALSE;
			}
			else if (cmd_token[0] == _T("/k")) {
				// 현재 방에 상대가 있는지 확인
				// 없으면 없다고 알리기
				// 있으면 그대로 내보내고 알리기
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
							// pTarget 이 진짜로 나가게 만들어야함. (완료)
							sendData.Format(_T("\\x80x2|/|-1|/|%s"), token[1]);
							SendData(pTarget, sendData);	// pTarget->Send(sendData, sendData.GetLength() * 2);

							// CloseRoom(pTarget, pRoomInfo->m_RoomIndex);
							break;
						}
					}
				}
			}
			else if (cmd_token[0] == _T("/b")) {
				// 현재 방에 상대가 있는지 확인
				// 없으면 없다고 알리기
				// 있으면 닉네임 등록 후 알리기
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

							sendData.Format(_T("\\x80x2|/|%s|/|[알림]|/|벤 등록되었습니다."), token[1]);
							SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);
							break;
						}
					}

					if (bInvalidUser) {
						sendData.Format(_T("\\x80x2|/|%s|/|[알림]|/|해당 유저가 존재하지 않습니다."), token[1]);
						SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);
					}
				}
			}
			else if (cmd_token[0] == _T("/bl")) {
				// 벤에 등록된 목록 알려주기
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

					sendData.Format(_T("\\x80x2|/|%s|/|[벤 인원]|/|%s"), token[1], userNameList);
					SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);
				}
			}
			else if (cmd_token[0] == _T("/c")) {
				// 벤 목록에 상대가 있는지 확인
				// 없으면 없다고 알리기
				// 있으면 닉네임 해제 후 알리기
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
							sendData.Format(_T("\\x80x2|/|%s|/|[알림]|/|해제 되었습니다."), token[1]);
							SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);
							break;
						}
					}

					if (bInvalidUser) {
						sendData.Format(_T("\\x80x2|/|%s|/|[알림]|/|해당 유저가 등록되지 않았습니다."), token[1]);
						SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);
					}
				}
			}
			else if (cmd_token[0] == _T("/l")) {
				// 현재 방에 있는 인원 알려주기
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

					sendData.Format(_T("\\x80x2|/|%s|/|[현재 인원]|/|%s"), token[1], userNameList);
					SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);
				}
			}
			else if (cmd_token[0] == _T("/ll")) {
				// 현재 방에 있는 인원 수 알려주기
				bAllSay = FALSE;
				RoomInfo *pRoomInfo = m_RoomInfo[_ttoi(token[1])];

				if (pRoomInfo) {
					std::vector<CClientSock *>::iterator iter;
					CString sendData;

					sendData.Format(_T("\\x80x2|/|%s|/|[알림]|/|현재 %u명 있습니다."), token[1], pRoomInfo->m_Account.size());
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
	// 4 |/| 0: 추가요청   |/| 추가할 닉네임
	// 4 |/| 1: 허    용   |/| 추가할 닉네임
	// 4 |/| 2: 대화요청   |/| 닉네임
	// 4 |/| 3: 삭제요청   |/| 삭제할 닉네임
	// 4 |/| 9: 친구목록   |/| 닉네임1, 닉네임2, ...
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
				// 추가
				if (FriendQuery(m_UserName[pSock], token[2]) == 0) {
					FriendQuery(token[2], m_UserName[pSock]);

					CString sendData;
					// 나에게
					sendData.Format(_T("\\x80x4|/|1|/|%s"), token[2]);
					SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);

					// 상대에게
					CClientSock *pTarget = (*iter).first;
					sendData.Format(_T("\\x80x4|/|1|/|%s"), m_UserName[pSock]);
					SendData(pTarget, sendData);	// pTarget->Send(sendData, sendData.GetLength() * 2);
				}
				break;
			}
		}
	}
	else if (token[1] == _T("2")) {
		// token[2] : 닉네임

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
				// 방 생성하기
				//    - 방 생성시, 일반 방과 귓말 방을 구분하여 리스트 전송시 주의! (완료)
				//    - 귓말 방은 명령어 사용 못하게 하기. (완료)
				//    - 마스터가 존재하지 않으며, 유저 2명 만이 존재 (완료)
				//    - 귓말을 재 요청시 존재하는 방이 있는데 접속하지 않은 사람이 있으면
				//      해당 사람만 초청.

				sendData = m_UserName[pSock] + m_UserName[pTarget];
				if (m_WhispRoomInfo[sendData] != NULL) {	// 귓말방이 존재함
													// 나간사람 초대하기
					int num = m_WhispRoomInfo[sendData]->m_Account.size();
					switch (num) {
						//case 2: // 초대할 필요도 없고 데이터 쌓을 필요도 없다.
						//break;
					case 1: // 1명만 초대해 주자.
						if (m_UserName[m_WhispRoomInfo[sendData]->m_Account.at(0)] == m_UserName[pSock]) {
							// 상대에게

							// 사용자 넣기
							m_RoomInfo[m_WhispRoomInfo[sendData]->m_RoomIndex]->m_Account.push_back(pTarget);

							sendData.Format(_T("\\x80x3|/|0|/|%u|/|%s|/|암호화"), m_WhispRoomInfo[sendData]->m_RoomIndex, m_UserName[pSock]);
							SendData(pTarget, sendData);	// pTarget->Send(sendData, sendData.GetLength() * 2);
						}
						else {	// 이것도 버근데...? 상대가 왜 나를 초대하지..?
								// 나에게

								// 사용자 넣기
							m_RoomInfo[m_WhispRoomInfo[sendData]->m_RoomIndex]->m_Account.push_back(pSock);

							sendData.Format(_T("\\x80x3|/|0|/|%u|/|%s|/|암호화"), m_WhispRoomInfo[sendData]->m_RoomIndex, m_UserName[pTarget]);
							SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);
						}
						break;
					case 0:	// 뭐지? 버근데 ㅡㅡ..
						m_WhispRoomInfo.erase(sendData);	// 데이터 삭제
						break;
					}
				}
				else {								// 귓말방이 존재하지 않음
					if (CreateRoom(pSock, ROOM_WHISPER, _T("타이틀"), _T("비밀번호")) == -1)
						return;

					// 귓말방에 데이터 넣기
					m_WhispRoomInfo[sendData] = m_RoomInfo[m_RoomIndex];

					// 상대 사용자 넣기
					m_RoomInfo[m_RoomIndex]->m_Account.push_back(pTarget);

					// 닉네임 넣기
					m_RoomInfo[m_RoomIndex]->m_WhisperAccount.push_back(m_UserName[pSock]);
					m_RoomInfo[m_RoomIndex]->m_WhisperAccount.push_back(m_UserName[pTarget]);

					// 명령어 사용 금지!
					m_RoomInfo[m_RoomIndex]->m_MasterAccount = NULL;

					// 나에게
					sendData.Format(_T("\\x80x3|/|0|/|%u|/|%s|/|암호화"), m_RoomIndex, m_UserName[pTarget]);
					SendData(pSock, sendData);	// pSock->Send(sendData, sendData.GetLength() * 2);

					// 상대에게
					sendData.Format(_T("\\x80x3|/|0|/|%u|/|%s|/|암호화"), m_RoomIndex, m_UserName[pSock]);
					SendData(pTarget, sendData);	// pTarget->Send(sendData, sendData.GetLength() * 2);

					// 주의!
					m_RoomIndex++;
				}
				break;
			}
		}
	}
	else if (token[1] == _T("3")) {
		// 친구 삭제하기
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
		// 친구 목록 불러오기
		CString totalUserName;
		FriendLoadQuery(m_UserName[pSock], totalUserName);

		if (!totalUserName.GetLength()) {
			return;
		}

		// 친구 목록 보내기
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
	//CListBox *pList = (CListBox*)m_pMainWnd->GetDlgItem(IDC_LIST_MSG);		// 모든 메시지
	//CListBox *pUser = (CListBox*)m_pMainWnd->GetDlgItem(IDC_LIST_USER);		// 모든 접속 유저

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
	//	ss += _T("마지막");
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
	// 들어간 방 검사
	BOOL bMessage = FALSE;
	RoomInfo* pRoomInfo = NULL;
	std::vector<UINT32> dellist;
	pRoomInfo = m_RoomInfo[Index];
	if (pRoomInfo) {
		// 내가 방에 들어가 있는지 체크
		std::vector<CClientSock *>::iterator v_iter;
		for (v_iter = pRoomInfo->m_Account.begin(); v_iter != pRoomInfo->m_Account.end(); v_iter++) {
			if (pSock == (*v_iter)) {
				pRoomInfo->m_Account.erase(v_iter);
				bMessage = TRUE;
				break;
			}
		}

		if (pRoomInfo->m_Account.size() > 0) {
			// 방에 들어갔는데 마스터가 나가면 다음 사람한테 Turn 넘기기
			if (pRoomInfo->m_MasterAccount == pSock) {
				pRoomInfo->m_MasterAccount = pRoomInfo->m_Account.at(0);
			}

			if (bMessage) {
				CString sendData;
				sendData.Format(_T("\\x80x1|/|1|/|2|/|%u|/|%s|/|암호화"), pRoomInfo->m_RoomIndex, m_UserName[pSock]);
				for (v_iter = pRoomInfo->m_Account.begin(); v_iter != pRoomInfo->m_Account.end(); v_iter++) {
					SendData((*v_iter), sendData);	// (*v_iter)->Send(sendData, sendData.GetLength() * 2);
				}
			}
		}
		else {
			// 만약 아무 사람도 없으면 방 삭제하기
			dellist.push_back(Index);
		}
	}

	INT32 index;
	CString str;
	for (int i = 0; i < dellist.size(); i++) {
		index = dellist.at(i);	// m_RoomInfo 의 Key
		if (m_RoomInfo[index]->m_Status == ROOM_WHISPER) {
			// m_WhispRoomInfo 의 Key
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
	// 들어간 방 검사
	BOOL bMessage = FALSE;
	RoomInfo* pRoomInfo = NULL;
	std::vector<UINT32> dellist;
	std::map<UINT32, RoomInfo*>::iterator iter;
	for (iter = m_RoomInfo.begin(); iter != m_RoomInfo.end(); iter++) {
		pRoomInfo = (*iter).second;
		bMessage = FALSE;

		if (pRoomInfo) {
			// 내가 방에 들어가 있는지 체크
			std::vector<CClientSock *>::iterator v_iter;
			for (v_iter = pRoomInfo->m_Account.begin(); v_iter != pRoomInfo->m_Account.end(); v_iter++) {
				if (pSock == (*v_iter)) {
					pRoomInfo->m_Account.erase(v_iter);
					bMessage = TRUE;
					break;
				}
			}
			
			if (pRoomInfo->m_Account.size() > 0) {
				// 방에 들어갔는데 마스터가 나가면 다음 사람한테 Turn 넘기기
				if (pRoomInfo->m_MasterAccount == pSock) {
					pRoomInfo->m_MasterAccount = pRoomInfo->m_Account.at(0);
				}

				if (bMessage) {
					CString sendData;
					sendData.Format(_T("\\x80x1|/|1|/|2|/|%u|/|%s|/|암호화"), pRoomInfo->m_RoomIndex, m_UserName[pSock]);
					for (v_iter = pRoomInfo->m_Account.begin(); v_iter != pRoomInfo->m_Account.end(); v_iter++) {
						SendData((*v_iter), sendData);	// (*v_iter)->Send(sendData, sendData.GetLength() * 2);
					}
				}
			}
			else {
				// 만약 아무 사람도 없으면 방 삭제하기
				dellist.push_back((*iter).first);
			}
		}
	}

	INT32 index;
	CString str;
	for (int i = 0; i < dellist.size(); i++) {
		index = dellist.at(i);	// m_RoomInfo 의 Key
		if (m_RoomInfo[index]->m_Status == ROOM_WHISPER) {
			// m_WhispRoomInfo 의 Key
			str = m_RoomInfo[index]->m_WhisperAccount.at(0) + m_RoomInfo[index]->m_WhisperAccount.at(1);

			m_RoomInfo[index]->m_WhisperAccount.clear();
			m_WhispRoomInfo.erase(str);
		}
		m_RoomInfo.erase(index);
	}
	if (dellist.size() > 0) {
		SendRoomList();
	}

	// 데이터 검사
	if (m_UserName[pSock]) {
		// 로그아웃 쿼리 전송
		UpdateQuery(2, m_UserName[pSock], 0);
		// 이름 지우기
		DeleteUserList(m_UserName[pSock]);
		m_UserName.erase(pSock);
	}

	// 공개 키 삭제
	m_CryptInfo.erase(pSock);

	CClientSock *pt;
	POSITION fpos, pos = m_UserList.GetHeadPosition();  // ①
	while(pos)
	{
		fpos = pos;  //  ②
		pt = (CClientSock*)m_UserList.GetNext(pos);  //  ③, ④
		if(pt == pSock)
		{
			pSock->Close();
			delete pSock;  // ⑤
			m_UserList.RemoveAt(fpos); // ⑥
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
			
			// 귓말 방이면 리스트 목록에서 제외
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
			// 너무 빨리 보내서 인식을 못함!
			Sleep(1);
			SendData(pt, sendData);	// pt->Send(sendData, sendData.GetLength() * 2);
		}
	}
}