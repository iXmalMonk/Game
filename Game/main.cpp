#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<iostream>
#include<Windows.h>

#define WIDTH 1280
#define HEIGHT 1024

#define MAX_BULLETS_FOR_PISTOL 15
#define MAX_BULLETS_FOR_RIFLE 30
#define MAX_BULLETS_FOR_LASER 10
#define MAX_ZOMBIE 100

#define TEXT_SIZE 10

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

void hideConsole(bool flag)
{
	HWND hide_console;
	AllocConsole();
	hide_console = FindWindowA("ConsoleWindowClass", NULL);
	ShowWindow(hide_console, !flag);
}

#pragma region Structs

struct Loop {
	bool launched = true;
	bool menu = true;
	bool info = false;
	bool play = false;
	bool restart = false;
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

	SDL_Rect restart_rect;
	SDL_Texture* restart_texture = loadTextureFromFile("png/restart.png", &restart_rect);
	SDL_Rect restart_rect_dst = { WIDTH / 2 - restart_rect.w / 2, HEIGHT / 2 - restart_rect.h, restart_rect.w, restart_rect.h };

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

	SDL_Rect laser_bullet_rect;
	SDL_Texture* laser_bullet_texture = loadTextureFromFile("png/laser_bullet.png", &laser_bullet_rect);

	SDL_Rect zombie_up_rect;
	SDL_Texture* zombie_up_texture = loadTextureFromFile("png/zombie_up.png", &zombie_up_rect);

	SDL_Rect zombie_down_rect;
	SDL_Texture* zombie_down_texture = loadTextureFromFile("png/zombie_down.png", &zombie_down_rect);

	SDL_Rect zombie_right_rect;
	SDL_Texture* zombie_right_texture = loadTextureFromFile("png/zombie_right.png", &zombie_right_rect);

	SDL_Rect zombie_left_rect;
	SDL_Texture* zombie_left_texture = loadTextureFromFile("png/zombie_left.png", &zombie_left_rect);

	SDL_Rect pistol_rect;
	SDL_Texture* pistol_texture = loadTextureFromFile("png/pistol.png", &pistol_rect);
	SDL_Rect pistol_rect_dst = { 0, 0, 50, 25 };

	SDL_Rect rifle_rect;
	SDL_Texture* rifle_texture = loadTextureFromFile("png/rifle.png", &rifle_rect);
	SDL_Rect rifle_rect_dst = { 0, 0, 50, 25 };

	SDL_Rect laser_rect;
	SDL_Texture* laser_texture = loadTextureFromFile("png/laser.png", &laser_rect);
	SDL_Rect laser_rect_dst = { 0, 0, 50, 25 };

	SDL_Rect ininfo_rect;
	SDL_Texture* ininfo_texture = loadTextureFromFile("png/ininfo.png", &ininfo_rect);
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
	
	SDL_Rect rect_dst = { 0, 0, w, h };
	
	int frame = 0;
	int frame_count = 9;
	int frame_current = 0;
	int frame_max = 175;
	bool animate = false;
	
	int speed = 10;
	
	bool life = false;
	
	int max_hp = 100;
	int current_hp = 0;
};

struct Bullet {
	int w = 10;
	int h = 10;
	
	SDL_Rect rect_dst = { 0, 0, w, h };
	
	bool life = false;
	
	double alpha = 0;
};

struct Pistol {
	Bullet bullet[MAX_BULLETS_FOR_PISTOL];
	
	int speed = 25;
	
	int delay = 19;
	
	int damage = 25;

	bool flag = true;
};

struct Rifle {
	Bullet bullet[MAX_BULLETS_FOR_RIFLE];

	int speed = 30;

	int delay = 10;

	int damage = 35;
};

struct Laser {
	Bullet bullet[MAX_BULLETS_FOR_LASER];

	int speed = 45;
	
	int delay = 19;

	int damage = 25;

	bool flag = true;
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
	
	int max_hp = 100;
	int current_hp = 0;

	//int current = MAX_ZOMBIE / 10;
	int current = 4;

	float alpha = 0;

	int delay = 250;

	int damage = 10;

	int score = 1;
};

enum Weapon { pstl, rfl, lsr }; // pistol, rifle, laser

