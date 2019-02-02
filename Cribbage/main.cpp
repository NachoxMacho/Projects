#include <iostream> // for input and output
#include <algorithm> // for sorting
#include <ctime> // for setting the seed based on time
#include "cards.h" // for all the cards
#include "ai.h" // for bob

int main()
{
	// set the random seed so each game is different
	// 1549039473 next
	//*/
	int seed = time(NULL);
	/*/
	int seed = 1549045139;
	//*/
	srand(seed);
	// output seed to terminal for troubleshooting
	std::cout << "Seed: " << seed << std::endl;
	std::cout << "--------------------" << std::endl;

	// create the players and dealer pointers
	player * p1; // pointer to player 1
	AI * bob;
	player *dealer; // pointer to the dealer, which is the person with the crib | will be equal to p1 or p2

	deck full_deck(true); // a whole deck of cards
	p1 = new player(); // the first player
	bob = new AI();
	dealer = bob; // assign dealer to p1 ( FIX: make this random)

	// the game is a race to 121 points
	// while nobody has won the game
	while (p1->score < 121 && bob->score < 121)
	{
		// "deal" the cards to each player
		p1->draw_hand(full_deck);
		bob->draw_hand(full_deck);

		// have each player select the cards to throw to the dealer
		p1->pick_crib_cards(dealer, std::cout);
		bob->pick_crib_cards(dealer);

		// cut a card from the rest of the deck
		card flip = full_deck.draw();

		std::cout << "--------------------" << std::endl;
		std::cout << "The flipped card is "; flip.print(std::cout); std::cout << std::endl;
		std::cout << "--------------------" << std::endl;

		// if a Jack is cut the dealer gets 2 points
		if (flip.Rank() == 11) dealer->score += 2;

		// this section is dealing with pegging
		int current_score = 0; // this is the running total

		player * current_player = (dealer == p1) ? bob : p1; // whoever's turn it is to play a card
		player * last_card_played; // the last player to play a card

		deck played; // a list of cards played sinc eht last 31 or go
		bool go = false; // if a player can't play
		bool player_turn = (dealer != p1);

		while (p1->hand_size() > 0 || bob->hand_size() > 0)
		{
			// if a player's hand is empty and the other's isn't, other player plays remaining cards
			if (current_player->hand_size() == 0)
			{
				// swap player
				current_player = (current_player == p1) ? bob : p1;
				player_turn = !player_turn;
			}

			card playing;

			if (player_turn)
			{
				// output stats
				std::cout << "Total: " << current_score << " | P1: " << p1->score << " | PB: " << bob->score << std::endl;
				played.print(std::cout);

				std::cout << std::endl << "--------------------" << std::endl;

				// have the player select a card from their hand to be played
				playing = p1->play_peg_card(std::cout, current_score);
			}
			else
				playing = bob->play_peg_card(current_score, played);

			// if no card can be played
			if (playing == card())
			{
				// both players have passed
				if (go)
				{
					std::cout << "GO, score 1 point" << std::endl;
					// give the player who last play a point and reset
					last_card_played->score += 1;
					go = false;
					played.clear();
					current_score = 0;
				}
				// only on e player has passed
				else
				{
					std::cout << "GO" << std::endl;
					go = true;
				}
				std::cout << "--------------------" << std::endl;
				continue;
			}

			// if a card is played reset go
			go = false;

			// ===== Playing a Card =====

			std::cout << "--------------------" << std::endl;
			current_score += playing.Value(); // add value to 'current_score'
			last_card_played = current_player; // set 'last_card_played' player to current player
			current_player = (current_player == p1) ? bob : p1; // swap the current player
			player_turn = !player_turn; // swap the player turn

			// if playing the card makes 15, the 'last_card_played' player gets 2 points
			if (current_score == 15) { last_card_played->score += 2; }

			// pair scoring
			// 2 of a kind = 2 points  (1 pairs)
			// 3 of a kind = 6 points  (3 pairs)
			// 4 of a kind = 12 points (6 pairs)

			// if the first card matches, 2 points are added equaling 2 points total
			// if the second card matches, another 4 points are added equaling 6 points total
			// if the third card matches, another 6 points are added equaling 12 points total
			for (int i = 1; i < played.size(); i++)
			{
				if (played[played.size() - i].Rank() == playing.Rank()) { last_card_played->score += 2 * i; }
				// stop as soon as cards aren't matching
				else break;
			}

			// add the played card to the pool of previously played card
			played += playing;

			// run scoring
			// 1 point per card in the run
			// only score the largest run
			if (played[played.size() - 1].Rank() != playing.Rank()) // there can't be any runs if a pair was found
			{
				int highest_run = 0;
				for (int i = 3; i < 10; i++)
				{
					// i is the size of run we are checking for
					// if there aren't enough cards played already, can't check for more runs

					if (played.size() < i)
					{
						last_card_played->score += highest_run;
						break;
					}

					// all the cards currently under consideration for a run
					deck run;

					// add i number of cards to 'run'
					for (int j = played.size() - 1; j > -1 && run.size() < i + 1; j--) { run += played[j]; }

					// sort the cards
					run.sort();

					// if the cards in 'run' form a run
					bool run_found = true;
					for (int j = 0; j < run.size() - 1; j++)
					{
						if (run[j].Rank() != run[j + 1].Rank() - 1 && run[j].Rank() != 0)
						{
							run_found = false;
							break;
						}
					}

					// if a run is found, store highest run
					if (run_found) highest_run = i;
				}
			}

			// if playing the card makes 31, the 'last_card_played' player gets 2 points, the running total goes to 0 and pool of previously played cards is emptied
			if (current_score == 31)
			{
				last_card_played->score += 2;
				current_score = 0;
				played.clear();
			}
		}

		// reset players hands to before pegging
		p1->return_played();
		bob->return_played();
		last_card_played->score++;

		// output round stats
		std::cout << "--------------------" << std::endl;
		std::cout << "Round Summary: " << std::endl;
		flip.print(std::cout);
		std::cout << std::endl;
		std::cout << "Player 1 Hand scores " << p1->score_hand(flip) << std::endl;
		p1->hand_print(std::cout); std::cout << std::endl;
		std::cout << "Player B Hand scores " << bob->score_hand(flip) << std::endl;
		bob->hand_print(std::cout); std::cout << std::endl;
		std::cout << "Crib scores " << dealer->score_crib(flip) << std::endl;
		dealer->crib_print(std::cout); std::cout << std::endl;
		std::cout << "Score is " << p1->score << " to " << bob->score << std::endl;
		std::cout << "--------------------" << std::endl;

		// change the dealer and clear the crib
		dealer->crib.clear();
		dealer = (dealer == p1) ? bob : p1;

		// reset the deck
		full_deck = deck(true);
	}

	// after the game is done, delete the players and return
	delete p1, bob;
	return 0;
}