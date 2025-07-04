/**************************
 * Includes
 *
 **************************/
#if __cplusplus < 201103L
#error This program requires ISO C++11 or newer versions of C++!!
#endif
//#pragma optimize("O2")
#include <windows.h>
#include <gl/gl.h>
#include <map>
#include <random>
#include <cstring>
#include <chrono>
#include <thread>
#include <queue>
#include <my/font.h>
/**************************
 * Function Declarations
 *
 **************************/

LRESULT CALLBACK WndProc (HWND hWnd, UINT message,
                          WPARAM wParam, LPARAM lParam);
void EnableOpenGL (HWND hWnd, HDC *hDC, HGLRC *hRC);
void DisableOpenGL (HWND hWnd, HDC hDC, HGLRC hRC);




struct AABB {
	double minx,miny,maxx,maxy;
};
struct player {
	AABB ab;
	double x,y;
} Player;
enum cube_type {Air=1,Absolute_green=2};
struct cube {
	cube_type type;
};
#define MAX_HEIGHT 16
#define CHUNK_SIZE 16
struct chunk {
	cube cubes[CHUNK_SIZE][MAX_HEIGHT];
	bool generated;
	chunk() {
		generated = 0;
	}
};

std::map<long long,chunk> mp;

long long seed;
int canseechunk = 10;
float size = 0.1;
double oned, oneu;
std::mt19937 generator;
bool drwH=1, drwC=1;

WNDCLASS wc;
HWND hWnd;
HGLRC hRC;
MSG msg;
BOOL bQuit = FALSE;
HDC hDC;



inline long long gt(long long i) {
	return (i%CHUNK_SIZE+CHUNK_SIZE)%CHUNK_SIZE;
}

void setseed(long long seed_) {
	seed = seed_;
	generator = std::mt19937(seed);
}
void generatechunk(long long x) {
//	fout << "Generate, " << x << "\n";
//	fout.flush();
	for(long long i = x-canseechunk; i <= x+canseechunk; ++i) {

		if(mp[i].generated == 1) {
//			fout << "[Note] Chunk " << i << " has been loaded, skipped.\n";
		} else {
			for(int j=0; j<CHUNK_SIZE; ++j) {
				std::uniform_int_distribution<int> uniform_dist(1, MAX_HEIGHT-1);
				int height = uniform_dist(generator);
//				fout << "[Note] Heightof x=" << j+i*CHUNK_SIZE << ": " << height << "\n";
//				fout.flush();
//				int height = MAX_HEIGHT-1;
				for(int m=0; m<height; ++m) {
					mp[i].cubes[j][m].type=Absolute_green;
				}
				for(int m=height; m<MAX_HEIGHT; ++m) {
					mp[i].cubes[j][m].type=Air;
				}
			}
			mp[i].generated = 1;
			fout << "[Note] Generate of chunk, " << i << ", end\n";
		}
		fout << "[Note] Generate, " << x << ", end\n";
//		fout.flush();
	}
}
void draw(font::drawtype x,font::drawtype y) {
	long long px = (long long)(x),py = (long long)(y);
//	fout << time() << "[Note] Drawing " << px << ", " << py << ":" << std::endl;
	x -= Player.x;
	y -= Player.y;
	if(mp[px / CHUNK_SIZE].generated == 0) {
//		fout << time() << "[WARN] mp " << px / CHUNK_SIZE << " (from px=" << px << " )not found!\nDraw skipped!" << std::endl;
//		fout << "       Try to rescue... Generating..." << std::endl;
//		generatechunk(px / CHUNK_SIZE);
		return;
	}
	vec2 lu {size*(x-0.5f), size*(y+0.5f)},ru {size*(x+0.5f), size*(y+0.5f)},
	     ld {size*(x-0.5f), size*(y-0.5f)},rd {size*(x+0.5f), size*(y-0.5f)};
//	 fout  << time() << "[Note] Up_Left:    (" << lu.x << ", " << lu.y << "),\n"
//	      << "       Up_Right:   (" << ru.x << ", " << ru.y << "),\n"
//	     << "       Down_Left:  (" << ld.x << ", " << ld.y << "),\n"
//	     << "       Down_Right: (" << rd.x << ", " << rd.y << ")." << std::endl;
	if(!(out(lu)&&out(ru)&&out(ld)&&out(rd))) {
//     	fout << time() << "[Note] Type: " << mp[px / CHUNK_SIZE].cubes[gt(px)][gt(py)].type << std::endl;
		if(mp[px / CHUNK_SIZE].cubes[gt(px)][gt(py)].type == Absolute_green) {
//			fout.flush();
			glPushMatrix ();

			glBegin (GL_TRIANGLES);
			glColor3f (0.0f, 1.0f, 0.0f);
			glVertex2f (ld.x, ld.y);
			glColor3f (1.0f, 1.0f, 0.0f);
			glVertex2f (rd.x, rd.y);
			glColor3f (0.0f, 1.0f, 0.0f);
			glVertex2f (ru.x, ru.y);
			glEnd ();

			glPopMatrix ();
			glPushMatrix ();

			glBegin (GL_TRIANGLES);
			glColor3f (0.0f, 1.0f, 0.0f);
			glVertex2f (ld.x, ld.y);
			glColor3f (1.0f, 1.0f, 0.0f);
			glVertex2f (lu.x, lu.y);
			glColor3f (0.0f, 1.0f, 0.0f);
			glVertex2f (ru.x, ru.y);
			glEnd ();

			glPopMatrix ();
		} else if(mp[px / CHUNK_SIZE].cubes[gt(px)][gt(py)].type == Air) {
//			 fout << time() << "[Note] Air" << std::endl;
		} else {
//			 fout << time() << "[Warn] Not a block!!!!!!!!!!" << std::endl;
		}
	} else {
//		 fout << time() << "[Note] Skipped\n";
	}
//	 fout << time() << "[Note] Drawing " << px << ", " << py << " ended\n";
//	 fout.flush();
}
//void generatechunk(double x,double y=0){
//	int x0 = int(x),y0 = int(y);
//	for(int i = (x0/CHUNK_SIZE*CHUNK_SIZE)-1; i <= (x0/CHUNK_SIZE*CHUNK_SIZE)+1;++i){
//		for(int j=0;j<CHUNK_SIZE;++j){
////			std::mt19937 generator(seed & x0);
////			std::uniform_int_distribution<int> uniform_dist(0, MAX_HEIGHT);
////			int height = uniform_dist(generator);
//			int height = MAX_HEIGHT-1;
//			for(int m=0;m<height;++m){
//				mp[i].cubes[j][m].type=Absolute_green;
//			}
//			for(int m=height;m<MAX_HEIGHT;++m){
//				mp[i].cubes[j][m].type=Air;
//			}
//		}
//	}
//}
void drawchunk(long long x) {
//	chunk c = mp[x];
	fout<<x<<std::endl;
	if(mp[x].generated == 0) {
//		std::thread Gen(generatechunk, x);
//		Gen.detach();
		generatechunk(x) ;
	}
	for(int i=0; i<CHUNK_SIZE; ++i) {
		for(int j=0; j<MAX_HEIGHT; ++j) {
			draw(x*(font::drawtype)(1)*CHUNK_SIZE+i,j);
		}
	}
}
void drawvectors(double x,double y) {
	long long x0 = (long long)(x),y0 = (long long)(y);
	for(long long i = (x0/CHUNK_SIZE)-canseechunk; i <= (x0/CHUNK_SIZE)+canseechunk; ++i) {
//		for(int j = (y0/CHUNK_SIZE*CHUNK_SIZE)-calseechunk;j <= (x0/CHUNK_SIZE*CHUNK_SIZE)+canseechunk;++j){
//			if(j >= 0 && j <= MAX_HEIGHT / CHUNK_SIZE)
//	 			 drawchunk(i,j);
//		}
		drawchunk(i);
	}
	fout << time() << "[Note] drawvectors, " << x << ", " << y << ", ended." << std::endl;
}

