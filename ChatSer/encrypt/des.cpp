
#include "../stdafx.h"

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

//#include "des.h"

unsigned char IP[] = {
	58, 50, 42, 34, 26, 18, 10, 2,
	60, 52, 44, 36, 28, 20, 12, 4,
	62, 54, 46, 38, 30, 22, 14, 6,
	64, 56, 48, 40, 32, 24, 16, 8,
	57, 49, 41, 33, 25, 17, 9, 1,
	59, 51, 43, 35, 27, 19, 11, 3,
	61, 53, 45, 37, 29, 21, 13, 5,
	63, 55, 47, 39, 31, 23, 15, 7
};
unsigned char IIP[] = {
	40, 8, 48, 16, 56, 24, 64, 32,
	39, 7, 47, 15, 55, 23, 63, 31,
	38, 6, 46, 14, 54, 22, 62, 30,
	37, 5, 45, 13, 53, 21, 61, 29,
	36, 4, 44, 12, 52, 20, 60, 28,
	35, 3, 43, 11, 51, 19, 59, 27,
	34, 2, 42, 10, 50, 18, 58, 26,
	33, 1, 41, 9, 49, 17, 57, 25
};
unsigned char PC_1[] = {
	57, 49, 41, 33, 25, 17, 9,
	1, 58, 50, 42, 34, 26, 18,
	10, 2, 59, 51, 43, 35, 27,
	19, 11, 3, 60, 52, 44, 36,
	63, 55, 47, 39, 31, 23, 15,
	7, 62, 54, 46, 38, 30, 22,
	14, 6, 61, 53, 45, 37, 29,
	21, 13, 5, 28, 20, 12, 4
};
unsigned char PC_2[] = {
	14, 17, 11, 24, 1, 5,
	3, 28, 15, 6, 21, 10,
	23, 19, 12, 4, 26, 8,
	16, 7, 27, 20, 13, 2,
	41, 52, 31, 37, 47, 55,
	30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53,
	46, 42, 50, 36, 29, 32
};
unsigned char SBox_DES[8][4][16] = {
	{ { 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7 },
	{ 0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8 },
	{ 4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0 },
	{ 15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13 } },

	{ { 15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10 },
	{ 3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5 },
	{ 0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15 },
	{ 13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9 } },

	{ { 10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8 },
	{ 13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1 },
	{ 13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7 },
	{ 1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12 } },

	{ { 7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15 },
	{ 13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9 },
	{ 10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4 },
	{ 3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14 } },

	{ { 2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9 },
	{ 14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6 },
	{ 4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14 },
	{ 11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3 } },

	{ { 12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11 },
	{ 10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8 },
	{ 9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6 },
	{ 4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13 } },

	{ { 4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1 },
	{ 13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6 },
	{ 1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2 },
	{ 6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12 } },

	{ { 13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7 },
	{ 1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2 },
	{ 7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8 },
	{ 2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11 } }
};
const unsigned char LSHIFT[] = {
	1, 1, 2, 2,
	2, 2, 2, 2,
	1, 2, 2, 2,
	2, 2, 2, 1
};
unsigned char P[] = {
	16, 7, 20, 21, 29, 12, 28, 17,
	1, 15, 23, 26, 5, 18, 31, 10,
	2, 8, 24, 14, 32, 27, 3, 9,
	19, 13, 30, 6, 22, 11, 4, 25
};


typedef unsigned char    BYTE;
typedef unsigned short    WORD;
typedef unsigned long    DWORD;
typedef unsigned char    BYTE;

DWORD dRoundKey[32] = { 0, };


char *uintToBinary(DWORD i)
{
	static char s[32 + 1] = { '0', };
	int count = 32;

	do {
		s[--count] = '0' + (char)(i & 1);
		i = i >> 1;
	} while (count);

	return s;
}



void InitialPermutation(DWORD *Data)
{
	DWORD tmpMask;
	DWORD setMask;
	DWORD NewData[2] = { 0, };


	for (int i = 31; i >= 0; i--)
	{
		tmpMask = 0x00000001;
		setMask = 0x00000001 << (31 - i);
		if ((IP[i] <= 32) && (Data[1] & (tmpMask << (32 - IP[i])))) {
			NewData[1] = NewData[1] | setMask;
		}
		else if ((IP[i] > 32) && (Data[0] & (tmpMask << (64 - IP[i])))) {
			NewData[1] = NewData[1] | setMask;
		}
	}


	for (int i = 63; i >= 32; i--)
	{
		tmpMask = 0x00000001;
		setMask = 0x00000001 << (63 - i);
		if ((IP[i] <= 32) && (Data[1] & (tmpMask << (32 - IP[i])))) {
			NewData[0] = NewData[0] | setMask;
		}
		else if ((IP[i] > 32) && (Data[0] & (tmpMask << (64 - IP[i])))) {
			NewData[0] = NewData[0] | setMask;
		}
	}
	Data[1] = NewData[1];
	Data[0] = NewData[0];
}

