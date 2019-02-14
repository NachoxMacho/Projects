#include <iostream>
#include "game.h"

int main()
{
	city c;

	for (int i = 0; i < 3; i++)
		if (!c.house_full[i])
			std::cout << i << " is true" << std::endl;


	return 0;
}