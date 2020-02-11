#include "BbSize.h"

BbSize BbSize::operator+(BbSize rhs)
{
	int a, b;
	a = this->bit + rhs.bit;
	b = this->Byte + rhs.Byte;
	return BbSize(b, a);
}

BbSize BbSize::operator+=(BbSize rhs)
{
	int a, b;
	a = this->bit + rhs.bit;
	b = this->Byte + rhs.Byte;
	*this = BbSize(b, a);
	return *this;
}

BbSize BbSize::operator+(int const rhs)
{
	BbSize bb;
	bb.Byte = this->Byte + rhs;
	return bb;
}

BbSize BbSize::operator=(int rhs)
{
	this->Byte = rhs;
	return *this;
}

bool BbSize::operator<(BbSize rhs)
{
	int a, b;
	a = this->Byte;
	if (this->bit) a++;
	b = rhs.Byte;
	if (rhs.bit) b++;
	return a < b;
}

BbSize BbSize::operator*(int rhs)
{
	int a = this->Byte;
	if (this->bit) a++;
	return BbSize(a * rhs, 0);
}

int BbSize::getBits() {	
	return bit;
}


int BbSize::getBytes() {
	return Byte;
}

BbSize::BbSize() {
	bit = 0;
	Byte = 0;
};

BbSize::BbSize(int _Byte, int _bit) : Byte(_Byte), bit(_bit)
{
	if (bit > 8) {
		Byte += bit / 8;
		bit %= 8;
	}
}
