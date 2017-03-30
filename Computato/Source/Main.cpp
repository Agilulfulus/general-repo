#include "Computato.h"

Parser * p;

int main(int argc, char* argv[])
{
	std::string filename;

	if (argc > 1)
	{
		filename = argv[1];
	}

	if (filename != "")
	{
		system("cls");
		system("color F0");

		p = new Parser(filename);

		p->run();

		//system("pause");

		system("color 07");
	}

	delete p;

	return 0;
}