struct Bonus {
	int x = 0;
	int y = 0;
	int rand = 11;

	bool pistol = false;
	bool life_pistol = false;
	int time_life_pistol = 0;
	int time_life_max_pistol = 10000;
	int rand_index_pistol = 0;

	bool rifle = false;
	bool life_rifle = false;
	int time_life_rifle = 0;
	int time_life_max_rifle = 10000;
	int rand_index_rifle = 1;

	bool laser = false;
	bool life_laser = false;
	int time_life_laser = 0;
	int time_life_max_laser = 10000;
	int rand_index_laser = 2;
};

struct Score {
	int current_score = 0;
	int time_current_score = 0;

	int max_score = 0;
	int time_max_score = 0;

	int minutes = 0;
	int seconds = 0;

	int current_minutes = 0;
	int current_seconds = 0;
};

struct Game {
	Loop loop;

	Mouse mouse;

	Png png;

	Time time;

	Player player;

	Pistol pistol;

	Rifle rifle;

	Laser laser;

	Zombie zombie[MAX_ZOMBIE];

	Weapon weapon = pstl;

	Bonus bonus;

	Score score;

	SDL_Event event;

	const int fps = 60;

	TTF_Font* font = TTF_OpenFont("fonts/font.ttf", 25);
};

#pragma endregion

#pragma region Functions

int getTextSize(const char* text)
{
	int counter = 0;

	while (text[counter] != '\0') counter++;

	return counter;
}

void printValue(Game game, int value, int x, int y, int size)
{
	char text[TEXT_SIZE];

	_itoa_s(value, text, TEXT_SIZE, 10);

	SDL_Surface* text_surface = TTF_RenderText_Blended(game.font, text, { 0, 0, 0, 255 });

	SDL_Rect rect = { x, y, 0.75 * getTextSize(text) * size , size };

	SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);

	SDL_RenderCopy(renderer, text_texture, NULL, &rect);

	SDL_FreeSurface(text_surface);

	SDL_DestroyTexture(text_texture);
}

void printText(Game game, const char* text, int x, int y, int size)
{
	SDL_Surface* text_surface = TTF_RenderText_Blended(game.font, text, { 0, 0, 0, 255 });

	SDL_Rect rect = { x, y, 0.75 * getTextSize(text) * size, size };

	SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);

	SDL_RenderCopy(renderer, text_texture, NULL, &rect);

	SDL_FreeSurface(text_surface);

	SDL_DestroyTexture(text_texture);
}

void resetScore(Game& game)
{
	game.score.current_score = 0;
	game.score.time_current_score = 0;
}

void saveScore(Game& game)
{
	FILE* file;

	if (fopen_s(&file, "score.bin", "wt") != 0) exit(1);

	if (game.score.current_score > game.score.max_score) fwrite(&game.score.current_score, sizeof(int), 1, file);
	else fwrite(&game.score.max_score, sizeof(int), 1, file);
	if (game.score.time_current_score > game.score.time_max_score) fwrite(&game.score.time_current_score, sizeof(int), 1, file);
	else fwrite(&game.score.time_max_score, sizeof(int), 1, file);

	resetScore(game);

	fclose(file);
}

void loadScore(Game& game)
{
	FILE* file;

	if (fopen_s(&file, "score.bin", "rt") != 0) exit(1);

	fread(&game.score.max_score, sizeof(int), 1, file);
	fread(&game.score.time_max_score, sizeof(int), 1, file);

	fclose(file);
}

void timeScore(Game& game)
{
	static int time = 0;
	time += game.time.dlt;
	if (time >= 1000)
	{
		game.score.time_current_score += 1;
		//printf("%i\n", game.score.time_current_score);
		time = 0;
	}
}

void timeConvertation(Game& game)
{
	game.score.minutes = game.score.time_max_score / 60;
	game.score.seconds = game.score.time_max_score - game.score.minutes * 60;
}

