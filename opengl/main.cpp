#include "imgui.h"
#include "imgui_impl_glut.h"
#include "imgui_impl_opengl2.h"
#include <iostream>
#include <string>
#include <random>
#include <vector>
using namespace std;

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

#ifdef _MSC_VER
#pragma warning (disable: 4505) // unreferenced local function has been removed
#endif

int WindowWidth = 800;
int WindowHeight = 600;

string CurrentState = "intro";

float Delay = 0.5;


vector<float> BackGroundColor{ 0.933, 0.933, 0.933 };
vector<float> InitialColor{ 0.678, 0.847, 0.901 };
vector<float> ComparingColor{ 0.0, 0.501, 0.0 };
vector<float> FinalColor{ 1.0, 0.647, 0.0 };
vector<float> PivotColor{ 0.6, 0.196, 0.8 };

class Element
{
public:
	float Pos;
	float Width;
	float Height;
	vector<float> Color;

	Element(float Pos_, float Width_, float Height_, vector<float> Color_) :Pos(Pos_), Width(Width_), Height(Height_), Color(Color_) {}

	void Draw()
	{
		glBegin(GL_QUADS);
		glColor3f(Color[0], Color[1], Color[2]);
		glVertex2d(Pos, 0);
		glVertex2d(Pos + Width, 0);
		glVertex2d(Pos + Width, Height);
		glVertex2d(Pos, Height);
		glEnd();
	}
};

class Elements
{
public:
	vector<Element> Vec;

	void Init(int Size_)
	{
		Vec.clear();
		float UnitWidth = (WindowWidth * 1.0) / Size_;
		float UnitHeight = (WindowHeight * 1.0) / Size_;
		float Height_ = UnitHeight;
		float Pos_ = 1;
		for (int i = 0;i < Size_; i++)
		{
			Vec.push_back(Element(Pos_, UnitWidth, Height_, InitialColor));
			Pos_ += UnitWidth;
			Height_ += UnitHeight;
		}
	}

	void Draw()
	{
		for (int i = 0; i < Vec.size(); i++)
			Vec[i].Draw();
	}
	void SetColor()
	{
		for (int i = 0; i < Vec.size(); i++)
		{
			Vec[i].Color = InitialColor;
		}
	}
};

Elements elements;

///////////////////////////////////////////////////////////////////////////////////////////

void Init()
{

	glClearColor(BackGroundColor[0], BackGroundColor[1], BackGroundColor[2], 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, WindowWidth, 0, WindowHeight, 0, 1);
	glMatrixMode(GL_MODELVIEW);

	elements.Init(WindowWidth);

}

