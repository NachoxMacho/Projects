#pragma once // make sure this header file is only compiled once
#include <vector> // for the list of cards
#include <cassert> // for assertions 

// how many cards each player is dealt for each hand
#define max_hand_size 6

// all the i + difsible suits in a standard deck of cards
enum suits { Heart, Diamond, Club, Spade};

struct card
{   // ========== METHODS ==========

	// ===== CONSTRUCTOR =====
	// by default the constructor will store a 0 of Hearts in the card which is in valid
	card() { suit = Heart, value = 0; }
	// ===== CONSTRUCTOR =====
	// if a suit and value are passed to the constructor then those will be assigned to the card assuming they are valid
	// a valid card has a suit of { Heart, Diamond, Club, Spade} and a value from 1 - 13 inclusive
	card(suits suit, int v) { if (v > 0 && v < 14) this->suit = suit, this->value = v; }

	// ===== VALUE =====
	// the value is a number from 1 - 10
	// this is used in totals and scoring 15s
	int Value() { if (value > 10) return 10; return value; }

	// ===== RANK =====
	// the "real" value of the card, a number from 1 - 13
	// each number represents a card from Ace through King
	int Rank() { return value; }

	// ===== SUIT =====
	// returns the suit value
	suits Suit() { return suit; }

	// ===== PRINT =====
	// will output the card's suit and rank to out_s
	void print(std::ostream &out_s)
	{
		// if the card is a 10, Jack, Queen, or King show the symbol
		if (value > 9)
		{
			switch (value)
			{
			case 10: out_s << "T"; break;
			case 11: out_s << "J"; break;
			case 12: out_s << "Q"; break;
			case 13: out_s << "K"; break;
			}
		}
		// otherwise output value
		else
			out_s << value;

		// output the initial of the suit
		switch (suit)
		{
		case Heart:   out_s << "H "; break;
		case Diamond: out_s << "D "; break;
		case Club:    out_s << "C "; break;
		case Spade:   out_s << "S "; break;
		}
	}

	// ========== OPERATORS ==========

	// ===== '==' =====
	// returns true if the suit and rank of the cards match
	bool operator == (card c) { return (suit == c.Suit() && value == c.Rank()); }

	// ===== '=' =====
	// will set this card's suit and rank to the card on the right hand side
	void operator = (card c) { suit = c.Suit(), value = c.Rank(); }

	// ===== '<' =====
	// a card is < another if it's rank is smaller than the others
	// this is primarily used in sorting
	bool operator < (card c) { return this->value < c.Rank(); }

	// ===== '!=' =====
	// cards are considered unequal if their suit or rank does not match
	bool operator != (card c) { return !(suit == c.Suit() && value == c.Rank()); }

private:

	// ========== VARIABLES ==========
	suits suit; // the suit of the card
	int value; // the 'rank' of the card or a number from 1 - 13
};

class deck
{
	// ========== VARIABLES ==========
	std::vector<card> cards; // a list of all the cards in the deck

public: // ========== METHODS ==========

	// ===== CONSTRUCTOR =====
	// if fill is set to false it will do nothing
	// if fill is set to true it will load a standard deck of cards in this deck
	deck(bool fill = false) 
	{
		// if fill is false do nothing
		if (!fill)
			return;
		// generate a standard deck of cards
		for (int i = 1; i < 14; i++)
		{
			cards.push_back(card(Heart, i));
			cards.push_back(card(Diamond, i));
			cards.push_back(card(Club, i));
			cards.push_back(card(Spade, i));
		}
	}

	// ===== CONSTRUCTOR =====
	// will copy all the cards from 'd' into this deck
	deck(deck * d)
	{
		this->cards.clear();
		for (int i = 0; i < d->cards.size(); i++)
			this->cards.push_back(d->cards[i]);
	}
	
	// ===== DRAW =====
	// if index is out of bounds a generic constructor card is returned which is 0 of Hearts
	// if a valid index is passed the card at that index is returned and removed from the deck
	card draw(int index) { if (index < 0 || index >= cards.size()) return card(); card c = cards[index]; remove(index); return c; }

	// ===== DRAW RANDOM =====
	// if no index is specified a random card is drawn from the deck
	card draw() { return draw(rand() % cards.size()); }

