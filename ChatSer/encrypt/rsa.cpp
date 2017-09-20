
#include "../stdafx.h"
//#include "rsa.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <conio.h>

#define FALSE 0
#define TRUE 1

int n, message_length;

int IsNotPrime(int n);		//�Ҽ������˻�
long mod(long n, long e, long m);	//

int GCD(long x, long y)		//�ִ� ����� �˻�
{
	return y == 0 ? x : GCD(y, x % y);
}

//void main(void)
//{
//	long send_msg[100];
//	char recv_msg[100], buffer[100];
//	int public_key, private_key;
//	long e_pi;
//	e_pi = Make_Random_Prime_Number();	// �ΰ��� �Ҽ��� ����� �������� n���� ���Ѵ�. 
//	public_key = Make_Public_Key(e_pi);	// ����Ű�� �����.
//	private_key = Make_Private_Key(public_key, e_pi);	//����Ű�� �����. 
//	printf("e_pi=%d, n=%d, public_key=%d, private_key=%d\n\n", e_pi, n, public_key, private_key);
//
//	printf("\t���� �Է��ϼ��� = ");
//	scanf("%s", buffer);
//	printf("\t���� ����= %s \n", buffer);
//
//	Make_Cyper_text(buffer, send_msg, public_key);
//	Make_Plain_text(send_msg, recv_msg, private_key);
//	printf("\t �ص��� ���� = %s \n", recv_msg);
//}

long Make_Random_Prime_Number()// ���� �ڼ�(2��) ������
{
	int i;
	int Prime[2]; // P�� Q �ΰ��� �ڼ��� ����Ű, ���Ű�� �⺻ �Ҽ�
	time_t t;
	srand((unsigned int)time(&t)); //��������
	for (i = 0; i<2; i++)				// 2���� ������ �ڼ� P�� Q�� �����Ѵ�. 
	{
		do	Prime[i] = rand() % 100;  //2�ڸ��� ���� 
		while (IsNotPrime(Prime[i]));	 //�Ҽ��� �ƴϸ� �ݺ��Ѵ�.
	}
	n = Prime[0] * Prime[1];		// �ΰ��� �Ҽ� p,q�� �̿��� n�� ����
	// printf("\nP=%d, Q=%d e_pi=%d n=%d \n", Prime[0], Prime[1], n);
	return (Prime[0] - 1)*(Prime[1] - 1);  // ���Ϸ� ���̰�;
}

void SetPrime(int num)
{
	n = num;
}

int GetPrime(void)
{
	return n;
}

int Make_Public_Key(long e_pi)
{
	long e;
	do
	{
		e = rand() % 100;	//3�ڸ��� ����	
		if ((e<e_pi) && (GCD(e, e_pi) == 1)) return e;	// ���Ϸ� ���̿� ���� ���� e�� ����. 
	} while (1);
}

int Make_Private_Key(int e, long e_pi)
{
	int d = 0;
	while (((e*d) % e_pi) != 1) d++;		//����Ű ����
	return d;	//����Ű�� �����Ѵ�. 
}


int IsNotPrime(int n)		//�Ҽ��� �ƴ��� �˻�
{
	int  i, limit;
	if (!(n % 2))  return (TRUE);	//¦���̸� �Ҽ��� �ƴϴ�.
	limit = (int)sqrt((double)n) + 1;	//n����+1�� �Ͽ� ���� ���� �Ҽ��� ã�´�.
	for (i = 3; i <= limit; i += 2) //3���� Ȧ�� ������ ������ ������ �Ѵ�. 
	if (!(n%i))  return (TRUE);
	return (FALSE);
}

long mod(long n, long e, long m)// residue = n^e (mod m)�� �����Ѵ�.
{
	long i, residue = 1;
	for (i = 1; i <= e; i++)
	{
		residue *= n;	//residue=residue*n
		residue %= m;
	}//residue=residue%n �����÷θ� �����ϰ��� mod������ �̿��Ͽ� �ڸ����� ���δ�.
	return residue;
}


int Make_Cyper_text(char *Plain_text, long *Cyper_text, int key)
{
	int i;
	message_length = strlen(Plain_text);
	for (i = 0; i<message_length; i++)	  //�޼��� ���̿� �°� �ݺ��Ѵ�.
		Cyper_text[i] = (long)mod(Plain_text[i], key, n);  //��ȣȭ
	Cyper_text[i] = '\0';		//���� ǥ��

	//printf("====================================================\n");
	//printf(" ��ȣȭ�� ������ = ");
	//for (i = 0; i<message_length; i++)
	//	printf(" %X ", Cyper_text[i]);	//��ȣȭ�� �����͸� ����Ͽ� �����ش�.
	//printf("\n");
	return 0;
}

int Make_Plain_text(long *Cyper_text, int len, char *Plain_text, int key)
{
	int  i;
	for (i = 0; i<len; i++) // �޼��� ���̿� �°� �ݺ��Ѵ�.
		Plain_text[i] = (char)mod(Cyper_text[i], key, n);	//��ȣȭ
	Plain_text[i] = '\0';	//���� ǥ��

	/*
	printf("receive message = ");
	for (i = 0; i<len; i++)
		printf(" %d ", Plain_text[i]);	//��ȣȭ�� �����͸� ����Ͽ� �����ش�.
	printf("\n");
	*/
	return 0;
}

