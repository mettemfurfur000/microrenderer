#include "headers/script_parser.h"

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("Usage: %s <filename>\n", argv[0]);
		return 0;
	}

	parse_config(argv[1]);

	return 0;
}

/*
const char *font_name = "fonts/Modeseven-L3n5.ttf";
	SDL_Point const global_center = {WIDTH / 2, HEIGHT / 2};
	SDL_Rect rect = {WIDTH / 4, HEIGHT / 4, WIDTH / 2, HEIGHT / 2};

	SDL_Surface *surface;

	if (!(surface = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, DEPTH, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff)))
	{
		printf("SDL_CreateRGBSurface: %s\n", SDL_GetError());
		exit(1);
	}

	TTF_Font *font;

	if (!(font = TTF_OpenFont(font_name, 20)))
	{
		printf("TTF_OpenFont: %s\n", SDL_GetError());
		exit(1);
	}

	SDL_Color black_color = {0x00, 0x00, 0x00, 0xff};

	SDL_Surface *text_surface = TTF_RenderText_Blended(font, "[ TEST 123#$%", black_color);

	SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);

	SDL_Rect text_rect = {global_center.x, global_center.y, 0, 0};

	SDL_QueryTexture(text_texture, 0, 0, &text_rect.w, &text_rect.h);
	// text_rect.x += text_rect.w / 2;
	text_rect.y -= text_rect.h / 2;

	SDL_Point rotate_center = {-32, text_rect.h / 2};
	text_rect.x -= rotate_center.x;

	// Rendering
	SDL_SetRenderDrawColor(renderer, 25, 25, 175, 255);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderDrawRect(renderer, &rect);
	SDL_RenderDrawPoint(renderer, WIDTH / 2, HEIGHT / 2);

	const int rotates = 5;
	const double angle = 360.0 / rotates;
	for (int i = 0; i < rotates; i++)
		SDL_RenderCopyEx(renderer, text_texture, 0, &text_rect, i * angle, &rotate_center, 0);

	// Saving to file
	SDL_RenderReadPixels(renderer, 0, SDL_PIXELFORMAT_RGBA8888, surface->pixels, BYTES_PER_PIXEL * WIDTH);

	IMG_SavePNG(surface, "screen.png"); // Output buffer

	SDL_FreeSurface(surface);

	return 0;
}
*/