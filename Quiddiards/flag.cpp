#include "flag.h"
#include "util.h"

Flag::Flag(QVector3D center, FlagType type)
	:Object(center), type(type)
{
	for (int x = 0; x < RESO; x++){
		for (int y = 0; y < RESO; y++){
			points[x][y][0] = x;		// x axis
			points[x][y][1] = y;		// y axis
			points[x][y][2] = sin(x * 2 * 3.14f / RESO);
		}
	}
}

Flag::~Flag()
{
}

void Flag::wave(float dt){
	static float rad = 0;
	static const float interval = 0.07f;
	rad += dt;
	while (rad > interval){
		rad -= interval;
		for (int y = 0; y < RESO; y++){
			float temp = points[0][y][2];
			for (int x = 0; x < RESO - 1; x++){
				points[x][y][2] = points[x + 1][y][2];
			}
			points[RESO - 1][y][2] = temp;
		}
	}
}