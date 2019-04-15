#pragma once
#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H
#include<vector>

#include"GLEW/glew.h"
#include"glm/glm.hpp"

#include"Shader.hpp"
#include"Model3D.hpp"
#include"Texture2D.h"
struct Particles {
	glm::vec2 Position, Velocity;
	glm::vec4 color;
	GLfloat life;
	Particles() :Position(0.0f), Velocity(0.0f), color(1.0f), life(0.0f) {}
};
class ParticleGenerator {
	public:
		ParticleGenerator(gps::Shader shader,Texture2D texture, GLuint amount);
		void Update(GLfloat dt, GLuint newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
		void Draw();
	private:
		std::vector<Particles> particles;
		GLuint amount;
		Texture2D texture;
		GLuint VAO;
		gps::Shader shader;
		void init();
		GLuint firstUnusedParticle();
		void respawnParticle(Particles &particle, glm::vec2 oofset = glm::vec2(0.0f, 0.0f));

};
#endif // !PARTICLE_GENERATOR_H
