#include "object.h"
Object::Object(QVector3D center, float mass)
	:center(center), mass(mass)
{

}

Object::~Object(){

}

void Object::setX(float x){
	center.setX(x);
}
void Object::setY(float y){
	center.setY(y);
}
void Object::setZ(float z){
	center.setZ(z);
}

void Object::setCenter(QVector3D center){
	this->center = center;
}

void Object::setMass(float mass){
	this->mass = mass;
}