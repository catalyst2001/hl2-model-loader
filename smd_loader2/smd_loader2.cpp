#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#pragma comment(lib, "glut32.lib")
#include <Windows.h>
#include <gl/GL.h>
#include "GL/glut.h"

struct Vertex {
	float x, y, z;
};

struct Triangle {
	union {
		struct { unsigned int v1, v2, v3; };
		unsigned int v[3];
	};
};

struct Bone {
	std::string name;
	int parentIndex;
	float position[3];
	float rotation[3];
};

struct Model {
	std::vector<Vertex> vertices;
	std::vector<Triangle> triangles;
	std::vector<Bone> skeleton;
};

bool loadSMDModel(Model &model, const std::string& filename)
{
	std::ifstream file(filename);
	if (!file) {
		std::cerr << "Failed to open file: " << filename << std::endl;
		return false;
	}

	std::string line;
	std::string section;
	while (std::getline(file, line)) {
		std::istringstream iss(line);
		std::string type;
		iss >> type;

		if (type == "nodes") {
			section = "nodes";
		}
		else if (type == "skeleton") {
			section = "skeleton";
		}
		else if (type == "triangles") {
			section = "triangles";
		}
		else if (section == "nodes") {
			Bone bone;
			iss >> bone.name >> bone.parentIndex;
			model.skeleton.push_back(bone);
		}
		else if (section == "skeleton") {
			// Загрузка позиций и поворотов костей
			unsigned int boneIndex;
			iss >> boneIndex;
			Bone& bone = model.skeleton[boneIndex];
			iss >> bone.position[0] >> bone.position[1] >> bone.position[2];
			iss >> bone.rotation[0] >> bone.rotation[1] >> bone.rotation[2];
		}
		else if (section == "triangles") {
			Triangle triangle;
			iss >> triangle.v1 >> triangle.v2 >> triangle.v3;
			model.triangles.push_back(triangle);
		}
	}
	file.close();
	return true;
}

Model model;

void drawModel(const Model& model) {
	glBegin(GL_TRIANGLES);
	for (const Triangle& triangle : model.triangles) {
		for (int i = 0; i < 3; i++) {
			const Vertex& vertex = model.vertices[triangle.v[i]];
			glVertex3f(vertex.x, vertex.y, vertex.z);
		}
	}
	glEnd();
}

void drawSkeleton(const Model& model, int boneIndex) {
	const Bone& bone = model.skeleton[boneIndex];

	glPushMatrix();

	// Применяем позицию и поворот кости
	glTranslatef(bone.position[0], bone.position[1], bone.position[2]);
	glRotatef(bone.rotation[0], 1.0f, 0.0f, 0.0f);
	glRotatef(bone.rotation[1], 0.0f, 1.0f, 0.0f);
	glRotatef(bone.rotation[2], 0.0f, 0.0f, 1.0f);

	// Рисуем кость
	glColor3f(1.0f, 1.0f, 1.0f);
	glutWireCube(1.0f);

	// Рекурсивно отображаем дочерние кости
	for (int i = 0; i < model.skeleton.size(); i++) {
		if (model.skeleton[i].parentIndex == boneIndex) {
			drawSkeleton(model, i);
		}
	}

	glPopMatrix();
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Перемещаем камеру
	gluLookAt(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Отображаем модель
	drawModel(model);

	// Отображаем скелет
	drawSkeleton(model, 0); // Предполагаем, что корневая кость имеет индекс 0

	glutSwapBuffers();
}

int main(int argc, char** argv) {
	// Инициализация GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Valve SMD Model");

	// Инициализация OpenGL
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	// Регистрация функций обратного вызова
	glutDisplayFunc(display);

	// Загрузка модели и скелета
	if (!loadSMDModel(model, "model/boxjoints.smd")) {
		printf("Failed to load model!\n");
		return 1;
	}

	// Запуск главного цикла GLUT
	glutMainLoop();

	return 0;
}