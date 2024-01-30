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
				SDL_SetRenderDrawColor(renderer, 173, 116, 76, 255);
			else
				SDL_SetRenderDrawColor(renderer, 173, 140, 116, 255);
			
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
			
			Piece piece = game.board.get_tile(x, y);
			int fill_color = (1 - piece.team) * 255;
			int outline_color = piece.team * 255;
			int origin_x = window_size[0] / 2 - tile_size * 4 + translated_x * tile_size;
			int origin_y = window_size[1] / 2 - tile_size * 4 + translated_y * tile_size;

			if (piece.type != -1)
			{
				SDL_SetRenderDrawColor(renderer, fill_color, fill_color, fill_color, 255);
				rect.x = origin_x + tile_size * 0.15;
				rect.y = origin_y + tile_size * 0.85;
				rect.w = tile_size * 0.7;
				rect.h = tile_size * 0.05;
				SDL_RenderFillRect(renderer, &rect);
				rect.x = origin_x + tile_size * 0.2;
				rect.y = origin_y + tile_size * 0.8;
				rect.w = tile_size * 0.6;
				rect.h = tile_size * 0.1;
				SDL_RenderFillRect(renderer, &rect);
				rect.x = origin_x + tile_size * 0.3;
				rect.y = origin_y + tile_size * 0.75;
				rect.w = tile_size * 0.4;
				rect.h = tile_size * 0.15;
				SDL_RenderFillRect(renderer, &rect);

				rect.x = origin_x + tile_size * 0.45;
				rect.y = origin_y + tile_size * 0.5;
				rect.w = tile_size * 0.1;
				rect.h = tile_size * 0.4;
				SDL_RenderFillRect(renderer, &rect);
				rect.x = origin_x + tile_size * 0.425;
				rect.y = origin_y + tile_size * 0.65;
				rect.w = tile_size * 0.15;
				rect.h = tile_size * 0.2;
				SDL_RenderFillRect(renderer, &rect);
				rect.x = origin_x + tile_size * 0.38;
				rect.y = origin_y + tile_size * 0.7;
				rect.w = tile_size * 0.24;
				rect.h = tile_size * 0.2;
				SDL_RenderFillRect(renderer, &rect);
			}

			switch (piece.type)
			{
				case PAWN:
					rect.x = origin_x + tile_size * 0.39;
					rect.y = origin_y + tile_size * 0.250;
					rect.w = tile_size * 0.230;
					rect.h = tile_size * 0.301;
					SDL_RenderFillRect(renderer, &rect);
					rect.x = origin_x + tile_size * 0.350;
					rect.y = origin_y + tile_size * 0.29;
					rect.w = tile_size * 0.300;
					rect.h = tile_size * 0.231;
					SDL_RenderFillRect(renderer, &rect);
					break;

				case KNIGHT:
					rect.x = origin_x + tile_size * 0.45;
					rect.y = origin_y + tile_size * 0.3;
					rect.w = tile_size * 0.400;
					rect.h = tile_size * 0.21;
					SDL_RenderFillRect(renderer, &rect);
					rect.x = origin_x + tile_size * 0.35;
					rect.y = origin_y + tile_size * 0.4;
					rect.w = tile_size * 0.1;
					rect.h = tile_size * 0.4;
					SDL_RenderFillRect(renderer, &rect);
					rect.x = origin_x + tile_size * 0.32;
					rect.y = origin_y + tile_size * 0.55;
					rect.w = tile_size * 0.2;
					rect.h = tile_size * 0.3;
					SDL_RenderFillRect(renderer, &rect);
					rect.x = origin_x + tile_size * 0.4;
					rect.y = origin_y + tile_size * 0.15;
					rect.w = tile_size * 0.1;
					rect.h = tile_size * 0.3;
					SDL_RenderFillRect(renderer, &rect);
					rect.x = origin_x + tile_size * 0.45;
					rect.y = origin_y + tile_size * 0.22;
					rect.w = tile_size * 0.075;
					rect.h = tile_size * 0.3;
					SDL_RenderFillRect(renderer, &rect);
					break;

				case BISHOP:
					rect.x = origin_x + tile_size * 0.39;
					rect.y = origin_y + tile_size * 0.200;
					rect.w = tile_size * 0.230;
					rect.h = tile_size * 0.351;
					SDL_RenderFillRect(renderer, &rect);
					rect.x = origin_x + tile_size * 0.350;
					rect.y = origin_y + tile_size * 0.29;
					rect.w = tile_size * 0.300;
					rect.h = tile_size * 0.116;
					SDL_RenderFillRect(renderer, &rect);
					rect.x = origin_x + tile_size * 0.45;
					rect.y = origin_y + tile_size * 0.100;
					rect.w = tile_size * 0.10;
					rect.h = tile_size * 0.351;
					SDL_RenderFillRect(renderer, &rect);
					break;

				case ROOK:
					rect.x = origin_x + tile_size * 0.15;
					rect.y = origin_y + tile_size * 0.35;
					rect.w = tile_size * 0.7;
					rect.h = tile_size * 0.16;
					SDL_RenderFillRect(renderer, &rect);
					rect.x = origin_x + tile_size * 0.15;
					rect.y = origin_y + tile_size * 0.25;
					rect.w = tile_size * 0.1;
					rect.h = tile_size * 0.11;
					SDL_RenderFillRect(renderer, &rect);
					rect.x = origin_x + tile_size * 0.35;
					rect.y = origin_y + tile_size * 0.25;
					rect.w = tile_size * 0.1;
					rect.h = tile_size * 0.11;
					SDL_RenderFillRect(renderer, &rect);
					rect.x = origin_x + tile_size * 0.55;
					rect.y = origin_y + tile_size * 0.25;
					rect.w = tile_size * 0.1;
					rect.h = tile_size * 0.11;
					SDL_RenderFillRect(renderer, &rect);
					rect.x = origin_x + tile_size * 0.75;
					rect.y = origin_y + tile_size * 0.25;
					rect.w = tile_size * 0.1;
					rect.h = tile_size * 0.25;
					SDL_RenderFillRect(renderer, &rect);
					break;

				case QUEEN:
					rect.x = origin_x + tile_size * 0.25;
					rect.y = origin_y + tile_size * 0.5;
					rect.w = tile_size * 0.5;
					rect.h = tile_size * 0.25;
					SDL_RenderFillRect(renderer, &rect);
					rect.x = origin_x + tile_size * 0.3;
					rect.y = origin_y + tile_size * 0.45;
					rect.w = tile_size * 0.4;
					rect.h = tile_size * 0.2;
					SDL_RenderFillRect(renderer, &rect);

					rect.x = origin_x + tile_size * 0.425;
					rect.y = origin_y + tile_size * 0.3;
					rect.w = tile_size * 0.15;
					rect.h = tile_size * 0.3;
					SDL_RenderFillRect(renderer, &rect);
					rect.x = origin_x + tile_size * 0.475;
					rect.y = origin_y + tile_size * 0.2;
					rect.w = tile_size * 0.05;
					rect.h = tile_size * 0.3;
					SDL_RenderFillRect(renderer, &rect);

					rect.x = origin_x + tile_size * 0.25;
					rect.y = origin_y + tile_size * 0.425;
					rect.w = tile_size * 0.15;
					rect.h = tile_size * 0.3;
					SDL_RenderFillRect(renderer, &rect);
					rect.x = origin_x + tile_size * 0.3;
					rect.y = origin_y + tile_size * 0.325;
					rect.w = tile_size * 0.05;
					rect.h = tile_size * 0.3;
					SDL_RenderFillRect(renderer, &rect);

					rect.x = origin_x + tile_size * 0.61;
					rect.y = origin_y + tile_size * 0.425;
					rect.w = tile_size * 0.15;
					rect.h = tile_size * 0.3;
					SDL_RenderFillRect(renderer, &rect);
					rect.x = origin_x + tile_size * 0.66;
					rect.y = origin_y + tile_size * 0.325;
					rect.w = tile_size * 0.05;
					rect.h = tile_size * 0.3;
					SDL_RenderFillRect(renderer, &rect);

					rect.x = origin_x + tile_size * 0.2;
					rect.y = origin_y + tile_size * 0.5;
					rect.w = tile_size * 0.61;
					rect.h = tile_size * 0.15;
					SDL_RenderFillRect(renderer, &rect);
					rect.x = origin_x + tile_size * 0.15;
					rect.y = origin_y + tile_size * 0.55;
					rect.w = tile_size * 0.71;
					rect.h = tile_size * 0.05;
					SDL_RenderFillRect(renderer, &rect);
					break;

				case KING:
					rect.x = origin_x + tile_size * 0.25;
					rect.y = origin_y + tile_size * 0.5;
					rect.w = tile_size * 0.5;
					rect.h = tile_size * 0.25;
					SDL_RenderFillRect(renderer, &rect);
					rect.x = origin_x + tile_size * 0.3;
					rect.y = origin_y + tile_size * 0.45;
					rect.w = tile_size * 0.4;
					rect.h = tile_size * 0.2;
					SDL_RenderFillRect(renderer, &rect);

					rect.x = origin_x + tile_size * 0.45;
					rect.y = origin_y + tile_size * 0.1;
					rect.w = tile_size * 0.1;
					rect.h = tile_size * 0.5;
					SDL_RenderFillRect(renderer, &rect);
					rect.x = origin_x + tile_size * 0.35;
					rect.y = origin_y + tile_size * 0.2;
					rect.w = tile_size * 0.3;
					rect.h = tile_size * 0.1;
					SDL_RenderFillRect(renderer, &rect);

					rect.x = origin_x + tile_size * 0.25;
					rect.y = origin_y + tile_size * 0.35;
					rect.w = tile_size * 0.1;
					rect.h = tile_size * 0.2;
					SDL_RenderFillRect(renderer, &rect);
					rect.x = origin_x + tile_size * 0.66;
					rect.y = origin_y + tile_size * 0.35;
					rect.w = tile_size * 0.1;
					rect.h = tile_size * 0.2;
					SDL_RenderFillRect(renderer, &rect);


					break;
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
