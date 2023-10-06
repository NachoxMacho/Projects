#pragma once // make sure this header file is only compiled once
#include <iostream>
#include <ostream>
#include <vector> // for the list of cards
#include <cassert> // for assertions 
#include <algorithm> // for sorting
#include "cards.h"

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

