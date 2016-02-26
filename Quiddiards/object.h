#pragma once

#include <qvector3d.h>

class Object{
public:
	Object(QVector3D center = { 0, 0, 0 }, float mass = 1.0f);
	~Object();

	float getX() const{ return center.x(); }
	float getY() const{ return center.y(); }
	float getZ() const{ return center.z(); }
	void setX(float);
	void setY(float);
	void setZ(float);

	QVector3D getCenter() const{ return center; }
	void setCenter(QVector3D);
	float getMass() const{ return mass; }
	void setMass(float);
protected:
	QVector3D center;
	float mass;
};
