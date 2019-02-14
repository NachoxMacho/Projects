#include <cassert>
#include <string>

#define number_of_cities 7 * number_of_players
#define number_of_players 3

struct city
{
	// house[0] costs 10
	// house[1] costs 15
	// house[2] costs 20
	// OR
	// house[i] cost = 10 + i * 5
	bool house_full[3]{ false };

	// the name of the city
	std::string name;

	city * city_connections[number_of_cities]{ NULL };
	int connection_weight[number_of_cities]{ 0 };
	int connections;

	city()
	{
		
	}
};

struct power_plant
{

};

struct resource
{
	std::string name;
	int price;
};

class game
{
	city board[number_of_cities];

	game()
	{

	}

	game(char sections[number_of_players], bool german_side)
	{
		for (int i = 0; i < number_of_players; i++)
		{
			load_section(sections[i], german_side);
		}
	}

	void load_section(char section, bool german_side)
	{
		switch (section)
		{
		case 'y':
			break;
		}
	}

	city operator[] (int index) { if (index >= 0 && index < number_of_cities) return board[index]; return city(); }
};

class player
{

};