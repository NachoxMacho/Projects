#pragma once
#include <vector>
#include "image.h"

#define min_density 0.7

struct segment
{
	//==========================================================================
	//								VARS
	//==========================================================================

	// list of pixels that this line deals with
	std::vector<pixel> pixels;
	
	// the distance and angle for hough transform. Also acts as ID in hough array 
	int r, t;

	//==========================================================================
	//								INFO
	//==========================================================================

	// if t is the same between 2 segments they are parallel
	// if the lines are parallel, the difference in r is the distance between the 2 lines

	//==========================================================================
	//						COSNTRUCTORS & DESTRUCTORS
	//==========================================================================

	// ===== CONSTRUCTOR =====
	// 1. Basic constructor, r and t equal to -1
	// 2. Constructor with provided r and t values. pixels is still empty
	// 3 & 4. Constructor given another segment. Copies the r & t values regardless and if copy is true, will copy the pixels array
	segment() { r = t = -1; }
	segment(int r, int t) { this->r = r, this->t = t; }
	segment(segment s, bool copy)
	{
		this->r = s.r, this->t = s.t;

		if (copy)
		{
			for (int i = 0; i < s.pixels.size(); i++)
				this->pixels.push_back(s.pixels[i]);
		}
	}
	segment(segment * s, bool copy)
	{
		this->r = s->r, this->t = s->t;

		if (copy)
		{
			for (int i = 0; i < s->pixels.size(); i++)
				this->pixels.push_back(s->pixels[i]);
		}
	}

	//==========================================================================
	//								METHODS
	//==========================================================================

	pixel begin() { return pixels[0]; }
	pixel end() { return pixels[pixels.size() - 1]; }
	pixel edge_begin()
	{
		for (int i = 0; i < pixels.size(); i++)
			if (pixels[i].edge)
				return pixels[i];

		return pixel(-1, -1);
	}

	pixel edge_end()
	{
		for (int i = pixels.size() - 1; i >= 0; i--)
			if (pixels[i].edge)
				return pixels[i];

		return pixel(-1, -1);
	}

	// ===== display =====
	// display the line
	bmpBITMAP_FILE * display(bmpBITMAP_FILE * img)
	{
		bmpBITMAP_FILE * new_img = new bmpBITMAP_FILE(img, false);
		for (int i = 0; i < pixels.size(); i++)
		{
			assert(pixels[i] != pixel());
			assert(pixels[i].y >= 0 && pixels[i].x >= 0);
			/*if (!(pixels[i].y < 768 && pixels[i].x < 1024))
				continue;*/
				// std::cout << "P: " << pixels[i].y << ", " << pixels[i].x << " | I: " << i << " | M: " << pixels.size() << std::endl;
			assert(pixels[i].y < 768 && pixels[i].x < 1024);
			if (pixels[i].edge)
				new_img->image_ptr[pixels[i].y][pixels[i].x] = BLACK;
		}
		return new_img;
	}

	// ===== display =====
	// display the section of line between s and e
	// order of s & e does NOT matter
	bmpBITMAP_FILE * display(bmpBITMAP_FILE * img, int s, int e)
	{
		assert(e < this->pixels.size() && s >= 0);
		if (s > e) std::swap(s, e);
		assert(s < e);

		bmpBITMAP_FILE * new_img = new bmpBITMAP_FILE(img, false);
		for (int i = s; i < e; i++)
		{
			assert(pixels[i].y < 768 && pixels[i].x < 1024);
			if (pixels[i].edge)
				new_img->image_ptr[pixels[i].y][pixels[i].x] = BLACK;
		}
		return new_img;
	}

	// ===== display_edge_segment =====
	// displays the edge section containing p
	bmpBITMAP_FILE * display_edge_segment(bmpBITMAP_FILE * img, pixel p) { return this->get_edge_segment(p).display(img); }

	// ===== find_pixel =====
	// since pixels are always added in sequential order
	// we can use a binary search on the pixels
	// returns the index of the pixel in this->pixels
	int find_pixel(pixel p, bool debug = false)
	{
		// if there are no pixels to search
		if (pixels.size() == 0)
			return -1;

		int low = 0, high = pixels.size() - 1;

		// check the ends of the segment
		if (pixels[low] == p)
			return low;

		if (pixels[high] == p)
			return high;

		while (low < high)
		{
			int mid = ceil(((double)high + (double)low) / 2);
			assert(mid >= 0 && mid < pixels.size());

			if(debug) std::cout << "Low :" << low << " | High: " << high << " | Mid: " << mid << std::endl;

			// check if we found the target
			if (pixels[mid] == p)
			{
				if(debug)std::cout << "Found" << std::endl;
				return mid;
			}

			// move high OR low based on target value 
			else if (pixels[mid] < p && high != mid)
				high = mid;
			else
				low = mid;
		}

		if (debug)std::cout << "NOT Found" << std::endl;

		return -1;
	}

