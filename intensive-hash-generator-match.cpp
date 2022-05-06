#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <string>
#include <openssl/sha.h>

// Argument 1: file to hash
// Argument 2: file with string prefix to match in hash

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
	
	f.open(argv[2], std::ios::in | std::ios::binary);
	if (!f.is_open())
		return 1;
	
	unsigned char vbuf[HASH_LEN] = { 0 };
	f.seekg(0, std::ios::end);
	std::streampos vsize = f.tellg();
	f.seekg(0, std::ios::beg);
	if (!f.read((char *)vbuf, vsize))
		return 1;
	
	f.close();
	
    unsigned char obuf[HASH_LEN];
	int n = (int)vsize < HASH_LEN ? (int)vsize : HASH_LEN;
	int l = (int)isize;
	unsigned long long ctr = 0;
	auto start = std::chrono::high_resolution_clock::now();
	auto end = start;
	double sec;
	int i;
	bool flag;
	while (true) {
		SHA256(ibuf, l, obuf);
		
		ctr++;
		
		flag = true;
		for (i = 0; i < n; i++) {
			if (obuf[i] != vbuf[i]) {
				flag = false;
				break;
			}
		}
		if (flag)
			break;
		
		l = HASH_LEN;
		memcpy(ibuf, obuf, l);
	}
	end = std::chrono::high_resolution_clock::now();
	sec = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
	
	std::cout << "Hash: ";
	for (auto c : obuf)
		std::cout << std::setfill('0') << std::setw(2) << std::hex << (int)c;
	std::cout << std::endl;
	std::cout << "Iter: " << std::dec << ctr << std::endl;
	std::cout << "Time: " << std::dec << sec << "s" << std::endl;
	
    return 0;
}
