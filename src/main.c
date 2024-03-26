#include <SDL2/SDL.h>

#include <SDL2/SDL_error.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define FILE	(u8) 0b00000111
#define RANK	(u8) 0b00111000

#define WHITE	(u64) 0
#define BLACK	(u64) 0x80

// Level I
#define PAWN		(u8) 0x0
#define ELEPHANT	(u8) 0x5
#define WARRIOR		(u8) 0x1
#define HORSE		(u8) 0x4
#define CAMEL		(u8) 0x6
#define MONKEY		(u8) 0x7
#define GENERAL		(u8) 0x2

// Level II
#define BISHOP		(u8) 0x8
#define CARDINAL	(u8) 0x9
#define HAWK		(u8) 0xa
#define TOWER		(u8) 0xb
#define CASTLE		(u8) 0xc
#define BULL		(u8) 0xd
#define PEGASUS		(u8) 0xe
#define KING		(u8) 0x3

// Level III
#define QUEEN		(u8) 0xf
#define DRAGON		(u8) 0x10

// Level VI

// Level V

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef struct {
	u64 white_mask;
	u64 black_mask;
	u64 rank_1;
	u64 rank_2;
	u64 rank_3;
	u64 rank_4;
	u64 rank_5;
	u64 rank_6;
	u64 rank_7;
	u64 rank_8;
} Board;

typedef struct {
	u16 window_w;
	u16 window_h;
	u16 square_w;
	u16 square_h;
	u8 file_amount;
	u8 rank_amount;
} Prop;

const char *show_prop(Prop *const props) {
	char *res;
	sprintf(res, "%d, %d\n%d %d", props->window_w, props->window_h, props->square_w, props->square_h);
	return res;
}

int frame(SDL_Window *window, SDL_Renderer *renderer, Board *state, Prop *const props) {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
	SDL_RenderClear(renderer);

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				return 0;
			default:
				break;
		}
	}

	SDL_Rect rect;
	rect.w = props->square_w;
	rect.h = props->square_h;

	u8 color = 0;
	for (int i = 0; i < props->file_amount * props->rank_amount; i++) {
		rect.x = (i & FILE) * props->square_w;
		rect.y = (i >> 3 & FILE) * props->square_h;
		color = ((i & 0x1 ^ i >> 3 & 0x1) * 0xff);
		SDL_SetRenderDrawColor(renderer, color, color, color, 0xff);
		SDL_RenderFillRect(renderer, &rect);
	}

	SDL_RenderPresent(renderer);
	return 1;
}

Prop getProps() {
	const short WINDOW_W = 960;
	const short WINDOW_H = 960;
	const short FILE_AMOUNT = 8;
	const short RANK_AMOUNT = 8;
	Prop prop = {
		.window_w = WINDOW_W,
		.window_h = WINDOW_H,
		.file_amount = FILE_AMOUNT,
		.rank_amount = RANK_AMOUNT,
		.square_w = WINDOW_W / FILE_AMOUNT,
		.square_h = WINDOW_H / RANK_AMOUNT,
	};
	return prop;
}

Board getBoard() {
	Board board = {
		.white_mask = 0x000000000000ffff,
		.black_mask = 0xffff000000000000,
		.rank_8 = (BLACK | TOWER) << 56
			| (BLACK | HORSE) << 48
			| (BLACK | BISHOP) << 40
			| (BLACK | QUEEN) << 32
			| (BLACK | KING) << 24
			| (BLACK | BISHOP) << 16
			| (BLACK | HORSE) << 8
			| (BLACK | TOWER),
		.rank_7 = 0x8080808080808080,
		.rank_6 = 0, .rank_5 = 0, .rank_4 = 0, .rank_3 = 0,
		.rank_2 = 0x0,
		.rank_1 = (u64)TOWER << 56
			| (u64)HORSE << 48
			| (u64)BISHOP << 40
			| (u64)QUEEN << 32
			| (u64)KING << 24
			| (u64)BISHOP << 16
			| (u64)HORSE << 8
			| (u64)TOWER,
	};
	return board;
}

int main() {
	const char WINDOW_TITLE[] = "Chess";

	int errnum = 0;
	if ((errnum = SDL_Init(SDL_INIT_EVERYTHING)) < 0) {
		fprintf(stderr, "Error: couldn't initialize SDL\nSDL exited with error:\n%s\n", SDL_GetError());
		return 1;
	}

	Prop props = getProps();
	int WINDOW_FLAGS = 0;
	SDL_Window *window = SDL_CreateWindow(
			WINDOW_TITLE,
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			props.window_w, props.window_h,
			WINDOW_FLAGS);
	if (window == NULL) {
		fprintf(stderr, "Error: couldn't open window\nSDL exited with error:\n%s\n", SDL_GetError());
		return 1;
	}

	int RENDER_FLAGS = SDL_RENDERER_ACCELERATED;
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, RENDER_FLAGS);
	if (renderer == NULL) {
		fprintf(stderr, "Error: couldn't create renderer\nSDL exited with error:\n%s\n", SDL_GetError());
	}

	Board board = getBoard();
	while (frame(window, renderer, &board, &props));

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();

	return 0;
}
