#pragma once
#include "object.h"

class Flag : public Object
{
public:
	enum FlagType{ GRYFFINDOR, SLYTHERIN, HUFFLEPUFF, RAVENCLAW };
	static const int RESO = 12;

	Flag(QVector3D center = { 0.0f, 0.0f, 0.0f }, FlagType type = GRYFFINDOR);
	~Flag();
	FlagType getType()const{ return type; }
	float at(unsigned i, unsigned j, unsigned k){ return points[i][j][k]; }
	float* getPoints(){ return (float*)points; }
	QVector3D getColor() const{
		switch (type){
		case GRYFFINDOR:
			return{ 1.0f, 0.0f, 0.0f };
		case SLYTHERIN:
			return{ 0.0f, 1.0f, 0.0f };
		case HUFFLEPUFF:
			return{ 1.0f, 1.0f, 0.0f };
		case RAVENCLAW:
			return{ 0.0f, 0.0f, 1.0f };
		default:
			return{ 0.0f, 0.0f, 0.0f };
		}
	}
	void wave(float delta = 1.0f);
	float getWid()const{ return 8.0f; }
	float getHgt()const{ return 6.0f; }
	float getBend()const{ return 1.0f; }
	float getStickR()const{ return 0.1f; }
private:
	FlagType type;
	float points[RESO][RESO][3];
};