	// ===== REMOVE =====
	// if a specific index is given, the card will be removed from the deck
	void remove(int index) { cards.erase(cards.begin() + index); }
	// ===== REMOVE =====
	// if a card is given, the deck will be searched for the first appearance of the card and remove that
	// if no matching card is found nothing happens
	void remove(card c) { int i = search(c); if (i != -1) remove(i); }

	// ===== SEARCH =====
	// will linearly search the deck for the card
	int search(card c)
	{
		// starting at the front, check for a match
		for (int i = 0; i < cards.size(); i++)
			if (cards[i] == c) // if a match is found return the position
				return i;
		return -1; // return -1 if no match
	}

	// ===== ADD =====
	// will add the card 'c' to the deck
	void add(card c) { cards.push_back(c); }

	// ===== SIZE =====
	// returns how many cards are in the deck
	int size() { return cards.size(); }

	// ===== CLEAR =====
	// will remove all cards from the deck
	void clear() { cards.clear(); }

	// ===== PRINT =====
	// will print all the cards in the deck
	void print(std::ostream & out_s)
	{
		// for each card, call it's print function
		for (int i = 0; i < size(); i++)
			cards[i].print(out_s);
	}

	// ===== SORT =====
	// will sort the cards in the deck by rank
	void sort() { std::sort(cards.begin(), cards.end()); }

	// ========== OPERATORS ==========

	// ===== '-=' =====
	// will attempt to remove the card 'c' from this deck
	void operator -= (card c) { remove(c); }

	// ===== '+=' =====
	// will add the card 'c' to the deck
	void operator += (card c) { add(c); }

	// ===== '[]' =====
	// returns the card in position 'index' from 'cards'
	card operator [] (int index)
	{
		// if index is out of bounds return generic constructor
		if (index >= size() || index < 0)
			return card();

		// return card in position 'index'
		return cards[index];
	}

	// ===== '=' =====
	// will clear the deck of all cards and copy all cards from 'd' to this deck
	void operator = (deck d)
	{
		// clear current deck
		cards.clear();
		// copy over 'd' deck
		for (int i = 0; i < d.size(); i++)
			add(d[i]);
	}

	// ===== '-' =====
	// will return the deck without the card 'c'
	deck operator - (card c)
	{
		deck new_deck(this);
		new_deck.remove(c);

		return new_deck;
	}

	// ===== '+' =====
	// adds the card 'c' to the deck
	deck operator + (card c)
	{
		deck new_deck(this);
		new_deck.add(c);

		return new_deck;
	}
};

class player
{
	// SCORING
	// 2 points per pair
	// 1 point for knobs (have a jack of the same suit as the flipped card)
	// 1 point for each card in a flush (all cards of the same suit. the crib must have all 4 and the flipped card to count)
	// 1 point for each card in a run (only counting largest runs with different cards)

public:

	// ========== VARIABLES ==========
	deck hand; // all the cards in the player's hand
	deck crib; // all the cards in the player's crib
	deck played; // all the cards that the player has already played
	int score; // how many points the player has

	player()
	{
		score = 0;
		hand.clear();
		crib.clear();
		played.clear();
	}
	
	// ========== METHODS ==========

	// ===== HAND SIZE =====
	// will return the number of cards in hand
	int hand_size() { return hand.size(); }

	// ===== HAND PRINT =====
	// will print all the cards in hand to out_s
	void hand_print(std::ostream &out_s) { hand.print(out_s); }

	// ===== CRIB PRINT =====
	// will print all the cards in crib to out_s
	void crib_print(std::ostream &out_s) { crib.print(out_s); }

	// ===== RETURN PLAYED =====
	// will set hand = played and then clear all the cards in played
	void return_played() { hand = played; played.clear(); }

	// ===== DRAW HAND =====
	// will clear the current hand and then draw fron the deck 'base' until it hand_size() == max_hand_size
	// will also sort the cards when done drawing
	void draw_hand(deck & base)
	{
		// clear the hand
		hand.clear();
		// draw new hand
		while (hand.size() < max_hand_size)
			hand += base.draw();
		// sort the hand
		hand.sort();
	}