	void fill(int s, int e)
	{
		// Preconditions:
		if (s > e) std::swap(s, e);
		if (e > pixels.size()) e = pixels.size();
		if (s < 0) s = 0;
		assert(s >= 0);
		assert(e <= pixels.size());
		assert(s <= e);
		for (int i = s; i < e; i++)
			pixels[i].edge = true;
	}

	void clear(int s, int e)
	{
		// Preconditions:
		if (s > e) std::swap(s, e);
		if (e > pixels.size()) e = pixels.size();
		if (s < 0) s = 0;
		assert(s >= 0);
		assert(e <= pixels.size());
		assert(s <= e);
		for (int i = s; i < e; i++)
			pixels[i].edge = false;
	}

	// ===== density =====
	// returns how many edge pixels in the segment
	int edge_pixels()
	{
		int edge = 0;
		for (int i = 0; i < pixels.size(); i++)
			if (pixels[i].edge)
				edge++;

		return edge;
	}

	// ===== density =====
	// returns how many edge pixels there are from s to e
	// order of s & e does NOT matter
	int edge_pixels(int s, int e)
	{
		// Preconditions:
		if (s > e) std::swap(s, e);
		if (e > pixels.size()) e = pixels.size();
		if (s < 0) s = 0;
		assert(s >= 0);
		assert(e < pixels.size());
		assert(s <= e);

		int edge = 0;
		for (int i = s; i < e; i++)
			if (pixels[i].edge)
				edge++;

		return edge;
	}

	int next_black_pixel(int p)
	{
		if (p < 0 || p >= pixels.size())
			return -1;
		assert(p >= 0 && p < pixels.size());
		for (int i = p + 1; i < pixels.size(); i++)
			if (pixels[i].edge)
				return i;

		return -1;
	}

	int next_white_pixel(int p)
	{
		if (p < 0 || p >= pixels.size())
			return -1;
		assert(p >= 0 && p < pixels.size());
		for (int i = p + 1; p < pixels.size(); p++)
			if (!pixels[i].edge)
				return i;

		return -1;
	}

	// ===== density =====
	// returns how dense the segment is on a scale from 0 to 1
	float density()
	{
		int edge = this->edge_pixels();
		return edge / this->pixels.size();
	}

	// ===== density =====
	// returns how dense the section from s to e is on a scale from 0 to 1
	// order of s & e does NOT matter
	float density(int s, int e)
	{
		// Preconditions:
		if (s > e) std::swap(s, e);
		if (e > pixels.size()) e = pixels.size();
		if (s < 0) s = 0;
		assert(s >= 0 && e < pixels.size() && s <= e);

		int edge = 0;
		for (int i = s; i < e; i++)
			if (pixels[i].edge)
				edge++;

		return edge / (e - s);
	}

	// ===== delete_edge_segment =====
	// counts the size of the edge section that contains pixel p
	// same as get_edge_segment(p).edge_pixels() OR get_edge_segment(p).pixels.size()
	int get_edge_length(pixel p)
	{
		// if it's i white pixel, there is no edge segment
		if (!p.edge)
			return 0;

		// check in the list
		int index = find_pixel(p);

		// if it's not found, return -1
		if (index == -1)
			return -1;

		// find the start of this smaller edge section
		int start = index;
		while (start > 0 && pixels[start].edge)
			start--;

		// find the end of this smaller edge section
		int end = index;
		while (end < pixels.size() - 1 && pixels[end].edge)
			end++;

		assert(start >= 0 && end < pixels.size());

		// return the difference
		return end - start;
	}

