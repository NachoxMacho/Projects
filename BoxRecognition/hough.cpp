#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <cassert>
#include "pixel.h"

typedef unsigned char  byte_t;

#define BLACK 0
#define WHITE 255

//================ Assemble_Integer ========================
int Assemble_Integer(unsigned char bytes[])
{
	int an_integer;

	an_integer =
		int(bytes[0])
		+ int(bytes[1]) * 256
		+ int(bytes[2]) * 256 * 256
		+ int(bytes[3]) * 256 * 256 * 256;
	return an_integer;
}

struct bmpFILEHEADER
{
	byte_t 	bfType[2];  //Bitmap identifier,  Must be "BM"
	byte_t	bfSize[4];
	byte_t       bfReserved[4];
	byte_t       bfOffbits[4];  //specifies the location
								//(in bytes) in the file of the
};

struct bmpINFOHEADER
{
	byte_t	biSize[4];    // Size of the bmpInfoHeader,
							  // i.e. sizeof(bmpInfoheader)
	byte_t	biWidth[4];   // Width of bitmap, in pixels
							  // change this if you change
			// the size of the image-> see ***** note below

	byte_t  	biHeight[4];  // Height of bitmap, in pixels
							  //change this if you change the
			// size of the image-> see ***** note below
	byte_t 	biPlanes[2];  // Should/must be 1.
	byte_t	biBitCount[2]; // The bit depth of the bitmap.
							// For 8 bit bitmaps, this is 8
	byte_t  	biCompression[4];   // Should be 0 for
									// uncompressed bitmaps
	byte_t       biSizeImage[4];    //The size of the padded
									// image, in bytes
	byte_t       biXPelsPerMeter[4]; //Horizontal resolution,
						// in pixels per meter.  Not signif.
	byte_t       biYPelsPermeter[4];  //Vertical resolution,
									  //as above.
	byte_t       biClrUsed[4];   //Indicates the number of
								 //colors in the palette.
	byte_t       biClrImportant[4]; //Indicates number of
			//colors to display the bitmap.
		// Set to zero to indicate all colors should be used.
};

struct bmpPALETTE
{
	byte_t	palPalette[1024]; // this will need to be
				   //improved if the program is to scale.
					// unless we change the palette,
				   // this will do.
};

struct bmpBITMAP_FILE	// note:  this structure may not be
						// written to file all at once.
						// the two headers may be written
						// normally, but the image requires
						// a write for each line followed
						//  by a possible 1 - 3 padding bytes.
{
	bmpFILEHEADER	fileheader;
	bmpINFOHEADER	infoheader;
	bmpPALETTE		palette;     //this implementation
		// will not generalize.  Fixed at 256 shades of grey.
	byte_t		**image_ptr; //this points to the 
		// image-> Allows the allocation of a two dimensional
		// array dynamically

	// ===== DESTRUCTOR =====
	~bmpBITMAP_FILE()
	{
		// delete the image_ptr
		for (int i = 0; i < Assemble_Integer(infoheader.biHeight); i++)
			delete[] this->image_ptr[i];
		delete[] this->image_ptr;

		fileheader.bfType[0] = 'X';  // just to mark it as
		fileheader.bfType[1] = 'X';  // unused.
	}

	// ===== CONSTRUCTOR =====
	// create this bmp with the same dimensions as img
	// if copy is true, also copy the image, else just blank the image
	bmpBITMAP_FILE(bmpBITMAP_FILE * img, bool copy)
	{
		fileheader = img->fileheader;
		infoheader = img->infoheader;
		palette = img->palette;

		image_ptr = new byte_t*[Assemble_Integer(img->infoheader.biHeight)];

		for (int i = 0; i < Assemble_Integer(img->infoheader.biHeight); i++)
			image_ptr[i] = new byte_t[Assemble_Integer(img->infoheader.biWidth)];

		for (int i = 0; i < Assemble_Integer(img->infoheader.biHeight); i++)
			for (int j = 0; j < Assemble_Integer(img->infoheader.biWidth); j++)
				if (copy)
					image_ptr[i][j] = img->image_ptr[i][j];
				else
					image_ptr[i][j] = 255;
	}

	// ===== CONSTRUCTOR =====
	// Basic constructor, does nothing 
	bmpBITMAP_FILE() {}

	// ===== copy =====
	// copies the image from img (pointer)
	// same as = new bmpBITMAP_FILE(img, true);
	void copy(bmpBITMAP_FILE *img)
	{
		int height = Assemble_Integer(img->infoheader.biHeight);
		int width = Assemble_Integer(img->infoheader.biWidth);
		fileheader = img->fileheader;
		infoheader = img->infoheader;
		palette = img->palette;

		image_ptr = new byte_t*[height];

		for (int i = 0; i < height; i++)
			image_ptr[i] = new byte_t[width];

		//load the bytes into the new array one byte at a time
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++)
				image_ptr[i][j] = img->image_ptr[i][j];
	}


	// ===== copy =====
	// copies the image from img (object)
	// same as = new bmpBITMAP_FILE(&img, true);
	void copy(bmpBITMAP_FILE &img)
	{
		int height = Assemble_Integer(img.infoheader.biHeight);
		int width = Assemble_Integer(img.infoheader.biWidth);
		fileheader = img.fileheader;
		infoheader = img.infoheader;
		palette = img.palette;

		image_ptr = new byte_t*[height];

		for (int i = 0; i < height; i++)
			image_ptr[i] = new byte_t[width];

		//load the bytes into the new array one byte at a time
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++)
				image_ptr[i][j] = img.image_ptr[i][j];
	}

	// ===== clear =====
	// resets the image to a blank white image
	void clear()
	{
		for (int i = 0; i < Assemble_Integer(this->infoheader.biHeight); i++)
			for (int j = 0; j < Assemble_Integer(this->infoheader.biWidth); j++)
				this->image_ptr[i][j] = 255;
	}

	// ===== '+=' =====
	// will set a pixel to be black if either this OR img's corresponding pixel is black
	void operator +=(bmpBITMAP_FILE &img)
	{
		for (int i = 0; i < Assemble_Integer(infoheader.biHeight); i++)
			for (int j = 0; j < Assemble_Integer(infoheader.biWidth); j++)
				if (image_ptr[i][j] != 0 && img.image_ptr[i][j] == 0)
					image_ptr[i][j] = 0;
	}

	// ===== '+=' =====
	// will set a pixel to be black if either this OR img's corresponding pixel is black
	void operator +=(bmpBITMAP_FILE * img)
	{
		for (int i = 0; i < Assemble_Integer(infoheader.biHeight); i++)
			for (int j = 0; j < Assemble_Integer(infoheader.biWidth); j++)
				if (img->image_ptr[i][j] == 0)
					image_ptr[i][j] = 0;
	}

	int operator[](pixel p) { return image_ptr[p.x][p.y]; }
};