	// ===== PICK CRIB CARDS =====
	// will move 2 cards from 'this->hand' to 'dealer->crib'
	// will remove the 2 moved cards from the hand
	void pick_crib_cards(player * dealer, std::ostream &out_s)
	{
		int c1 = -1; // the first card to send to the crib
		int c2 = -1; // the second card to send to the crib
		// while valid cards are NOT picked (numbers 1 -> 6 and not the same)
		while (c1 < 0 || c1 >= hand.size() || c2 < 0 || c2 >= hand.size() || c2 == c1)
		{
			// print the hand
			hand.print(out_s); out_s << std::endl;
			if (dealer == this)
				std::cout << "Pick cards to send to your crib: ";
			else
				std::cout << "Pick cards to send to opponent's crib: ";
			std::cin >> c1 >> c2;
			--c1, --c2;
		}

		// add the cards to the crib
		dealer->crib += hand[c1];
		dealer->crib += hand[c2];
 		
		// remove the cards from this player's hand
		hand.remove(c1);
		if (c2 > c1)
			c2--;
		hand.remove(c2);
	}

	// ===== PLAY PEG CARD =====
	// if no card can legally be played will return generic card()
	// otherwise will return a card from the hand and move that card to played
	// NOTE: the card returned + running_total will be <= 31
	card play_peg_card(std::ostream &out_s, int running_total)
	{
		// check if there are any cards that can be played legally
		bool can_play = false;
		for (int i = 0; i < hand.size(); i++)
		{
			if (hand[i].Value() + running_total <= 31)
			{
				can_play = true;
				break;
			}
		}

		// if not, return an error card
		if (!can_play) { return card(); }

		int c = -1; // the card position
		while (c < 0 || c >= hand.size())
		{
			// print the hand
			hand.print(out_s); 
			std::cout << "Pick a card to play: ";
			std::cin >> c;
			// decrease the position since people start counting at 1
			--c; 
			// if the selected card is not legal, ask for another card
			if (hand[c].Value() + running_total > 31)
				c = -1;
		}
		// store the card to be returned
		card return_card = hand[c];
		// remove the card from the hand
		hand.remove(c);
		// add the card to played
		played += return_card;

		return return_card;
	}
	
	// ===== SCORE PAIRS =====
	// will return the score of all pairs in the hand
	// will not add the score to anything such as 'this->score'
	// each pair is worth 2 points
	int score_pairs(deck &p_hand)
	{
		int hand_score = 0; // the score of pairs in the hand

		// for each pair of cards in the hand
		for (int i = 0; i < p_hand.size(); i++)
			for (int j = i + 1; j < p_hand.size(); j++)
				// if they are a pair, add 2 to the score
				if (p_hand[i].Rank() == p_hand[j].Rank())
					hand_score += 2;
		// return the score
		return hand_score;
	}
	
	// ===== SCORE RUNS =====
	// will return the score of all runs in 'p_hand'
	// will not add the score to anything such as 'this->score'
	// a run is worth 1 point for each card in the run
	// EX: 3 3 4 5 is worth 6 points from runs as there are 2, 3 card runs
	int score_runs(deck &p_hand)
	{
		int hand_score = 0; // the score of runs in 'p_hand'

		// sort the cards in the hand
		p_hand.sort();

		for (int i = 0; i < p_hand.size(); i++)
		{
			int multiplier = 1; // how many duplicate runs there are
			int duplicates = 1; // how many duplicates of the card currently being examined
			int dif = 1; // how many cards away from 'i' that is being examined
			int run_size = 1; // how many different numbers make up the run

			for (dif; dif + i < p_hand.size() + 1; dif++)
			{
				// if the next card is a duplicate of the last, increase duplicates
				if (p_hand[i + dif - 1].Rank() == p_hand[i + dif].Rank()) { duplicates++; }
				// if the next card is a continuation of the run, increase multiplier by duplicates, reset duplicates and increase run_size
				else if (p_hand[i].Rank() + run_size == p_hand[i + dif].Rank()) { multiplier *= duplicates, duplicates = 1, run_size++; }
				// otherwise there is no more run
				else { break; }
			}

			// add the last card consequences
			multiplier *= duplicates;
			duplicates = 1;

			if (i + dif == p_hand.size() || p_hand[i].Rank() + run_size - 1 != p_hand[i + dif].Rank()) dif--; // if dif is overshooting the run, decrease the dif
			if (run_size > 2) { hand_score += run_size * multiplier; } // if at least a 3 card run, add the score
			i += dif; // prevent double counting, move i up to the end of the run
		}
		// return the score
		return hand_score;
	}

