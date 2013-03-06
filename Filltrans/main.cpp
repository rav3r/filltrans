#pragma comment(lib, "FreeImage.lib")
#pragma comment(lib, "FreeImagePlus.lib")

#include <iostream>
#include <vector>
#include "FreeImagePlus.h"

struct Pixel
{
	Pixel() {x=y=0;}
	Pixel(int _x, int _y) {x=_x; y=_y;}
	int x, y;
};

int main(int argc, char *argv[])
{
	if(argc < 2)
		return 0;

	const char* filename = argv[1];

	fipImage image;
	if(!image.load(filename))
		return 0;

	image.convertTo32Bits();

	int		width	= image.getWidth();
	int		height	= image.getHeight();

	int k = 0;

	std::vector<Pixel> open;
	Pixel tmp;
	int *dist = new int[width*height];

	for(int i=0; i<width*height; i++)
		dist[i] = -1;

	for(int x=0; x<width; x++)
	{
		for(int y=0; y<height; y++)
		{
			RGBQUAD px;
			image.getPixelColor(x, y, &px);

			if(px.rgbReserved != 0)
			{
				tmp.x				= x;
				tmp.y				= y;

				dist[y*width + x]	= 0;

				open.push_back(tmp);
			}
		}
	}

	for(unsigned int i=0; i<open.size(); i++)
	{
		Pixel	p = open[i];
		int		d = dist[p.y*width + p.x];

		if(p.x > 0			&& dist[p.y*width		+ p.x-1]	== -1)		{ open.push_back(Pixel(p.x-1, p.y)); dist[p.y*width		+ p.x-1]	= d+1; }
		if(p.x < width-1	&& dist[p.y*width		+ p.x+1]	== -1)		{ open.push_back(Pixel(p.x+1, p.y)); dist[p.y*width		+ p.x+1]	= d+1; }
		if(p.y > 0			&& dist[(p.y-1)*width	+ p.x]		== -1)		{ open.push_back(Pixel(p.x, p.y-1)); dist[(p.y-1)*width + p.x]		= d+1; }
		if(p.y < height-1	&& dist[(p.y+1)*width	+ p.x]		== -1)		{ open.push_back(Pixel(p.x, p.y+1)); dist[(p.y+1)*width + p.x]		= d+1; }
	}

	for(unsigned int i=0; i<open.size(); i++)
	{
		Pixel	p = open[i];
		int		d = dist[p.y*width + p.x];

		if(d == 0) continue;

		int R=0, G=0, B=0;
		int c=0;
		RGBQUAD px;

		if(p.x > 0			&& dist[p.y*width		+ p.x-1]	< d)		{ image.getPixelColor(p.x-1, p.y, &px); R+=px.rgbRed; G+=px.rgbGreen; B+=px.rgbBlue; c++; }
		if(p.x < width-1	&& dist[p.y*width		+ p.x+1]	< d)		{ image.getPixelColor(p.x+1, p.y, &px); R+=px.rgbRed; G+=px.rgbGreen; B+=px.rgbBlue; c++; }
		if(p.y > 0			&& dist[(p.y-1)*width	+ p.x]		< d)		{ image.getPixelColor(p.x, p.y-1, &px); R+=px.rgbRed; G+=px.rgbGreen; B+=px.rgbBlue; c++; }
		if(p.y < height-1	&& dist[(p.y+1)*width	+ p.x]		< d)		{ image.getPixelColor(p.x, p.y+1, &px); R+=px.rgbRed; G+=px.rgbGreen; B+=px.rgbBlue; c++; }

		px.rgbRed		= R/c;
		px.rgbGreen		= G/c;
		px.rgbBlue		= B/c;
		px.rgbReserved	= 0;

		image.setPixelColor(p.x, p.y, &px);
	}

	image.save(filename);
	return 0;
}