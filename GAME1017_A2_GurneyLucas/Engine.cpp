#include "Engine.h"
#include "Utilities.h"
#include "TextureManager.h"
#include <iostream>
#include <ctime>
#define WIDTH 1024
#define HEIGHT 768
#define FPS 60
#define BGSCROLL 2 // Could these scroll/speed values be handled in the class? Yes. Consider it!
#define PSPEED 6
using namespace std;

Engine::Engine() :m_iESpawn(0), m_iESpawnMax(60), m_pivot({ 0,0 }), m_bRunning(false), m_gapCtr(0), m_gapMax(3)
{	
	cout << "Engine class constructed!" << endl; 
}
Engine::~Engine(){}

bool Engine::Init(const char* title, int xpos, int ypos, int width, int height, int flags)
{
	cout << "Initializing game." << endl;
	srand((unsigned)time(NULL));
	// Attempt to initialize SDL.
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		// Create the window.
		m_pWindow = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
		if (m_pWindow != nullptr) // Window init success.
		{
			m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, 0);
			if (m_pRenderer != nullptr) // Renderer init success.
			{
				m_pSprText = IMG_LoadTexture(m_pRenderer, "Img/sprites.png");
				TEMA::Init();
				TEMA::Load("BG.png", "bg");
				if (Mix_Init(MIX_INIT_MP3) != 0) // Mixer init success.
				{
					Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 2048);
					Mix_AllocateChannels(16);
					m_pMusic = Mix_LoadMUS("Aud/02 Green Hill Zone.mp3");
				}
				else return false;
			}
			else return false; // Renderer init fail.
		}
		else return false; // Window init fail.
	}
	else return false; // SDL init fail.
	m_fps = (Uint32)round((1 / (double)FPS) * 1000); // Sets FPS in milliseconds and rounds.
	m_iKeystates = SDL_GetKeyboardState(nullptr);
	srand((unsigned)time(NULL)); // Seed random number sequence.
	m_vecbg.reserve(9); // Canadian programming. Ensures push_back doesn't have to reallocated vector.
	// Create the vector now.
	for (int i = 0; i < 9; i++)
		m_vecbg.push_back(new Box({ 128 * i,384 }));
	// populate the map with prototype boxes.
	m_map.emplace(0, new Box({ 1024, 384 }, true, 1, { 1024, 384, 128, 128 }, { 255,0,0,255 }));
	m_map.emplace(1, new Box({ 1024, 384 }, true, 1, { 1024, 0, 128, 320 }, { 0,255,0,255 }));
	m_map.emplace(2, new Box({ 1024, 384 }, true, 1, { 1024, 384, 128, 64 }, { 0,255,255,255 }));
	m_map.emplace(3, new Box({ 1024, 384 }, true, 2));
	m_map[3]->Addspritebg(0, { 1024,384,128,128 }, { 255,0,0,255 });
	m_map[3]->Addspritebg(1, { 1024,0,128,192 }, { 0,0,255,255 });
	m_map.emplace(4, new Box({ 1024, 384 }, true, 2));
	m_map[4]->Addspritebg(1, { 1024,256,128,64 }, { 255,0,0,255 });
	m_map[4]->Addspritebg(1, { 1024,384,128,64 }, { 0,255,255,255 });
	// Populate the background vector.
	m_vec.reserve(10);
	m_vec.push_back(new Background({ 0,0,1024,768 }, { 0,0,1024,768 }, 1));
	m_vec.push_back(new Background({ 0,0,1024,768 }, { 1024,0,1024,768 }, 1));
	m_vec.push_back(new Background({ 1024,0,256,512 }, { 0,0,256,512 }, 3));
	m_vec.push_back(new Background({ 1024,0,256,512 }, { 256,0,256,512 }, 3));
	m_vec.push_back(new Background({ 1024,0,256,512 }, { 512,0,256,512 }, 3));
	m_vec.push_back(new Background({ 1024,0,256,512 }, { 768,0,256,512 }, 3));
	m_vec.push_back(new Background({ 1024,0,256,512 }, { 1024,0,256,512 }, 3));
	m_vec.push_back(new Background({ 1024,512,512,256 }, { 0,512,512,256 }, 4));
	m_vec.push_back(new Background({ 1024,512,512,256 }, { 512,512,512,256 }, 4));
	m_vec.push_back(new Background({ 1024,512,512,256 }, { 1024,512,512,256 }, 4));
	m_player = new Player( {0,0,94,100}, {256,384-50,94,100} );
	Mix_PlayMusic(m_pMusic, -1); // Play. -1 = looping.
	Mix_VolumeMusic(20); // 0-MIX_MAX_VOLUME (128).
	m_bRunning = true; // Everything is okay, start the engine.
	cout << "Success!" << endl;
	return true;
}

void Engine::Wake()
{
	m_start = SDL_GetTicks();
}

void Engine::Sleep()
{
	m_end = SDL_GetTicks();
	m_delta = m_end - m_start;
	if (m_delta < m_fps) // Engine has to sleep.
		SDL_Delay(m_fps - m_delta);
}

