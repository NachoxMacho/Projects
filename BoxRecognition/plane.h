#pragma once
#include "segment.h"

struct plane
{
	segment l1, l2, l3, l4;

	plane(segment n1, segment n2, segment n3, segment n4) { l1 = n1, l2 = n2, l3 = n3, l4 = n4; }
	plane() { ; }

	float density()
	{
		float densTotal = l1.density() + l2.density() + l3.density() + l4.density();
		return densTotal / 4;
	}

	pixel l1_int_l2()
	{
		return l1.intersect(l2);
	}

	pixel l3_int_l2()
	{
		return l3.intersect(l2);
	}

	pixel l1_int_l4()
	{
		return l1.intersect(l4);
	}

	pixel l3_int_l4()
	{
		return l3.intersect(l4);
	}

	bmpBITMAP_FILE * print(bmpBITMAP_FILE * img)
	{
		bmpBITMAP_FILE * new_img = l1.display(img);

		new_img->operator+=(l2.display(img));
		new_img->operator+=(l3.display(img));
		new_img->operator+=(l4.display(img));
		return new_img;
	}
};