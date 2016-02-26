#include "ground.h"
#include "util.h"

Ground::Ground(QVector3D center, float range, float height)
	:Object(center), range(range), height(height)
{
	if (range != 0){
		init();
	}
}


Ground::~Ground()
{
}

void Ground::init(){
	const int octave = 8, tileNum = RESO / octave;
	const float footage = range * 2 / RESO;
	for (int i = 0; i < RESO; i += octave){
		for (int j = 0; j < RESO; j += octave){
			points[i][j] = { (i - RESO / 2)*footage, (j - RESO / 2)*footage, randf(0.0f, height) };
		}
	}
	float tTable[octave];
	for (int i = 0; i < octave; i++){
		float t = i / float(octave);
		tTable[i] = t*t*t*(t*(6 * t - 15) + 10);
	}
	for (int i = 0; i < RESO; i++){
		int x0 = roundDown(i, octave), x1 = (x0 + octave) % RESO;
		for (int j = 0; j < RESO; j++){
			int y0 = roundDown(j, octave), y1 = (y0 + octave) % RESO;
			points[i][j] = { (i - RESO / 2)*footage, (j - RESO / 2)*footage, quadricInter(points[x0][y0].z(), points[x1][y0].z(), points[x0][y1].z(), points[x1][y1].z(), tTable[i%octave], tTable[j%octave]) };
		}
	}
}

void Ground::wave(float dt){
	static float rad = 0;
	rad += dt;
	static const float interval = 0.25f;
	while (rad > interval){
		rad = rad - interval;
		for (int y = 0; y < RESO; y++){
			float temp = points[0][y][2];
			for (int x = 0; x < RESO - 1; x++){
				points[x][y][2] = points[x + 1][y][2];
			}
			points[RESO - 1][y][2] = temp;
		}
	}
}

float Ground::depth(QVector3D position){
	const float footage = range * 2 / RESO;
	float x = position.x() / footage + RESO / 2, y = position.y() / footage + RESO / 2;
	int x0 = int(floorf(x)), x1 = (x0 + 1) % RESO, y0 = int(floorf(y)), y1 = (y0 + 1) % RESO;
	return (quadricInter(points[x0][y0].z(), points[x0][y1].z(), points[x1][y0].z(), points[x1][y1].z(), x - x0, y - y0) - position.z());
}