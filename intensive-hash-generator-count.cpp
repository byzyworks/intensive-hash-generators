#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <string>
#include <openssl/sha.h>

// Argument 1: file to hash
// Argument 2: number of times to recursively hash the file

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
	
	unsigned long long wait;
	if (argv[2]) {
		wait = std::stoll(argv[2]);
	} else {
		wait = 0x100000;
	}
	
    unsigned char obuf[HASH_LEN];
	int l = (int)isize;
	auto start = std::chrono::high_resolution_clock::now();
	auto end = start;
	double sec;
	if (wait == 0) {
		memcpy(obuf, ibuf, l);
	} else {
		for (unsigned long long i = 0; i < wait; i++) {
			SHA256(ibuf, l, obuf);
			
			l = HASH_LEN;
			memcpy(ibuf, obuf, l);
		}
	}
	end = std::chrono::high_resolution_clock::now();
	sec = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
	
	std::cout << "Hash: ";
	for (auto c : obuf)
		std::cout << std::setfill('0') << std::setw(2) << std::hex << (int)c;
	std::cout << std::endl;
	std::cout << "Time: " << std::dec << sec << "s" << std::endl;
	
    return 0;
}