template<class RandomIt, class URBG>
void Shuffling(RandomIt first, RandomIt last, URBG&& g)
{
	typedef typename std::iterator_traits<RandomIt>::difference_type diff_t;
	typedef std::uniform_int_distribution<diff_t> distr_t;
	typedef typename distr_t::param_type param_t;

	distr_t D;
	diff_t n = last - first;
	for (diff_t i = n - 1; ~i; --i)
	{
		swap(first[i].Height, first[D(g, param_t(0, i))].Height);
		if (first[i].Color != InitialColor)
		{
			first[i].Color = InitialColor;
			first[D(g, param_t(0, i))].Color = InitialColor;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
void ControlPanel()
{

	{
		static int SizeSlider = WindowWidth;
		ImGui::Begin("Settings");

		ImGui::Text("Array length:   ");
		ImGui::SameLine();
		ImGui::PushItemWidth(150);
		if (ImGui::SliderInt("", &SizeSlider, 1, WindowWidth))
			elements.Init(SizeSlider);
		ImGui::PopItemWidth();

		ImGui::Text("Delay:          ");
		ImGui::SameLine();
		ImGui::PushItemWidth(150);
		ImGui::SliderFloat("Sec", &Delay, 0, 1);
		ImGui::PopItemWidth();

		ImGui::NewLine();
		ImGui::Text("               ");
		ImGui::SameLine();
		if (ImGui::Button("Shuffle", { 96, 25 }))
		{
			random_device rd;
			mt19937 g(rd());
			Shuffling(elements.Vec.begin(), elements.Vec.end(), g);
		}

		ImGui::NewLine();

		if (ImGui::Button("Bubble sort", { 102 , 25 }))
			CurrentState = "bubble sort";

		ImGui::SameLine();

		if (ImGui::Button("Smart bubble", { 102, 25 }))
			CurrentState = "smart bubble";

		ImGui::SameLine();

		if (ImGui::Button("Selection sort", { 102, 25 }))
			CurrentState = "selection sort";


		if (ImGui::Button("Insertion sort", { 102, 25 }))
			CurrentState = "insertion sort";

		ImGui::SameLine();

		if (ImGui::Button("Merge sort", { 102, 25 }))
			CurrentState = "merge sort";

		ImGui::SameLine();

		if (ImGui::Button("Quick sort", { 102, 25 }))
			CurrentState = "quick sort";

		ImGui::End();
	}

}

///////////////////////////////////////////////////////////////////////////////////////////

void DrawData(bool Sleeping = true)
{
	glClear(GL_COLOR_BUFFER_BIT);
	elements.Draw();
	glutSwapBuffers();
	if (Sleeping)
		Sleep(Delay * 1000);
}

void DrawDataElementByElementWithFinalColor()
{
	for (int i = 0; i < elements.Vec.size(); i++)
	{
		elements.Vec[i].Color = FinalColor;
		DrawData();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////

void BubbleSort(vector<Element>& vec)
{

	for (int pass = 0;pass < vec.size() - 1;pass++)
	{
		int i = 0;
		for (;i < vec.size() - pass - 1;i++)
		{
			vec[i].Color = ComparingColor;
			vec[i + 1].Color = ComparingColor;
			DrawData();
			if ((vec[i + 1].Height < vec[i].Height))
			{
				auto temp = vec[i + 1].Height;
				vec[i + 1].Height = vec[i].Height;
				vec[i].Height = temp;
			}
			vec[i].Color = InitialColor;
			vec[i + 1].Color = InitialColor;
			if (i != vec.size() - pass - 2)
				DrawData();
			else
				DrawData(false);
		}
		vec[i].Color = FinalColor;
	}
	vec[0].Color = FinalColor;
}

///////////////////////////////////////////////////////////////////////////////////////////

void SmartBubbleSort(vector<Element>& vec)
{
	for (int pass = 0;pass < vec.size() - 1;pass++)
	{
		int i = 0;
		bool finish = false;

		for (;i < vec.size() - pass - 1;i++)
		{
			vec[i].Color = ComparingColor;
			vec[i + 1].Color = ComparingColor;

			DrawData();

			if ((vec[i + 1].Height < vec[i].Height))
			{
				finish = true;
				auto temp = vec[i + 1].Height;
				vec[i + 1].Height = vec[i].Height;
				vec[i].Height = temp;
			}

			vec[i].Color = InitialColor;
			vec[i + 1].Color = InitialColor;


			if (i != vec.size() - pass - 2)
				DrawData();
			else
				DrawData(false);


		}

		if (!finish) {
			int idx = 0;
			while (idx < vec.size() && vec[idx].Color == InitialColor) {
				vec[idx].Color = FinalColor;
				idx++;
			}
			return;
		}

		vec[i].Color = FinalColor;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////

int Partitioning(vector<Element>& v, int first, int last)
{
	int leftmark = first + 1, rightmark = last;float pivot = v[first].Height;
	v[first].Color = PivotColor;
	v[leftmark].Color = ComparingColor;
	v[rightmark].Color = ComparingColor;
	DrawData();
	bool done = false;
	while (!done)
	{
		while (rightmark >= leftmark && pivot >= v[leftmark].Height)
		{
			v[leftmark++].Color = InitialColor;
			if (rightmark > leftmark)
				v[leftmark].Color = ComparingColor;
			DrawData();
		}
		while (rightmark >= leftmark && pivot <= v[rightmark].Height)
		{
			if (rightmark != leftmark)
				v[rightmark].Color = InitialColor;
			rightmark--;
			if (rightmark != first)
				v[rightmark].Color = ComparingColor;
			DrawData();
		}
		if (leftmark > rightmark)
			done = true;
		else
			swap(v[leftmark].Height, v[rightmark].Height);
	}
	swap(v[first].Height, v[rightmark].Height);
	if (leftmark >= 0 && leftmark < v.size())
		v[leftmark].Color = InitialColor;
	v[rightmark].Color = FinalColor;
	v[first].Color = InitialColor;
	DrawData();
	return rightmark;
}
void QuickSortHelper(vector<Element>& v, int first, int last)
{
	if (last > first)
	{
		int split_point = Partitioning(v, first, last);
		QuickSortHelper(v, first, split_point - 1);
		QuickSortHelper(v, split_point + 1, last);
	}
}
void QuickSort(vector<Element>& v)
{
	QuickSortHelper(v, 0, v.size() - 1);
	DrawDataElementByElementWithFinalColor();
}

///////////////////////////////////////////////////////////////////////////////////////////

void Display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplGLUT_NewFrame();

	ControlPanel();

	if (CurrentState != "intro")
		if (elements.Vec[0].Color == FinalColor)
			elements.SetColor();
	if (CurrentState == "intro")
		elements.Draw();
	else if (CurrentState == "bubble sort")
	{
		BubbleSort(elements.Vec);
		CurrentState = "intro";
	}
	else if (CurrentState == "smart bubble")
	{
		SmartBubbleSort(elements.Vec);
		CurrentState = "intro";
	}
	else if (CurrentState == "quick sort")
	{
		QuickSort(elements.Vec);

		CurrentState = "intro";
	}

	ImGui::Render();
	ImGuiIO& io = ImGui::GetIO();


	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

	glutSwapBuffers();
}


int main(int argc, char** argv)
{

	glutInit(&argc, argv);
#ifdef __FREEGLUT_EXT_H__
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
#endif
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_MULTISAMPLE);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WindowWidth, WindowHeight);
	glutCreateWindow("Sorting algorithms visualizer");
	Init();
	glutDisplayFunc(Display);
	glutIdleFunc(Display);
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplGLUT_Init();
	ImGui_ImplGLUT_InstallFuncs();
	ImGui_ImplOpenGL2_Init();

	glutMainLoop();

	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplGLUT_Shutdown();
	ImGui::DestroyContext();

	return 0;
}