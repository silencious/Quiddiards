#include "ball.h"
#include <util.h>

Ball::Ball(QVector3D center, float radius, QVector3D color)
	:Object(center), radius(radius), color(color)
{
}

Ball::~Ball(){

}

void Ball::setDX(float dx){
	velocity.setX(dx);
}
void Ball::setDY(float dy){
	velocity.setY(dy);
}
void Ball::setDZ(float dz){
	velocity.setZ(dz);
}

void Ball::setColor(QVector3D color){
	this->color = color;
}
void Ball::setVelocity(QVector3D velocity){
	this->velocity = velocity;
}
void Ball::move(float time){
	QVector3D vec = velocity * time;
	if (vec.length() > FLT_EPSILON){
		center += vec;
		vec.setZ(0);
		float angle = 57.3f * vec.length() / radius;	// 57.3=180/pi
		QVector3D n = { -vec.y(), vec.x(), 0 };
		rotation.rotate(-angle, n.normalized());		// use negative to convert from object's perspective to looker's perspective
	}
}

void Ball::grav(float time){
	setDZ(getDZ() - gravity*time);
}

void Ball::floa(float depth, float time){
	setDZ(getDZ() + qMin(2 * depth / radius, 1.5f)*gravity*time);
}

Quaffle::Quaffle(QVector3D center, float radius, QVector3D color)
	:Ball(center, radius, color)
{
}

Quaffle::~Quaffle(){

}

void Quaffle::move(float time){
	Ball::move(time);
	float s = velocity.length();
	velocity = qMax(0.0f, s - friction*time)*velocity.normalized();
}

Bludger::Bludger(QVector3D center, float radius, QVector3D color)
	:Ball(center, radius, color)
{
}

Bludger::~Bludger(){

}

void Bludger::move(float time){
	Ball::move(time);
	float prev = velocity.length();
	if (abs(prev - speed) > FLT_EPSILON){
		velocity.normalize();
		velocity *= (prev + speed) / 2;
	}
}

Snitch::Snitch(QVector3D center, float radius, QVector3D color)
	:Ball(center, radius, color), sleep(false)
{
}

Snitch::~Snitch(){

}

void Snitch::setSleep(bool sleep){
	this->sleep = sleep;
}

void Snitch::move(float time){
	Ball::move(time);
	if (sleep){
		if (velocity.length() > speed){
			velocity = velocity.normalized()*speed;
		}
	}
	else{
		float delta = speed / 10;
		float prev = velocity.length();
		velocity += {randf(-delta, delta), randf(-delta, delta), randf(-delta, delta)};
		if (abs(prev - speed) > FLT_EPSILON){
			velocity.normalize();
			velocity *= (prev + speed) / 2;
		}
	}
}

void Snitch::grav(float time){
	if (sleep){
		Ball::grav(time);
	}
}

void Snitch::floa(float depth, float time){
	if (sleep){
		setDZ(getDZ() + qMin(2 * depth / radius, 2.0f)*gravity*time);
	}
}

CueBall::CueBall(QVector3D center, float radius, QVector3D color)
	:Ball(center, radius, color)
{
}

CueBall::~CueBall(){

}

void CueBall::move(float time){
	Ball::move(time);
	float s = velocity.length();
	velocity = qMax(0.0f, s - friction*time)*velocity.normalized();
}

void CueBall::hit(QVector3D impulse){
	velocity += impulse.normalized() * speed / mass;
}