void InverseInitialPermutation(DWORD *Data)
{
	DWORD tmpMask;
	DWORD setMask;
	DWORD NewData[2] = { 0, };
	for (int i = 31; i >= 0; i--)
	{
		tmpMask = 0x00000001;
		setMask = 0x00000001 << (31 - i);
		if ((IIP[i] <= 32) && (Data[1] & (tmpMask << (32 - IIP[i])))) {
			NewData[1] = NewData[1] | setMask;
		}
		else if ((IIP[i] > 32) && (Data[0] & (tmpMask << (64 - IIP[i])))) {
			NewData[1] = NewData[1] | setMask;
		}
	}

	for (int i = 63; i >= 32; i--)
	{
		tmpMask = 0x00000001;
		setMask = 0x00000001 << (63 - i);
		if ((IIP[i] <= 32) && (Data[1] & (tmpMask << (32 - IIP[i])))) {
			NewData[0] = NewData[0] | setMask;
		}
		else if ((IIP[i] > 32) && (Data[0] & (tmpMask << (64 - IIP[i])))) {
			NewData[0] = NewData[0] | setMask;
		}
	}
	Data[1] = NewData[1];
	Data[0] = NewData[0];
}


void ExpansionPermutation(DWORD RightHalf, DWORD *eWord)
{
	DWORD New[2] = { 0, 0 };

	New[1] |= ((RightHalf & 0x00000001) << 23);
	New[1] |= (((RightHalf & 0xF8000000) >> 9) & 0x007C0000);
	New[1] |= ((RightHalf & 0x1F800000) >> 11);
	New[1] |= ((RightHalf & 0x01F80000) >> 13);
	New[1] |= ((RightHalf & 0x001F8000) >> 15);

	New[0] |= ((RightHalf & 0x0001F800) << 7);
	New[0] |= ((RightHalf & 0x00001F80) << 5);
	New[0] |= ((RightHalf & 0x000001F8) << 3);
	New[0] |= ((RightHalf & 0x0000001F) << 1);
	New[0] |= (((RightHalf & 0x80000000) >> 31) & 0x00000001);

	eWord[1] = New[1];
	eWord[0] = New[0];
}


DWORD PermutationFunction(DWORD dWord)
{
	DWORD tmpMask = 0x00000001;
	DWORD keyMask = 0x00000001;
	DWORD dwTmp = 0;
	for (int i = 0; i < 32; i++)
	{
		if (dWord & (tmpMask << (32 - P[i])))
			dwTmp |= (keyMask << (31 - i));
	}
	return dwTmp;
}


void PermutedChoice1(DWORD *Key)
{
	DWORD tmpMask;
	DWORD KeyMask;
	DWORD NewKey[2] = { 0, 0 };

	tmpMask = 0x00000001;

	for (int i = 0; i < 28; i++) {
		KeyMask = 0x08000000 >> i;

		if ((PC_1[i] <= 32) && (Key[1] & (tmpMask << (32 - PC_1[i])))) {
			NewKey[1] = NewKey[1] | KeyMask;
		}
		else if ((PC_1[i] > 32) && (Key[0] & (tmpMask << (64 - PC_1[i])))) {
			NewKey[1] = NewKey[1] | KeyMask;
		}
	}

	for (int i = 28; i < 56; i++) {
		KeyMask = 0x08000000 >> (i - 28);

		if ((PC_1[i] <= 32) && (Key[1] & (tmpMask << (32 - PC_1[i])))) {
			NewKey[0] = NewKey[0] | KeyMask;
		}
		else if ((PC_1[i] > 32) && (Key[0] & (tmpMask << (64 - PC_1[i])))) {
			NewKey[0] = NewKey[0] | KeyMask;
		}
	}
	Key[0] = NewKey[0];
	Key[1] = NewKey[1];
}

void PermutedChoice2(DWORD *Key)
{
	DWORD tmpMask;
	DWORD KeyMask;
	DWORD NewKey[2] = { 0, 0 };

	tmpMask = 0x00000001;
	for (int i = 0; i < 24; i++) {
		KeyMask = 0x00800000 >> i;
		if ((PC_2[i] <= 28) && (Key[1] & (tmpMask << (28 - PC_2[i])))) {
			NewKey[1] = NewKey[1] | KeyMask;
		}
		else if ((PC_2[i] > 28) && (Key[0] & (tmpMask << (56 - PC_2[i])))) {
			NewKey[1] = NewKey[1] | KeyMask;
		}
	}

	for (int i = 24; i < 48; i++) {
		KeyMask = 0x00800000 >> (i - 24);

		if ((PC_2[i] <= 28) && (Key[1] & (tmpMask << (28 - PC_2[i])))) {
			NewKey[0] = NewKey[0] | KeyMask;
		}
		else if ((PC_2[i] > 28) && (Key[0] & (tmpMask << (56 - PC_2[i])))) {
			NewKey[0] = NewKey[0] | KeyMask;
		}
	}
	Key[0] = NewKey[0];
	Key[1] = NewKey[1];
}


