#include <cstring>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <string>
#include <openssl/sha.h>

// Argument 1: file to hash
// Argument 2: number of times after which to print a hash

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
		wait = std::stoi(argv[2]);
	} else {
		wait = 0x100000;
	}
	
    unsigned char obuf[HASH_LEN];
	int l = (int)isize;
	unsigned long long ctr = 0;
	bool flag;
	while (true) {
		SHA256(ibuf, l, obuf);
		
		if (ctr == 0) {
			for (auto c : obuf)
				std::cout << std::setfill('0') << std::setw(2) << std::hex << (int)c;
			std::cout << std::endl;
		}
		ctr = (ctr + 1) % wait;
		
		l = HASH_LEN;
		memcpy(ibuf, obuf, l);
	}
	
    return 0;
}