#include "Computato.h"

Parser * p;

int main(int argc, char* argv[])
{
	std::string filename;

	system("cls");
	system("color F0");

	if (argc > 1)
	{
		filename = argv[1];
	}

	if (filename != "")
	{
		p = new Parser(filename);

		p->run();
	}

	system("color 07");

	delete p;

	return 0;
}