unsigned long SubstitutionChoice(DWORD *eWord)
{
	int row, col;
	DWORD tmp0, tmp1;
	tmp0 = eWord[0];
	tmp1 = eWord[1];
	int shift;

	eWord[0] = 0;
	eWord[1] = 0;

	for (int i = 0; i < 4; i++)
	{
		shift = (4 * (3 - i));
		tmp1 <<= 6;
		row = ((tmp1 & 0x20000000) >> 28) | ((tmp1 & 0x01000000) >> 24);
		col = (tmp1 & 0x1E000000) >> 25;
		eWord[1] |= (SBox_DES[i][row][col] << shift);
		tmp0 <<= 6;
		row = ((tmp0 & 0x20000000) >> 28) | ((tmp0 & 0x01000000) >> 24);
		col = (tmp0 & 0x1E000000) >> 25;
		eWord[0] |= (SBox_DES[i + 4][row][col] << shift);
	}
	eWord[0] |= (eWord[1] << 16);
	return eWord[0];
}


void Round(DWORD *dWord, DWORD RoundKey1, DWORD RoundKey0)
{
	DWORD tmp[2] = { 0, 0 };

	ExpansionPermutation(dWord[0], tmp);


	tmp[1] = tmp[1] ^ RoundKey1;
	tmp[0] = tmp[0] ^ RoundKey0;

	tmp[0] = SubstitutionChoice(tmp);

	tmp[0] = PermutationFunction(tmp[0]);

	tmp[0] = tmp[0] ^ dWord[1];

	dWord[1] = dWord[0];
	dWord[0] = tmp[0];

}

void Round_Decrypt(DWORD *dWord, DWORD RoundKey1, DWORD RoundKey0)
{
	DWORD tmp[2] = { 0, 0 };

	ExpansionPermutation(dWord[1], tmp);

	tmp[1] = tmp[1] ^ RoundKey1;
	tmp[0] = tmp[0] ^ RoundKey0;

	tmp[0] = SubstitutionChoice(tmp);

	tmp[0] = PermutationFunction(tmp[0]);

	tmp[0] = tmp[0] ^ dWord[0];

	dWord[0] = dWord[1];
	dWord[1] = tmp[0];

}



void KeySchedule(char *Key, DWORD *RoundKey)
{
	int SHIFT = 0;
	DWORD dwKey[2] = { 0, 0 };
	DWORD tmp0, tmp1;

	for (int i = 0; i < 4; i++)
	{
		SHIFT = ((3 - i) * 8);
		dwKey[1] ^= ((DWORD)Key[i]) << SHIFT;
		dwKey[0] ^= ((DWORD)Key[i + 4]) << SHIFT;
	}

	PermutedChoice1(dwKey);

	for (int i = 0; i < 16; i++) {
		if (LSHIFT[i] == 1) {
			tmp0 = ((dwKey[0] & 0x08000000) >> 27);
			tmp0 |= ((dwKey[0] & 0x07FFFFFF) << 1);
			dwKey[0] = tmp0;
			tmp1 = ((dwKey[1] & 0x08000000) >> 27);
			tmp1 |= ((dwKey[1] & 0x07FFFFFF) << 1);
			dwKey[1] = tmp1;
		}
		else {
			tmp0 = ((dwKey[0] & 0x0c000000) >> 26);
			tmp0 |= ((dwKey[0] & 0x03FFFFFF) << 2);
			dwKey[0] = tmp0;
			tmp1 = ((dwKey[1] & 0x0c000000) >> 26);
			tmp1 |= ((dwKey[1] & 0x03FFFFFF) << 2);
			dwKey[1] = tmp1;
		}

		PermutedChoice2(dwKey);

		dRoundKey[2 * i + 1] = dwKey[1];
		dRoundKey[2 * i] = dwKey[0];
		// printf("%d 번째 키 : %0xd%0xd \n", i + 1, dwKey[1], dwKey[0]);

		dwKey[1] = tmp1;
		dwKey[0] = tmp0;
	}
}



void MakeKey(char *point2key)
{
	KeySchedule(point2key, dRoundKey);
}