	// ===== get_edge_segment =====
	// returns the edge section that contains the pixel p
	segment get_edge_segment(pixel p)
	{
		// Preconditions:
		if (!(p.x >= 0 && p.y >= 0 && p.x < 1024 && p.y < 768))
			std::cout << "P: " << p.x << ", " << p.y << std::endl;
		assert(p.x >= 0 && p.y >= 0 && p.x < 1024 && p.y < 768);

		// check in the list
		int index = find_pixel(p);

		// if it's not found, return -1
		if (index == -1)
			return segment(this, false);

		assert(index >= 0 && index < pixels.size());

		// if it's a white pixel, there is no edge segment
		if (!pixels[index].edge)
			return segment(this, false);

		// find the start of this smaller edge section
		int start = index;
		while (start > 0 && pixels[start].edge)
			start--;

		// find the end of this smaller edge section
		int end = index;
		while (end < pixels.size() - 1 && pixels[end].edge)
			end++;

		assert(start >= 0 && end < pixels.size());
		segment s(this, false);

		// add the pixels to the new segment
		for (int i = start; i < end; i++)
			s.pixels.push_back(this->pixels[i]);

		// return the found section
		return s;
	}

	// ===== delete_edge_segment =====
	// remove the edge section that contains pixel p
	void delete_edge_segment(pixel p)
	{
		// check in the list
		int index = find_pixel(p);

		// if it's not found, return -1
		if (index == -1)
			return;

		// find the start of this smaller edge section
		int start = index;
		while (start > 0 && pixels[start].edge)
			start--;

		// find the end of this smaller edge section
		int end = index;
		while (end < pixels.size() - 1 && pixels[end].edge)
			end++;

		assert(start >= 0 && end < pixels.size());

		// set all the pixels in the section to false
		for (int i = start + 1; i < end; i++)
			pixels[i].edge = false;
	}

	// ===== add_edge_segment =====
	// makes the pixels between the indices of s and e edge pixels
	// order of s & e does NOT matter
	void add_edge_segment(int s, int e)
	{
		// Preconditions:
		if (s > e) std::swap(s, e);
		if (e > pixels.size()) e = pixels.size();
		if (s < 0) s = 0;
		assert(s >= 0 && e < pixels.size() && s <= e);
		
		// set all the pixels between s & e to be edge pixels
		for (int i = s; i < e; i++)
			pixels[i].edge = true;
	}

	// ===== join_edge_segment =====
	// join the 2 sections that contain p1 and p2 into one section
	// will make all pixels between the 2 sections into edge elements
	void join_edge_segments(pixel p1, pixel p2)
	{
		assert(this->find_pixel(p1) != -1 && this->find_pixel(p2) != -1);
		segment s1 = get_edge_segment(p1);
		segment s2 = get_edge_segment(p2);
		
		// if they are in the same edge segment, do nothing
		if (s1.find_pixel(p2) != -1)
			return;

		// get the edges of both segments
		int s = this->find_pixel(s1.end());
		int e = this->find_pixel(s2.begin());

		// if the second segment is before the first, swap the points
		if (e < s)
			s = find_pixel(s2.end()), e = find_pixel(s1.begin());


		assert(s != -1 && e != -1);
		assert(s >= 0 && s < pixels.size());
		assert(e >= 0 && e < pixels.size());

		// add the edge section between the 2 sections
		this->add_edge_segment(s, e);
	}

	// ===== clear =====
	// remove all pixels from pixels vector
	void clear()
	{
		while (pixels.size() > 0)
			pixels.pop_back();
	}

	// ===== intersect =====
	// find where this segment & segment s intersect
	pixel intersect(segment s)
	{
		// if the lines are parallel, they wont intersect
		if (t == s.t)
			return pixel(-1, -1);


		if (s.pixels.size() > pixels.size())
		{
			for (int i = 0; i < pixels.size(); i++)
			{
				if (s.pixels[i].edge)
					if (s.find_pixel(pixels[i]) != -1)
						return pixels[i];
			}
		}
		else
		{
			// for each pixel s has, search for it in this segment
			for (int i = 0; i < s.pixels.size(); i++)
			{
				if (pixels[i].edge)
					if (this->find_pixel(s.pixels[i]) != -1)
						return s.pixels[i];
			}
		}

		// if they don't intersect, return -1
		return pixel(-1, -1);
	}

	// ===== get_segment_between =====
	// get the section of this line segment between the 2 pixels
	// order of s & e does not matter
	segment get_segment_between(pixel s, pixel e)
	{
		assert(this->find_pixel(s) != -1 && this->find_pixel(e) != -1);

		int start = find_pixel(s);
		int end = find_pixel(e);

		if (start > end)
		{
			int t = start;
			start = end, end = t;
		}

		assert(start >= 0 && end < pixels.size());

		segment f(this, false);

		for (int i = start; i < end; i++)
		{
			f.pixels.push_back(this->pixels[i]);
		}

		return f;
	}

