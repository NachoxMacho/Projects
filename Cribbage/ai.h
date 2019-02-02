#pragma once
#include "cards.h" // to get the player and cards

class AI : public player
{
public:

	void pick_crib_cards(player * dealer)
	{
		int c1; // this is the first card to remove to get the best hand
		int c2; // this is the second card to remove to get the best hand
		int best_potential = 0; // this is the current best potential a hand has

		
		// calculate average potential score
		// if score is better than the previous average
			// store the pair of cards and score

		for (int i = 0; i < hand.size(); i++)
		{
			for (int j = i + 1; j < hand.size(); j++)
			{

				// for each pair of cards removed from hand
				deck p_hand(hand - hand[i] - hand[j]); // the potential hand

				int p_score = score_p_hand(p_hand); // the current score of this potential hand


				// check if there is a flush
				// if there is there are 4 points in hand and 1 more point 
				// possible if the flipped card is in the same suit
				if (is_flush(p_hand))
					p_score += 1;


				// check if p_hand has a Jack in it for knobs
				// one more potential point for each jack
				for (int k = 0; k < p_hand.size(); k++) { if (p_hand[k].Rank() == 11) p_score += 1; }


				for (int c = 1; c < 14; c++)
				{
					deck p_flip(p_hand + card(Heart, c));
					p_score += score_runs(p_flip);
					p_score += score_pairs(p_flip);
					p_score += score_15(p_flip);
				}

				if (p_score > best_potential)
				{
					c1 = i;
					c2 = j;
					best_potential = p_score;
				}
			}
		}

		dealer->crib += hand[c1];
		dealer->crib += hand[c2];

		hand.remove(c1);
		if (c2 > c1)
			c2--;
		hand.remove(c2);
	}

	int score_p_hand(deck &p_hand)
	{
		// a potential 4 card hand can score in the following ways:
		// a flush for 4 points
		// a 15 for 2 points
		// a run which is 1 point for each card
		// a pair for 2 points

		int hand_score = 0;
		
		// check for a flush
		if (is_flush(p_hand))
			hand_score += 4;

		for (int i = 0; i < p_hand.size(); i++)
		{
			for (int j = i + 1; j < p_hand.size(); j++)
			{
				if (p_hand[i].Rank() == p_hand[j].Rank())
					hand_score += 2;
			}
		}

		// check for 15s
		hand_score += score_15(p_hand);

		// check for runs
		hand_score += score_runs(p_hand);

		return hand_score;
	}

	// will return true if all cards in 'h' are the same suit
	bool is_flush(deck &h)
	{
		suits s = h[0].Suit();
		for (int i = 1; i < h.size(); i++)
		{
			if (h[i].Suit() != s)
				return false;
		}
		return true;
	}

	card play_peg_card(int running_total, deck &played)
	{
		// sort the hand of cards
		hand.sort();

		card playing; // the best card to play
		int best_score = -1; // the best score
		for (int i = hand.size() - 1; i > -1; i--)
		{
			// if the card is not legal, can't play it
			if (hand[i].Value() + running_total > 31)
				continue;

			int p_score = 0;

			// if playing a card makes 15 or 31, it's worth 2 points
			if (running_total + hand[i].Value() == 15) { p_score += 2; }
			if (running_total + hand[i].Value() == 31) { p_score += 2; }

			// pair scoring
			// 2 of a kind = 2 points  (1 pairs)
			// 3 of a kind = 6 points  (3 pairs)
			// 4 of a kind = 12 points (6 pairs)

			// if the first card matches, 2 points are added equaling 2 points total
			// if the second card matches, another 4 points are added equaling 6 points total
			// if the third card matches, another 6 points are added equaling 12 points total
			for (int j = 1; j < played.size(); j++)
			{
				if (played[played.size() - j].Rank() == hand[i].Rank()) { p_score += 2 * j; }
				// stop as soon as cards aren't matching
				else break;
			}

			// add the played card to the pool of previously played card
			played += hand[i];

			// run scoring
			// 1 point per card in the run
			// only score the largest run
			if (played[played.size() - 1].Rank() != playing.Rank()) // there can't be any runs if a pair was found
			{
				int highest_run = 0;
				for (int k = 3; k < 10; k++)
				{
					// k is the size of run we are checking for
					// if there aren't enough cards played already, can't check for more runs

					if (played.size() < k)
					{
						p_score += highest_run;
						break;
					}

					// all the cards currently under consideration for a run
					deck run;

					// add k number of cards to 'run'
					for (int j = played.size() - 1; j > -1 && run.size() < k + 1; j--) { run += played[j]; }

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
					if (run_found) highest_run = k;
				}
			}

			played -= hand[i];

			// if a better card is found, replace the old best card
			if (p_score > best_score)
			{
				playing = hand[i];
				best_score = p_score;
			}
		}
		// if a card was selected
		if (playing != card())
		{
			// output the card
			std::cout << "I will play ";
			playing.print(std::cout);
			std::cout << "getting " << best_score << " points." << std::endl;
			// remove the card from the hand and add it to played
			this->played += playing;
			hand.remove(playing);
		}
		return playing;
	}

};