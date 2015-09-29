#ifndef _SIZEOT_TEST_
#define _SIZEOT_TEST_
struct s1
{
	bool a;
	short b;
	int c;
};

struct s2
{
	int c;
	short b;
	bool a;
	bool d;
};

class c1
{

};

class c2
{
	void print()
	{
		std::cout << "print" << std::endl;
	}
};

class c3
{
	void print()
	{
		std::cout << "print" << std::endl;
	}

private:
	int m_a;
};

class c4
{
public:
	virtual void print()
	{
		std::cout << "print" << std::endl;
	}
};

class c5
	: public c4
{

};

class c6
	: public c4, c2
{

};

class c7
{
public:
	virtual void print2();
};

class c8
	: public c4, c7
{

};

void runTest();

#endif
