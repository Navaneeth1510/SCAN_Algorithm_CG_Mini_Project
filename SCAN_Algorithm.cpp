#include<gl/glut.h>
#include <iostream>
#include<stdio.h>
#include<string.h>
#include<math.h>
void draw_circle(int x, int y);
void timerFunc(int val);

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------


//globals
int req_seq[] = { 176, 79, 34, 60, 100, 11, 41, 114 };
int head = 90;

int track[] = { 1,0,0,0,0,0,0,0,0,0,0 };

int points[] = { 0,20,40,60,80,100,120,140,160,180,200,220 };

int point[10][2];
int k = 0;

int left = 0;

int in3 = 0;

int currentLineIndex = 1;
const int totalLines = 10;
float thickness = 3.0f;
int delay = 1500;

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------

//       -------------------------------------------------------- Incremental Line Drawing Algorithm --------------------------------------------------------

//set the pixel values
void setPixel(int x, int y) {
	glColor3f(0.7, 1, 1);
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
}
//implementation of incremental line drawing algorithm
void plotLineIncremental(int x0, int y0, int x1, int y1, float thickness) {
	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);
	int x = x0;
	int y = y0;
	int sx = (x0 < x1) ? 1 : -1;
	int sy = (y0 < y1) ? 1 : -1;

	if (dx > dy) {
		int p = 2 * dy - dx; 
		int inc1 = 2 * dy;   
		int inc2 = 2 * (dy - dx); 

		for (int i = -thickness / 2; i <= thickness / 2; i++) {
			x = x0;
			y = y0 + i;
			int offset = i;

			for (int j = x0; j != x1; j += sx) {
				setPixel(x, y);
				if (p >= 0) {
					y += sy;
					p += inc2;
				}
				else {
					p += inc1;
				}
				x += sx;
			}
			setPixel(x, y);
		}
	}
	else {
		int p = 2 * dx - dy;
		int inc1 = 2 * dx;
		int inc2 = 2 * (dx - dy);

		for (int i = -thickness / 2; i <= thickness / 2; i++) {
			x = x0 + i;
			y = y0;
			int offset = i;

			for (int j = y0; j != y1; j += sy) {
				setPixel(x, y);
				if (p >= 0) {
					x += sx;
					p += inc2;
				}
				else {
					p += inc1;
				}
				y += sy;
			}
			setPixel(x, y);
		}
	}
}
//draw dotted line from seek point till etched seek points below
void draw_dotted_lines(int x, int y) {
	glColor3f(1, 1, 1);
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(1, 0xCCCC);
	glLineWidth(0.5f);
	glBegin(GL_LINES);
	glVertex2f(x, y);
	glVertex2f(x, 36);
	glEnd();
	glDisable(GL_LINE_STIPPLE);
	glFlush();
}
//setup the order of seek points to be serviced
void plot() {
	int init_y = 435, init_x = (head * 2) + 100;
	glColor3f(0.8, 0.2, 0.2);

	// Sort the seek points
	for (int i = 0; i < 8; i++) {
		for (int j = i + 1; j < 8; j++) {
			if (req_seq[i] > req_seq[j]) {
				int t = req_seq[i];
				req_seq[i] = req_seq[j];
				req_seq[j] = t;
			}
		}
	}
	//left direction
	if (left == 0) {
		int head_pos = -1;
		for (int i = 0; i < 8; i++) {
			if (req_seq[i] < head) {
				head_pos = i;
			}
			else {
				break;
			}
		}
		init_x = (head * 2) + 100;
		point[0][0] = init_x;
		point[0][1] = init_y;
		int k = 1;
		for (int i = head_pos; i >= 0; i--) {
			int x = (req_seq[i] * 2) + 100;
			int y = init_y - (k * 40);
			point[k][0] = x;
			point[k++][1] = y;
		}
		point[k][0] = 100;
		point[k++][1] = init_y - (k * 40);
		for (int i = head_pos + 1; i < 8; i++) {
			int x = (req_seq[i] * 2) + 100;
			int y = init_y - (k * 40);
			point[k][0] = x;
			point[k++][1] = y;
		}
		currentLineIndex = 1;
	}
	//right direction
	else {
		int head_pos = -1;
		for (int i = 0; i < 8; i++) {
			if (req_seq[i] >= head) {
				head_pos = i;
				break;
			}
		}
		init_x = (head * 2) + 100;
		point[0][0] = init_x;
		point[0][1] = init_y;

		int k = 1;
		for (int i = head_pos; i < 8; i++) {
			int x =(req_seq[i] * 2) + 100;
			int y = init_y - (k * 40);
			point[k][0] = x;
			point[k++][1] = y;
		}
		point[k][0] = 540;
		point[k++][1] = init_y - (k * 40);
		for (int i = head_pos - 1; i >= 0; i--) {
			int x = (req_seq[i] * 2) + 100;
			int y = init_y - (k * 40);
			point[k][0] = x;
			point[k++][1] = y;
		}
		currentLineIndex = 1;
	}
	glutTimerFunc(0, timerFunc, 0); 
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------

//     --------------------------------------------------------------- String print functions ----------------------------------------------------------------

//display with large font size
void output_large(int x, int y, char* string) {
	glColor3f(1.0, 0.647, 0.0);
	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
	glFlush();
}
//display with small font size
void output_small(int x, int y, char* string) {
	glColor3f(1,1,1);
	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(string);

	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
	}
	glFlush();
}
//screen2 content display
void output_content(float x, float y, void* font, const char* string) {
	glColor3f(1, 1, 1);
	const char* c;
	glRasterPos2f(x, y);
	for (c = string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
	glFlush();
}
//display the points on graph
void output_number(int x, int y) {
	glColor3f(1, 1, 1);
	for (int i = 0; i < sizeof(points) / sizeof(points[0]); ++i) {
		int num = points[i];
		char str[5];
		int result = sprintf_s(str, sizeof(str), "%d", num);
		if (result < 0) {
			printf("Buffer error occurred\n");
		}
		else {
			if (num < 99)
				glRasterPos2f(x + (i * 40) - 11, y);
			else
				glRasterPos2f(x + (i * 40) - 15, y);
			int len = (int)strlen(str);
			for (int j = 0; j < len; j++) {
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[j]);
			}
		}
	}
	glFlush();
}
//display disk numbers
void output_disk(int x, int y) {
	glColor3f(0,0,0);
	int k = 0;
	for (int i = 20; i <= 220;i+=20 ) {
		int num = i;
		int x1 = x;
		int y1 = y + (k * 21);
		k++;
		char str[5];
		int result = sprintf_s(str, sizeof(str), "%d", num);
		if (result < 0) {
			printf("Buffer error occurred\n");
		}
		else {
			glRasterPos2f(x1,y1);
			int len = (int)strlen(str);
			for (int j = 0; j < len; j++) {
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, str[j]);
			}
		}
	}
	glFlush();
}
//display the seek points below
void output_down_number(int x, int y) {
	glColor3f(1, 1, 1);
	for (int i = 0; i < sizeof(req_seq) / sizeof(req_seq[0]); ++i) {
		int num = req_seq[i];
		char str[5];
		int result = sprintf_s(str, sizeof(str), "%d", num);
		if (result < 0) {
			printf("Buffer error occurred\n");
		}
		else {
			int adjusted_x = x + (num * 2);
			if (num < 99)
				glRasterPos2f(adjusted_x-5, y);
			else
				glRasterPos2f(adjusted_x - 10, y);

			int len = (int)strlen(str);
			for (int j = 0; j < len; j++) {
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, str[j]);
			}
		}
	}
	glFlush();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------