	// ===== SCORE 15 =====
	// will return the score of 15s in 'p_hand'
	// each unique set of cards that add up to 15 is worth 2 points
	// the value of the card is equla to the rank unless the rank > 10 where the value is just 10
	// will not add the score to anything such as 'this->score'
	int score_15(deck &p_hand)
	{
		int hand_score = 0; // the score of the hand
		
		// for each set of 2, 3, 4, and 5
		for (int i = 0; i < p_hand.size(); i++)
			for (int j = i + 1; j < p_hand.size(); j++)
				// check sets of 2
				if (p_hand[i].Value() + p_hand[j].Value() == 15)
					hand_score += 2;
				else 
					for (int k = j + 1; k < p_hand.size(); k++)
						// check sets of 3
						if (p_hand[i].Value() + p_hand[j].Value() + p_hand[k].Value() == 15)
							hand_score += 2;
						else 
							for (int l = k + 1; l < p_hand.size(); l++)
								// check sets of 4
								if (p_hand[i].Value() + p_hand[j].Value() + p_hand[k].Value() + p_hand[l].Value() == 15)
									hand_score += 2;
								else 
									for (int m = l + 1; m < p_hand.size(); m++)
										// check the set of 5
										if (p_hand[i].Value() + p_hand[j].Value() + p_hand[k].Value() + p_hand[l].Value() + p_hand[m].Value() == 15)
											hand_score += 2;
		// return the score
		return hand_score;
	}

	// ===== SCORE HAND =====
	// will score 'hand' and return the value
	// will also add the returned value to the player's score
	// see scoring for more information about how scoring is calculated
	int score_hand(card flipped)
	{
		int hand_score = 0; // the hand's score
		suits s = hand[0].Suit(); // the starting suit for the flush
		bool four_card = true; // if a four card flush is found

		// check for a flush && knobs
		for (int i = 1; i < hand.size(); i++)
		{
			// flush check
			if (hand[i].Suit() != s)
				four_card = false;

			// knobs check
			if (hand[i].Rank() == 11 && hand[i].Suit() == flipped.Suit())
				hand_score += 1;
		}

		// score the flush if it's there
		if (four_card && flipped.Suit() != s)
			hand_score += 4;
		else if (four_card && flipped.Suit() == s)
			hand_score += 5;

		// add the card to the hand to simplify searching for 15s and runs
		hand += flipped;

		hand_score += score_15(hand);
		hand_score += score_runs(hand);
		hand_score += score_pairs(hand);

		// remove the card from the hand
		hand -= flipped;
		// make sure the score didn't drop below zero
		assert(hand_score >= 0);
		// add the score to the total score
		score += hand_score;
		return hand_score;
	}

	// ===== SCORE CRIB =====
	// will score 'crib' and return the value
	// will also add the returned value to the player's score
	// see scoring for more information about how scoring is calculated
	int score_crib(card flipped)
	{
		// if there is no crib there is no score
		if (crib.size() == 0)
			return 0;

		int crib_score = 0; // the hand's score
		suits s = crib[0].Suit(); // the starting suit for the flush
		bool four_card = true; // if the 4 crib cards all match on suit

		// check if all 5 cards together == 15
		if (crib[0].Value() + crib[1].Value() + crib[2].Value() + crib[3].Value() + crib[4].Value() == 15)
			crib_score += 2;

		// check for a flush and knobs
		for (int i = 0; i < hand.size(); i++)
		{
			// flush check
			if (crib[i].Suit() != s)
				four_card = false;
			
			// knobs check
			if (crib[i].Rank() == 11 && crib[i].Suit() == flipped.Suit())
				crib_score += 1;
		}

		// the crib can only get a 5 card flush
		if (four_card && flipped.Suit() == s)
			crib_score += 5;

		// add the card to the crib
		crib += flipped;

		// call the functions for scoring 15s, runs, and pairs
		crib_score += score_15(crib);
		crib_score += score_runs(crib);
		crib_score += score_pairs(crib);

		// remove the card from the crib
		crib -= flipped;
		// make sure the score didn't drop below zero
		assert(crib_score >= 0);
		// add the score to the total score
		score += crib_score;
		return crib_score;
	}
};
