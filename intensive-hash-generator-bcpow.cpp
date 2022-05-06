#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <string>
#include <openssl/sha.h>

// Argument 1: file to hash
// Argument 2: number of zeroes at beginning of hash required

# define HASH_LEN 32

int main(int argc, char *argv[])
{	
	std::ifstream f;
	f.open(argv[1], std::ios::in | std::ios::binary);
	if (!f.is_open())
		return 1;
	
	unsigned char ibuf[HASH_LEN] = { 0 };
	f.seekg(0, std::ios::end);
    std::streampos isize = f.tellg();
	f.seekg(0, std::ios::beg);
	if (!f.read((char *)ibuf, isize))
		return 1;
	
	f.close();
	
	int zeroes;
	if (argv[2]) {
		zeroes = std::stoi(argv[2]);
	} else {
		zeroes = 2;
	}
	
	uint64_t nonce;
	
	unsigned char ibuf_orig[HASH_LEN];
	for (int i = 0; i < HASH_LEN; i++)
		ibuf_orig[i] = ibuf[i];
	
    unsigned char obuf[HASH_LEN];
	int n = (zeroes < HASH_LEN ? zeroes : HASH_LEN) - 1;
	int l = (int)isize > 8 ? (int)isize : 8;
	auto start = std::chrono::high_resolution_clock::now();
	auto end = start;
	double sec;
	int i;
	bool flag;
	while (true) {
		ibuf[7] += (nonce >> 56) & 0xff;
		ibuf[6] += (nonce >> 48) & 0xff;
		ibuf[5] += (nonce >> 40) & 0xff;
		ibuf[4] += (nonce >> 32) & 0xff;
		ibuf[3] += (nonce >> 24) & 0xff;
		ibuf[2] += (nonce >> 16) & 0xff;
		ibuf[1] += (nonce >> 8) & 0xff;
		ibuf[0] += nonce & 0xff;
		
		SHA256(ibuf, l, obuf);
		if (zeroes == 0)
			break;
		
		flag = false;
		for (i = 0; i < n; i++) {
			if (i % 2 == 0) {
				if (obuf[i / 2] / 0x10 != 0) {
					flag = false;
					break;
				}
			} else {
				if (obuf[i / 2] % 0x10 != 0) {
					flag = false;
					break;
				}
			}
		}
		if (i == n) {
			if (i % 2 == 0) {
				if (obuf[i / 2] / 0x10 == 0)
					flag = true;
			} else {
				if (obuf[i / 2] % 0x10 == 0)
					flag = true;
			}
		}
		if (flag)
			break;
		
		for (i = 0; i < 8; i++)
			ibuf[i] = ibuf_orig[i];
		
		nonce++;
	}
	end = std::chrono::high_resolution_clock::now();
	sec = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
	
	unsigned long long temp = 0;
	temp += (nonce & 0xff) << 56;
	temp += (nonce & 0xff00) << 40;
	temp += (nonce & 0xff0000) << 24;
	temp += (nonce & 0xff000000) << 8;
	temp += (nonce & 0xff00000000) >> 8;
	temp += (nonce & 0xff0000000000) >> 24;
	temp += (nonce & 0xff000000000000) >> 40;
	temp += (nonce & 0xff00000000000000) >> 56;
	nonce = temp;
	
	std::cout << "Nput: ";
	for (auto c : ibuf_orig)
		std::cout << std::setfill('0') << std::setw(2) << std::hex << (int)c;
	std::cout << std::endl;
	std::cout << "Nnce: " << std::setfill('0') << std::setw(16) << std::hex << nonce << "000000000000000000000000000000000000000000000000" << std::endl;
	std::cout << "Oput: ";
	for (auto c : ibuf)
		std::cout << std::setfill('0') << std::setw(2) << std::hex << (int)c;
	std::cout << std::endl;
	std::cout << "Hash: ";
	for (auto c : obuf)
		std::cout << std::setfill('0') << std::setw(2) << std::hex << (int)c;
	std::cout << std::endl;
	std::cout << "Time: " << std::dec << sec << "s" << std::endl;
	
    return 0;
}