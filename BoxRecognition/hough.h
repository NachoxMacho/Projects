#pragma once
#include <cmath>
#include <vector>
#include "modimage.h"
#include "segment.h"
#include "plane.h"

// this is how many continous pixels need to be in an edge for a line to be considered a full line
#define min_edge_size 9

// set to true to get cout statments for troubleshooting
#define DEBUG false 

// used to convert between radians and degree for computing hough transform
#define DEG2RAD (3.14 / 180)

// how far apart lines need to be to detect vertices
#define min_angle 20

// get the distance from 0, 0  for the line that goes through x, y at angle t
#define h_dist(x, y, t) round(abs(((double)x * cos(t * DEG2RAD)) + ((double)y * sin(t * DEG2RAD))))

// Plan of Attack:
// 1. Store all pixels regardless of color in line segments instead of you’ve transform | DONE
// 2. Figure out pixel density of lines ? | CAN DO
// 3. Figure out where lines intersect and check for edge pixels | DONE
// 4. Form a potential plane as a group of 4 vertices | WORKING ON
// 5. Score plane by edge pixel density in edges
// 6. Check for edges shared by planes
// 7. If 3 planes share 3 edges that is probably a box ?

struct Hough
{
	// Variables
	segment ** acc = NULL; // the hough transform array
	int houghH, houghW; // dimensions of the array acc
	bmpBITMAP_FILE * orig_image = NULL;

	plane planes[50];

	// ===== CONSTRUCTOR =====
	// Given an image produce a thinned edge version of the image
	// Then create the transform for the image
	Hough(bmpBITMAP_FILE  * img)
	{
		/*
		orig_image = Edge_Image(img);
		/*/
		orig_image = Remove_Border(Thin_Edge(Edge_Image(img)));
		//*/
		houghH = houghW = 0;
		transform();
	}

	// ===== DESTRUCTOR =====
	// Sole purpose is to delete the array acc & orig_image
	~Hough()
	{
		if (orig_image != NULL)
			delete orig_image;

		if (acc == NULL)
			return;

		assert(acc != NULL);

		for (int i = 0; i < houghH; i++)
			delete[] acc[i];
		delete[] acc;
	}

	// ===== Transform =====
	// 1. recreates the acc array based on the size of the image
	// 2. scans the images and for each edge pixel, 
		// 2.1. for every degree, find the matching r2 value and store the pixel in that segment
	void transform()
	{
		// Preconditions:
		assert(acc == NULL && orig_image != NULL);

		// get diagonal dimension of picture
		houghH = (sqrt(img_height(orig_image) * img_height(orig_image) +
			img_width(orig_image) * img_width(orig_image)));

		// this is how precise the degrees should be
		houghW = 180;

		// create the array
		acc = new segment*[houghH];
		for (int i = 0; i < houghH; i++)
			acc[i] = new segment[houghW];

		// clear the intial array
		for (int i = 0; i < houghH; i++)
			for (int j = 0; j < houghW; j++)
				acc[i][j].r = i, acc[i][j].t = j;

		if (DEBUG) std::cout << "Made Array of size: " << houghH << " x " << houghW << std::endl;

		// for each pixel
		for (int y = 1; y < img_height(orig_image) - 1; y++)
			for (int x = 1; x < img_width(orig_image) - 1; x++)
				for (int t = 0; t < houghW; t++)
					// add the pixel to the respective segment
					acc[(int)h_dist(x, y, t)][t].pixels.push_back(pixel(x, y, orig_image->image_ptr[y][x] == BLACK));

	}

