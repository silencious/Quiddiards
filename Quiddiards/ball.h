#pragma once

#include <object.h>
#include <qmatrix4x4.h>

class Ball : public Object
{
public:
	Ball(QVector3D center = { 0.0f, 0.0f, 0.0f }, float radius = 0.5f, QVector3D color = { 1.0f, 1.0f, 1.0f });
	~Ball();

	float getR() const{ return radius; }
	float getDX() const{ return velocity.x(); }
	float getDY() const{ return velocity.y(); }
	float getDZ() const{ return velocity.z(); }
	void setDX(float);
	void setDY(float);
	void setDZ(float);

	QVector3D getBottom() const{ return{ getX(), getY(), getZ() - getR() }; }
	QVector3D getColor() const{ return color; }
	QMatrix4x4 getRotation() const{ return rotation; }
	QVector3D getVelocity() const{ return velocity; }
	void setColor(QVector3D);
	void setDirection(QVector3D);
	void setVelocity(QVector3D);
	virtual void move(float time = 1.0f);
	virtual void grav(float time = 1.0f);
	virtual void floa(float depth, float time = 1.0f);
	static bool collide(const Ball& b1, const Ball& b2) { return (b1.center - b2.center).length() < b1.radius + b2.radius; }
	bool collide(const Ball& b) const { return collide(*this, b); }

	virtual unsigned getType() const { return BALL; }
	virtual float getSpeed() const{ return speed; }

public:
	enum type{ BALL, QUAFFLE, BLUDGER, SNITCH, CUEBALL };

protected:
	const float radius;
	QVector3D color;
	QMatrix4x4 rotation;
	QVector3D velocity;
	const float friction = 0.5f;
	const float gravity = 9.8f;
	float speed = 1.0f;
};

class Quaffle : public Ball{
public:
	Quaffle(QVector3D center = { 0.0f, 0.0f, 0.0f }, float radius = 0.54f, QVector3D color = { 1.0f, 0.0f, 0.0f });
	~Quaffle();
	virtual unsigned getType() const { return QUAFFLE; }
	virtual void move(float time = 1.0f);
};

class Bludger : public Ball{
public:
	Bludger(QVector3D center = { 0.0f, 0.0f, 0.0f }, float radius = 0.5f, QVector3D color = { 0.0f, 0.0f, 1.0f });
	~Bludger();
	virtual unsigned getType() const { return BLUDGER; }
	virtual void move(float time = 1.0f);
	virtual float getSpeed() const{ return speed; }
protected:
	float speed = 3.5f;
};

class Snitch : public Ball{
public:
	Snitch(QVector3D center = { 0.0f, 0.0f, 0.0f }, float radius = 0.4f, QVector3D color = { 1.0f, 0.84f, 0.0f });
	~Snitch();
	virtual unsigned getType() const { return SNITCH; }
	virtual void move(float time = 1.0f);
	virtual void grav(float time = 1.0f);
	virtual void floa(float depth, float time = 1.0f);
	bool isSleep() const{ return sleep; }
	void setSleep(bool);
	unsigned getInterval() const{ return interval; }
	virtual float getSpeed() const{ return speed; }
protected:
	float speed = 5.0f;
private:
	bool sleep;
	const float interval = 8;	// 6 seconds
};
class CueBall : public Ball{
public:
	CueBall(QVector3D center = { 0.0f, 0.0f, 0.0f }, float radius = 0.52f, QVector3D color = { 1.0f, 1.0f, 1.0f });
	~CueBall();
	virtual unsigned getType() const { return CUEBALL; }
	virtual void move(float time = 1.0f);
	void hit(QVector3D impulse);
	virtual float getSpeed() const{ return speed; }
protected:
	float speed = 5.0f;
};
