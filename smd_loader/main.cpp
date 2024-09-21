#include <stdio.h>
#include <stddef.h> //offsetof
#include "smd_loader.h"
#include "glwnd.h"

GLUquadric *p_quadratic;
smd_model model;
float angle = 0.f;
double last_time, curr_time = 1.0;
double elapsed_time = 1.0;

INT Width, Height;

glm::vec3 position_from_matrix(glm::mat4x4 &matrix)
{
	return glm::vec3(matrix[3][0], matrix[3][1], matrix[3][2]);
}

/* not work */
glm::vec3 bone_world_position(const smd_model *p_model, int curr_bone_idx)
{
	if (curr_bone_idx == -1)
		return glm::vec3(0.f, 0.f, 0.f);

	glm::mat4x4 bone_transform = glm::translate(glm::mat4x4(1.f), p_model->keyframes[0].positions[curr_bone_idx].position);
	int parent = p_model->hierarchy[curr_bone_idx].parent_id;
	while (parent != -1) {
		bone_transform = glm::translate(bone_transform, p_model->keyframes[0].positions[parent].position);
		parent = p_model->hierarchy[parent].parent_id;
	}
	return position_from_matrix(bone_transform);
}

/* not work */
glm::vec3 bone_world_position1(const smd_model *p_model, int curr_bone_idx)
{
	if (curr_bone_idx == -1)
		return glm::vec3(0.f, 0.f, 0.f);

	glm::vec3 pos = p_model->keyframes[0].positions[curr_bone_idx].position;
	int parent = p_model->hierarchy[curr_bone_idx].parent_id;
	while (parent != -1) {
		pos += p_model->keyframes[0].positions[parent].position;
		parent = p_model->hierarchy[parent].parent_id;
	}
	return pos;
}

/* not work */
#define MAX_BONES_INHERIT_DEPTH 32
glm::vec3 bone_world_position2(const smd_model *p_model, int curr_bone_idx)
{
	if (curr_bone_idx == -1)
		return glm::vec3(0.f, 0.f, 0.f);

	int inherit_depth = 0;
	glm::vec3 inherit_positions[MAX_BONES_INHERIT_DEPTH];

	/* write positions to parrent */
	int parent_id = p_model->hierarchy[curr_bone_idx].parent_id;
	inherit_positions[inherit_depth++] = p_model->keyframes[0].positions[curr_bone_idx].position;

	while (parent_id != -1) {
		inherit_positions[inherit_depth++] = p_model->keyframes[0].positions[parent_id].position;
		assert(inherit_depth < MAX_BONES_INHERIT_DEPTH);
		parent_id = p_model->hierarchy[parent_id].parent_id;
	}

	/* make transformations with positions in left side */
	glm::vec3 position(0.f, 0.f, 0.f);
	for(int i = 0; i < inherit_depth; i++)
		position += inherit_positions[i];

	return position;
}

glm::vec3 bone_world_position3(const smd_model *p_model, int keyframe_idx, int boneIndex) {
	glm::mat4 modelMatrix(1.0f);
	const smd_anim_keyframe &keyframe = p_model->keyframes[keyframe_idx];
	while (boneIndex != -1) {
		smd_kfbone bone = keyframe.positions[boneIndex];

		// Применяем позицию и поворот кости
		glm::mat4 boneMatrix(1.0f);
		boneMatrix = glm::translate(boneMatrix, glm::vec3(bone.position[0], bone.position[1], bone.position[2]));

		// Создаем матрицу поворота
		glm::mat4 rotationMatrix(1.0f);
		rotationMatrix = glm::rotate(rotationMatrix, glm::radians(bone.rotation[0]), glm::vec3(1.0f, 0.0f, 0.0f));
		rotationMatrix = glm::rotate(rotationMatrix, glm::radians(bone.rotation[1]), glm::vec3(0.0f, 1.0f, 0.0f));
		rotationMatrix = glm::rotate(rotationMatrix, glm::radians(bone.rotation[2]), glm::vec3(0.0f, 0.0f, 1.0f));

		// Умножаем общую матрицу моделирования на матрицу кости
		modelMatrix = modelMatrix * rotationMatrix * boneMatrix;

		boneIndex = p_model->hierarchy[boneIndex].parent_id;
	}

	// Извлекаем позицию из матрицы моделирования
	glm::vec3 worldPosition = glm::vec3(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]);

	return worldPosition;
}

double get_millis()
{
	LARGE_INTEGER counter, freq;
	QueryPerformanceCounter(&counter);
	QueryPerformanceFrequency(&freq);
	return counter.QuadPart / (double)freq.QuadPart;
}

void drawBone(const smd_model *p_model, int keyframe_idx, int boneIndex) {
	const smd_anim_keyframe &keyframe = p_model->keyframes[keyframe_idx];
	smd_kfbone bone = keyframe.positions[boneIndex];
	glPushMatrix();

	// Применяем позицию и поворот кости
	glTranslatef(bone.position[0], bone.position[1], bone.position[2]);
	glRotatef(bone.rotation[0], 1.0f, 0.0f, 0.0f);
	glRotatef(bone.rotation[1], 0.0f, 1.0f, 0.0f);
	glRotatef(bone.rotation[2], 0.0f, 0.0f, 1.0f);

	// Рисуем кость
	glColor3f(1.0f, 1.0f, 1.0f);
	gluSphere(p_quadratic, 1.f, 5, 5);

	// Рекурсивно отображаем дочерние кости
	for (int i = 0; i < p_model->hierarchy.size(); i++)
		if (p_model->hierarchy[i].parent_id == boneIndex)
			drawBone(p_model, keyframe_idx, i);

	glPopMatrix();
}

