#include "cuebroom.h"

CueBroom::CueBroom(QVector3D center, QVector3D aim)
	:Object(center), aim(aim)
{

}

CueBroom::~CueBroom(){

}

QMatrix4x4 CueBroom::getRotation() const{
	QMatrix4x4 m;
	QVector3D n = QVector3D::normal(getDirection(), { 0, 0, 1 });
	float angle = 57.3 * acosf(QVector3D::dotProduct(getDirection().normalized(), { 0, 0, 1 }));
	m.rotate(angle, n);
	return m;
}

void CueBroom::setAim(QVector3D aim){
	center += (aim - this->aim);
	this->aim = aim;
}
void CueBroom::setDirection(QVector3D direction){
	this->aim = center + direction;
}

void CueBroom::rotate(int key){
	QVector3D z = { 0, 0, aim.z() };
	z.normalize();
	//QVector3D n = QVector3D::normal(z, aim - center);
	QMatrix4x4 m;
	switch (key){
		//case Qt::Key_W:
		//	m.rotate(5, n);
		//	break;
	case Qt::Key_A:
		m.rotate(-1, z);
		break;
		//case Qt::Key_S:
		//	m.rotate(-5, n);
		//	break;
	case Qt::Key_D:
		m.rotate(1, z);
		break;
	default:
		break;
	}
	QVector3D diff = { center.x() - aim.x(), center.y() - aim.y(), center.z() - aim.z() };
	center = aim + diff*m.transposed();
}