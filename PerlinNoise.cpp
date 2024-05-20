#include<iostream>
#include<math.h>
#include"SFML/Graphics.hpp"

typedef struct { float x, y; } vector2;

vector2 randomGradient(int ix, int iy)
{
	const unsigned w = 8 * sizeof(unsigned); // w = 8 * 4 = 32
	const unsigned s = w / 2; // s = 32 / 2 = 16
	unsigned a = ix, b = iy;
	a *= 3284157443;

	b ^= a << s | a >> w - s;
	b *= 191150717;

	a ^= b << s | b >> w - s;
	a *= 2048419325;
	float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]

	// create the vector from the angle
	vector2 v;
	v.x = sin(random);
	v.y = sin(random);
	return v;
}

float dotGridGradient(int ix, int iy, float x, float y)
{
	// get gradient from integer coordinates
	vector2 gradient = randomGradient(ix, iy);

	// compute the distance vector
	float dx = x - (float)ix;
	float dy = y - (float)iy;

	// compute the dot-product
	return(dx * gradient.x + dy * gradient.y);
}

float interpolate(float a0, float a1, float w)
{
	return(a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
}

float perlin(float x, float y)
{
	// determine grid cell corner coords
	int x0 = (int)x;
	int y0 = (int)y;
	int x1 = x0 + 1;
	int y1 = y0 + 1;

	// compute interpolation weight
	float sx = x - (float)x0;
	float sy = y - (float)y0;

	// compute interpolate top two corners
	float n0 = dotGridGradient(x0, y0, x, y);
	float n1 = dotGridGradient(x1, y0, x, y);
	float ix0 = interpolate(n0, n1, sx);

	// compute itnerpolate bottom two corners
	n0 = dotGridGradient(x0, y1, x, y);
	n1 = dotGridGradient(x1, y1, x, y);
	float ix1 = interpolate(n0, n1, sx);

	// final step: interpolate between two previously interpolated values, now in y
	float value = interpolate(ix0, ix1, sy);

	return value;
}

int main()
{
	const int windowWidth = 1920;
	const int windowHeight = 1080;

	sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight, 32), "Perlin");

	sf::Uint8* pixels = new sf::Uint8[windowWidth * windowHeight * 4];
	// ^^^^^^^^  Uint8, bo maksymalnie 4 byte czyli 255 ||||4 bo R, G, B, Alpha


	const int GRID_SIZE = 400;

	for (int x = 0; x < windowWidth; x++)
	{
		for (int y = 0; y < windowHeight; y++)
		{
			int index = (y * windowWidth + x) * 4;


			float val = 0;

			float freq = 1;
			float amp = 1;

			for (int i = 0; i < 12; i++)
			{
				val += perlin(x * freq / GRID_SIZE, y * freq / GRID_SIZE) * amp;

				freq *= 2;
				amp /= 2;
			}

			// contrast
			val *= 1.2;

			// clipping
			if (val > 1.0f)
				val = 1.0f;
			else if (val < -1.0f)
				val = -1.0f;

			// convert 1 to -1 into 255 to 0
			int color = (int)(((val + 1.0f) * 0.5f) * 255);

			// set pixel color
			pixels[index] = color; // R
			pixels[index + 1] = color; // G
			pixels[index + 2] = color; // B
			pixels[index + 3] = 255;  // Alpha
		}
	}

	sf::Texture texture;
	sf::Sprite sprite;

	texture.create(windowWidth, windowHeight);

	texture.update(pixels);

	sprite.setTexture(texture);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		window.clear();
		window.draw(sprite);

		window.display();
	}

	return 0;
}
