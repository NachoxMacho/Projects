#pragma once
#include <cmath>

struct pixel
{
	//==========================================================================
	//								VARS
	//==========================================================================

	// coordinates of pixel
	int x, y;
	// if this pixel is an edge
	bool edge;

	//==========================================================================
	//								INFO
	//==========================================================================

	// a pixel is larger than another if it is farther from 0, 0
	// more specfically it is farther up and to the right
	// as such a pixel 1, 3 is larger than 768, 1 as the y value is tested first
	// this is used in the binary search that segments do (segment.h)

	//==========================================================================
	//						COSNTRUCTORS & DESTRUCTORS
	//==========================================================================

	// ===== CONSTRUCTOR =====
	pixel(int x, int y) { this->x = x, this->y = y; }
	pixel(int x, int y, bool edge) { this->x = x, this->y = y, this->edge = edge; }
	pixel() { x = y = 0, edge = false; }

	//==========================================================================
	//								METHODS
	//==========================================================================

	// ===== distance =====
	// returns the distnace between this pixel and p
	float distance(pixel p) { return sqrt((p.x - x) * (p.x - x) + (p.y - y) * (p.y - y)); }

	//==========================================================================
	//								OPERATORS
	//==========================================================================

	// ===== '=' =====
	// sets this pixel's coordinates to p's coordinates
	void operator =(pixel p) { this->x = p.x, this->y = p.y; }

	// ===== '==' =====
	// returns true if the coordinate values of the pixels match
	bool operator ==(pixel p) { return (this->x == p.x && this->y == p.y); }
	bool operator !=(pixel p) { return !(this->operator==(p)); }

	// ===== '>' & '<' & '>=' & '<=' =====
	// returns if p is coser or farther from the origin, see INFO for more
	bool operator <(pixel p) { return ((p.y == this->y && p.x < this->x) || (p.y < this->y)); }
	bool operator <=(pixel p) { return ((p.y == this->y && p.x < this->x) || (p.y < this->y) || (p.x == this->x && p.y == this->y)); }
	bool operator >=(pixel p) { return ((p.y == this->y && p.x > this->x) || (p.y > this->y) || (p.x == this->x && p.y == this->y)); }
	bool operator >(pixel p) { return ((p.y == this->y && p.x > this->x) || (p.y > this->y)); }

};
