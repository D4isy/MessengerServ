
#ifndef _RSA_H_
#define _RSA_H_

int GetPrime(void);
void SetPrime(int num);
int Make_Public_Key(long e_pi);
int Make_Private_Key(int e, long e_pi);
long Make_Random_Prime_Number();// 랜덤 솟수(2개) 생성기
int Make_Cyper_text(char *Plain_text, long *Cyper_text, int key); //평문을 암호문으로 만드는 함수
int Make_Plain_text(long *Cyper_text, int len, char *Plain_text, int key); //암호문을 평문으로 만드는 함수

#endif