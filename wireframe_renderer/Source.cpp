#include <string>
#include <iostream>
#include <math.h>
#include <cmath>
#include <SDL.h>
#include <stdio.h>
#include "PiImpactphys.h"

using namespace std;

//Screen dimension constants
const int SCREEN_WIDTH = 512;
const int SCREEN_HEIGHT = 512;


//The window we'll be rendering to
SDL_Window* window = NULL;

//The surface contained by the window
SDL_Surface* screenSurface = NULL;

SDL_Event event;
SDL_Renderer* renderer;

bool render_lines = true;
bool hilight_vertex = true;
int vertex_is_hil = 0;

struct Vector3
{
	float x;
	float y;
	float z;

	void print() {
		std::cout << " Vetex data: X: " + to_string(x) + " Y: " + to_string(y) + " Z: " + to_string(z) + " END\n";
	};
	void set(float x, float y, float z) {
		Vector3::x = x; Vector3::y = y; Vector3::z = z;
	}
	Vector3 add(Vector3 one) {
		Vector3 temp;
		temp.set(Vector3::x + one.x, Vector3::y + one.y, Vector3::z + one.z);
		return temp;
	}
	Vector3 subtract(Vector3 one) {
		Vector3 temp;
		temp.set(Vector3::x - one.x, Vector3::y - one.y, Vector3::z - one.z);
		return temp;
	}
}; 


struct Vector3_int
{
	int x;
	int y;
	int z;

	void print() {
		cout << " Vetex data: X: " + to_string(x) + " Y: " + to_string(y) + " Z: " + to_string(z) + " END\n";
	};

	void Set_from_float_v3(Vector3 float_v3) {
		Vector3_int::x = float_v3.x;
		Vector3_int::y = float_v3.y;
		Vector3_int::z = float_v3.z;
	}
};


struct cam {
	Vector3 pos;
	int fov;

};


//Returns rotated point, (old point, Angal of rotation(degres))
Vector3 Rotate_point(Vector3 point, Vector3 ang) {
	Vector3 point_temp;
	Vector3 point_temp_2;
	//x rot
	if (ang.x != 0) {
	point_temp.x = point.x;
	point_temp.y = point.y * cos(ang.x * (M_PI/180)) - (point.z * sin(ang.x * (M_PI / 180)));
	point_temp.z = point.y * sin(ang.x * (M_PI / 180)) + point.z * cos(ang.x * (M_PI / 180));
	//cout << " Vetex data Rot x: X: " + to_string(point_temp.x) + " Y: " + to_string(point_temp.y) + " Z: " + to_string(point_temp.z) + " END\n";
	}
	else
	{
		point_temp.x = point.x;
		point_temp.y = point.y;
		point_temp.z = point.z;
		//cout << " Vetex data Rot x: X: " + to_string(point_temp.x) + " Y: " + to_string(point_temp.y) + " Z: " + to_string(point_temp.z) + " END\n";
		//std::cout << "wow";
	}
	//y rot
	if (ang.y != 0) {
		point_temp_2.x = (point_temp.z * sin(ang.y * (M_PI / 180))) + (point_temp.x * cos(ang.y * (M_PI / 180)));
		point_temp_2.y = point_temp.y;
		point_temp_2.z = (point_temp.y * cos(ang.y * (M_PI / 180))) - (point_temp.x * sin(ang.y * (M_PI / 180)));
		//cout << " Vetex data Rot y: X: " + to_string(point_temp_2.x) + " Y: " + to_string(point_temp_2.y) + " Z: " + to_string(point_temp_2.z) + " END\n";
	}
	else
	{
		point_temp_2.x = point_temp.x;
		point_temp_2.y = point_temp.y;
		point_temp_2.z = point_temp.z;
		//cout << " Vetex data Rot y: X: " + to_string(point_temp_2.x) + " Y: " + to_string(point_temp_2.y) + " Z: " + to_string(point_temp_2.z) + " END\n";
		//std::cout << "wow2";
	}
	//z rot
	if (ang.z != 0) {
		point_temp.x = (point_temp_2.x * cos(ang.z * (M_PI / 180))) - (point_temp_2.y * sin(ang.z * (M_PI / 180)));
		point_temp.y = (point_temp_2.x * sin(ang.z * (M_PI / 180))) + (point_temp_2.y * cos(ang.z * (M_PI / 180)));
		point_temp.z = point_temp_2.z;
		//cout << " Vetex data Rot z: X: " + to_string(point_temp.x) + " Y: " + to_string(point_temp.y) + " Z: " + to_string(point_temp.z) + " END\n";
	}
	else
	{
		point_temp.x = point_temp_2.x;
		point_temp.y = point_temp_2.y;
		point_temp.z = point_temp_2.z;
		//std::cout << "wow3";
		//cout << " Vetex data Rot z: X: " + to_string(point_temp.x) + " Y: " + to_string(point_temp.y) + " Z: " + to_string(point_temp.z) + " END\n";
	}
	return point_temp;
}

