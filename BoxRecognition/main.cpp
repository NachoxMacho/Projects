#include "hough.h"
#include <ctime>
using namespace std;

#define smooth_factor 5

/* Plane Detection methods
 * Guzman
 * Huffman & Clause
 */

// ===== Accessing BMP INFORMATION =====
// (0, 0) is bottom left
// accessing should go image_ptr[y][x]

//======================  MAIN  ============================
int main()
{
	bmpBITMAP_FILE * orig_image = new bmpBITMAP_FILE; // the base image
	bmpBITMAP_FILE * new_image, * search, * new_search; // a copy of the base image
	char smooth_file[20] = "Smooth.bmp", scan_file[20] = "Scanned.bmp", final_file[20] = "Final.bmp";

	// load the original file to memory
	Load_Bitmap_File(orig_image);

	cout << "INFO | Picture Information | " << 
		"Height: " << Assemble_Integer(orig_image->infoheader.biHeight) << 
		" | Width: " << Assemble_Integer(orig_image->infoheader.biWidth) << endl;
	cout << "---------------------------------------------------------------" << endl;

	// copy the original so it isn't modified & remove the original from memory
	new_image = new bmpBITMAP_FILE(orig_image, true);
	delete orig_image;

	int start = time(NULL);
	cout << "FUNC | Starting Smoothing (" << smooth_factor << " time(s))" << endl;
	for (int i = 0; i < smooth_factor; i++)
	{
		bmpBITMAP_FILE * im = Smooth_Image(new_image);
		delete new_image;
		new_image = im;
	}

	cout << "FUNC | Finished Smoothing " << time(NULL) - start  << " seconds to complete" << endl;

	// make the hough transform
	start = time(NULL);
	cout << "FUNC | Starting Constructing Hough " << endl;
	Hough *hough = new Hough(new_image);
	cout << "FUNC | Finished Constructing Hough " << time(NULL) - start << " seconds to complete" << endl;

	// condense lines
	start = time(NULL);
	cout << "FUNC | Starting Condensing Lines " << endl;
	Save_Bitmap_File(hough->scan_lines(), scan_file);
	cout << "FUNC | Finished Condensing Lines " << time(NULL) - start << " seconds to complete" << endl;

	// find planes
	start = time(NULL);
	cout << "FUNC | Starting Finding Planes " << endl;
	search = hough->display_lines();
	hough->scan_intersections();
	new_search = hough->display_lines();

	while (!Identical_Image(search, new_search))
	{
		delete search, new_search;
		search = hough->display_lines();
		hough->scan_intersections();
		new_search = hough->display_lines();
	}

	Save_Bitmap_File(hough->display_lines(), final_file);
	cout << "FUNC | Finished Finding Planes " << time(NULL) - start << " seconds to complete" << endl;

	// Remove the imgs from memory
	cout << "Removing images from memory" << endl;
	delete new_image, search, new_search; // imgs
	delete hough; // Hough
	return 0;
}
