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


void runTest();