Vector3 project_point(Vector3 point, float Focal_lenth, int object_size_multiplyer, Vector3 pointofset, Vector3 rot) {
	Vector3 Projected_point_var;
	Vector3 temppoint;
	//temppoint = point;
	temppoint = point.subtract(pointofset);
	temppoint = Rotate_point(temppoint, rot);
	Projected_point_var.x = (Focal_lenth * ((temppoint.x) * object_size_multiplyer)) / (Focal_lenth + ((temppoint.z)));
	Projected_point_var.y = (Focal_lenth * ((temppoint.y) * object_size_multiplyer)) / (Focal_lenth + ((temppoint.z)));
	return Projected_point_var;
}

int vertex_count = 7;
float Vertex_table[24] = { 64, 64, 64, -64, -64, 64, -64, 64, -64, -64, 64, 64, 64, 64, -64, -64, -64, -64, 64, -64, -64, 64, -64, 64 };


Vector3 Vertex_table_baked[500];

int line_count = 17;
int LineTable[40] = { 7, 0, 7, 1, 7, 2, 3, 1, 3, 0, 3, 6, 5, 6, 5, 0, 5, 2, 4, 1, 4, 2, 4, 6, 7, 4, 7, 5, 7, 3, 6, 0, 6, 1, 0, 1 };

int Tri_count = 1;
int TriTable[50] = { 1, 2, 17 };

char screenbuffer[31][17];
int screenx = 32;
int screeny = 18;
int fov = -5;

Vector3 campos;
Vector3 camrot;

Uint32 start = 0;


