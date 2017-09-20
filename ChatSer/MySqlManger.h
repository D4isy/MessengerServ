#pragma once

int Sql_Initialize(void);
void Sql_Finish(void);

int Sql_UpdateQuery(char *query);
int Sql_Query(char *query);
int Sql_GetFields(void);
int Sql_GetRows(void);
int Sql_NextRow(void);
int Sql_GetData(int idx, char **data);
void Sql_Free_Result(void);