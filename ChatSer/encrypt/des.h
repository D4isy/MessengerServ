
#ifndef _DES_H_
#define _DES_H_

#define COMBINE01(a, b, c, d) (((a) & 0xFF) << 24) | (((b) & 0xFF) << 16) | (((c) & 0xFF) << 8) | (((d) & 0xFF));

void MakeKey(char *point2key);
void Encrypt_Data(DWORD *data);
void Decrypt_Data(DWORD *data);

#endif