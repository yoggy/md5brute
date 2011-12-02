//
//	MD5 bruteforce sample program?
//	(CODEGATE2011 CTF Final - Geol or Yut Prob4)
//
//	prerequire:
//		$ sudo apt-get install libssl-dev  boost1.40-all-dev
//
//	how to comple:
//		$ g++ -O3 -fopenmp -lboost_date_time -lssl -o md5brute md5brute.cpp
//		$ g++ -O3 -mtune=amdfam10 -funroll-all-loops -ffast-math -fprefetch-loop-arrays -mabm -msse4a -fopenmp -lboost_date_time -lssl -o md5brute md5brute.cpp
//
#include <stdio.h>

#include <string>
#include <iostream>
#include <sstream>

#include <boost/cstdint.hpp>
#include <boost/format.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <string.h> //memcmp()

#include <openssl/md5.h>

#ifdef _OPENMP
#include <omp.h>
#endif

using namespace std;
using namespace boost;
using boost::posix_time::ptime;
using boost::posix_time::second_clock;
using boost::posix_time::time_duration;

const char *characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
uint64_t length = 5;
//const char *target = "594f803b380a41396ed63dca39503542"; // echo -n aaaaa | md5sum
const char *target = "95ebc3c7b3b9f1d2c40fec14415d3cb8"; // echo -n zzzzz | md5sum
//const char *target = "595cd91f6c087ec5d81e56862a791283"; // h4Ck3Ra1bfda8b8fa2872e

uint64_t pow(const uint64_t &a, const uint64_t &b)
{
	uint64_t c = 1;
	for (uint64_t i = 0; i < b; ++i) {
		c = c * a;
	}
	return c;
}

void string_to_hex(const string &src, unsigned char *md, size_t md_length)
{			
	for (unsigned int i = 0; i < md_length; ++i) {
		int n;
		istringstream ss;
		ss.str(src.substr(i * 2, 2));
		ss >> std::hex >> n;
		md[i] = (unsigned char)n;
	}
}

string hex_to_string(unsigned char *md, size_t md_length)
{
	stringstream ss;
	for (unsigned int i = 0; i < md_length; ++i) {
		ss << std::hex << (unsigned int)md[i];
	}
	return ss.str();
}

inline void make_string(const char *characters, const size_t &characters_len, const uint64_t &i, char *str, const size_t &str_len)
{
	uint64_t b = characters_len;
	uint64_t n = i;

	for (uint64_t d = 0; d < str_len; ++d) {
		uint64_t r = n % b;
		str[d] = characters[r];
		n = n / b;
	}
}

int main(int argc, char *argv[])
{
	unsigned char target_md[MD5_DIGEST_LENGTH];
	string_to_hex(target, target_md, MD5_DIGEST_LENGTH);
	cout << "target_hash = " << hex_to_string(target_md, MD5_DIGEST_LENGTH) << endl;

	uint64_t characters_len = strlen(characters);
	
	uint64_t total = pow(characters_len, length);
	cout << format("total = %ld") % total << endl;

	ptime st = second_clock::local_time();

	#ifdef _OPENMP
	#pragma omp parallel for 
	#endif
	for (uint64_t i = 0; i < total; ++i) {
		// make target string
		char src[length+1];
		src[length] = 0; //null terminate
		make_string(characters, characters_len, i, src, length);	

		unsigned char md[MD5_DIGEST_LENGTH];
	    MD5((const unsigned char*)src, length, md);

		if (memcmp(md, target_md, MD5_DIGEST_LENGTH) == 0) {
			#ifdef _OPENMP
			cout << "[thread:" << omp_get_thread_num() << "] ";
			#endif 
			cout << format("hit!! : i=%ld, src=%s") % i % string(src) << endl;
			time_duration dt = second_clock::local_time() - st;
			cout << "process time = " << dt << endl;
			exit(0);
		}
		if (i % 10000000 == 0) {
			time_duration dt = second_clock::local_time() - st;
			#ifdef _OPENMP
			cout << "[thread:" << omp_get_thread_num() << "] ";
			#endif 
			cout << format("i=%ld src=%s md5=%s (%3.2f%%) ")
					% i
					% string(src)
					% hex_to_string(md, MD5_DIGEST_LENGTH)
					% (i/(double)total*100.0)
				<< "process_time=" << dt
				<< endl;
		}
	}

	return 0;
}
