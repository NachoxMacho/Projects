#pragma once // make sure this header file is only compiled once
#include <iostream>
#include <ostream>
#include <vector> // for the list of cards
#include <cassert> // for assertions 
#include <algorithm> // for sorting

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

