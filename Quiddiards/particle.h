#pragma once

#include <vector>
#include <deque>
#include "qvector4d.h"
#include "object.h"
#include "util.h"

using namespace std;

class Particle : public Object{
public:
	Particle(QVector3D center = { 0.0f, 0.0f, 12.0f },
		QVector3D color = { 1.0f, 0.12f, 0.0f},
		QVector3D velocity = { 0.0f, 0.0f, 0.0f },
		QVector3D acceleration = { 0.0f, 0.0f, 0.0f },
		float age = 1.0f,
		float life = 1.0f,
		float size = 0.01f);
	~Particle();
	QVector3D color;
	QVector3D velocity;
	QVector3D acceleration;
	float age;
	float life;
	float size;
};

class ParticleSystem{
public:
	ParticleSystem(int count, float gravity);
	~ParticleSystem();
	bool isActive()const{ return !positions.empty(); }
	void simulate(float dt);
	void aging(float dt);
	void kinematics(float dt);
	void spark(QVector3D position);
	vector<Particle>& getParticles(){ return particles; }
	const QVector3D sparkColor = { 1.0f, 0.2f, 0.0f };

private:
	const float speed = 15.0f;
	const float sparkLife = 0.2f;	// seconds
	const float sparkSize = 0.03f;
	int ptcCount;
	float gravity;
	vector<Particle> particles;
	deque<QVector4D> positions;
};