// void Encrypt_Data(DWORD *data, DWORD *RoundKey)
void Encrypt_Data(DWORD *data)
{
	int round = 0;

	InitialPermutation(data);

	for (round = 0; round<16; round++) {
		Round(data, dRoundKey[2 * round + 1], dRoundKey[2 * round]);
	}

	InverseInitialPermutation(data);
}

// void Decrypt_Data(DWORD *data, DWORD *RoundKey)
void Decrypt_Data(DWORD *data)
{
	int round = 15;

	InitialPermutation(data);

	for (round = 15; round >= 0; round--) {
		Round_Decrypt(data, dRoundKey[2 * round + 1], dRoundKey[2 * round]);
	}

	InverseInitialPermutation(data);
}

/*
#define COMBINE01(a, b, c, d) (((a) & 0xFF) << 24) | (((b) & 0xFF) << 16) | (((c) & 0xFF) << 8) | (((d) & 0xFF));

int main(void)
{
	int input;
	unsigned long plainText[] = { 0x675a6967, 0x5e5a6b5a };

	char* key = "BBBBBBBB";

	char data[9] = { 0, };
	int len;


	printf("------------------------\n");
	printf("(8) 키값 : %s\n", key);
	printf("MakeKey...\n");
	MakeKey(key);
	printf("------------------------\n");
	printf("입력> ");
	fgets(data, sizeof(data), stdin);
	len = strlen(data);
	data[len] = 0x00;

	plainText[0] = COMBINE01(data[0], data[1], data[2], data[3]);
	plainText[1] = COMBINE01(data[4], data[5], data[6], data[7]);
	printf("------------------------\n");
	printf("평문 : %08x %08x \n", plainText[0], plainText[1]);
	printf("------------------------\n\n\n");
	while (1)
	{
		printf("IP : 1, IIP : 2, 암호화 : 3, 복호화 : 4 종료 : 5 \n::");
		scanf("%d", &input);

		switch (input)
		{
		case 1:
			printf("------------------------IP---------------------\n");
			InitialPermutation(plainText);
			printf("평문 : %08x %08x \n", plainText[0], plainText[1]);
			printf("치환문 : %08x %08x \n", plainText[1], plainText[0]);
			printf("---------------------------------------------------\n\n\n");
			break;
		case 2:
			printf("------------------------IIP---------------------\n");
			InitialPermutation(plainText);
			InverseInitialPermutation(plainText);
			printf("평문 : %08x %08x \n", plainText[0], plainText[1]);
			printf("Inverse치환문 : %08x %08x \n", plainText[0], plainText[1]);
			printf("---------------------------------------------------\n\n\n");
			break;
		case 3:
			printf("------------------------암호화---------------------\n");
			printf("평문 : %08x %08x \n", plainText[0], plainText[1]);
			Encrypt_Data(plainText, dRoundKey);
			printf("암호문 : %08x %08x \n", plainText[0], plainText[1]);
			printf("---------------------------------------------------\n");

			data[0] = (plainText[0] >> 24) & 0xFF;
			data[1] = (plainText[0] >> 16) & 0xFF;
			data[2] = (plainText[0] >> 8) & 0xFF;
			data[3] = (plainText[0] >> 0) & 0xFF;
			data[4] = (plainText[1] >> 24) & 0xFF;
			data[5] = (plainText[1] >> 16) & 0xFF;
			data[6] = (plainText[1] >> 8) & 0xFF;
			data[7] = (plainText[1] >> 0) & 0xFF;
			data[8] = 0x00;
			printf("암호문 : %s \n", data);
			printf("---------------------------------------------------\n");
			break;
		case 4:
			printf("------------------------복호화---------------------\n");
			printf("평문 : %08x %08x \n", plainText[0], plainText[1]);
			Decrypt_Data(plainText, dRoundKey);
			printf("복호문 : %08x %08x \n", plainText[0], plainText[1]);
			printf("---------------------------------------------------\n");

			data[0] = (plainText[0] >> 24) & 0xFF;
			data[1] = (plainText[0] >> 16) & 0xFF;
			data[2] = (plainText[0] >> 8) & 0xFF;
			data[3] = (plainText[0] >> 0) & 0xFF;
			data[4] = (plainText[1] >> 24) & 0xFF;
			data[5] = (plainText[1] >> 16) & 0xFF;
			data[6] = (plainText[1] >> 8) & 0xFF;
			data[7] = (plainText[1] >> 0) & 0xFF;
			data[8] = 0x00;
			printf("복호문 : %s \n", data);
			printf("---------------------------------------------------\n");
			break;
		case 5:
			printf("종료\n");
			exit(1);
		default:
			printf("다시 입력해주세요.");
			break;
		}
	}
	return 0;
}
*/