	// ===== scan_lines =====
	// this will condense lines down to one segment and remove segments that don't have enough edge pixels
	bmpBITMAP_FILE * scan_lines()
	{
		// Preconditions:
		assert(orig_image != NULL && acc != NULL);
		assert(houghH > 0 && houghW > 0);

		// Init: 
		// new_img    = Blank image

		bmpBITMAP_FILE * new_img = new bmpBITMAP_FILE(orig_image, false);
		int lines = 0;
		for (int r = 0; r < houghH; r++)
		{
			for (int t = 0; t < houghW; t++)
			{
				if (acc[r][t].pixels.size() < min_edge_size)
					continue;

				if (acc[r][t].edge_pixels() < min_edge_size)
				{
					acc[r][t].pixels.clear();
					continue;
				}

				int start = -1, end = -1;

				for (int i = 0; i < acc[r][t].pixels.size() - 1; i++)
				{
					if (!acc[r][t].pixels[i].edge)
						continue;
					if (acc[r][t].get_edge_length(acc[r][t].pixels[i]) < min_edge_size)
					{
						i += acc[r][t].get_edge_length(acc[r][t].pixels[i]);
						continue;
					}

					start = i;
					break;
				}

				for (int i = acc[r][t].pixels.size() - 1; i >= 0; i--)
				{
					if (!acc[r][t].pixels[i].edge)
						continue;
					if (acc[r][t].get_edge_length(acc[r][t].pixels[i]) < min_edge_size)
					{
						i -= acc[r][t].get_edge_length(acc[r][t].pixels[i]);
						continue;
					}

					end = i;
					break;
				}

				if (start == end)
				{
					acc[r][t].pixels.clear();
					continue;
				}

				if (acc[r][t].edge_pixels(start, end) < min_edge_size)
				{
					acc[r][t].pixels.clear();
					continue;
				}

				assert(start <= end);
				assert(start >= 0);
				assert(end < acc[r][t].pixels.size());

				acc[r][t].fill(start, end);
				acc[r][t].clear(0, start - 1);
				acc[r][t].clear(end + 1, acc[r][t].pixels.size());

				bmpBITMAP_FILE * im = acc[r][t].display(new_img, start, end);
				new_img->operator+=(im);
				delete im;

				// acc[r][t].pixels.erase(acc[r][t].pixels.begin(), acc[r][t].pixels.begin() + start - 1);
				// acc[r][t].pixels.erase(acc[r][t].pixels.begin() + end, acc[r][t].pixels.end());
				lines++;

				// std::cout << "Changing Line " << r << "-" << t << " | New Size: " << acc[r][t].pixels.size() << std::endl;
			}

		}

		if (DEBUG) std::cout << "Kept " << lines << " lines" << std::endl;
		return new_img;
	}

	// ===== scan_intersections =====
	// will scan the number of intersections
	void scan_intersections()
	{

		int lines = 0;
		for (int r = 0; r < houghH; r++)
		{
			for (int t = 0; t < houghW; t++)
			{
				if (acc[r][t].pixels.size() < min_edge_size)
					continue;

				// std::cout << "Scanning Line " << r << "-" << t << std::endl;

				int intersections = 0;

				for (int r1 = 0; r1 < houghH; r1++)
				{
					for (int t1 = 0; t1 < houghW; t1++)
					{
						// std::cout << "Testing Line " << r1 << "-" << t1 << std::endl;
						if (acc[r1][t1].pixels.size() < min_edge_size || abs(t1 - t) < 30 || abs(t1 - t) > 180 - 30 )
							continue;

						// std::cout << "Testing Line " << r1 << "-" << t1 << std::endl;
						pixel intersect = acc[r][t].intersect(acc[r1][t1]);

						if (intersect != pixel(-1, -1) && intersect.edge && acc[r][t].get_edge_length(intersect) > 1)
						{
							// std::cout << "Found Intersection" << std::endl;
							intersections++;
						}
						// std::cout << "Done Testing" << std::endl;
					}
					
				}

				if (intersections < 2)
					acc[r][t].pixels.clear();
				else
					lines++;
			}
		}

		if (DEBUG) std::cout << "Kept " << lines << " lines" << std::endl;
	}

