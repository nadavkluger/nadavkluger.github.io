#include <iostream>

using namespace std;

typedef unsigned char byte;
typedef unsigned short ushort;

#define BITS(x) (g_num_of_bits[x >> 1])

struct NumBits {
	ushort Low : 4;
	ushort High : 4;
};

// No need for extra +1 due to spiecal case handling of 0xf
NumBits g_num_of_bits[SHRT_MAX + 1];

void Init() {
	ushort num;
	byte res;
	NumBits *tmp;

	for (ushort i = 0; i < USHRT_MAX; i++)
	{
		num = i;
		res = 0;
		do { res += num & 1; } while (num >>= 1);

		tmp = &BITS(i);
		if (i & 1) { tmp->High = res;}
		else { tmp->Low = res; }
	}
}

template <typename T>
byte CountBits(T num) {
	static ushort* pnum;
	static byte i, res;

	pnum = (ushort*)&num;
	res = 0;

	for (i = 0; i < sizeof(num) >> 1; i++)
	{
		res += (ushort)~pnum[i] ? (pnum[i] & 1 ? BITS(pnum[i]).High : BITS(pnum[i]).Low) : 16;
	}

	return res;
}

void main() {
	long long x = 17;
	
	Init();

	cout << (int) CountBits(x) << endl;
}