void render_to_buffer(char Buffer[], int screenx, int screeny, Vector3 offset, int z_farplane) {
	//vertexes
	start = SDL_GetTicks();
	for (int i = 0; i <= vertex_count && ((offset.z + Vertex_table[2 + (i * 3)]) < z_farplane); i++)
	{
		Vector3 temp;
		temp.x = (offset.x + Vertex_table[0 + (i*3)]);
		temp.y = (offset.y + Vertex_table[1 + (i*3)]);
		temp.z = (offset.z + Vertex_table[2 + (i*3)]);
		
		Vertex_table_baked[i] = (project_point(temp, fov, 40, campos, camrot));
		// + to_string(Vertex_table_baked[i].y) + to_string(Vertex_table_baked[i].z)
		//std::cout << " X: " + std::to_string(Vertex_table_baked[i].x) + " Y: " + std::to_string(Vertex_table_baked[i].y) + " Z: " + std::to_string(Vertex_table_baked[i].z) + "\n";
		int xtemp = round(Vertex_table_baked[i].x + (screenx/2));
		int ytemp = round(Vertex_table_baked[i].y + (screeny / 2));

		//Vertex_table_baked[i].print();
		//cout << " Vetex data 2d: X: " + to_string(round(Vertex_table_baked[i].x)) + " Y: " + to_string(round(Vertex_table_baked[i].y)) + " END\n";

		if (xtemp >= screenx || ytemp >= screeny) {
			xtemp = 0;
			ytemp = 0;
		}
		//screenbuffer[xtemp][ytemp] = '1';
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
		SDL_RenderDrawPoint(renderer, xtemp, ytemp);
		
		xtemp = NULL;
		ytemp = NULL;
		temp.x = NULL;
		temp.y = NULL;
		temp.z = NULL;
		//Buffer = NULL;
		//screenx = NULL;
		//screeny = NULL;
	}
	//lines
	/*
	for (int i = 0; i <= line_count; i++){

		int dx = Vertex_table_baked[LineTable[(i*2) + 1]].x - Vertex_table_baked[LineTable[(i * 2)]].x;
		int dy = Vertex_table_baked[LineTable[(i * 2) + 1]].y - Vertex_table_baked[LineTable[(i * 2)]].y;
		//cout << " DX: " + to_string(dx) + " DY: " + to_string(dy) + " END\n";
		float xmin;
		float xmax;

		if (Vertex_table_baked[LineTable[(i * 2)]].x < Vertex_table_baked[LineTable[(i * 2) + 1]].x) {
			xmin = Vertex_table_baked[LineTable[(i * 2)]].x;
			xmax = Vertex_table_baked[LineTable[(i * 2) + 1]].x;
		}
		else {
			xmin = Vertex_table_baked[LineTable[(i * 2) + 1]].x;
			xmax = Vertex_table_baked[LineTable[(i * 2)]].x;
		}
		float slope = 0;
		if (dx != 0) {
			slope = dy / dx;
		}
		for (float x = xmin; x <= xmax; x += 1.0f) {
			float y = Vertex_table_baked[LineTable[(i * 2)]].y + ((x - Vertex_table_baked[LineTable[(i * 2)]].x) * slope);
			//Color color = color1 + ((color2 - color1) * ((x - x1) / xdiff));
			int xi=round(x + (screenx / 2));
			int yi= round(y + (screeny / 2));
			//screenbuffer[xi][yi] = '1';
			if (render_lines == true) {
				SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
				SDL_RenderDrawPoint(renderer, xi, yi);
			}
		}
	}
	*/
	
	for (int i = 0; i <= line_count; i++) {
		if (render_lines == true) {
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderDrawLine(renderer, Vertex_table_baked[LineTable[(i * 2) + 1]].x + (screenx / 2), Vertex_table_baked[LineTable[(i * 2) + 1]].y + (screeny / 2), Vertex_table_baked[LineTable[(i * 2)]].x + (screenx / 2), Vertex_table_baked[LineTable[(i * 2)]].y + (screeny / 2));
		}
	}
	if (hilight_vertex == true) {
		int xtemp = round(Vertex_table_baked[vertex_is_hil].x + (screenx / 2));
		int ytemp = round(Vertex_table_baked[vertex_is_hil].y + (screeny / 2));

		//Vertex_table_baked[i].print();
		//cout << " Vetex data 2d: X: " + to_string(round(Vertex_table_baked[i].x)) + " Y: " + to_string(round(Vertex_table_baked[i].y)) + " END\n";

		if (xtemp >= screenx || ytemp >= screeny) {
			xtemp = 0;
			ytemp = 0;
		}
		//screenbuffer[xtemp][ytemp] = '1';
		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
		SDL_RenderDrawPoint(renderer, xtemp, ytemp);
		SDL_RenderDrawPoint(renderer, xtemp + 1, ytemp);
		SDL_RenderDrawPoint(renderer, xtemp, ytemp+1);
		SDL_RenderDrawPoint(renderer, xtemp-1, ytemp);
		SDL_RenderDrawPoint(renderer, xtemp, ytemp-1);

		for (int i = 0; i <= Tri_count; i++) {
			for (int i = 0; i <= 3; i++) {
				int tablelinetemp[5];
				tablelinetemp[0] = LineTable[TriTable[(Tri_count * 3)]];
				tablelinetemp[1] = LineTable[TriTable[(Tri_count * 3) + 1]];
				tablelinetemp[2] = LineTable[TriTable[(Tri_count * 3) + 2]];
				tablelinetemp[3] = LineTable[TriTable[(Tri_count * 3)] + 1];
				tablelinetemp[4] = LineTable[TriTable[(Tri_count * 3) + 1] + 2];
				tablelinetemp[5] = LineTable[TriTable[(Tri_count * 3) + 2] + 3];


				//SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
				//SDL_RenderDrawLine(renderer, Vertex_table_baked[LineTable[(i * 2) + 1]].x + (screenx / 2), Vertex_table_baked[LineTable[(i * 2) + 1]].y + (screeny / 2), Vertex_table_baked[LineTable[(i * 2)]].x + (screenx / 2), Vertex_table_baked[LineTable[(i * 2)]].y + (screeny / 2));
			}
		}
	}

	cout << "Render time Ms:" << SDL_GetTicks() - start << "  ";

}
bool setup = true;
Vector3 worldOfset;