	// ===== find_vertices =====
	// identify all the potential vertices in the image
	bmpBITMAP_FILE * find_vertices()
	{
		// Preconditions:
		assert(orig_image != NULL && acc != NULL);
		assert(houghH > 0 && houghW > 0);

		// Init: 
		// new_img    = Blank image
		// line_count = counter for debug purposes
		bmpBITMAP_FILE * new_img = new bmpBITMAP_FILE(orig_image, false);
		int line_count = 0;


		// for each pixel in the image
		for (int y = 1; y < img_height(orig_image) - 1; y++)
		{
			for (int x = 1; x < img_width(orig_image) - 1; x++)
			{
				// do nothing if the pixel is white
				if (orig_image->image_ptr[y][x] == WHITE)
					continue;

				// for all black pixels (edge pixels)
				assert(orig_image->image_ptr[y][x] == BLACK);

				// make a new list of degrees of lines that run through this pixel
				int lines = 0;
				int lineAngles[20];
				for (int i = 0; i < 20; i++)
					lineAngles[i] = -180;

				// for each angle between 0 and 180
				for (int t = 0; t < houghW; t++)
				{
					// get the distance (identify which line at that angle)
					double r = h_dist(x, y, t);
					// r = round(abs(r));
					assert(r < houghH);


					// get the line segment
					segment s = acc[(int)r][t].get_edge_segment(pixel(x, y, true));

					// if it is a significant line segment
					if (s.edge_pixels() > 5)
					{
						// check if other significant line segments are too close to this line
						bool similar = false;
						for (int i = 0; i < lines; i++)
							if (abs(lineAngles[i] - t) < min_angle || abs(lineAngles[i] - t) > 180 - min_angle)
								similar = true;

						// if no similar lines are found, add it to the list
						if (!similar)
							lineAngles[lines++] = t;
					}
				}

				// if there is more than one line going through this pixel, that are more than 20 degrees apart, mark the pixel
				if (lines > 1)
					new_img->image_ptr[y][x] = BLACK;
			}
		}

		return new_img;
	}

	// ===== display_lines =====
	// given an image, will display all lines found by transform and/or scan_lines
	inline bmpBITMAP_FILE * display_lines_at_angle(int t)
	{
		// Preconditions:
		assert(acc != NULL && orig_image != NULL);

		bmpBITMAP_FILE * lines = new bmpBITMAP_FILE(orig_image, false);
		// std::ofstream line_file("lines.txt");
		int count = 0;


		for (int r = 0; r < houghH; r += 10)
		{
			lines->operator+=(acc[r][t].display(lines));
		}

		std::cout << "Generated image with " << count << " lines" << std::endl;
		return lines;
	}

	// ===== display_lines =====
	// given an image, will display all lines found by transform and/or scan_lines
	inline bmpBITMAP_FILE * display_lines()
	{
		// Preconditions:
		assert(acc != NULL && orig_image != NULL);

		bmpBITMAP_FILE * lines = new bmpBITMAP_FILE(orig_image, false);
		int count = 0;

		for (int r = 0; r < houghH; r++)
		{
			for (int t = 0; t < houghW; t++)
			{
				if (acc[r][t].pixels.size() < min_edge_size)
					continue;

				bmpBITMAP_FILE * im = acc[r][t].display(lines);
				lines->operator+=(im);
				delete im;
			}
		}

		return lines;
	}

	// ===== between =====
	// will return the line segment from acc that runs through both pixels if possible
	// pixel order does not matter
	segment between(pixel p1, pixel p2)
	{
		// for eac potential angle
		for (int t = 0; t < houghW; t++)
		{
			// calculate the r values
			double r1 = h_dist(p1.x, p1.y, t);
			double r2 = h_dist(p2.x, p2.y, t);
			assert(r1 < houghH && r2 < houghH);

			// if the r values match we found a line
			if (r1 == r2)
				return acc[(int)r1][t].get_segment_between(p1, p2); // return the part between the two pixels

		}

		// else no line was found
		return segment();
	}

};