struct debug {
	float x,y;
	std::string s;
};
std::vector<debug> v;

inline void Add_debug(float x,float y,std::string s) {
	v.push_back({x,y,s});
}
void drawall() {
	for(debug d : v) {
		drawstring(d.x,d.y,d.s,1,1);
	}
	v.clear();
}



const double wait = 1.0 / 20;
double draw_delay = 1.0 / 50, fps, sumfps, max_fps = 60;
std::queue<double> fpss;
const long long calsum = 50;
bool cal,drw;

void draw1tick() {
//	int count = 0;

	auto t1 = clock(), t2 = clock();

	glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
	glClear (GL_COLOR_BUFFER_BIT);
	Add_debug(-0.5f,0.9f,("X: " + std::to_string(Player.x)));
	Add_debug(-0.5f,0.85f,("Y: " + std::to_string(Player.y)));
	Add_debug(-0.5f,0.8f,("Upd: " + std::to_string(oneu) + " sec"));
	Add_debug(-0.5f,0.75f,("Drw: " + std::to_string(oned) + " sec"));
	Add_debug(-0.5f,0.7f,("     " + std::to_string(draw_delay) + " sec"));
	Add_debug(-0.5f,0.65f,(std::to_string((std::round(fps))) + " fps"));
	Add_debug(-0.5f,0.6f,(std::to_string((std::round(1.0 / oneu))) + " tps"));
	drawall();

	drawvectors(Player.x,Player.y);

	SwapBuffers (hDC);


	drw = 1;
	drwH = 1;

//	while(((t2 = clock()) - t1) *1.0/ CLOCKS_PER_SEC < 1 / max_fps) ;

//	drw = 1;

//	fout << time() << "[Note] draw1tick, ended." << std::endl;
}
/**************************
 * WinMain
 *
 **************************/

