#include "particle.h"

Particle::Particle(QVector3D center, QVector3D color, QVector3D velocity, QVector3D acceleration, float age, float life, float size)
	:Object(center), color(color), velocity(velocity), acceleration(acceleration), age(age), life(life), size(size)
{
}


Particle::~Particle(){

}


ParticleSystem::ParticleSystem(int count, float gravity)
	:ptcCount(count), gravity(gravity)
{
	for (int i = 0; i < ptcCount; i++){
		particles.push_back(Particle({ 0.0f, 0.0f, 0.0f },
			sparkColor,
			{ 0.0f, 0.0f, 0.0f },
			{ 0.0f, 0.0f, gravity },
			sparkLife,
			randi(0.6, 1.0),
			randf(sparkSize - sparkSize / 5, sparkSize + sparkSize / 5)));
	}
}

ParticleSystem::~ParticleSystem()
{
}

/* dt is the seconds since last frame */
void ParticleSystem::simulate(float dt){
	aging(dt);
	kinematics(dt);
}

void ParticleSystem::aging(float dt){
	for (int i = 0; i < positions.size(); i++){
		positions[i].setW(positions[i].w() + dt);
	}
	while (!positions.empty() && positions[0].w() > sparkLife){
		positions.pop_front();
	}
	for (vector<Particle>::iterator it = particles.begin(); it != particles.end(); it++){
		it->age += dt;
	}
	if (positions.empty()){
		return;
	}
	for (vector<Particle>::iterator it = particles.begin(); it != particles.end(); it++){
		if (it->age >= it->life){
			/* reuse the particle */
			int n = randi(0, positions.size() - 1);
			it->setCenter(positions[n].toVector3D());
			it->age = 0.0f;
			it->velocity = speed*QVector3D(randf(-1.0f, 1.0f), randf(-1.0f, 1.0f), randf(0.01f, 2.0f)).normalized();
		}
	}
}

void ParticleSystem::kinematics(float dt){
	if (positions.empty()){
		return;
	}
	for (vector<Particle>::iterator it = particles.begin(); it != particles.end(); it++){
		it->setCenter(it->getCenter() + it->velocity*dt);
		it->velocity = it->velocity + it->acceleration*dt;
	}
}

void ParticleSystem::spark(QVector3D position){
	positions.push_back(QVector4D(position, 0.0f));
}