//================== Open_Output_File ===================
void Open_Output_File(std::ofstream &out_file, char out_file_name[])
{
	if (out_file_name[0] == '\0')
	{
		// char out_file_name[80];

		std::cout << "Save file as: ";
		std::cin >> out_file_name;
	}

	out_file.open(out_file_name, std::ios::out | std::ios::binary);
	if (!out_file)
	{
		std::cout << "\nCannot open " << out_file_name << std::endl;
		std::exit(101);
	}

	return;
}

//============== open_input_file ===========================
void open_input_file(std::ifstream &in_file)
{
	char in_file_name[80];

	std::cout << "---------------------------------------------------------------" << std::endl;
	std::cout << "Enter the name of the file which contains the bitmap: ";
	std::cin >> in_file_name;
	std::cout << "---------------------------------------------------------------" << std::endl;

	//cout << "You entered: " << in_file_name << endl;

	in_file.open(in_file_name, std::ios::in | std::ios::binary);
	if (!in_file)
		std::cerr << "Error opening file\a\a\n", std::exit(101);

	return;
}

//==================== Calc_Padding ========================
int Calc_Padding(int pixel_width)
{
	// Each scan line must end on a 4 byte boundry. Therefore, if the pixel_width is not evenly divisible by 4, extra bytes are added (either 1 - 3 extra bytes)

	int remainder;
	int padding;

	remainder = pixel_width % 4;

	switch (remainder)
	{
	case 0:	padding = 0;
		break;
	case 1: padding = 3;
		break;
	case 2: padding = 2;
		break;
	case 3: padding = 1;
		break;
	default: std::cerr << "Error:  Padding was set to "
		<< padding << "\n";
		exit(101);
	}

	//cout << "Padding determined: " << padding << "\n";

	return padding;
}

//================== load_Bitmap_File ======================
void Load_Bitmap_File(bmpBITMAP_FILE * image)
{
	std::ifstream fs_data;

	int bitmap_width;
	int bitmap_height;

	int padding;
	long int cursor1; // used to navigate through the bitfiles

	open_input_file(fs_data);


	fs_data.read((char *)&image->fileheader, sizeof(bmpFILEHEADER));
	fs_data.read((char *)&image->infoheader, sizeof(bmpINFOHEADER));
	fs_data.read((char *)&image->palette, sizeof(bmpPALETTE));	// this will need to be dynamic, once the size of the palette can vary

	bitmap_height = Assemble_Integer(image->infoheader.biHeight);
	bitmap_width = Assemble_Integer(image->infoheader.biWidth);
	padding = Calc_Padding(bitmap_width);

	// allocate a 2 dim array
	image->image_ptr = new byte_t*[bitmap_height];
	for (int i = 0; i < bitmap_height; i++)
		image->image_ptr[i] = new byte_t[bitmap_width];

	cursor1 = Assemble_Integer(image->fileheader.bfOffbits);
	fs_data.seekg(cursor1);  //move the cursor to the
							// beginning of the image data

	//load the bytes into the new array one line at a time
	for (int i = 0; i < bitmap_height; i++)
	{
		fs_data.read((char *)image->image_ptr[i],
			bitmap_width);
		// insert code here to read the padding,
		// if there is any
	}

	fs_data.close();  //close the file
	// (consider replacing with a function w/error checking)
}