void addfps(double fps0) {
	if(fpss.size() > calsum) {
		sumfps += fps0;
		fpss.push(fps0);
		sumfps -= fpss.front();
		fpss.pop();
	} else {
		sumfps += fps0;
		fpss.push(fps0);
	}
	fps = 1 / sumfps * fpss.size();
//	fout << time() << "fps: " << fps << std::endl;
}
void cal1tick() {
	auto t1 = clock(), t2 = clock();

	// new...

//	double g = pow(2,-50) * Player.x;
//	Player.x += g*0.25;
	Player.x += pow(2,-5);

	do {
//		draw1tick();
//		addfps(oned = (clock() - t2)*1.0 / CLOCKS_PER_SEC);
	} while((t2 = clock()) - t1 + draw_delay * CLOCKS_PER_SEC <= wait);
	oneu = (clock() - t1)*1.0 / CLOCKS_PER_SEC;
	draw_delay += oneu - wait;
	cal = 1;
	drwC = 1;
//	fout << time() << "[Note] cal1tick, ended." << std::endl;
}
std::thread Drw(draw1tick);
std::thread Cal(cal1tick);
void cal_draw1tick() {
	if(drwH == 1) {
		drwH = 0;
		Drw.detach();
	}

}
void cal_cal1tick() {
	if(drwC == 1) {
		drwC = 0;
		Cal.detach();
	}

}
std::thread Cal_(cal_cal1tick);
void go1tick() {
	//?
//	std::thread Cal(cal1tick), Drw(draw1tick);
//	do{
//	    fout << time() << "[Note] go1tick: loop, with calH:" << calH << ", drwH: " << drwH << ", cal: " << cal << ", drw: " << drw << "." << std::endl;
//		if(!calH) {
//			if(drw){
//				drw = 0;
//				Drw.detach();
//				drwH=1;
//			}
//		}
//		if(!drwH) {
//			if(cal){
//				cal = 0;
//				Cal.detach();
//				calH=1;
//			}
//		}
//	}while(!(calH&&drwH));
	drwC = 1;
	Cal_.detach();
	do
		cal_draw1tick();
	while(drwC==0);
	fout << time() << "[Note] go1tick, ended." << std::endl;
}

int WINAPI WinMain (HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine,
                    int iCmdShow) {



	/*Player.x = pow(2,40),*/Player.y = MAX_HEIGHT;
	Player.x = 0;
	setseed(0);
	/* register window class */
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon (NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor (NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "GLSample";
	RegisterClass (&wc);

	/* create main window */
	hWnd = CreateWindow (
	           "GLSample", "OpenGL Sample",
	           WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE,
	           0, 0, 1024, 1024,
	           NULL, NULL, hInstance, NULL);

	/* enable OpenGL for the window */
	EnableOpenGL (hWnd, &hDC, &hRC);


	/* program main loop */
	font::init();
	long long g = 1,cnt = 0;
	while (/*!bQuit*/ true) {
		/* check for messages */
		if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE)) {
			/* handle or dispatch messages */
			if (msg.message == WM_QUIT) {
				bQuit = TRUE;
			} else {
				TranslateMessage (&msg);
				DispatchMessage (&msg);
			}
		} else {
			/* OpenGL animation code goes here */
//			std::thread T(draw1tick);
//			T.detach();
//			T.join();

//			draw1tick();
//			cal1tick();
			go1tick();
//			if(cnt>=1000) {
//				g *= 2;
//				Player.x = g;
//				cnt -= 1000;
//			}
//			go1tick();
//			cnt++;
//			std::this_thread::sleep_for(std::chrono::milliseconds(50));

//			Sleep (1);
		}
//		fout << "[Note] 1 round ended." << std::endl;
	}

	/* shutdown OpenGL */
	DisableOpenGL (hWnd, hDC, hRC);

	/* destroy the window explicitly */
	DestroyWindow (hWnd);

	return msg.wParam;
}


/********************
 * Window Procedure
 *
 ********************/

LRESULT CALLBACK WndProc (HWND hWnd, UINT message,
                          WPARAM wParam, LPARAM lParam) {

	switch (message) {
		case WM_CREATE:
			return 0;
		case WM_CLOSE:
			PostQuitMessage (0);
			return 0;

		case WM_DESTROY:
			return 0;

		case WM_KEYDOWN:
			switch (wParam) {
				case VK_ESCAPE:
					PostQuitMessage(0);
					return 0;
			}
			return 0;

		default:
			return DefWindowProc (hWnd, message, wParam, lParam);
	}
}


/*******************
 * Enable OpenGL
 *
 *******************/

void EnableOpenGL (HWND hWnd, HDC *hDC, HGLRC *hRC) {
	PIXELFORMATDESCRIPTOR pfd;
	int iFormat;

	/* get the device context (DC) */
	*hDC = GetDC (hWnd);

	/* set the pixel format for the DC */
	ZeroMemory (&pfd, sizeof (pfd));
	pfd.nSize = sizeof (pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW |
	              PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;
	iFormat = ChoosePixelFormat (*hDC, &pfd);
	SetPixelFormat (*hDC, iFormat, &pfd);

	/* create and enable the render context (RC) */
	*hRC = wglCreateContext( *hDC );
	wglMakeCurrent( *hDC, *hRC );

}


/******************
 * Disable OpenGL
 *
 ******************/

void DisableOpenGL (HWND hWnd, HDC hDC, HGLRC hRC) {
	wglMakeCurrent (NULL, NULL);
	wglDeleteContext (hRC);
	ReleaseDC (hWnd, hDC);
}