void Engine::HandleEvents()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT: // User pressed window's 'x' button.
			m_bRunning = false;
			break;
		case SDL_KEYDOWN: // Try SDL_KEYUP instead.
			if (event.key.keysym.sym == SDLK_ESCAPE)
				m_bRunning = false;
			break;
		case SDL_KEYUP:
			if (event.key.keysym.sym == SDLK_SPACE)
				m_bCanShoot = true;
			break;
		}
		
	}
}

// Keyboard utility function.
bool Engine::KeyDown(SDL_Scancode c)
{
	if (m_iKeystates != nullptr)
	{
		if (m_iKeystates[c] == 1)
			return true;
		else
			return false;
	}
	return false;
}

void Engine::CheckCollision()
{
	SDL_Rect p = { m_player->GetDstP()->x-100, m_player->GetDstP()->y, 100, 94 };
	for (int i = 0; i < (int)m_vEnemies.size(); i++)
	{
		SDL_Rect e = { m_vEnemies[i]->GetDstP()->x, m_vEnemies[i]->GetDstP()->y-40, 56, 40 };
		if (SDL_HasIntersection(&p, &e))
		{
			// Game over!
			cout << "Player goes boom!" << endl;
			Mix_PlayChannel(-1, m_vSounds[2], 0);
			break;
		}
	}
}


void Engine::Update()
{
	for (unsigned int i = 0; i < m_vec.size(); i++)
		m_vec[i]->Update();
	// Player animation/movement.
	m_player->Animate(); // Oh! We're telling the player to animate itself. This is good! Hint hint.
	if (KeyDown(SDL_SCANCODE_A) && m_player->GetDstP()->x > m_player->GetDstP()->h)
		m_player->GetDstP()->x -= PSPEED;
	else if (KeyDown(SDL_SCANCODE_D) && m_player->GetDstP()->x < WIDTH/2)
		m_player->GetDstP()->x += PSPEED;
	
	if (m_vecbg[0]->GetPosbg().x <= -128)
	{
		// Pop the first vector element/column off.
		delete m_vecbg[0]; // Deallocate Box via pointer.
		m_vecbg[0] = nullptr; // Optional wrangle.
		m_vecbg.erase(m_vecbg.begin()); // Destroys first element of vector.
		// Add a new Box element to the end.
		if (m_gapCtr++ % m_gapMax == 0)
		{
			m_vecbg.push_back(m_map[(rand() % 5)]->Clonebg());
		}
		else m_vecbg.push_back(new Box({ 1024,384 }, false));
	}
	for (unsigned int i = 0; i < m_vecbg.size(); i++)
		m_vecbg[i]->Updatebg();
}


void Engine::Render()
{
	
	SDL_RenderClear(m_pRenderer); // Clear the screen with the draw color.
	// Player.
	SDL_RenderCopyEx(m_pRenderer, m_pSprText, m_player->GetSrcP(), m_player->GetDstP(), m_player->GetAngle(), &m_pivot, SDL_FLIP_NONE );
	// Render stuff.
	for (unsigned int i = 0; i < m_vec.size(); i++)
		m_vec[i]->Render();

	for (unsigned int i = 0; i < m_vecbg.size(); i++)
		m_vecbg[i]->Renderbg();
	//render ground line.
	int x1 = 0, x2 = 1024, y1, y2;
	y1 = y2 = 448;
	  SDL_SetRenderDrawColor(m_pRenderer, 0, 128, 0, 255);
	SDL_RenderDrawLine(m_pRenderer, x2, y1, x2, y2);

	SDL_RenderPresent(m_pRenderer);
}

void Engine::Clean()
{
	cout << "Cleaning game." << endl;
	for (unsigned int i = 0; i < m_vecbg.size(); i++)
	{
		delete m_vecbg[i];
		m_vecbg[i] = nullptr;
	}
	m_vecbg.clear(); 
	m_vecbg.shrink_to_fit(); 
	delete m_player;
	m_player = nullptr;
	SDL_DestroyRenderer(m_pRenderer);
	SDL_DestroyWindow(m_pWindow);
	for (unsigned int i = 0; i < m_vec.size(); i++)
	{
		delete m_vec[i];
		m_vec[i] = nullptr;
	}
	m_vec.clear();
	m_vec.shrink_to_fit();
	for (int i = 0; i < (int)m_vSounds.size(); i++)
		Mix_FreeChunk(m_vSounds[i]);
	m_vSounds.clear();
	Mix_FreeMusic(m_pMusic);
	Mix_CloseAudio();
	TEMA::Quit();
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}

int Engine::Run()
{
	if (m_bRunning)
		return -1;
	if (Init("GAME1017 Shooter Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE) == false)
		return 1;
	while (m_bRunning)
	{
		Wake();
		HandleEvents();
		Update();
		Render();
		if (m_bRunning)
			Sleep();
	}
	Clean();
	return 0;
}

Engine& Engine::Instance()
{
	static Engine instance; 
	return instance;
}

SDL_Renderer* Engine::GetRenderer() { return m_pRenderer; }