//============== Display_Bitmap_File =======================
void Display_Bitmap_File(bmpBITMAP_FILE * image)
{
	int bitmap_width;
	int bitmap_height;

	//display the palatte here too, perhaps.

	bitmap_height = Assemble_Integer(image->infoheader.biHeight);
	bitmap_width = Assemble_Integer(image->infoheader.biWidth);

	for (int i = 0; i < 1; i++)
	{
		for (int j = 0; j < bitmap_width; j++)
			std::cout << std::setw(4) << int(image->image_ptr[i][j]);
		std::cout << "\n\nNew Line\n\n";
	}
}

//================== Save_Bitmap_File ======================
void Save_Bitmap_File(bmpBITMAP_FILE * image, char out_file_name[])
{
	std::ofstream fs_data;

	// int width;
	// int height;
	int padding;
	long int cursor1;	// used to navigate through the
						// bitfiles

	Open_Output_File(fs_data, out_file_name);

	fs_data.write((char *)&image->fileheader,
		sizeof(bmpFILEHEADER));
	if (!fs_data.good())
	{
		std::cout << "\aError 101 writing bitmapfileheader";
		std::cout << " to file.\n";
		exit(101);
	}

	fs_data.write((char *)&image->infoheader,
		sizeof(bmpINFOHEADER));
	if (!fs_data.good())
	{
		std::cout << "\aError 102 writing bitmap";
		std::cout << " infoheader to file.\n";
		exit(102);
	}

	fs_data.write((char *)&image->palette,
		sizeof(bmpPALETTE));
	if (!fs_data.good())
	{
		std::cout << "\aError 103 writing bitmap palette to";
		std::cout << "file.\n";
		exit(103);
	}
	//this loop writes the image data
	for (int i = 0; i < Assemble_Integer(image->infoheader.biHeight); i++)
	{
		for (int j = 0; j < Assemble_Integer(image->infoheader.biWidth); j++)
		{
			fs_data.write((char *)&image->image_ptr[i][j],
				sizeof(byte_t));
			if (!fs_data.good())
			{
				std::cout << "\aError 104 writing bitmap data";
				std::cout << "to file.\n";
				exit(104);
			}
		}
	}

	fs_data.close();
}

// ===== Identical_Image =====
// will return true if every pixel in img1 matches the corresponding pixel in img2
bool Identical_Image(bmpBITMAP_FILE  * img1, bmpBITMAP_FILE  * img2)
{
	// Preconditions:
	assert(img1 != NULL && img2 != NULL);

	// if images are different sizes, they aren't identical
	if (Assemble_Integer(img1->infoheader.biHeight) != Assemble_Integer(img2->infoheader.biHeight) || Assemble_Integer(img1->infoheader.biWidth) != Assemble_Integer(img2->infoheader.biWidth))
		return false;

	int dif = 0;

	// disregard the edges when comparing
	for (int i = 1; i < Assemble_Integer(img1->infoheader.biHeight) - 1; i++)
		for (int j = 1; j < Assemble_Integer(img1->infoheader.biWidth) - 1; j++)
			if (img1->image_ptr[i][j] != img2->image_ptr[i][j])
				dif++;

	// cout << "Differences: " << dif << endl;
	return dif == 0;
}

// ===== Identical_Image =====
// will return true if every pixel in img1 matches the corresponding pixel in img2
bmpBITMAP_FILE * Difference_Image(bmpBITMAP_FILE  * img1, bmpBITMAP_FILE  * img2)
{
	// Preconditions:
	assert(img1 != NULL && img2 != NULL);

	// if images are different sizes, they aren't identical
	if (Assemble_Integer(img1->infoheader.biHeight) != Assemble_Integer(img2->infoheader.biHeight) || Assemble_Integer(img1->infoheader.biWidth) != Assemble_Integer(img2->infoheader.biWidth))
		return new bmpBITMAP_FILE(img1, false);

	int dif = 0;
	bmpBITMAP_FILE * new_img = new bmpBITMAP_FILE(img1, false);

	// disregard the edges when comparing
	for (int i = 1; i < Assemble_Integer(img1->infoheader.biHeight) - 1; i++)
		for (int j = 1; j < Assemble_Integer(img1->infoheader.biWidth) - 1; j++)
			if (img1->image_ptr[i][j] != img2->image_ptr[i][j])
				new_img->image_ptr[i][j] = 0, dif++;

	std::cout << "Differences: " << dif << std::endl;
	return new_img;
}
