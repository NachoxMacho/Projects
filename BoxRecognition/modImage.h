#pragma once
#include "image.h"

#define edgeThreshold 50
// #define angleThreshold 30


#define img_height(im) Assemble_Integer(im->infoheader.biHeight)
#define img_width(im) Assemble_Integer(im->infoheader.biWidth)

// Will blur the image by averaging the pixels around a pixel
bmpBITMAP_FILE *  Smooth_Image(bmpBITMAP_FILE * img)
{
	bmpBITMAP_FILE * new_img = new bmpBITMAP_FILE(img, false);
	for (int i = 1; i < img_height(img) - 1; i++)
		for (int j = 1; j < img_width(img) - 1; j++)
			new_img->image_ptr[i][j] =
				(img->image_ptr[i][j] +
				img->image_ptr[i + 1][j] +
				img->image_ptr[i - 1][j] +
				img->image_ptr[i][j + 1] +
				img->image_ptr[i][j - 1] +
				img->image_ptr[i + 1][j + 1] +
				img->image_ptr[i - 1][j - 1] +
				img->image_ptr[i - 1][j + 1] +
				img->image_ptr[i + 1][j - 1]) / 9;
	return new_img;
}

// Applies the sobel kernel to eah pixel and sets border pixels to WHITE
bmpBITMAP_FILE * Sobel_Image(bmpBITMAP_FILE * img)
{
	bmpBITMAP_FILE * sobel = new bmpBITMAP_FILE(img, true);

	for (int i = 1; i < img_height(img) - 1; i++)
	{
		for (int j = 1; j < img_width(img) - 1; j++)
		{
			// gy
			int sumv = (
				img->image_ptr[i - 1][j - 1] * -1 + // ul
				img->image_ptr[i - 1][j] * 0 +      // um
				img->image_ptr[i - 1][j + 1] * 1 +  // ur
				img->image_ptr[i][j - 1] * -2 +     // ml
				img->image_ptr[i][j] * 0 +          // mm
				img->image_ptr[i][j + 1] * 2 +      // mr
				img->image_ptr[i + 1][j - 1] * -1 + // ll
				img->image_ptr[i + 1][j] * 0 +      // lm
				img->image_ptr[i + 1][j + 1] * 1    // lr
				);

			//gx
			int sumh = (
				img->image_ptr[i - 1][j - 1] * -1 + // ul
				img->image_ptr[i - 1][j] * -2 +     // um
				img->image_ptr[i - 1][j + 1] * -1 + // ur
				img->image_ptr[i][j - 1] * 0 +      // ml
				img->image_ptr[i][j] * 0 +          // mm
				img->image_ptr[i][j + 1] * 0 +      // mr
				img->image_ptr[i + 1][j - 1] * 1 +  // ll
				img->image_ptr[i + 1][j] * 2 +      // lm
				img->image_ptr[i + 1][j + 1] * 1    // lr
				);

			sobel->image_ptr[i][j] = sqrt(sumh * sumh + sumv * sumv);
		}
	}
	return sobel;
}

// Produce a black and white image based on the sobel output, which should output an edge image
bmpBITMAP_FILE * Edge_Image(bmpBITMAP_FILE * img)
{
	bmpBITMAP_FILE * edge = Sobel_Image(img);

	for (int i = 1; i < img_height(edge) - 1; i++)
		for (int j = 1; j < img_width(edge) - 1; j++)
			edge->image_ptr[i][j] = (edge->image_ptr[i][j] > edgeThreshold)? BLACK: WHITE;

	return edge;
}

bmpBITMAP_FILE * Remove_Border(bmpBITMAP_FILE * img)
{
	bmpBITMAP_FILE * new_img = new bmpBITMAP_FILE(img, true);

	for (int i = 1; i < img_height(new_img) - 1; i++)
		for (int j = 1; j < img_width(new_img) - 1; j++)
			if (i < 4 || i > img_height(new_img) - 4 || j < 4 || j > img_width(new_img) - 4)
				new_img->image_ptr[i][j] = WHITE;

	return new_img;

}

