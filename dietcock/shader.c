#include<stdio.h>
#include<glad/glad.h>
#include"shader.h"
#include<stdlib.h>

static char* read_file(const char* filepath) {
	FILE* file = fopen(filepath, "rb");
	if (!file) {
		printf("shader file successfully not read %s\n", filepath);
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	long length = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* buffer = (char*)malloc(length + 1);
	if (!buffer) {
		printf("shader memory allocation failed");
		fclose(file);
		return NULL;
	}

	size_t read_bytes = fread(buffer, 1, length, file);
	buffer[read_bytes] = '\0';
	fclose(file);

	return buffer;
}
Shader shader_create(const char* vertex_path, const char* fragment_path) {
	Shader shader = { 0 };

	char* vertex_code = read_file(vertex_path);
	char* fragment_code = read_file(fragment_path);

	if (!vertex_code || !fragment_code) {
		free(vertex_code);
		free(fragment_code);
		return shader;
	}

	unsigned int vertex, fragment;
	int success;
	char info_log[512];

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, (const char* const*)&vertex_code, NULL);
	glCompileShader(vertex);

	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, 512, NULL, info_log);
		printf("vertex shader compilation failed\n %s", info_log);
	}
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, (const char* const*)&fragment_code, NULL);
	glCompileShader(fragment);

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, NULL, info_log);
		printf("fragment shader compilation failed\n %s", info_log);
	}

	shader.ID = glCreateProgram();
	glAttachShader(shader.ID, vertex);
	glAttachShader(shader.ID, fragment);
	glLinkProgram(shader.ID);

	glGetProgramiv(shader.ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shader.ID, 512, NULL, info_log);
		printf("shader compilation failed %s\n", info_log);
	}
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	free(vertex_code);
	free(fragment_code);

	return shader;
}

void shader_use(Shader* shader) {
	glUseProgram(shader->ID);
}

void shader_set_bool(Shader* shader, const char* name, bool value) {
	glUniform1i(glGetUniformLocation(shader->ID, name), (int)value);
}

void shader_set_int(Shader* shader, const char* name, int value) {
	glUniform1i(glGetUniformLocation(shader->ID, name), value);
}

void shader_set_float(Shader* shader, const char* name, float value) {
	glUniform1f(glGetUniformLocation(shader->ID, name), value);
}