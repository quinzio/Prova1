#pragma once
class BbSize
{
private:
	int bit;
	int Byte;
public:
	BbSize operator+(BbSize rhs);
	BbSize operator+=(BbSize rhs);
	BbSize operator+(int rhs);
	BbSize operator=(int rhs);
	bool operator<(BbSize rhs);
	BbSize operator*(int rhs);
	BbSize(int _Byte, int _bit);
	BbSize();
};