void timeConvertationInRL(Game& game)
{
	game.score.current_minutes = game.score.time_current_score / 60;
	game.score.current_seconds = game.score.time_current_score - game.score.current_minutes * 60;
}

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
				(game.loop.info or game.loop.restart))
			{
				game.loop.info = false;
				game.loop.restart = false;
				game.loop.menu = true;
			}
			if ((game.png.restart_rect_dst.x + game.png.restart_rect_dst.w) >= game.mouse.x and game.mouse.x >= game.png.restart_rect_dst.x and
				(game.png.restart_rect_dst.y + game.png.restart_rect_dst.h) >= game.mouse.y and game.mouse.y >= game.png.restart_rect_dst.y and
				game.loop.restart)
			{
				game.loop.restart = false;
				game.loop.play = true;
			}
			if (game.loop.play)
			{
				game.mouse.left_button = true;
				game.pistol.flag = true;
				game.laser.flag = true;
			}
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

void createPlayer(Game& game)
{
	if (!game.player.life)
	{
		game.player.life = true;
		game.player.rect_dst.x = WIDTH / 2 - game.player.w / 2;
		game.player.rect_dst.y = HEIGHT / 2 - game.player.h / 2;
		game.player.current_hp = game.player.max_hp;
	}
}

void logicPlayer(Game& game)
{
	if (game.player.current_hp <= 0)
	{
		//printf("player died\n");
		game.player.life = false;
		
		// restart
		game.loop.play = false;
		game.loop.restart = true;
		// restart

		saveScore(game);

		for (int i = 0; i < game.zombie->current; i++) game.zombie[i].life = false; // zombie
	}

	if (game.player.life)
	{
		if (game.player.keyboard.up)
		{
			game.player.rect_dst.y -= game.player.speed;
			game.player.direction = up;
		}
		if (game.player.keyboard.down)
		{
			game.player.rect_dst.y += game.player.speed;
			game.player.direction = down;
		}
		if (game.player.keyboard.right)
		{
			game.player.rect_dst.x += game.player.speed;
			game.player.direction = right;
		}
		if (game.player.keyboard.left)
		{
			game.player.rect_dst.x -= game.player.speed;
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
	}
}

void drawPlayer(Game game)
{
	if (game.player.life)
	{
		if (game.player.direction == up) SDL_RenderCopy(renderer, game.png.player_up_texture, &game.png.player_up_rect, &game.player.rect_dst);
		if (game.player.direction == down) SDL_RenderCopy(renderer, game.png.player_down_texture, &game.png.player_down_rect, &game.player.rect_dst);
		if (game.player.direction == right) SDL_RenderCopy(renderer, game.png.player_right_texture, &game.png.player_right_rect, &game.player.rect_dst);
		if (game.player.direction == left) SDL_RenderCopy(renderer, game.png.player_left_texture, &game.png.player_left_rect, &game.player.rect_dst);
	}
}

void createBullets(Game& game)
{
	static int delay = 0;

	if (game.mouse.left_button and delay >= game.pistol.delay and game.pistol.flag and game.weapon == pstl)
	{
		for (int i = 0; i < MAX_BULLETS_FOR_PISTOL; i++)
			if (!game.pistol.bullet[i].life)
			{
				game.pistol.bullet[i].rect_dst.x = game.player.rect_dst.x + game.player.w / 2;
				game.pistol.bullet[i].rect_dst.y = game.player.rect_dst.y + game.player.h / 2;
				game.pistol.bullet[i].alpha = atan2(game.mouse.y - game.pistol.bullet[i].rect_dst.y, game.mouse.x - game.pistol.bullet[i].rect_dst.x);
				game.pistol.bullet[i].life = true;
				break;
			}
		delay = 0;
		game.pistol.flag = false;
	}

	if (game.mouse.left_button and delay >= game.rifle.delay and game.weapon == rfl)
	{
		for (int i = 0; i < MAX_BULLETS_FOR_RIFLE; i++)
			if (!game.rifle.bullet[i].life)
			{
				game.rifle.bullet[i].rect_dst.x = game.player.rect_dst.x + game.player.w / 2;
				game.rifle.bullet[i].rect_dst.y = game.player.rect_dst.y + game.player.h / 2;
				game.rifle.bullet[i].alpha = atan2(game.mouse.y - game.rifle.bullet[i].rect_dst.y, game.mouse.x - game.rifle.bullet[i].rect_dst.x);
				game.rifle.bullet[i].life = true;
				break;
			}
		delay = 0;
	}

	if (game.mouse.left_button and delay >= game.laser.delay and game.laser.flag and game.weapon == lsr)
	{
		for (int i = 0; i < MAX_BULLETS_FOR_LASER; i++)
			if (!game.laser.bullet[i].life)
			{
				game.laser.bullet[i].rect_dst.x = game.player.rect_dst.x + game.player.w / 2;
				game.laser.bullet[i].rect_dst.y = game.player.rect_dst.y + game.player.h / 2;
				game.laser.bullet[i].alpha = atan2(game.mouse.y - game.laser.bullet[i].rect_dst.y, game.mouse.x - game.laser.bullet[i].rect_dst.x);
				game.laser.bullet[i].life = true;
				break;
			}
		delay = 0;
		game.laser.flag = false;
	}

	if (delay >= game.pistol.delay + game.rifle.delay + game.laser.delay ) delay = game.pistol.delay + game.rifle.delay + game.laser.delay;
	else delay++;
}

void logicBullets(Game& game)
{
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
					//printf("hit - %i\n", game.zombie[j].current_hp);
					game.pistol.bullet[i].life = false;
					break;
				}
			}
			// zombie
		}
	}

	for (int i = 0; i < MAX_BULLETS_FOR_RIFLE; i++)
	{
		if (game.rifle.bullet[i].life)
		{
			game.rifle.bullet[i].rect_dst.x += cos(game.rifle.bullet[i].alpha) * game.rifle.speed;
			game.rifle.bullet[i].rect_dst.y += sin(game.rifle.bullet[i].alpha) * game.rifle.speed;

			if (game.rifle.bullet[i].rect_dst.x > WIDTH or game.rifle.bullet[i].rect_dst.y > HEIGHT or
				game.rifle.bullet[i].rect_dst.x + game.rifle.bullet->w < 0 or game.rifle.bullet[i].rect_dst.y + game.rifle.bullet->h < 0) game.rifle.bullet[i].life = false;

			// zombie
			for (int j = 0; j < game.zombie->current; j++)
			{
				if (game.rifle.bullet[i].rect_dst.x > game.zombie[j].rect_dst.x and game.rifle.bullet[i].rect_dst.x < game.zombie[j].rect_dst.x + game.zombie->w and
					game.rifle.bullet[i].rect_dst.y > game.zombie[j].rect_dst.y and game.rifle.bullet[i].rect_dst.y < game.zombie[j].rect_dst.y + game.zombie->h)
				{
					game.zombie[j].current_hp -= game.rifle.damage;
					//printf("hit - %i\n", game.zombie[j].current_hp);
					game.rifle.bullet[i].life = false;
					break;
				}
			}
			// zombie
		}
	}

	for (int i = 0; i < MAX_BULLETS_FOR_LASER; i++)
	{
		if (game.laser.bullet[i].life)
		{
			game.laser.bullet[i].rect_dst.x += cos(game.laser.bullet[i].alpha) * game.laser.speed;
			game.laser.bullet[i].rect_dst.y += sin(game.laser.bullet[i].alpha) * game.laser.speed;

			if (game.laser.bullet[i].rect_dst.x > WIDTH or game.laser.bullet[i].rect_dst.y > HEIGHT or
				game.laser.bullet[i].rect_dst.x + game.laser.bullet->w < 0 or game.rifle.bullet[i].rect_dst.y + game.laser.bullet->h < 0) game.laser.bullet[i].life = false;

			// zombie
			for (int j = 0; j < game.zombie->current; j++)
			{
				if (game.laser.bullet[i].rect_dst.x > game.zombie[j].rect_dst.x and game.laser.bullet[i].rect_dst.x < game.zombie[j].rect_dst.x + game.zombie->w and
					game.laser.bullet[i].rect_dst.y > game.zombie[j].rect_dst.y and game.laser.bullet[i].rect_dst.y < game.zombie[j].rect_dst.y + game.zombie->h)
				{
					game.zombie[j].current_hp -= game.laser.damage;
					//printf("hit - %i\n", game.zombie[j].current_hp);
				}
			}
			// zombie
		}
	}
}

