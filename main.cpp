#include <iostream>
#include <SDL.h>
#include "input.hpp"
#include "chess.hpp"

SDL_Window* window;
SDL_Renderer* renderer;

InputManager input;

bool game_is_running = false;
float time_since_last_frame = 0.0;
int target_fps = 30;
int window_size[2];

Chess game;

const int margin = 10;
int tile_size = 1;
int mouse_tile_x;
int mouse_tile_y;

int grabbing_x = -1;
int grabbing_y = -1;

bool flip_board = true;

void initialize_window()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow(
		"Chess",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		100,
		100,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED
	);
	renderer = SDL_CreateRenderer(window, -1, 0);

	game_is_running = true;
}

void cleanup()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void setup()
{
	input.set_monitering(SDLK_ESCAPE);
}

void get_input()
{
	input.update();

	mouse_tile_x = -window_size[0] / 2.0f / tile_size + (float)(input.mouse_x) / tile_size + 4;
	mouse_tile_y = -window_size[1] / 2.0f / tile_size + (float)(input.mouse_y) / tile_size + 4;

	if (flip_board && game.turn == 1)
		mouse_tile_x = 7 - mouse_tile_x;
	else
		mouse_tile_y = 7 - mouse_tile_y;

	if (mouse_tile_x < 0 || mouse_tile_x > 7 || mouse_tile_y < 0 || mouse_tile_y > 7)
	{
		mouse_tile_x = -1;
		mouse_tile_y = -1;
	}

	if (input.mouse_down && mouse_tile_x != -1 && grabbing_x == -1 && game.board.get_tile(mouse_tile_x, mouse_tile_y).team == game.turn)
	{
		grabbing_x = mouse_tile_x;
		grabbing_y = mouse_tile_y;
	}
	else if (!input.mouse_down && mouse_tile_x != -1 && grabbing_x != -1)
	{
		game.attempt_move(grabbing_x, grabbing_y, mouse_tile_x, mouse_tile_y);

		grabbing_x = -1;
		grabbing_y = -1;
	}

	if (input.is_pressed(SDLK_ESCAPE) || input.clicked_x)
		game_is_running = false;
}

void update()
{
	int time_to_wait = (1000.0f / target_fps) - (SDL_GetTicks() - time_since_last_frame);

	if (time_to_wait > 0 && time_to_wait <= (1000.0f / target_fps))
		SDL_Delay(time_to_wait);

	float delta = (SDL_GetTicks() - time_since_last_frame) / 1000.0f;
	time_since_last_frame = SDL_GetTicks();
}

void draw_circle(int x, int y, int radius, int red, int green, int blue, int alpha)
{
	SDL_SetRenderDrawColor(renderer, red, green, blue, alpha);
	for (int w = 0; w < radius * 2; w++)
	{
		for (int h = 0; h < radius * 2; h++)
		{
			int dx = radius - w;
			int dy = radius - h;
			if ((dx * dx + dy * dy) <= (radius * radius))
			{
				SDL_RenderDrawPoint(renderer, x + dx, y + dy);
			}
		}
	}
}

void draw()
{
	SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
	SDL_RenderClear(renderer);

	SDL_GetWindowSize(window, &window_size[0], &window_size[1]);

	if (window_size[0] < window_size[1])
		tile_size = (window_size[0] - margin * 2) / 8;
	else
		tile_size = (window_size[1] - margin * 2) / 8;

	for (int x = 0; x < 8; ++x)
		for (int y = 0; y < 8; ++y)
		{
			int translated_x;
			int translated_y;
			if (flip_board && game.turn == 1)
			{
				translated_x = 7 - x;
				translated_y = y;
			}
			else
			{
				translated_x = x;
				translated_y = 7 - y;
			}

			if ((x + y) % 2 == 0)
				SDL_SetRenderDrawColor(renderer, 200, 100, 0, 255);
			else
				SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
			
			if (grabbing_x == x && grabbing_y == y)
				SDL_SetRenderDrawColor(renderer, 100, 200, 100, 255);
			else if (mouse_tile_x == x && mouse_tile_y == y)
				SDL_SetRenderDrawColor(renderer, 200, 200, 100, 255);
			
			SDL_Rect rect;
			rect.x = window_size[0] / 2 - tile_size * 4 + translated_x * tile_size;
			rect.y = window_size[1] / 2 - tile_size * 4 + translated_y * tile_size;
			rect.w = tile_size;
			rect.h = tile_size;

			SDL_RenderFillRect(renderer, &rect);

			if (grabbing_x != -1 && game.get_controlled_tiles(game.board, grabbing_x, grabbing_y)[x][y])
				draw_circle(rect.x + rect.w / 2, rect.y + rect.h / 2, tile_size / 4, 100, 100, 100, 100);
			
			for (int i = 0; i < game.board.get_tile(x, y).type + 1; ++i)
			{
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
				SDL_Rect rect2;
				rect2.x = window_size[0] / 2 - tile_size * 4 + translated_x * tile_size + 5 + i * 10;
				rect2.y = window_size[1] / 2 - tile_size * 4 + translated_y * tile_size + 5;
				rect2.w = 5;
				rect2.h = 5;
				SDL_RenderFillRect(renderer, &rect2);

				if (game.board.get_tile(x, y).team == 0)
				{
					SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
					rect2.x += 1;
					rect2.y += 1;
					rect2.w -= 2;
					rect2.h -= 2;
					SDL_RenderFillRect(renderer, &rect2);
				}
			}
		}

	SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[])
{
	initialize_window();

	setup();

	while (game_is_running)
	{
		get_input();
		update();
		draw();
	}

	cleanup();

	return 0;
}