//      ---------------------------------------------------------------- Drawing functions ----------------------------------------------------------------

//underline functionality
void draw_line(int x, int y, int x1, int y1, float thickness) {
	glColor3f(1, 1, 1);
	glLineWidth(thickness);
	glBegin(GL_LINES);
	glVertex2f(x, y);
	glVertex2f(x1, y1);
	glEnd();
	glFlush();
}
//draw the disc
void draw_disk() {
	int r = 240;
	int x = 790, y = 260;
	for (int j = 0; j < 11; j++) {
		r = r - 20;
		k++;
		if (track[j]== 0)
			glColor3f(0.7, 0.7, 0.7);
		else
			glColor3f(0, 0.5019607843137255, 0.5019607843137255);
		glBegin(GL_POLYGON);
		for (float i = 0; i < 360; i += 0.01) {
			float x1 = r * cos(i) + x;
			float y1 = r * sin(i) + y;
			glVertex2f(x1, y1);
		}
		glEnd();
	}
	r = 220;
	glColor3f(0, 0, 0);
	for (int j = 1; j <= 11; j++) {
		r = r - 20;
		glBegin(GL_POINTS);
		for (float i = 0; i < 360; i += 0.1) {
			float x1 = r * cos(i) + x;
			float y1 = r * sin(i) + y;
			glVertex2f(x1, y1);
		}
		glEnd();
	}
}
//point the seek sequence points
void draw_circle(int x, int y) {
	int r = 4;
	if (x == 100 || x==540) {
		glColor3f(0, 0, 1);
	}
	else {
		glColor3f(1, 0, 0);
	}
	glBegin(GL_POLYGON);
	for (float theta = 0; theta < 360; theta++) {
		float x1 = r * cos(theta)+x;
		float y1 = r * sin(theta)+y;
		glVertex2f(x1, y1);
	}
	glEnd();
	glFlush();
}
//the graph polygon
void draw(float p[4][2]) {
	glColor3f(0, 1, 0);
	glLineWidth(3.0f);
	glColor3f(0, 0.5019607843137255, 0.5019607843137255);
	glBegin(GL_POLYGON);
	glVertex2fv(p[0]);
	glVertex2fv(p[1]);
	glVertex2fv(p[2]);
	glVertex2fv(p[3]);
	glEnd();
	glFlush();
}
//etch the points on graph
void marks(int x, int y) {
	int  up = y - 5;
	int down = y + 5;
	for (int i = 1; i <= 10; i++) {
		glColor3f(1, 0, 0);
		int x1 = x - (i * 40);
		glBegin(GL_LINES);
		glVertex2f(x1, up);
		glVertex2f(x1, down);
		glEnd();
		glFlush();
	}
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------

//    ---------------------------------------------------------------- screen implemetations ----------------------------------------------------------------

void screen1() {
	in3 = 0;
	glutTimerFunc(0,NULL,0);
	char main[] = "SCAN Algorithm";
	char below[] = "Project by : ";
	char name1[] = "Navaneeth N";
	char name2[] = "Rahul G Athreyas";
	glClearColor(0, 0, 0, 0);
	glColor3f(1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	output_large(450, 350, main);
	output_large(800, 100, below);
	output_small(800, 70, name1);
	output_small(800, 45, name2);

}
void screen2() {
	in3 = 0;
	glutTimerFunc(0, NULL, 0);
	char head[] = "SCAN Algorithm";
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);

	output_content(50, 570, GLUT_BITMAP_HELVETICA_18, "Step 1: Initialize the scan line.");
	output_content(50, 540, GLUT_BITMAP_HELVETICA_18, "Step 2: Find intersections.");
	output_content(50, 510, GLUT_BITMAP_HELVETICA_18, "Step 3: Sort intersections.");
	output_content(50, 480, GLUT_BITMAP_HELVETICA_18, "Step 4: Fill pixels between pairs of intersections.");
	output_content(50, 450, GLUT_BITMAP_HELVETICA_18, "Step 5: Repeat for all scan lines.");
	output_content(70, 380, GLUT_BITMAP_HELVETICA_18, "In the SCAN Disk Scheduling Algorithm, the head starts from one end of the disk and moves towards the");
	output_content(50, 350, GLUT_BITMAP_HELVETICA_18, "other end, servicing requests in between one by one and reaching the other end. Then the direction of the");
	output_content(50, 320, GLUT_BITMAP_HELVETICA_18, "head is reversed and the process continues as head continuously scans back and forth to access the disk.");
	output_content(50, 290, GLUT_BITMAP_HELVETICA_18, "So, this algorithm works as an elevator and is hence also known as the elevator algorithm. As a result,");
	output_content(50, 260, GLUT_BITMAP_HELVETICA_18, "the requests at the midrange are serviced more and those arriving behind the disk arm will have to wait.");

	output_large(450, 680, head);
}
void screen3() {
	in3 = 1;
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	char head[] = "SCAN Algorithm";
	char problem[] = "Problem :";
	char req[] = "Request sequence = [ 176, 79, 34, 60, 92, 11, 41, 114 ]";
	char headpos[] = "Initial head position = 90";
	char trackpos[] = "Track:";
	output_large(450, 680, head);
	output_large(50, 600, problem);
	output_small(75, 550, req);
	output_small(75, 530, headpos);
	output_large(760, 520, trackpos);

	draw_circle(480, 435);
	float points[4][2] = { {100,35},{100,475},{540,475},{540,35} };
	draw(points);
	marks(540, 475);
	output_number(100, 490);
	output_down_number(100, 20);
	draw_disk();
	output_disk(785, 260);
	plot();
}
void screen4() {
	in3 = 0;
	glutTimerFunc(0, NULL, 0);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	char head[] = "SCAN Algorithm";
	output_large(450, 680, head);

	char problem[] = "Answer :";
	output_large(50, 600, problem);
	int seek = 0;
	for (int i = 1; i < 10; i++) {
		int s;
		if ((point[i][0] / 2) > (point[i - 1][0] / 2)) {
			s = (point[i][0] / 2) - (point[i - 1][0] / 2);
		}
		else
			s = (point[i-1][0] / 2) - (point[i][0] / 2);
		seek += s;
	}

	char answer[] = "The total number of seek operations = ";
	output_small(75, 550, answer);
	char str[5];
	int result = sprintf_s(str, sizeof(str), "%d", seek);
	if (result < 0) {
		printf("Buffer error occurred\n");
	}
	else {
		glColor3f(1.0, 0.647, 0.0);
		glRasterPos2f(395, 550);
		int len = (int)strlen(str);
		for (int j = 0; j < len; j++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[j]);
		}
	}
	glFlush();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------

//        ---------------------------------------------------------------- events ----------------------------------------------------------------

void display() {
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	screen1();
	glFlush();
}
void timerFunc(int value) {
	if (in3 == 1) {
		if (currentLineIndex < totalLines) {
			glColor3f(0.8, 0.2, 0.2);
			plotLineIncremental(point[currentLineIndex - 1][0], point[currentLineIndex - 1][1], point[currentLineIndex][0], point[currentLineIndex][1], thickness);
			if (point[currentLineIndex][0] != 100 && point[currentLineIndex][0] != 540) {
				draw_dotted_lines(point[currentLineIndex][0], point[currentLineIndex][1]);
				draw_circle(point[currentLineIndex-1][0], point[currentLineIndex-1][1]);
				for (int i = 0; i < 11; i++)
					track[i] = 0;
				int seek_point = (point[currentLineIndex][0] - 100) / 2;
				int index = seek_point / 20;
				printf("%d at %d \n ", seek_point, index);
				printf("------------\n");
				track[10-index] = 1;
				draw_disk();
				output_disk(785, 260);

			}
			currentLineIndex++;
			glutTimerFunc(delay, timerFunc, 0);
			draw_circle(point[currentLineIndex - 1][0], point[currentLineIndex - 1][1]);
			output_disk(785, 260);
		}
	}
}
void keyboard(unsigned char key, int x, int y) {
	if (key == '1')
		screen1();
	else if (key == '2')
		screen2();
	else if (key == '3')
		screen3();
	else
		screen4();
}
void mouse(int b, int s, int x, int y) {
	if (b == GLUT_LEFT_BUTTON && s == GLUT_DOWN && in3 == 1) {
		left = 0;
		screen3();
	}
	if (b == GLUT_RIGHT_BUTTON && s == GLUT_DOWN && in3 == 1) {
		left = 1;
		screen3();
	}
}
void myinit() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 1040, 0, 720);
	glMatrixMode(GL_MODELVIEW);
}
void main() {
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(1040, 720);
	glutInitWindowPosition(0, 0);	
	glutCreateWindow("SCAN Algorithm");
	myinit();
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
}