// !remove
//double get_pixel_angle(bmpBITMAP_FILE * img, int i, int j)
//{
//	// within the picture limits and not an edge pixel
//	assert(i < Assemble_Integer(img->infoheader.biHeight) - 1 && i > 0 && j < Assemble_Integer(img->infoheader.biWidth) - 1 && j > 0);
//
//	// gy
//	int sumv = (
//		img->image_ptr[i - 1][j - 1] * -1 + // ul
//		img->image_ptr[i - 1][j] * 0 +      // um
//		img->image_ptr[i - 1][j + 1] * 1 +  // ur
//		img->image_ptr[i][j - 1] * -2 +     // ml
//		img->image_ptr[i][j] * 0 +          // mm
//		img->image_ptr[i][j + 1] * 2 +      // mr
//		img->image_ptr[i + 1][j - 1] * -1 + // ll
//		img->image_ptr[i + 1][j] * 0 +      // lm
//		img->image_ptr[i + 1][j + 1] * 1    // lr
//		);
//
//	// gx
//	int sumh = (
//		img->image_ptr[i - 1][j - 1] * -1 + // ul
//		img->image_ptr[i - 1][j] * -2 +     // um
//		img->image_ptr[i - 1][j + 1] * -1 + // ur
//		img->image_ptr[i][j - 1] * 0 +      // ml
//		img->image_ptr[i][j] * 0 +          // mm
//		img->image_ptr[i][j + 1] * 0 +      // mr
//		img->image_ptr[i + 1][j - 1] * 1 +  // ll
//		img->image_ptr[i + 1][j] * 2 +      // lm
//		img->image_ptr[i + 1][j + 1] * 1    // lr
//		);
//
//	return (180 * atan2(sumv, sumh)) / 3.14;
//}

// !remove
//bmpBITMAP_FILE * Angle_Image(bmpBITMAP_FILE * img)
//{
//	bmpBITMAP_FILE * angle = new bmpBITMAP_FILE(img, true);
//
//	for (int i = 1; i < Assemble_Integer(img->infoheader.biHeight) - 1; i++)
//		for (int j = 1; j < Assemble_Integer(img->infoheader.biWidth) - 1; j++)
//			angle->image_ptr[i][j] = (get_pixel_angle(img, i, j) * 255 / 360);
//
//	return angle;
//}

/*
 * (-1, -1) (-1,  0) (-1, +1)
 * ( 0, -1) ( 0,  0) ( 0, +1)
 * (+1, -1) (+1,  0) (+1, +1)
 */

bool lower(bmpBITMAP_FILE  * img, int i, int j) { return (img->image_ptr[i][j] == BLACK && img->image_ptr[i + 1][j] == WHITE); }
bool upper(bmpBITMAP_FILE  * img, int i, int j) { return (img->image_ptr[i][j] == BLACK && img->image_ptr[i - 1][j] == WHITE); }
bool left (bmpBITMAP_FILE  * img, int i, int j) { return (img->image_ptr[i][j] == BLACK && img->image_ptr[i][j - 1] == WHITE); }
bool right(bmpBITMAP_FILE  * img, int i, int j) { return (img->image_ptr[i][j] == BLACK && img->image_ptr[i][j + 1] == WHITE); }