void fn_draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	last_time = curr_time;
	curr_time = get_millis();
	elapsed_time = (curr_time - last_time);
	//double fps = 1.0 / elapsed_time;
	//printf("FPS: %lf   Angle: %f\r", fps, angle);

	angle += (float)elapsed_time * 20.0;
	if (angle > 359.999)
		angle = 0.f;

	glTranslatef(0.0f, -35.0f, -130.0f);
	glRotatef(-90.f, 1.f, 0.f, 0.f);
	glRotatef(angle, 0.0, 0.0, 1.0);

	//for (int i = 0; i < model.meshes.size(); i++) {
	//	glVertexPointer(3, GL_FLOAT, sizeof(smd_vertex), ((const char *)model.meshes[i].vertices.data() + offsetof(smd_vertex, vertex)));
	//	glNormalPointer(GL_FLOAT, sizeof(smd_vertex), ((const char *)model.meshes[i].vertices.data() + offsetof(smd_vertex, normal)));
	//	glDrawArrays(GL_TRIANGLES, 0, model.meshes[i].vertices.size());
	//}

	glPushAttrib(GL_CURRENT_BIT);
	glColor3f(0.0f, 1.0f, 0.0f);

	glPointSize(5.0);
	glBegin(GL_POINTS);
	for (size_t i = 0; i < model.hierarchy.size(); i++) {
		smd_kfbone &bone = model.keyframes[0].positions[i];
			glm::vec3 wpos = bone_world_position3(&model, 0, i);
			glVertex3fv(glm::value_ptr(wpos));
	}
	glEnd();
	glPointSize(1.f);

	//glm::mat4x4 world_matrix;
	//glLoadMatrixf((float *)&world_matrix);

	//drawBone(&model, 0, 0);
	glPopAttrib();

	//glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_NORMAL_ARRAY);
	//for (int i = 0; i < model.meshes.size(); i++) {
	//	glVertexPointer(3, GL_FLOAT, sizeof(smd_vertex), ((const char *)model.meshes[i].vertices.data() + offsetof(smd_vertex, vertex)));
	//	glNormalPointer(GL_FLOAT, sizeof(smd_vertex), ((const char *)model.meshes[i].vertices.data() + offsetof(smd_vertex, normal)));
	//	glDrawElements(GL_TRIANGLES, model.meshes[i].triangles.size(), GL_UNSIGNED_INT, model.meshes[i].triangles.data());
	//}
	//glDisableClientState(GL_VERTEX_ARRAY);
	//glDisableClientState(GL_NORMAL_ARRAY);
}

void fn_window_resize(HWND hWnd, int width, int height)
{
	if (!height)
		height = 1;

	Width = width;
	Height = height;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, width / (double)height, 0.1, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void fn_mousemove(HWND hWnd, int x, int y)
{
}

void fn_mouseclick(HWND hWnd, int x, int y, int button, int state)
{
}

void fn_charinput(HWND hWnd, char symbol)
{
}

//https://docs.microsoft.com/ru-ru/windows/win32/inputdev/wm-keydown
void fn_keydown(HWND hWnd, INT state, WPARAM wparam, LPARAM lparam)
{
	INT key = (INT)wparam;
	if (state == KEY_DOWN) {
		switch (key) {
		case 27:
			PostQuitMessage(0);
			break;
		}
	}
}

//Add this GL functions
void fn_windowcreate(HWND hWnd)
{
	typedef BOOL (__stdcall *wglSwapIntervalEXTPfn)(int interval);
	wglSwapIntervalEXTPfn wglSwapIntervalEXT = (wglSwapIntervalEXTPfn)wglGetProcAddress("wglSwapIntervalEXT");
	if (wglSwapIntervalEXT)
		wglSwapIntervalEXT(1); //VSYNC

	RECT rct;
	GetClientRect(hWnd, &rct);
	glViewport(0, 0, (GLsizei)rct.right, (GLsizei)rct.bottom);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, rct.right / (double)rct.bottom, 0.1, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	p_quadratic = gluNewQuadric();
}

void fn_windowclose(HWND hWnd)
{
	exit(0);
}

int main()
{
	int status = smd_load_model(&model, "model/ct_gign_reference.smd");
	if (status != SMDLDR_STATUS_OK) {
		printf("Loading SMD model failed. Error: %d\n", status);
		return 1;
	}

	if (model.keyframes[0].positions.size() != model.hierarchy.size()) {
		MessageBoxA(0, 0, 0, 0);
	}

	for (size_t i = 0; i < model.keyframes[0].positions.size(); i++) {
		smd_kfbone *p_bone = &model.keyframes[0].positions[i];
		printf("%d (%f %f %f) (%f %f %f)\n", model.hierarchy[i].parent_id,
			p_bone->position.x, p_bone->position.y, p_bone->position.z, 
			p_bone->rotation.x, p_bone->rotation.y, p_bone->rotation.z
		);
	}

	//return 0;
	printf("\n\n\n\n" "Initializing OpenGL renderer...\n");

	create_window("Test Valve SMD model view before anim", "TestSmdViewer",
		24,					  //Colors bits
		32,					  //Depth bits
		fn_draw,			  //Draw function
		fn_window_resize,	  //Window resize function
		fn_mousemove,		  //Mouse move function
		fn_mouseclick,		  //Mouse click function
		fn_charinput,		  //Char input function
		fn_keydown,			  //Keydown function
		fn_windowcreate,	  //Window create function
		fn_windowclose,		  //Window close function
		0, 0,
		800,				  //Window width
		600);				  //Window height

	return 0;
}