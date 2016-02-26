#pragma once

#include "object.h"
#include "qmatrix4x4.h"

class CueBroom : public Object
{
public:
	CueBroom(QVector3D center = { 1, 1, 1 }, QVector3D aim = { 0, 0, 0 });
	~CueBroom();

	float getR0() const{ return r0; }
	float getR1() const{ return r1; }
	float getStickLen() const{ return stickLen; }
	float getTailLen() const{ return tailLen; }
	QVector3D getHead() const{ return center + stickLen / 2 * (getDirection().normalized()); }
	QVector3D getJoint() const{ return center - stickLen / 2 * (getDirection().normalized()); }
	QVector3D getEnd() const{ return center - (stickLen / 2 + tailLen)*(getDirection().normalized()); }
	QMatrix4x4 getRotation() const;
	QVector3D getDirection() const { return aim - center; }
	void setAim(QVector3D);
	void setDirection(QVector3D);
	void rotate(int key);

private:
	QVector3D aim;
	const float r0 = 0.03f;
	const float r1 = 0.08f;
	const float stickLen = 10.0f;
	const float tailLen = 5.0f;
};