// 111
// 010
// 111
bool a1(bmpBITMAP_FILE  * img, int i, int j) 
{ 
	return (
	(img->image_ptr[i - 1][j - 1] == BLACK   || img->image_ptr[i][j - 1] == BLACK || img->image_ptr[i + 1][j - 1] == BLACK  ) &&
	(img->image_ptr[i - 1][j]     == WHITE && img->image_ptr[i][j]     == BLACK && img->image_ptr[i + 1][j]     == WHITE) &&
	(img->image_ptr[i - 1][j + 1] == BLACK   || img->image_ptr[i][j + 1] == BLACK || img->image_ptr[i + 1][j + 1] == BLACK  )); 
}
// 101
// 111
// 101
bool a2(bmpBITMAP_FILE  * img, int i, int j)
{
	return (
		(img->image_ptr[i - 1][j - 1] == BLACK || img->image_ptr[i - 1][j] == BLACK || img->image_ptr[i - 1][j + 1] == BLACK) &&
		(img->image_ptr[i][j - 1]   == WHITE && img->image_ptr[i][j]     == BLACK && img->image_ptr[i][j + 1]     == WHITE) &&
		(img->image_ptr[i + 1][j - 1] == BLACK || img->image_ptr[i + 1][j] == BLACK || img->image_ptr[i + 1][j + 1] == BLACK));
}
// 011 
// 111
// 110
bool a3(bmpBITMAP_FILE  * img, int i, int j)
{
	return (
		(img->image_ptr[i + 1][j] == BLACK || img->image_ptr[i][j - 1] == BLACK || img->image_ptr[i + 1][j - 1] == BLACK) &&
		(img->image_ptr[i - 1][j - 1] == WHITE && img->image_ptr[i][j] == BLACK && img->image_ptr[i + 1][j + 1] == WHITE) &&
		(img->image_ptr[i - 1][j] == BLACK || img->image_ptr[i][j + 1] == BLACK || img->image_ptr[i - 1][j + 1] == BLACK));
}
// 110
// 111
// 011
bool a4(bmpBITMAP_FILE  * img, int i, int j)
{
	return (
		(img->image_ptr[i - 1][j]       == BLACK || img->image_ptr[i][j - 1] == BLACK || img->image_ptr[i - 1][j - 1] == BLACK) &&
		(img->image_ptr[i - 1][j + 1] == WHITE && img->image_ptr[i][j] == BLACK     && img->image_ptr[i + 1][j - 1] == WHITE) &&
		(img->image_ptr[i + 1][j]       == BLACK || img->image_ptr[i][j + 1] == BLACK || img->image_ptr[i + 1][j + 1] == BLACK));
}

bool a(bmpBITMAP_FILE  * img, int i, int j) { return (a1(img, i, j) || a2(img, i, j) || a3(img, i, j) || a4(img, i, j)); }

// xxx
//  10
// 01
bool b1(bmpBITMAP_FILE  * img, int i, int j)
{
	return (
		(img->image_ptr[i - 1][j - 1] == BLACK   || img->image_ptr[i - 1][j] == BLACK || img->image_ptr[i - 1][j + 1] == BLACK) && // one edege pixel above
		(img->image_ptr[i][j + 1] == WHITE && img->image_ptr[i][j] == BLACK && img->image_ptr[i + 1][j] == BLACK && img->image_ptr[i + 1][j - 1] == WHITE)); // with
}
// x 0
// x11
// x0 
bool b2(bmpBITMAP_FILE  * img, int i, int j)
{
	return (
		(img->image_ptr[i - 1][j - 1] == BLACK || img->image_ptr[i][j - 1] == BLACK || img->image_ptr[i + 1][j - 1] == BLACK) && // one edge pixel to the left
		(img->image_ptr[i + 1][j] == WHITE && img->image_ptr[i][j] == BLACK && img->image_ptr[i][j + 1] == BLACK && img->image_ptr[i - 1][j + 1] == WHITE )); // with
}
//  10
// 01 
// xxx
bool b3(bmpBITMAP_FILE  * img, int i, int j)
{
	return (
		(img->image_ptr[i + 1][j - 1] == BLACK || img->image_ptr[i + 1][j] == BLACK || img->image_ptr[i + 1][j + 1] == BLACK) && // one edge pixel below
		(img->image_ptr[i][j - 1] == WHITE && img->image_ptr[i][j] == BLACK && img->image_ptr[i - 1][j] == BLACK && img->image_ptr[i - 1][j + 1] == WHITE)); // with
}
//  0x
// 11x
// 0 x
bool b4(bmpBITMAP_FILE  * img, int i, int j)
{
	return (
		(img->image_ptr[i - 1][j + 1] == BLACK || img->image_ptr[i][j + 1] == BLACK || img->image_ptr[i + 1][j + 1] == BLACK) && // one edge pixel to the right
		(img->image_ptr[i - 1][j] == WHITE && img->image_ptr[i][j] == BLACK && img->image_ptr[i][j - 1] == BLACK && img->image_ptr[i + 1][j - 1] == WHITE)); // with
}

