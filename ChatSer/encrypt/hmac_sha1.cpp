

#include "../stdafx.h"
//#include "sha1.h"
//#include "hmac_sha1.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void test(int err, const char* msg) {
	if (err) {
		fprintf(stderr, "%s: error %d\n", msg, err);
		exit(1);
	}
}

void hmac_sha1(uint8_t text[SHA1HashSize], uint8_t key[SHA1HashSize], uint8_t Message_Digest[SHA1HashSize])
{
	uint8_t m_ipad[64];
	uint8_t m_opad[64];

	int len;
	SHA1Context sha;

	uint8_t SHA1_Key[SHA1_BLOCK_SIZE];
	uint8_t AppendBuf1[HMAC_BUF_LEN];
	uint8_t AppendBuf2[HMAC_BUF_LEN];
	uint8_t szReport[HMAC_BUF_LEN];

	memset(SHA1_Key, 0, SHA1_BLOCK_SIZE);
	memset(Message_Digest, 0, SHA1HashSize);

	memset(m_ipad, 0x36, sizeof(m_ipad));
	memset(m_opad, 0x5c, sizeof(m_opad));

	len = strlen((char*)key);  /* key length */

	if (strlen((char*)key) > SHA1_BLOCK_SIZE)
	{
		test(SHA1Reset(&sha), "SHA1Reset");
		test(SHA1Input(&sha, key, len), "SHA1Input");
		test(SHA1Result(&sha, SHA1_Key), "SHA1Result");
	}
	else
		memcpy(SHA1_Key, key, len);

	for (int i = 0; i<sizeof(m_ipad); i++)
	{
		m_ipad[i] ^= SHA1_Key[i];
	}

	len = strlen((char*)text);
	memcpy(AppendBuf1, m_ipad, sizeof(m_ipad));
	memcpy(AppendBuf1 + sizeof(m_ipad), text, len);

	test(SHA1Reset(&sha), "SHA1Reset");
	test(SHA1Input(&sha, AppendBuf1, sizeof(m_ipad) + len), "SHA1Input");
	test(SHA1Result(&sha, szReport), "SHA1Result");

	/*printf("Key hmac =");
	for (int i = 0; i<20; ++i)
		printf(" %02x", szReport[i]);
	printf("\n");*/

	for (int j = 0; j<sizeof(m_opad); j++)
	{
		m_opad[j] ^= SHA1_Key[j];
	}

	memcpy(AppendBuf2, m_opad, sizeof(m_opad));
	memcpy(AppendBuf2 + sizeof(m_opad), szReport, SHA1HashSize);
	
	test(SHA1Reset(&sha), "SHA1Reset");
	test(SHA1Input(&sha, AppendBuf2, sizeof(m_opad) + SHA1HashSize), "SHA1Input");
	test(SHA1Result(&sha, Message_Digest), "SHA1Result");
}