void run(bool quit, SDL_Event e) {

	/*
	for (int d = 0; d <= screeny; d++) {
		for (int l = 0; l <= screenx; l++) {
			screenbuffer[l][d] = 32;
		}
	}
	*/

	setup = false;

	//worldOfset.x -= 0.001;
	//worldOfset.y -= 0.001;
	//worldOfset.z -= 0.5;

	render_to_buffer(screenbuffer[0], SCREEN_WIDTH, SCREEN_HEIGHT, worldOfset, 200);
	//std::cout << "render" << worldOfset.x << worldOfset.y << worldOfset.z;
	/*
	for (int d = 0; d <= screeny; d++) {
		for (int l = 0; l <= screenx; l++) {
			cout << screenbuffer[l][d];
			//cout << "0";
			if (l == screenx) {
				cout << "\n";

			}
		}
	}
	*/
	SDL_RenderPresent(renderer);
	//console
	std::string consoleinput;
	//consoleinput << std::cin;
	//getline(cin, consoleinput, '\n');
	std::cin >> consoleinput;
	std::cout << "input: " + consoleinput + "\n";
	if (consoleinput == "render_lines_true") {
		render_lines = true;
		std::cout << "render_lines is true\n";
	}
	else if (consoleinput == "render_lines_false") {
		render_lines = false;
		std::cout << "render_lines is false\n";
	}
	else if (consoleinput == "Toggel_vertex_hilight") {
		if (hilight_vertex == true) { hilight_vertex = false; }
		else { hilight_vertex = true; }
		std::cout << "Vertex hilight is " + std::to_string(hilight_vertex);
	}
	else if (consoleinput == "set_vertex_hilight") {
		std::cout << "set hilighted vertex to: ";
		cin >> vertex_is_hil;
		std::cout << "\nhilighted vertex is set to : " + std::to_string(vertex_is_hil) + "\n";
	}
	else if (consoleinput == "set_fov") {
		std::cout << "set fov to: "; cin >> fov;
		std::cout << "fov set\n";
	}
	else if (consoleinput == "set_vertex_count") {
		std::cout << "set vertex count to: "; cin >> vertex_count;
		std::cout << "vertex count set\n";
	}
	else if (consoleinput == "set_line_count") {
		std::cout << "set line count to: "; cin >> line_count;
		std::cout << "line count set\n";
	}
	else if (consoleinput == "set_line_buffer") {
		int tempplase;
		std::cout << "line: "; cin >> tempplase;
		std::cout << "\nset line to: "; cin >> LineTable[tempplase];
		std::cout << "\nline set\n";
	}
	else if (consoleinput == "set_vertex_buffer") {
		int tempplase;
		std::cout << "vetex: "; cin >> tempplase;
		std::cout << "\nset vetex to: "; cin >> Vertex_table[tempplase];
		std::cout << "\nvertex set\n";
	}
	//campos WIP
	else if (consoleinput == "set_cam_pos") {
		int x;
		int y;
		int z;
		cin >> x;
		cin >> y;
		cin >> z;
		campos.set(x, y, z);
		cout << campos.x;
		cout << campos.y;
		cout << campos.z;
		//std::cout << "\nset vetex to: "; cin >> Vertex_table[tempplase];
		std::cout << "\npos set\n";
	}
	else if (consoleinput == "set_cam_rot") {
		int x;
		int y;
		int z;
		cin >> x;
		cin >> y;
		cin >> z;
		camrot.set(x, y, z);
		//std::cout << "\nset vetex to: "; cin >> Vertex_table[tempplase];
		std::cout << "\nrot set\n";
	}
	else if (consoleinput == "")
	{
		
	}
	else
	{
		//std::cout << "comand uknown\n";
	}
}

