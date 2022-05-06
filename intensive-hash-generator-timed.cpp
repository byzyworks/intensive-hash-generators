#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <mutex>
#include <stdio.h>
#include <string>
#include <thread>
#include <openssl/sha.h>

// Argument 1: file to hash
// Argument 2: number of milliseconds after which to print a hash

# define HASH_LEN 32

typedef struct {
	unsigned char      ibuf[HASH_LEN];
	int                isize;
	unsigned char      obuf[HASH_LEN];
	unsigned long long ctr;
	std::mutex         mtx;
} Out;

int hash(Out *out) {
	unsigned char      *ibuf = out->ibuf;
	int                l     = out->isize;
	unsigned char      *obuf = out->obuf;
	unsigned long long ctr   = 0;
	bool               flag;
	while (true) {
		SHA256(ibuf, l, obuf);
		
		ctr++;
		
		if (out->mtx.try_lock()) {
			out->ctr = ctr;
			out->mtx.unlock();
			break;
		}
		
		l = HASH_LEN;
		memcpy(ibuf, obuf, l);
	}
	
	return 0;
}

int main(int argc, char *argv[])
{
	Out out;
	
	std::ifstream f;
	f.open(argv[1], std::ios::in | std::ios::binary);
	if (!f.is_open())
		return 1;
	
	f.seekg(0, std::ios::end);
	std::streampos isize = f.tellg();
	f.seekg(0, std::ios::beg);
	if (!f.read((char *)ibuf, isize))
		return 1;
	
	f.close();
	
	unsigned int wait;
	if (argv[2]) {
		wait = std::stoi(argv[2]);
	} else {
		wait = 0;
	}
	
	out.ibuf  = ibuf;
	out.isize = (int)isize;
	
	out.mtx.lock();
	std::thread hasher(hash, &out);
	std::this_thread::sleep_for(std::chrono::milliseconds(wait));
	out.mtx.unlock();
	hasher.join();
	
	std::cout << "Hash: ";
	for (auto c : out.obuf)
		std::cout << std::setfill('0') << std::setw(2) << std::hex << (int)c;
	std::cout << std::endl;
	std::cout << "Iter: " << std::dec << out.ctr << std::endl;
	
    return 0;
}