bool b(bmpBITMAP_FILE  * img, int i, int j) { return (b1(img, i, j) || b2(img, i, j) || b3(img, i, j) || b4(img, i, j)); }

bool c1(bmpBITMAP_FILE  * img, int i, int j)
{
	return (
		img->image_ptr[i][j] == BLACK && img->image_ptr[i + 1][j - 1] == BLACK &&
		img->image_ptr[i + 1][j] == WHITE && img->image_ptr[i][j - 1] == WHITE );
}
bool c2(bmpBITMAP_FILE  * img, int i, int j)
{
	return (
		img->image_ptr[i][j] == BLACK && img->image_ptr[i + 1][j + 1] == BLACK &&
		img->image_ptr[i + 1][j] == WHITE && img->image_ptr[i][j + 1] == WHITE);
}
bool c3(bmpBITMAP_FILE  * img, int i, int j)
{
	return (
		img->image_ptr[i][j] == BLACK && img->image_ptr[i - 1][j + 1] == BLACK &&
		img->image_ptr[i - 1][j] == WHITE && img->image_ptr[i][j + 1] == WHITE);
}
bool c4(bmpBITMAP_FILE  * img, int i, int j)
{
	return (
		img->image_ptr[i][j] == BLACK && img->image_ptr[i - 1][j - 1] == BLACK &&
		img->image_ptr[i - 1][j] == WHITE && img->image_ptr[i][j - 1] == WHITE);
}

bmpBITMAP_FILE * Thin_Edge(bmpBITMAP_FILE * img)
{
	bmpBITMAP_FILE * final = new bmpBITMAP_FILE(img, false), * c = new bmpBITMAP_FILE(img, false);

	int cycle = 0;

	while (true)
	{
		// load the final points image
		for (int i = 1; i < img_height(img) - 1; i++)
		{
			for (int j = 1; j < img_width(img) - 1; j++)
			{
				if (a(img, i, j))
					final->image_ptr[i][j] = BLACK;
				if ((cycle == 0 && (b1(img, i, j) || b2(img, i, j))) ||
					(cycle == 1 && (b3(img, i, j) || b4(img, i, j))) ||
					(cycle == 2 && (b1(img, i, j) || b4(img, i, j))) ||
					(cycle == 3 && (b2(img, i, j) || b3(img, i, j)))  )
					final->image_ptr[i][j] = BLACK;

			}
		}
		
		// test if the images are identical
		if (Identical_Image(img, final))
			break;

		assert(!Identical_Image(img, final));

		// load contour point image
		for (int i = 1; i < img_height(img) - 1; i++)
		{
			for (int j = 1; j < img_width(img) - 1; j++)
			{
				c->image_ptr[i][j] = WHITE;

				if ((cycle == 0 && lower(img, i, j)) ||
					(cycle == 1 && upper(img, i, j)) ||
					(cycle == 2 && left (img, i, j)) ||
					(cycle == 3 && right(img, i, j))  )
					c->image_ptr[i][j] = BLACK;
			}
		}

		// remove contour points and keep final points
		for (int i = 0; i < img_height(img); i++)
			for (int j = 0; j < img_width(img); j++)
				if (final->image_ptr[i][j] == BLACK)
					img->image_ptr[i][j] = BLACK;
				else if (c->image_ptr[i][j] == BLACK)
					img->image_ptr[i][j] = WHITE;

		// increase cycle
		cycle = (cycle % 4) + 1;
	}

	delete c;

	return final;
}