int main(int argc, char* args[]) {
	Vector3 point;
	Vector3 ang;
	point.set(1, 2, 3);
	ang.set(0, 0, 90);
	std::cout << " X:" << to_string(point.x);
	std::cout << " y:" << to_string(point.y);
	std::cout << " z:" << to_string(point.z);
	std::cout << " Xr:" << to_string(Rotate_point(point, ang).x);
	std::cout << " yr:" << to_string(Rotate_point(point, ang).y);
	std::cout << " zr:" << to_string(Rotate_point(point, ang).z);
	std::cout << "0\n";
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "SDL could not initialize! SDL_Error: %s\n", SDL_GetError();
		std::cout << "0c\n";
	}
	else
	{
		std::cout << "0b\n";
		//Create window
		//window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
		std::cout << "0d\n";
		if (window == NULL)
		{
			std::cout << "Window could not be created! SDL_Error: %s\n", SDL_GetError();
			std::cout << "0e\n";
		}
		else
		{
			std::cout << "0f\n";
			//Get window surface
			screenSurface = SDL_GetWindowSurface(window);

			//Fill the surface white
			//SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));

			//Update the surface
			//SDL_UpdateWindowSurface(window);

			//Hack to get window to stay up
			//DL_Event e; bool quit = false; while (quit == false) { while (SDL_PollEvent(&e)) { if (e.type == SDL_QUIT) quit = true; } }

		}
	}
	std::cout << "0z\n";


	SDL_Event e; 
	bool quit = false; 
	worldOfset.x = 0;
	worldOfset.y = 0;
	worldOfset.z = 0;
	while (quit == false)
	{

		SDL_UpdateWindowSurface(window);
		while (SDL_PollEvent(&e)) { 
			if (e.type == SDL_QUIT) quit = true; 
		}
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);

		SDL_RenderClear(renderer);
		/*
		
		//for (int d = 0; d <= screeny; d++) {
		//	for (int l = 0; l <= screenx; l++) {
		//		screenbuffer[l][d] = 32;
		//	}
		//}
		//*/
		//render_to_buffer(screenbuffer[0], SCREEN_WIDTH, SCREEN_HEIGHT);

		///*
		//for (int d = 0; d <= screeny; d++) {
		//	for (int l = 0; l <= screenx; l++) {
		//		cout << screenbuffer[l][d];
		//		//cout << "0";
		//		if (l == screenx) {
		//			cout << "\n";

		//		}
		//	}
		//}
		//*/
		//SDL_RenderPresent(renderer);
		////console
		//std::string consoleinput;
		//getline(cin, consoleinput, '\n');
		////std::cin >> consoleinput;
		//std::cout << "input: " + consoleinput + "\n";
		//if (consoleinput == "render_lines_true") {
		//	render_lines = true;
		//	std::cout << "render_lines is true\n";
		//}
		//else if (consoleinput == "render_lines_false") {
		//	render_lines = false;
		//	std::cout << "render_lines is false\n";
		//}
		//else if (consoleinput == "Toggel_vertex_hilight") {
		//	if (hilight_vertex == true) { hilight_vertex = false; }
		//	else{ hilight_vertex = true; }
		//	std::cout << "Vertex hilight is " + std::to_string(hilight_vertex);
		//}
		//else if (consoleinput == "set_vertex_hilight") {
		//	std::cout << "set hilighted vertex to: "; 
		//	cin >> vertex_is_hil;
		//	std::cout << "\nhilighted vertex is set to : " + std::to_string(vertex_is_hil) + "\n";
		//}
		//else if (consoleinput == "set_fov") {
		//	std::cout << "set fov to: "; cin >> fov;
		//	std::cout << "fov set\n";
		//}
		//else if (consoleinput == "set_vertex_count") {
		//	std::cout << "set vertex count to: "; cin >> vertex_count;
		//	std::cout << "vertex count set\n";
		//}
		//else if (consoleinput == "set_line_count") {
		//	std::cout << "set line count to: "; cin >> line_count;
		//	std::cout << "line count set\n";
		//}
		//else if (consoleinput == "set_line_buffer") {
		//	int tempplase;
		//	std::cout << "line: "; cin >> tempplase;
		//	std::cout << "\nset line to: "; cin >> LineTable[tempplase];
		//	std::cout << "\nline set\n";
		//}
		//else if (consoleinput == "set_vertex_buffer") {
		//	int tempplase;
		//	std::cout << "vetex: "; cin >> tempplase;
		//	std::cout << "\nset vetex to: "; cin >> Vertex_table[tempplase];
		//	std::cout << "\nvertex set\n";
		//}
		//else
		//{
		//	std::cout << "comand uknown\n";
		//}
		
		run(quit, e);
	}
	//std::cout << std::to_string(Vertex_table_baked[1].x);
	//Destroy window
	SDL_DestroyWindow(window);

	//Quit SDL subsystems
	SDL_Quit();
	return 0;
}