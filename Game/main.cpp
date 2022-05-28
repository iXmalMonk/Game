#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<iostream>
#include<Windows.h>

#define WIDTH 1280
#define HEIGHT 1024

#define MAX_BULLETS_FOR_PISTOL 15
#define MAX_ZOMBIE 100

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

void deInit(int error)
{
	if (renderer != NULL) SDL_DestroyRenderer(renderer);

	if (window != NULL) SDL_DestroyWindow(window);

	TTF_Quit();

	IMG_Quit();

	SDL_Quit();

	exit(error);
}

void init()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		printf("Error: %s", SDL_GetError());
		system("pause");
		deInit(1);
	}

	int img;

	if ((img = IMG_Init(IMG_INIT_PNG)) == 0)
	{
		printf("Error: %s", SDL_GetError());
		system("pause");
		deInit(1);
	}

	//if (img and IMG_INIT_PNG) printf("PNG +");
	//else printf("PNG -");

	TTF_Init();

	window = SDL_CreateWindow("Top-Down Shooter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

	if (window == NULL)
	{
		printf("Error: %s", SDL_GetError());
		system("pause");
		deInit(1);
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (renderer == NULL)
	{
		printf("Error: %s", SDL_GetError());
		system("pause");
		deInit(1);
	}
}

SDL_Texture* loadTextureFromFile(const char* filename, SDL_Rect* rect)
{
	SDL_Surface* surface = IMG_Load(filename);

	if (surface == NULL)
	{
		printf("Error: %s - %s", SDL_GetError(), filename);
		system("pause");
		deInit(1);
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

	*rect = { 0, 0, surface->w, surface->h };

	SDL_FreeSurface(surface);

	return texture;
}

SDL_Texture* loadTextureFromFont(const char* text, SDL_Rect* rect, int x, int y)
{
	TTF_Font* font = TTF_OpenFont("font.ttf", 100);

	SDL_Color color = { 0, 0, 0 };

	SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

	*rect = { x, y, surface->w, surface->h };

	SDL_FreeSurface(surface);

	return texture;
}

void hideConsole(bool flag)
{
	HWND hide_console;
	AllocConsole();
	hide_console = FindWindowA("ConsoleWindowClass", NULL);
	ShowWindow(hide_console, !flag);
}

struct Loop {
	bool launched = true;
	bool menu = true;
	bool info = false;
	bool play = false;
};

struct Mouse {
	int x = 0;
	int y = 0;

	bool left_button = false;
};

struct Png {
	SDL_Rect play_rect;
	SDL_Texture* play_texture = loadTextureFromFile("png/play.png", &play_rect);
	SDL_Rect play_rect_dst = { WIDTH / 2 - play_rect.w / 2, (HEIGHT / 4) * 1 - play_rect.h, play_rect.w, play_rect.h };

	SDL_Rect info_rect;
	SDL_Texture* info_texture = loadTextureFromFile("png/info.png", &info_rect);
	SDL_Rect info_rect_dst = { WIDTH / 2 - info_rect.w / 2, (HEIGHT / 4) * 2 - info_rect.h, info_rect.w, info_rect.h };

	SDL_Rect exit_rect;
	SDL_Texture* exit_texture = loadTextureFromFile("png/exit.png", &exit_rect);
	SDL_Rect exit_rect_dst = { WIDTH / 2 - exit_rect.w / 2, (HEIGHT / 4) * 3 - exit_rect.h, exit_rect.w, exit_rect.h };

	SDL_Rect menu_rect;
	SDL_Texture* menu_texture = loadTextureFromFile("png/menu.png", &menu_rect);
	SDL_Rect menu_rect_dst = { 0, HEIGHT - menu_rect.h, menu_rect.w, menu_rect.h };

	SDL_Rect player_up_rect;
	SDL_Texture* player_up_texture = loadTextureFromFile("png/player_up.png", &player_up_rect);

	SDL_Rect player_down_rect;
	SDL_Texture* player_down_texture = loadTextureFromFile("png/player_down.png", &player_down_rect);

	SDL_Rect player_right_rect;
	SDL_Texture* player_right_texture = loadTextureFromFile("png/player_right.png", &player_right_rect);

	SDL_Rect player_left_rect;
	SDL_Texture* player_left_texture = loadTextureFromFile("png/player_left.png", &player_left_rect);

	SDL_Rect bullet_rect;
	SDL_Texture* bullet_texture = loadTextureFromFile("png/bullet.png", &bullet_rect);

	SDL_Rect zombie_up_rect;
	SDL_Texture* zombie_up_texture = loadTextureFromFile("png/zombie_up.png", &zombie_up_rect);

	SDL_Rect zombie_down_rect;
	SDL_Texture* zombie_down_texture = loadTextureFromFile("png/zombie_down.png", &zombie_down_rect);

	SDL_Rect zombie_right_rect;
	SDL_Texture* zombie_right_texture = loadTextureFromFile("png/zombie_right.png", &zombie_right_rect);

	SDL_Rect zombie_left_rect;
	SDL_Texture* zombie_left_texture = loadTextureFromFile("png/zombie_left.png", &zombie_left_rect);
};

struct Time {
	int lst = SDL_GetTicks();
	int nw = 0;
	int dlt = 0;
};

enum Direction { up, down, right, left };

struct Keyboard {
	bool up = false;
	bool down = false;
	bool right = false;
	bool left = false;
};

struct Player {
	int w = 64;
	int h = 64;
	
	Direction direction = up;
	Keyboard keyboard;
	
	int x = WIDTH / 2 - w / 2;
	int y = HEIGHT / 2 - h / 2;
	
	SDL_Rect rect_dst = { x, y, w, h };
	
	int frame = 0;
	int frame_count = 9;
	int frame_current = 0;
	int frame_max = 175;
	bool animate = false;
	
	int speed = 10;
	
	bool life = true; //
	
	int hp = 100; //
};

struct Bullet {
	int w = 10;
	int h = 10;
	
	SDL_Rect rect_dst = { 0, 0, w, h };
	
	double alpha = 0;
	
	bool life = false;
};

struct Pistol {
	Bullet bullet[MAX_BULLETS_FOR_PISTOL];
	
	int speed = 25;
	
	int delay = 19;
	
	int damage = 25;
};

struct Zombie {
	int w = 64;
	int h = 64;
	
	Direction direction = up;
	
	SDL_Rect rect_dst{ 0, 0, w, h };
	
	int frame = 0;
	int frame_count = 4;
	int frame_current = 0;
	int frame_max = 175;
	
	int speed = 3;
	
	bool life = false;
	
	int current_hp = 0;
	int max_hp = 100;

	//int current = MAX_ZOMBIE / 10;
	int current = 1;

	float alpha = 0;
};

struct Game {
	Loop loop;

	Mouse mouse;

	Png png;

	Time time;

	Player player;

	Pistol pistol;

	Zombie zombie[MAX_ZOMBIE];

	SDL_Event event;

	const int fps = 60;
};

void events(Game& game)
{
	while (SDL_PollEvent(&game.event))
	{
		switch (game.event.type)
		{
		case SDL_QUIT:
			game.loop.launched = false;
			break;
		case SDL_MOUSEMOTION:
			game.mouse.x = game.event.motion.x;
			game.mouse.y = game.event.motion.y;
			break;
		case SDL_MOUSEBUTTONDOWN:
			if ((game.png.play_rect_dst.x + game.png.play_rect_dst.w) >= game.mouse.x and game.mouse.x >= game.png.play_rect_dst.x and
				(game.png.play_rect_dst.y + game.png.play_rect_dst.h) >= game.mouse.y and game.mouse.y >= game.png.play_rect_dst.y and
				game.loop.menu)
			{
				game.loop.menu = false;
				game.loop.play = true;
			}
			if ((game.png.info_rect_dst.x + game.png.info_rect_dst.w) >= game.mouse.x and game.mouse.x >= game.png.info_rect_dst.x and
				(game.png.info_rect_dst.y + game.png.info_rect_dst.h) >= game.mouse.y and game.mouse.y >= game.png.info_rect_dst.y and
				game.loop.menu)
			{
				game.loop.menu = false;
				game.loop.info = true;
			}
			if ((game.png.exit_rect_dst.x + game.png.exit_rect_dst.w) >= game.mouse.x and game.mouse.x >= game.png.exit_rect_dst.x and
				(game.png.exit_rect_dst.y + game.png.exit_rect_dst.h) >= game.mouse.y and game.mouse.y >= game.png.exit_rect_dst.y and
				game.loop.menu)
			{
				game.loop.menu = false;
				game.loop.launched = false;
			}
			if ((game.png.menu_rect_dst.x + game.png.menu_rect_dst.w) >= game.mouse.x and game.mouse.x >= game.png.menu_rect_dst.x and
				(game.png.menu_rect_dst.y + game.png.menu_rect_dst.h) >= game.mouse.y and game.mouse.y >= game.png.menu_rect_dst.y and
				game.loop.info)
			{
				game.loop.info = false;
				game.loop.menu = true;
			}
			if (game.loop.play) game.mouse.left_button = true;
			break;
		case SDL_MOUSEBUTTONUP:
			if (game.loop.play) game.mouse.left_button = false;
			break;
		case SDL_KEYDOWN:
			switch (game.event.key.keysym.scancode)
			{
			case SDL_SCANCODE_ESCAPE:
				if (game.loop.play)
				{
					game.loop.play = false;
					game.loop.menu = true;
				}
				if (game.loop.info)
				{
					game.loop.info = false;
					game.loop.menu = true;
				}
				break;
			case SDL_SCANCODE_W:
				game.player.keyboard.up = true;
				break;
			case SDL_SCANCODE_S:
				game.player.keyboard.down = true;
				break;
			case SDL_SCANCODE_D:
				game.player.keyboard.right = true;
				break;
			case SDL_SCANCODE_A:
				game.player.keyboard.left = true;
				break;
			}
			break;
		case SDL_KEYUP:
			switch (game.event.key.keysym.scancode)
			{
			case SDL_SCANCODE_W:
				game.player.keyboard.up = false;
				break;
			case SDL_SCANCODE_S:
				game.player.keyboard.down = false;
				break;
			case SDL_SCANCODE_D:
				game.player.keyboard.right = false;
				break;
			case SDL_SCANCODE_A:
				game.player.keyboard.left = false;
				break;
			}
			break;
		}
	}
}

void menuLoop(Game game)
{
	SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
	SDL_RenderClear(renderer);

	SDL_RenderCopy(renderer, game.png.play_texture, &game.png.play_rect, &game.png.play_rect_dst);
	SDL_RenderCopy(renderer, game.png.info_texture, &game.png.info_rect, &game.png.info_rect_dst);
	SDL_RenderCopy(renderer, game.png.exit_texture, &game.png.exit_rect, &game.png.exit_rect_dst);
}

void infoLoop(Game game)
{
	SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
	SDL_RenderClear(renderer);

	SDL_RenderCopy(renderer, game.png.menu_texture, &game.png.menu_rect, &game.png.menu_rect_dst);
}

void logicPlayer(Game& game)
{
	if (game.player.keyboard.up)
	{
		game.player.y -= game.player.speed;
		game.player.direction = up;
	}
	if (game.player.keyboard.down)
	{
		game.player.y += game.player.speed;
		game.player.direction = down;
	}
	if (game.player.keyboard.right)
	{
		game.player.x += game.player.speed;
		game.player.direction = right;
	}
	if (game.player.keyboard.left)
	{
		game.player.x -= game.player.speed;
		game.player.direction = left;
	}

	game.player.animate = game.player.keyboard.up or game.player.keyboard.down or game.player.keyboard.right or game.player.keyboard.left;

	if (game.player.animate)
	{
		game.player.frame_current += game.time.dlt;
		if (game.player.frame_current > game.player.frame_max)
		{
			game.player.frame_current -= game.player.frame_max;
			game.player.frame = (game.player.frame + 1) % game.player.frame_count;
			if (game.player.direction == up) game.png.player_up_rect.x = game.png.player_up_rect.w * game.player.frame;
			if (game.player.direction == down) game.png.player_down_rect.x = game.png.player_down_rect.w * game.player.frame;
			if (game.player.direction == right) game.png.player_right_rect.x = game.png.player_right_rect.w * game.player.frame;
			if (game.player.direction == left) game.png.player_left_rect.x = game.png.player_left_rect.w * game.player.frame;
		}
	}

	game.player.rect_dst.x = game.player.x;
	game.player.rect_dst.y = game.player.y;
}

void drawPlayer(Game game)
{
	if (game.player.direction == up) SDL_RenderCopy(renderer, game.png.player_up_texture, &game.png.player_up_rect, &game.player.rect_dst);
	if (game.player.direction == down) SDL_RenderCopy(renderer, game.png.player_down_texture, &game.png.player_down_rect, &game.player.rect_dst);
	if (game.player.direction == right) SDL_RenderCopy(renderer, game.png.player_right_texture, &game.png.player_right_rect, &game.player.rect_dst);
	if (game.player.direction == left) SDL_RenderCopy(renderer, game.png.player_left_texture, &game.png.player_left_rect, &game.player.rect_dst);
}

void logicBulletsForPistol(Game& game)
{
	static int delay = 0;

	if (game.mouse.left_button and delay == game.pistol.delay)
	{
		for (int i = 0; i < MAX_BULLETS_FOR_PISTOL; i++)
			if (!game.pistol.bullet[i].life)
			{
				game.pistol.bullet[i].rect_dst.x = game.player.x + game.player.w / 2;
				game.pistol.bullet[i].rect_dst.y = game.player.y + game.player.h / 2;
				game.pistol.bullet[i].alpha = atan2(game.mouse.y - game.pistol.bullet[i].rect_dst.y, game.mouse.x - game.pistol.bullet[i].rect_dst.x);
				game.pistol.bullet[i].life = true;
				break;
			}
		delay = 0;
	}

	if (delay >= game.pistol.delay) delay = game.pistol.delay;
	else delay++;

	for (int i = 0; i < MAX_BULLETS_FOR_PISTOL; i++)
	{
		if (game.pistol.bullet[i].life)
		{
			game.pistol.bullet[i].rect_dst.x += cos(game.pistol.bullet[i].alpha) * game.pistol.speed;
			game.pistol.bullet[i].rect_dst.y += sin(game.pistol.bullet[i].alpha) * game.pistol.speed;

			if (game.pistol.bullet[i].rect_dst.x > WIDTH or game.pistol.bullet[i].rect_dst.y > HEIGHT or
				game.pistol.bullet[i].rect_dst.x + game.pistol.bullet->w < 0 or game.pistol.bullet[i].rect_dst.y + game.pistol.bullet->h < 0) game.pistol.bullet[i].life = false;

			// zombie
			for (int j = 0; j < game.zombie->current; j++)
			{
				if (game.pistol.bullet[i].rect_dst.x > game.zombie[j].rect_dst.x and game.pistol.bullet[i].rect_dst.x < game.zombie[j].rect_dst.x + game.zombie->w and
					game.pistol.bullet[i].rect_dst.y > game.zombie[j].rect_dst.y and game.pistol.bullet[i].rect_dst.y < game.zombie[j].rect_dst.y + game.zombie->h)
				{
					game.zombie[j].current_hp -= game.pistol.damage;
					printf("hit - %i\n", game.zombie[j].current_hp);
					game.pistol.bullet[i].life = false;
					break;
				}
			}
			// zombie
		}
	}
}

void drawBulletsForPistol(Game game)
{
	for (int i = 0; i < MAX_BULLETS_FOR_PISTOL; i++)
		if (game.pistol.bullet[i].life) SDL_RenderCopy(renderer, game.png.bullet_texture, &game.png.bullet_rect, &game.pistol.bullet[i].rect_dst);
}

void logicZombie(Game& game)
{	
	for (int i = 0; i < game.zombie->current; i++)
	{
		if (!game.zombie[i].life)
		{
			game.zombie[i].life = true;
			if (rand() % 2 == 1) game.zombie[i].rect_dst.x = -game.zombie->w;
			else game.zombie[i].rect_dst.x = WIDTH;
			if (rand() % 2 == 1) game.zombie[i].rect_dst.y = -game.zombie->h;
			else game.zombie[i].rect_dst.y = HEIGHT;
			game.zombie[i].current_hp = game.zombie->max_hp;
		}
	}

	for (int i = 0; i < game.zombie->current; i++)
	{
		if (game.zombie[i].current_hp <= 0)
		{
			printf("zombie died\n");
			game.zombie[i].life = false;
		}

		if (game.zombie[i].life)
		{


			if (game.zombie[i].rect_dst.y > game.player.rect_dst.y + game.player.h / 2 or
				game.zombie[i].rect_dst.y + game.zombie->h / 2 < game.player.rect_dst.y or
				game.zombie[i].rect_dst.x > game.player.rect_dst.x + game.player.w / 2 or
				game.zombie[i].rect_dst.x + game.zombie->w / 2 < game.player.x)
			{
				int dlt_x = game.zombie[i].rect_dst.x;
				int dlt_y = game.zombie[i].rect_dst.y;

				game.zombie[i].alpha = atan2(game.player.rect_dst.y - game.zombie[i].rect_dst.y, game.player.rect_dst.x - game.zombie[i].rect_dst.x);
				game.zombie[i].rect_dst.x += cos(game.zombie[i].alpha) * game.zombie->speed;
				game.zombie[i].rect_dst.y += sin(game.zombie[i].alpha) * game.zombie->speed;

				dlt_x = dlt_x - game.zombie[i].rect_dst.x;
				dlt_y = dlt_y - game.zombie[i].rect_dst.y;

				if (dlt_y > dlt_x and dlt_y > game.zombie->speed / 2) game.zombie[i].direction = up;
				if (dlt_y < dlt_x and dlt_y < -(game.zombie->speed / 2)) game.zombie[i].direction = down;
				if (dlt_x < dlt_y and dlt_x < -(game.zombie->speed / 2)) game.zombie[i].direction = right;
				if (dlt_x > dlt_y and dlt_x > game.zombie->speed / 2) game.zombie[i].direction = left;
			}
			game.zombie[i].frame_current += game.time.dlt;
			if (game.zombie[i].frame_current > game.zombie->frame_max)
			{
				game.zombie[i].frame_current -= game.zombie->frame_max;
				game.zombie[i].frame = (game.zombie[i].frame + 1) % game.zombie->frame_count;
				if (game.zombie[i].direction == up) game.png.zombie_up_rect.x = game.png.zombie_up_rect.w * game.zombie[i].frame;
				if (game.zombie[i].direction == down) game.png.zombie_down_rect.x = game.png.zombie_down_rect.w * game.zombie[i].frame;
				if (game.zombie[i].direction == right) game.png.zombie_right_rect.x = game.png.zombie_right_rect.w * game.zombie[i].frame;
				if (game.zombie[i].direction == left) game.png.zombie_left_rect.x = game.png.zombie_left_rect.w * game.zombie[i].frame;
			}
		}
	}
}

void drawZombie(Game& game)
{
	for (int i = 0; i < game.zombie->current; i++)
	{
		if (game.zombie[i].life)
		{
			if (game.zombie[i].direction == up) SDL_RenderCopy(renderer, game.png.zombie_up_texture, &game.png.zombie_up_rect, &game.zombie[i].rect_dst);
			if (game.zombie[i].direction == down) SDL_RenderCopy(renderer, game.png.zombie_down_texture, &game.png.zombie_down_rect, &game.zombie[i].rect_dst);
			if (game.zombie[i].direction == right) SDL_RenderCopy(renderer, game.png.zombie_right_texture, &game.png.zombie_right_rect, &game.zombie[i].rect_dst);
			if (game.zombie[i].direction == left) SDL_RenderCopy(renderer, game.png.zombie_left_texture, &game.png.zombie_left_rect, &game.zombie[i].rect_dst);
		}
	}
}

void playLoop(Game& game)
{
	SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
	SDL_RenderClear(renderer);

	logicPlayer(game);
	logicBulletsForPistol(game);
	logicZombie(game);

	drawPlayer(game);
	drawBulletsForPistol(game);
	drawZombie(game);
}

#undef main
int main()
{
	hideConsole(false);

	srand(time(NULL));

	init();

	Game game;

	game.png.player_up_rect.w = game.player.w;
	game.png.player_down_rect.w = game.player.w;
	game.png.player_right_rect.w = game.player.w;
	game.png.player_left_rect.w = game.player.w;

	game.png.zombie_up_rect.w = game.zombie->w;
	game.png.zombie_down_rect.w = game.zombie->w;
	game.png.zombie_right_rect.w = game.zombie->w;
	game.png.zombie_left_rect.w = game.zombie->w;

	while (game.loop.launched)
	{
		game.time.nw = SDL_GetTicks();
		game.time.dlt = game.time.nw - game.time.lst;
		game.time.lst = game.time.nw;

		events(game);

		if (game.loop.menu) menuLoop(game);
		if (game.loop.info) infoLoop(game);
		if (game.loop.play) playLoop(game);

		SDL_RenderPresent(renderer);
		SDL_Delay(1000 / game.fps);
	}

	SDL_DestroyTexture(game.png.play_texture);
	SDL_DestroyTexture(game.png.info_texture);
	SDL_DestroyTexture(game.png.exit_texture);
	SDL_DestroyTexture(game.png.menu_texture);

	SDL_DestroyTexture(game.png.player_up_texture);
	SDL_DestroyTexture(game.png.player_down_texture);
	SDL_DestroyTexture(game.png.player_right_texture);
	SDL_DestroyTexture(game.png.player_left_texture);

	SDL_DestroyTexture(game.png.bullet_texture);

	SDL_DestroyTexture(game.png.zombie_up_texture);
	SDL_DestroyTexture(game.png.zombie_down_texture);
	SDL_DestroyTexture(game.png.zombie_right_texture);
	SDL_DestroyTexture(game.png.zombie_left_texture);

	deInit(0);
}