void drawBullets(Game game)
{
	for (int i = 0; i < MAX_BULLETS_FOR_PISTOL; i++)
		if (game.pistol.bullet[i].life) SDL_RenderCopy(renderer, game.png.bullet_texture, &game.png.bullet_rect, &game.pistol.bullet[i].rect_dst);

	for (int i = 0; i < MAX_BULLETS_FOR_RIFLE; i++)
		if (game.rifle.bullet[i].life) SDL_RenderCopy(renderer, game.png.bullet_texture, &game.png.bullet_rect, &game.rifle.bullet[i].rect_dst);

	for (int i = 0; i < MAX_BULLETS_FOR_LASER; i++)
		if (game.laser.bullet[i].life) SDL_RenderCopy(renderer, game.png.laser_bullet_texture, &game.png.laser_bullet_rect, &game.laser.bullet[i].rect_dst);
}

void createZombie(Game& game)
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
}

void logicZombie(Game& game)
{	
	static int delay = 0;

	for (int i = 0; i < game.zombie->current; i++)
	{
		if (game.zombie[i].current_hp <= 0)
		{
			//printf("zombie died\n");
			game.zombie[i].life = false;

			// score
			game.score.current_score += game.zombie->score;
			//printf("%i\n", game.score.current_score);
			// score
			
			// bonus
			if (rand() % game.bonus.rand == game.bonus.rand_index_pistol and !game.bonus.life_pistol)
			{
				game.bonus.pistol = true;
				game.bonus.x = game.zombie[i].rect_dst.x;
				game.bonus.y = game.zombie[i].rect_dst.y;
			}
			else if (rand() % game.bonus.rand == game.bonus.rand_index_rifle and !game.bonus.life_rifle)
			{
				game.bonus.rifle = true;
				game.bonus.x = game.zombie[i].rect_dst.x;
				game.bonus.y = game.zombie[i].rect_dst.y;
			}
			else if (rand() % game.bonus.rand == game.bonus.rand_index_laser and !game.bonus.life_laser)
			{
				game.bonus.laser = true;
				game.bonus.x = game.zombie[i].rect_dst.x;
				game.bonus.y = game.zombie[i].rect_dst.y;
			}
			// bonus
		}

		if (game.zombie[i].life)
		{
			if (game.zombie[i].rect_dst.y > game.player.rect_dst.y + game.player.h / 2 or
				game.zombie[i].rect_dst.y + game.zombie->h / 2 < game.player.rect_dst.y or
				game.zombie[i].rect_dst.x > game.player.rect_dst.x + game.player.w / 2 or
				game.zombie[i].rect_dst.x + game.zombie->w / 2 < game.player.rect_dst.x)
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
			// player
			else if (delay == game.zombie->delay)
			{
				game.player.current_hp -= game.zombie->damage;
				//printf("%i\n", game.player.current_hp);
				delay = 0;
			}
			// player

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

		if (delay >= game.zombie->delay) delay = game.zombie->delay;
		else delay++;
	}
}

void drawZombie(Game game)
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

void createBonus(Game& game)
{
	if (game.bonus.pistol and !game.bonus.life_pistol)
	{
		game.png.pistol_rect_dst.x = game.bonus.x;
		game.png.pistol_rect_dst.y = game.bonus.y;
		game.bonus.life_pistol = true;
		game.bonus.pistol = false;
	}

	if (game.bonus.rifle and !game.bonus.life_rifle)
	{
		game.png.rifle_rect_dst.x = game.bonus.x;
		game.png.rifle_rect_dst.y = game.bonus.y;
		game.bonus.life_rifle = true;
		game.bonus.rifle = false;
	}

	if (game.bonus.laser and !game.bonus.life_laser)
	{
		game.png.laser_rect_dst.x = game.bonus.x;
		game.png.laser_rect_dst.y = game.bonus.y;
		game.bonus.life_laser = true;
		game.bonus.laser = false;
	}
}

void logicBonus(Game& game)
{
	if (game.bonus.life_pistol)
	{
		game.bonus.time_life_pistol += game.time.dlt;

		if (game.player.rect_dst.x + game.player.rect_dst.w >= game.png.pistol_rect_dst.x and game.png.pistol_rect_dst.x >= game.player.rect_dst.x and
			game.player.rect_dst.y + game.player.rect_dst.h >= game.png.pistol_rect_dst.y and game.png.pistol_rect_dst.y >= game.player.rect_dst.y)
		{
			game.weapon = pstl;
			game.bonus.life_pistol = false;
			game.bonus.time_life_pistol = 0;
		}

		if (game.bonus.time_life_pistol >= game.bonus.time_life_max_pistol)
		{
			game.bonus.life_pistol = false;
			game.bonus.time_life_pistol = 0;
		}
	}

	if (game.bonus.life_rifle)
	{
		game.bonus.time_life_rifle += game.time.dlt;

		if (game.player.rect_dst.x + game.player.rect_dst.w >= game.png.rifle_rect_dst.x and game.png.rifle_rect_dst.x >= game.player.rect_dst.x and
			game.player.rect_dst.y + game.player.rect_dst.h >= game.png.rifle_rect_dst.y and game.png.rifle_rect_dst.y >= game.player.rect_dst.y)
		{
			game.weapon = rfl;
			game.bonus.life_rifle = false;
			game.bonus.time_life_rifle = 0;
		}

		if (game.bonus.time_life_rifle >= game.bonus.time_life_max_rifle)
		{
			game.bonus.life_rifle = false;
			game.bonus.time_life_rifle = 0;
		}
	}

	if (game.bonus.life_laser)
	{
		game.bonus.time_life_laser += game.time.dlt;

		if (game.player.rect_dst.x + game.player.rect_dst.w >= game.png.laser_rect_dst.x and game.png.laser_rect_dst.x >= game.player.rect_dst.x and
			game.player.rect_dst.y + game.player.rect_dst.h >= game.png.laser_rect_dst.y and game.png.laser_rect_dst.y >= game.player.rect_dst.y)
		{
			game.weapon = lsr;
			game.bonus.life_laser = false;
			game.bonus.time_life_laser = 0;
		}

		if (game.bonus.time_life_laser >= game.bonus.time_life_max_laser)
		{
			game.bonus.life_laser = false;
			game.bonus.time_life_laser = 0;
		}
	}
}

void drawBonus(Game game)
{
	if (game.bonus.life_pistol) SDL_RenderCopy(renderer, game.png.pistol_texture, &game.png.pistol_rect, &game.png.pistol_rect_dst);
	if (game.bonus.life_rifle) SDL_RenderCopy(renderer, game.png.rifle_texture, &game.png.rifle_rect, &game.png.rifle_rect_dst);
	if (game.bonus.life_laser) SDL_RenderCopy(renderer, game.png.laser_texture, &game.png.laser_rect, &game.png.laser_rect_dst);
}

void menuLoop(Game game)
{
	SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
	SDL_RenderClear(renderer);

	SDL_RenderCopy(renderer, game.png.play_texture, &game.png.play_rect, &game.png.play_rect_dst);
	SDL_RenderCopy(renderer, game.png.info_texture, &game.png.info_rect, &game.png.info_rect_dst);
	SDL_RenderCopy(renderer, game.png.exit_texture, &game.png.exit_rect, &game.png.exit_rect_dst);

	printText(game, "Max score:", 25, HEIGHT - 50, 25);
	printValue(game, game.score.max_score, 225, HEIGHT - 50, 25);

	printText(game, "Max time", 25, HEIGHT - 200, 25);

	printText(game, "Minutes:", 25, HEIGHT - 150, 25);
	printValue(game, game.score.minutes, 225, HEIGHT - 150, 25);

	printText(game, "Seconds:", 25, HEIGHT - 100, 25);
	printValue(game, game.score.seconds, 225, HEIGHT - 100, 25);
}

void infoLoop(Game game)
{
	SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
	SDL_RenderClear(renderer);

	SDL_RenderCopy(renderer, game.png.menu_texture, &game.png.menu_rect, &game.png.menu_rect_dst);
	SDL_RenderCopy(renderer, game.png.ininfo_texture, &game.png.ininfo_rect, NULL);
}

void playLoop(Game& game)
{
	SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
	SDL_RenderClear(renderer);

	createPlayer(game);
	logicPlayer(game);

	createBullets(game);
	logicBullets(game);

	createZombie(game);
	logicZombie(game);

	createBonus(game);
	logicBonus(game);

	drawPlayer(game);

	drawBullets(game);

	drawZombie(game);

	drawBonus(game);

	printText(game, "HP:", 25, HEIGHT - 50, 25);
	printValue(game, game.player.current_hp, 100, HEIGHT - 50, 25);

	printText(game, "Weapon:", 175, HEIGHT - 50, 25);
	if (game.weapon == pstl) printText(game, "pistol", 325, HEIGHT - 50, 25);
	if (game.weapon == rfl) printText(game, "rifle", 325, HEIGHT - 50, 25);
	if (game.weapon == lsr) printText(game, "laser", 325, HEIGHT - 50, 25);

	printText(game, "Score:", 450, HEIGHT - 50, 25);
	printValue(game, game.score.current_score, 575, HEIGHT - 50, 25);

	printText(game, "Time - M:     S:", 700, HEIGHT - 50, 25);
	printValue(game, game.score.current_minutes, 900, HEIGHT - 50, 25);
	printValue(game, game.score.current_seconds, 1000, HEIGHT - 50, 25);

	timeScore(game);
	timeConvertationInRL(game);
}

void restartLoop(Game game)
{
	SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
	SDL_RenderClear(renderer);

	SDL_RenderCopy(renderer, game.png.restart_texture, &game.png.restart_rect, &game.png.restart_rect_dst);
	SDL_RenderCopy(renderer, game.png.menu_texture, &game.png.menu_rect, &game.png.menu_rect_dst);
}

void loop(Game& game)
{
	if (game.loop.menu) menuLoop(game);
	if (game.loop.info) infoLoop(game);
	if (game.loop.play) playLoop(game);
	if (game.loop.restart) restartLoop(game);
}

void time(Game& game)
{
	game.time.nw = SDL_GetTicks();
	game.time.dlt = game.time.nw - game.time.lst;
	game.time.lst = game.time.nw;
}

void pngRect(Game& game)
{
	game.png.player_up_rect.w = game.player.w;
	game.png.player_down_rect.w = game.player.w;
	game.png.player_right_rect.w = game.player.w;
	game.png.player_left_rect.w = game.player.w;

	game.png.zombie_up_rect.w = game.zombie->w;
	game.png.zombie_down_rect.w = game.zombie->w;
	game.png.zombie_right_rect.w = game.zombie->w;
	game.png.zombie_left_rect.w = game.zombie->w;
}

void destroyTexture(Game& game)
{
	SDL_DestroyTexture(game.png.play_texture);
	SDL_DestroyTexture(game.png.info_texture);
	SDL_DestroyTexture(game.png.exit_texture);
	SDL_DestroyTexture(game.png.menu_texture);
	SDL_DestroyTexture(game.png.restart_texture);

	SDL_DestroyTexture(game.png.player_up_texture);
	SDL_DestroyTexture(game.png.player_down_texture);
	SDL_DestroyTexture(game.png.player_right_texture);
	SDL_DestroyTexture(game.png.player_left_texture);

	SDL_DestroyTexture(game.png.bullet_texture);
	SDL_DestroyTexture(game.png.laser_bullet_texture);

	SDL_DestroyTexture(game.png.zombie_up_texture);
	SDL_DestroyTexture(game.png.zombie_down_texture);
	SDL_DestroyTexture(game.png.zombie_right_texture);
	SDL_DestroyTexture(game.png.zombie_left_texture);

	SDL_DestroyTexture(game.png.pistol_texture);
	SDL_DestroyTexture(game.png.rifle_texture);
	SDL_DestroyTexture(game.png.laser_texture);

	SDL_DestroyTexture(game.png.ininfo_texture);
}

#pragma endregion

#undef main
int main()
{
	hideConsole(true);

	srand(time(NULL));

	init();

	Game game;

	pngRect(game);

	loadScore(game);

	timeConvertation(game);

	//printf("Score - %i\nTime - M: %i S: %i\n", game.score.max_score, game.score.minutes, game.score.seconds);

	while (game.loop.launched)
	{
		time(game);

		events(game);

		loop(game);

		SDL_RenderPresent(renderer);
		SDL_Delay(1000 / game.fps);
	}

	saveScore(game);

	destroyTexture(game);

	deInit(0);
}