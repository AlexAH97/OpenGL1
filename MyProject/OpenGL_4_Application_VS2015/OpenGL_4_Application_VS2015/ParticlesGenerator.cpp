#include"ParticleGenerator.h"
#include"Shader.hpp"
ParticleGenerator::ParticleGenerator(gps::Shader shader, Texture2D texture, GLuint amount) :shader(shader), texture(texture), amount(amount) {
	this->init();
}
void ParticleGenerator::Draw() {
	glBlendFunc(GL_SRC0_ALPHA, GL_ONE);
	this->shader.useShaderProgram();
	for (Particles particle : this->particles)
	{
		if (particle.life > 0.0f)
		{
			
		}
	}
}

void ParticleGenerator::init() {

}