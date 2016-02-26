#pragma once
#include "object.h"

class Ground :
	public Object
{
public:
	Ground(QVector3D center = { 0, 0, 0 }, float range = 0, float height = 0);
	~Ground();
	void init();
	void wave(float dt);
	QVector3D at(unsigned i, unsigned j)const{ return points[i][j]; }
	float depth(QVector3D);

	static const int RESO = 256;
	float range;
	float height;
	QVector3D points[RESO][RESO];

};

