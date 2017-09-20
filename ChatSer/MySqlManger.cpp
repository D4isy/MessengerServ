
#include "stdafx.h"
#include "./include/mysql.h"
#include "MySqlManger.h"

#pragma comment(lib, "./lib/libmysql.lib")

extern MYSQL_ROW g_row;
extern MYSQL_RES *g_res;
extern MYSQL g_mysql;
extern MYSQL g_mysqlHAH1;
extern MYSQL g_mysqlK3S3;

MYSQL m_MySql;
MYSQL_RES *m_Res = NULL;
MYSQL_ROW row;

int Sql_Initialize(void)
{
	mysql_init(&m_MySql);

	// DB 접속
	if (mysql_real_connect(&m_MySql, "localhost", "root", "apmsetup", "messenger", 3306, NULL, 0) == NULL) {
		return -1;
	}

	// 인코딩 설정
	if (mysql_set_character_set(&m_MySql, "euckr")) {
		return -1;
	}

	// 모든 계정 로그인 '안함' 초기화`
	//if (Sql_UpdateQuery("UPDATE `account` SET `login`='0'")) {
	//	return -1;
	//}
	return 0;
}

int Sql_UpdateQuery(char *query)
{
	if (mysql_query(&m_MySql, query))
		return -1;

	return 0;
}

int Sql_Query(char *query)
{
	if (mysql_query(&m_MySql, query))
		return -1;

	// 데이터 많을 때 조심.
	// 검색된 모든 데이터를 가져옴
	if ((m_Res = mysql_store_result(&m_MySql)) == NULL)
		return -1;

	return 0;
}

int Sql_GetFields(void)
{
	if (!m_Res)
		return -1;

	return mysql_num_fields(m_Res);
}

int Sql_GetRows(void)
{
	if (!m_Res)
		return -1;

	return mysql_num_rows(m_Res);
}

int Sql_NextRow(void)
{
	if ((row = mysql_fetch_row(m_Res)) == NULL)
		return -1;

	return 0;
}

int Sql_GetData(int idx, char **data)
{
	int nFields = Sql_GetFields();

	if (idx < 0 || nFields <= idx)
		return -1;

	if (data == NULL)
		return -1;

	*data = row[idx];
	return 0;
}

void Sql_Free_Result(void)
{
	if (m_Res)
		mysql_free_result(m_Res);
}

void Sql_Finish(void)
{
	// m_Mysql 메모리 해제
	mysql_close(&m_MySql);
}

void example()
{
	MYSQL m_MySql;
	MYSQL_RES *m_Res = NULL;
	MYSQL_ROW row;
	char *pResult;

	mysql_init(&m_MySql);

	// DB 접속
	if (mysql_real_connect(&m_MySql, "localhost", "root", "apmsetup", "messenger", 3306, NULL, 0) == NULL) {
		return;
	}

	if (mysql_set_character_set(&m_MySql, "euckr")) {
		// printf("Character set fail -> %s\n", mysql_error(&m_MySql));
		return;
	}

	if (mysql_query(&m_MySql, "set names euckr")) {
		return;
	}

	//// Query 요청
	//if (mysql_query(&m_MySql, "INSERT INTO `account` (`id`,`pass`,`login`,`time`,`root`) VALUES ('안녕','하세요','0','0','0')")) {
	//	return;
	//}

	// Query 요청
	if (mysql_query(&m_MySql, "SELECT `id`,`pass` FROM `account`")) {
		return;
	}

	// 결과를 m_Res에 저장
	if ((m_Res = mysql_store_result(&m_MySql)) == NULL) {
		return;
	}

	// m_Res의 값을 row로 저장해서 하나씩 갖고 오기

	while ((row = mysql_fetch_row(m_Res)) != NULL) {
		// row의 0번째 값을 pResult에 저장
		pResult = row[0];

		if (strcmp(pResult, "test") == 0) {
			MessageBox(NULL, _T("맞아쓰"), _T(""), 0);
		}
		CString str;
		CString id = CA2T(row[0]);
		CString pass = CA2T(row[1]);
		str.Format(_T("%s, %s"), id, pass);
		MessageBox(NULL, str, _T(""), 0);
	}

	// m_Res 메모리 삭제, pResult 사용 못함
	mysql_free_result(m_Res);

	// m_Mysql 메모리 해제
	mysql_close(&m_MySql);
}