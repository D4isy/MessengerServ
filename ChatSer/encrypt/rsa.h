
#ifndef _RSA_H_
#define _RSA_H_

int GetPrime(void);
void SetPrime(int num);
int Make_Public_Key(long e_pi);
int Make_Private_Key(int e, long e_pi);
long Make_Random_Prime_Number();// ���� �ڼ�(2��) ������
int Make_Cyper_text(char *Plain_text, long *Cyper_text, int key); //���� ��ȣ������ ����� �Լ�
int Make_Plain_text(long *Cyper_text, int len, char *Plain_text, int key); //��ȣ���� ������ ����� �Լ�

#endif