	// ===== best_segment =====
	// returns the best edge section in this segment
	segment best_segment()
	{
		int best_start = 0, best_end = 0, start = -1;

		for (int i = 0; i < pixels.size(); i++)
		{
			// if we are in a section
			if (start >= 0)
			{
				// go until we hit a white pixel
				if (pixels[i].edge)
					continue;

				if (i - start > best_end - best_start)
					best_start = start, best_end = i;

				// remove the start position
				start = -1;
			}

			// if we are searching for a section
			else
			{
				// go until we hit a black pixel
				if (!pixels[i].edge)
					continue;

				start = i;
			}
		}

		return get_edge_segment(pixels[best_start]);
	}

	// ===== best_segment =====
	// returns the best edge section between s & e
	segment best_segment(int s, int e)
	{
		// Preconditions:
		if (s > e) std::swap(s, e);
		if (e > pixels.size()) e = pixels.size();
		if (s < 0) s = 0;
		assert(s >= 0 && e < pixels.size() && s <= e);

		int best_start = s, best_end = s, start = -1;

		// loop through all of pixels are find the best edge section
		for (int i = s; i < e; i++)
		{
			// if we are in a section
			if (start >= 0)
			{
				// go until we hit a white pixel
				if (pixels[i].edge)
					continue;

				// if better, store it in best_start & best_end
				if (i - start > best_end - best_start)
					best_start = start, best_end = i;

				// remove the start position
				start = -1;
			}

			// if we are searching for a section
			else
			{
				// go until we hit a black pixel
				if (!pixels[i].edge)
					continue;

				// set the start of the edge section when we hit a black pixel
				start = i;
			}
		}

		// return the edge segment
		return get_edge_segment(pixels[best_start]);
	}

	// ===== condense =====
	// will attempt to condense the line into one large segment
	// replaced by scan_lines in hough.h
	void condense()
	{
		// if there are no pixels to condense
		if (pixels.size() == 0)
			return;

		int best_start = 0, best_end = 0, start = -1;
		int edge = 0;

		for (int i = 0; i < pixels.size(); i++)
		{
			if (start >= 0)
			{
				if (pixels[i].edge)
				{
					edge++;
					if (i - start > best_end - best_start)
						best_start = start, best_end = i;
				}
				else if ((float)edge / ((float)i - (float)start) > min_density)
				{
					std::cout << "Edge: " << edge << " | Dif: " << i - start << " | Density: " << (float)edge / ((float)i - (float)start) << std::endl;
					if (i - start > best_end - best_start)
						best_start = start, best_end = i;
				}
				else
				{
					i = start + 1;
					start = -1;
				}
			}
			else
			{
				if (!pixels[i].edge)
					continue;

				start = i;
				edge = 1;
			}
		}

		// nothing to condense
		if (best_start == best_end)
			return;

		assert(best_start >= 0 && best_start < pixels.size());
		// assert(pixels[best_start].edge && pixels[best_end].edge);
		assert(best_end >= 0 && best_end < pixels.size());
		assert(find_pixel(pixels[best_end]) != -1 && find_pixel(pixels[best_start]) != -1);

		add_edge_segment(best_start, best_end);
	}

	// ===== shave_whitespace =====
	// remove extra whitespace from end of line
	// depreciated
	void shave_whitespace()
	{
		// remove extra ending white pixels
		for (int i = pixels.size() - 1; i > 0; i--)
		{
			if (!pixels[i].edge)
			{
				pixels.pop_back();
				continue;
			}

			break;
		}

		if (pixels.size() == 0)
			return;

		// remove the front
		int front = 0;
		while (!pixels[front].edge && front < pixels.size()) front++;
		if (front == pixels.size() - 1)
			pixels.clear();
		else
			pixels.erase(pixels.begin(), pixels.begin() + front - 1);
	}

	//==========================================================================
	//								OPERATORS
	//==========================================================================

	// ===== '==' =====
	// returns true if all pixels in one line are in another and vice versa
	bool operator ==(segment s)
	{
		for (int i = 0; i < this->pixels.size(); i++)
			if (s.find_pixel(pixels[i]) == -1)
				return false;

		for (int i = 0; i < s.pixels.size(); i++)
			if (find_pixel(s.pixels[i]) == -1)
				return false;

		return true;
	}

	// ===== assignment operator =====
	// set this segment equal to the rhs
	void operator=(segment s)
	{
		this->clear();

		r = s.r;
		t = s.t;

		for (int i = 0; i < s.pixels.size(); i++)
			this->pixels.push_back(s.pixels[i]);
	}
};

