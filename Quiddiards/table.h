#pragma once

#include "object.h"

class Table : public Object
{
public:
	Table(QVector3D center = { 0.0f, 0.0f, 0.0f });
	~Table();
	float getRugLen() const{ return ruglen; }
	float getRugWid() const{ return rugwid; }
	float getRugHgt() const{ return leghgt + rugthick; }
	float getBtmHgt() const{ return leghgt; }
	float getDamLen() const{ return ruglen + 2 * damthick; }
	float getDamWid() const{ return rugwid + 2 * damthick; }
	float getDamHgt() const{ return leghgt + damhgt; }
	float getLegHgt() const{ return leghgt; }
	float getLegRad() const{ return legrad; }
	float getHoleRad() const{ return holerad; }
	
private:
	const float ruglen = 36.0f;
	const float rugwid = 18.0f;
	const float rugthick = 0.5f;

	const float damthick = 1.0f;
	const float damhgt = 1.0f;

	const float leghgt = 9.0f;
	const float legrad = 0.5f;		//radius of legs

	const float holerad = 0.5f;	//radius of holes
};
