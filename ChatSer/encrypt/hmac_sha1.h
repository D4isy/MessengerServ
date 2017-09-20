
#ifndef _HMAC_SHA1_H_
#define _HMAC_SHA1_H_

void hmac_sha1(uint8_t text[SHA1HashSize], uint8_t key[SHA1HashSize], uint8_t Message_Digest[SHA1HashSize]);

#endif