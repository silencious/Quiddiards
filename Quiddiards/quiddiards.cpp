#include "quiddiards.h"
#include <qmatrix4x4.h>
#include "util.h"
#include <qmessagebox.h>
#include <qinputdialog.h>

std::string Quiddiards::texFiles[TEXNUM] = { "rug.jpg", "dam.jpg", "leg.jpg", "stick.jpg", "tail.jpg", "quaffle.jpg", "bludger.jpg", "snitch.jpg", "cueball.jpg", "gryf.jpg", "slyt.jpg", "huff.jpg", "rave.jpg", "grass.jpg", "skybox_up.jpg", "skybox_front.jpg", "skybox_left.jpg", "skybox_back.jpg", "skybox_right.jpg", "skybox_down.jpg", "ground.jpg" };

Quiddiards::Quiddiards(QWidget* parent)
	: QOpenGLWidget(parent)
{
	/* init parameters */
	QSurfaceFormat format;
	format.setDepthBufferSize(24);
	format.setStencilBufferSize(8);
	format.setVersion(3, 2);
	QSurfaceFormat::setDefaultFormat(format);
	initUI();
}

Quiddiards::~Quiddiards()
{
	gluDeleteNurbsRenderer(nurbs);
	gluDeleteQuadric(quad);
	for (std::map<std::string, QOpenGLTexture*>::iterator it = textures.begin(); it != textures.end(); it++){
		delete it->second;
	}
	delete timer;
	delete ps;
	delete ground;
}

void Quiddiards::initUI(){
	ui.setupUi(this);
	connect(ui.actionAbout, SIGNAL(triggered()), this, SLOT(actAbout()));
	connect(ui.actionAmbient, SIGNAL(triggered()), this, SLOT(actAmbient()));
	connect(ui.actionControl, SIGNAL(triggered()), this, SLOT(actControl()));
	connect(ui.actionPause, SIGNAL(triggered()), this, SLOT(actPause()));
	connect(ui.actionQuit, SIGNAL(triggered()), this, SLOT(actQuit()));
	connect(ui.actionSpotlight, SIGNAL(triggered()), this, SLOT(actSpotlight()));
	connect(ui.actionStart, SIGNAL(triggered()), this, SLOT(actStart()));
	connect(ui.actionSunlight, SIGNAL(triggered()), this, SLOT(actSunlight()));
	connect(ui.actionWind, SIGNAL(triggered()), this, SLOT(actWind()));
}

void Quiddiards::initLights(){
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	/* lighting */
	glEnable(GL_LIGHTING);
	/* global light model*/
	//static GLfloat modAmb[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, modAmb);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
	/* material*/
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	static GLfloat matAmb[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	static GLfloat matSpe[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmb);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpe);
	glMateriali(GL_FRONT, GL_SHININESS, 10);

	/* light0, sunlight */
	//glEnable(GL_LIGHT0);
	static GLfloat sunAmb[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	static GLfloat sunDif[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	static GLfloat sunSpe[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, sunAmb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, sunDif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, sunSpe);

	/* light1, spotlight*/
	//glEnable(GL_LIGHT1);
	static GLfloat spotDif[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	//static GLfloat spotSpe[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 30.0f);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 10.0f);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, spotDif);
	//glLightfv(GL_LIGHT1, GL_SPECULAR, spotSpe);

	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.05f);
}

void Quiddiards::initObjects(){
	ground = new Ground({ 0, 0, 0 }, MAXRANGE, WAVEHGT);
	//float desktop = table.getRugHgt();
	float desktop = WAVEHGT / 2;
	table.setCenter({ 0, 0, 0 });

	// init balls and stick
	cueball.setCenter({ 0, 0, desktop + cueball.getR() });
	balls.clear();
	balls.push_back(&cueball);

	float l = cuebroom.getStickLen();
	cuebroom.setAim(cueball.getCenter());
	cuebroom.setCenter(cueball.getCenter() + QVector3D({ l, l / 3, l / 3 }));

	float boundX = COURTRANGE - 1, boundY = COURTRANGE - 1;
	for (unsigned i = 0; i < QUAFNUM; i++){
		do{
			quaffles[i].setCenter({ randf(-boundX, boundX), randf(-boundY, boundY), desktop + quaffles[i].getR() });
		} while (conflict(quaffles[i]));
		balls.push_back(&quaffles[i]);
	}

	float sB = bludgers[0].getSpeed(), sS = snitch.getSpeed();
	for (unsigned i = 0; i < BLUGNUM; i++){
		do{
			bludgers[i].setCenter({ randf(-boundX, boundX), randf(-boundY, boundY), desktop + bludgers[i].getR() });
		} while (conflict(bludgers[i]));
		bludgers[i].setVelocity({ randf(-sB, sB), randf(-sB, sB), 0.0f });
		balls.push_back(&bludgers[i]);
	}
	do{
		snitch.setCenter({ randf(-boundX, boundX), randf(-boundY, boundY), desktop + snitch.getR() + 5.0f });
	} while (conflict(snitch));
	snitch.setVelocity({ randf(-sS, sS), randf(-sS, sS), 0 });
	balls.push_back(&snitch);

	// init flag
	wind = { 1.0f, 1.0f };
	flags[0] = Flag({ COURTRANGE, COURTRANGE, FLAGHGT }, Flag::GRYFFINDOR);
	flags[1] = Flag({ -COURTRANGE, COURTRANGE, FLAGHGT }, Flag::SLYTHERIN);
	flags[2] = Flag({ -COURTRANGE, -COURTRANGE, FLAGHGT }, Flag::RAVENCLAW);
	flags[3] = Flag({ COURTRANGE, -COURTRANGE, FLAGHGT }, Flag::HUFFLEPUFF);
}

void Quiddiards::loadResources(){
	for (unsigned i = 0; i < TEXNUM; i++){
		std::string file = texFiles[i];
		QImage image;
		/* add perlin noise to balls texture */
		if (file == "quaffle.jpg"){
			perlin(image, QColor(255, 0, 0, 0), 0.7);
		}
		else if (file == "bludger.jpg"){
			perlin(image, QColor(0, 0, 255, 0), 0.7);
		}
		else if (file == "snitch.jpg"){
			perlin(image, QColor(255, 214, 0, 0), 0.7);
		}
		else if (file == "cueball.jpg"){
			perlin(image, QColor(240, 240, 240, 0), 0.7);
		}
		else{
			image.load((":/images/" + file).c_str());
		}
		QOpenGLTexture* tex = new QOpenGLTexture(image);
		tex->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
		tex->setWrapMode(QOpenGLTexture::MirroredRepeat);
		textures[file] = tex;
	}
}

void Quiddiards::perlin(QImage& image, QColor rgb, float factor){
	QColor hsv(rgb.toHsv());
	qreal value = hsv.valueF();
	const int rows = 64, cols = 64, octave = 7;
	float wNoise[rows][cols], pNoise[rows*octave][cols*octave];
	for (int i = 0; i < rows; i++){
		for (int j = 0; j < cols; j++){
			wNoise[i][j] = randf(factor, 1);
		}
	}
	image = QImage(rows*octave, cols*octave, QImage::Format_RGB32);
	image.fill(hsv);

	float tTable[octave];
	for (int i = 0; i < octave; i++){
		float t = i / float(octave);
		tTable[i] = t*t*t*(t*(6 * t - 15) + 10);
	}
	for (int i = 0; i < rows*octave; i++){
		int y0 = i / octave, y1 = (y0 + 1) % rows;
		for (int j = 0; j < cols*octave; j++){
			int x0 = j / octave, x1 = (x0 + 1) % cols;
			hsv.setHsvF(hsv.hueF(), hsv.saturationF(), value*quadricInter(wNoise[x0][y0], wNoise[x1][y0], wNoise[x0][y1], wNoise[x1][y1], tTable[j%octave], tTable[i%octave]));
			QRgb rgb = hsv.rgb();
			image.setPixel(j, i, hsv.rgb());
		}
	}
}

inline void Quiddiards::bindTexture(const std::string& filename){
	if (texFile != filename){
		if (textures.find(filename) == textures.end())
		{
			qWarning("No such image...");
		}
		texFile = filename;
	}
	textures[texFile]->bind();
}

inline void Quiddiards::releaseTexture(){
	textures[texFile]->release();
}

void Quiddiards::drawTable(){
	static GLfloat x1 = table.getRugLen() / 2,
		y1 = table.getRugWid() / 2,
		z1 = table.getRugHgt(),
		x2 = table.getDamLen() / 2,
		y2 = table.getDamWid() / 2,
		z2 = table.getDamHgt(),
		z3 = table.getBtmHgt();
	static GLfloat pTable[14 * 4 * 3] = {
		//desktop
		x1, y1, z1, -x1, y1, z1, -x1, -y1, z1, x1, -y1, z1,
		//bottom
		x2, y2, z3, x2, -y2, z3, -x2, -y2, z3, -x2, y2, z3,
		//front inner face
		x1, -y1, z2, x1, y1, z2, x1, y1, z1, x1, -y1, z1,
		//front upper face
		x1, y2, z2, x1, -y2, z2, x2, -y2, z2, x2, y2, z2,
		//front outer face
		x2, y2, z2, x2, -y2, z2, x2, -y2, z3, x2, y2, z3,
		//right inner face
		x1, y1, z2, -x1, y1, z2, -x1, y1, z1, x1, y1, z1,
		//right upper face
		x2, y2, z2, -x2, y2, z2, -x2, y1, z2, x2, y1, z2,
		//right outer face
		-x2, y2, z2, x2, y2, z2, x2, y2, z3, -x2, y2, z3,
		//back inner face
		-x1, y1, z2, -x1, -y1, z2, -x1, -y1, z1, -x1, y1, z1,
		//back upper face
		-x1, -y2, z2, -x1, y2, z2, -x2, y2, z2, -x2, -y2, z2,
		//back outer face
		-x2, -y2, z2, -x2, y2, z2, -x2, y2, z3, -x2, -y2, z3,
		//left inner face
		-x1, -y1, z2, x1, -y1, z2, x1, -y1, z1, -x1, -y1, z1,
		//left upper face
		-x2, -y2, z2, x2, -y2, z2, x2, -y1, z2, -x2, -y1, z2,
		//left outer face
		x2, -y2, z2, -x2, -y2, z2, -x2, -y2, z3, x2, -y2, z3
	};
	static GLfloat pLegs[4 * 3] = {
		//leg centers
		x1, y1, 0, -x1, y1, 0, -x1, -y1, 0, x1, -y1, 0
	};
	static GLfloat pHoles[6 * 3] = {
		//hole centers
		x1 - 0.01, y1 - 0.01, z1 + 0.01,
		x1 - 0.01, -y1 + 0.01, z1 + 0.01,
		-x1 + 0.01, -y1 + 0.01, z1 + 0.01,
		-x1 + 0.01, y1 - 0.01, z1 + 0.01,
		0, y1 - 0.01, z1 + 0.01,
		0, -y1 + 0.01, z1 + 0.01
	};

	static GLfloat pTableTex[14 * 4 * 2] = {
		8, 0, 8, 8, 0, 8, 0, 0,
		8, 0, 8, 8, 0, 8, 0, 0,
		0, 0, 4, 0, 4, 1, 0, 1,
		0, 0, 4, 0, 4, 1, 0, 1,
		0, 0, 4, 0, 4, 1, 0, 1,
		0, 0, 4, 0, 4, 1, 0, 1,
		0, 0, 4, 0, 4, 1, 0, 1,
		0, 0, 4, 0, 4, 1, 0, 1,
		0, 0, 4, 0, 4, 1, 0, 1,
		0, 0, 4, 0, 4, 1, 0, 1,
		0, 0, 4, 0, 4, 1, 0, 1,
		0, 0, 4, 0, 4, 1, 0, 1,
		0, 0, 4, 0, 4, 1, 0, 1,
		0, 0, 4, 0, 4, 1, 0, 1
	};
	glPushMatrix();

	//draw table
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, pTable);
	glTexCoordPointer(2, GL_FLOAT, 0, pTableTex);
	//rug
	//glColor3ub(0x0, 0x5f, 0x0);
	bindTexture("rug.jpg");
	glDrawArrays(GL_QUADS, 0, 4);
	releaseTexture();
	//dam
	//glColor3ub(0x7f, 0x4f, 0x0);
	bindTexture("dam.jpg");
	glDrawArrays(GL_QUADS, 4, 13 * 4);
	releaseTexture();
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	//draw legs
	//glColor3ub(0x7f, 0x3f, 0x0);
	bindTexture("leg.jpg");
	for (int i = 0; i < 4; i++){
		glPushMatrix();
		glTranslatef(pLegs[3 * i], pLegs[3 * i + 1], pLegs[3 * i + 2]);
		gluCylinder(quad, table.getLegRad(), table.getLegRad(), table.getLegHgt(), 12, 6);
		glPopMatrix();
	}
	releaseTexture();

	// draw holes
	//glDisable(GL_CULL_FACE);
	//glColor3ub(0x0, 0x0, 0x0);
	//for (int i = 0; i < 6; i++){
	//	glPushMatrix();
	//	glTranslatef(pHoles[3 * i], pHoles[3 * i + 1], pHoles[3 * i + 2]);
	//	gluDisk(quad, 0, table.getHoleRad(), 12, 1);
	//	glRotatef(90, 1, 0, 0);
	//	gluDisk(quad, 0, table.getHoleRad(), 12, 1);
	//	if (i < 4){
	//		glRotatef(90, 0, 1, 0);
	//		gluDisk(quad, 0, table.getHoleRad(), 12, 1);
	//	}
	//	glPopMatrix();
	//}
	//glEnable(GL_CULL_FACE);

	glPopMatrix();
}

void Quiddiards::drawBall(const Ball& ball){
	glPushMatrix();

	glTranslatef(ball.getX(), ball.getY(), ball.getZ());
	//transpose the matrix because qt uses row-major matrix while opengl use column major matrix
	glMultMatrixf(ball.getRotation().transposed().constData());


	switch (ball.getType()){
	case Ball::QUAFFLE:
		bindTexture("quaffle.jpg");
		break;
	case Ball::BLUDGER:
		bindTexture("bludger.jpg");
		break;
	case Ball::CUEBALL:
		bindTexture("cueball.jpg");
		break;
	case Ball::SNITCH:
		bindTexture("snitch.jpg");
		break;
	default:
		QVector3D color = ball.getColor();
		glColor3f(color.x(), color.y(), color.z());
		break;
	}

	gluSphere(quad, ball.getR(), 16, 16);
	releaseTexture();

	glPopMatrix();
}

void Quiddiards::drawCueBroom(){
	// draw cuebroom only if cueball is not running
	glPushMatrix();

	//glColor3ub(0x7f, 0x3f, 0x0);
	glTranslatef(cuebroom.getX(), cuebroom.getY(), cuebroom.getZ());
	glMultMatrixf(cuebroom.getRotation().transposed().constData());
	bindTexture("stick.jpg");
	gluCylinder(quad, cuebroom.getR1(), cuebroom.getR0(), cuebroom.getStickLen(), 12, 6);
	releaseTexture();
	glPopMatrix();
	//glPushMatrix();
	//glTranslatef(cuebroom.getJoint().x(), cuebroom.getJoint().y(), cuebroom.getJoint().z());
	//gluDisk(quad, 0, cuebroom.getR0(), 1, 1);
	//glPopMatrix();
	//glPushMatrix();
	//glTranslatef(cuebroom.getHead().x(), cuebroom.getHead().y(), cuebroom.getHead().z());
	//gluDisk(quad, 0, cuebroom.getR1(), 1, 1);
	//glPopMatrix();
}

void Quiddiards::drawGround(){
	glPushMatrix();
	//glTranslatef(center.x(), center.y(), 0);
	int n = Ground::RESO - 1;
	bindTexture("ground.jpg");
	glDisable(GL_CULL_FACE);
	glBegin(GL_QUADS);
	for (int x = 0; x < n; x++){
		for (int y = 0; y < n; y++){
			QVector3D p00 = ground->at(x, y), p10 = ground->at(x + 1, y), p11 = ground->at(x + 1, y + 1), p01 = ground->at(x, y + 1);
			QVector3D n = QVector3D::crossProduct(p11 - p00, p01 - p10);
			glNormal3f(n.x(), n.y(), n.z());
			glTexCoord2f(0, 0);
			glVertex3f(p00.x(), p00.y(), p00.z());
			glTexCoord2f(1, 0);
			glVertex3f(p10.x(), p10.y(), p10.z());
			glTexCoord2f(1, 1);
			glVertex3f(p11.x(), p11.y(), p11.z());
			glTexCoord2f(0, 1);
			glVertex3f(p01.x(), p01.y(), p01.z());
		}
	}
	glEnd();
	glEnable(GL_CULL_FACE);
	releaseTexture();
	glPopMatrix();
}

void Quiddiards::drawSkybox(){
	//draw sky
	// Note: the points are clockwise looking from outside, thus ccw from inside
	static GLfloat pSkybox[6 * 4 * 3] = {
		// front
		MAXRANGE, MAXRANGE, MAXRANGE,
		MAXRANGE, MAXRANGE, -MAXRANGE,
		MAXRANGE, -MAXRANGE, -MAXRANGE,
		MAXRANGE, -MAXRANGE, MAXRANGE,
		// right
		-MAXRANGE, MAXRANGE, MAXRANGE,
		-MAXRANGE, MAXRANGE, -MAXRANGE,
		MAXRANGE, MAXRANGE, -MAXRANGE,
		MAXRANGE, MAXRANGE, MAXRANGE,
		// back
		-MAXRANGE, -MAXRANGE, MAXRANGE,
		-MAXRANGE, -MAXRANGE, -MAXRANGE,
		-MAXRANGE, MAXRANGE, -MAXRANGE,
		-MAXRANGE, MAXRANGE, MAXRANGE,
		// left
		MAXRANGE, -MAXRANGE, MAXRANGE,
		MAXRANGE, -MAXRANGE, -MAXRANGE,
		-MAXRANGE, -MAXRANGE, -MAXRANGE,
		-MAXRANGE, -MAXRANGE, MAXRANGE,
		// up
		-MAXRANGE, MAXRANGE, MAXRANGE,
		MAXRANGE, MAXRANGE, MAXRANGE,
		MAXRANGE, -MAXRANGE, MAXRANGE,
		-MAXRANGE, -MAXRANGE, MAXRANGE,
		// down
		MAXRANGE, MAXRANGE, -MAXRANGE,
		-MAXRANGE, MAXRANGE, -MAXRANGE,
		-MAXRANGE, -MAXRANGE, -MAXRANGE,
		MAXRANGE, -MAXRANGE, -MAXRANGE,
	};

	static GLfloat pSkyboxTex[6 * 4 * 2] = {
		0, 0, 0, 1, 1, 1, 1, 0,
		0, 0, 0, 1, 1, 1, 1, 0,
		0, 0, 0, 1, 1, 1, 1, 0,
		0, 0, 0, 1, 1, 1, 1, 0,
		0, 0, 0, 1, 1, 1, 1, 0,
		0, 0, 0, 1, 1, 1, 1, 0,
	};

	static GLfloat pNorm[6 * 3] = {
		-1, 0, 0, 0, -1, 0, 1, 0, 0, 0, 1, 0, 0, 0, -1, 0, 0, 1
	};

	glPushMatrix();
	//glTranslatef(center.x(), center.y(), center.z());
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, pSkybox);
	glTexCoordPointer(2, GL_FLOAT, 0, pSkyboxTex);
	glNormalPointer(GL_FLOAT, 0, pNorm);

	//glColorMaterial(GL_FRONT, GL_SPECULAR);
	//glDisable(GL_LIGHTING);
	bindTexture("skybox_front.jpg");
	glDrawArrays(GL_QUADS, 0, 4);
	releaseTexture();
	bindTexture("skybox_right.jpg");
	glDrawArrays(GL_QUADS, 4, 4);
	releaseTexture();
	bindTexture("skybox_back.jpg");
	glDrawArrays(GL_QUADS, 8, 4);
	releaseTexture();
	bindTexture("skybox_left.jpg");
	glDrawArrays(GL_QUADS, 12, 4);
	releaseTexture();
	bindTexture("skybox_up.jpg");
	glDrawArrays(GL_QUADS, 16, 4);
	releaseTexture();
	bindTexture("skybox_down.jpg");
	//bindTexture("ground.jpg");
	glDrawArrays(GL_QUADS, 20, 4);
	releaseTexture();
	//glEnable(GL_LIGHTING);
	//glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	glPopMatrix();
}

void Quiddiards::drawFlag(Flag& flag){
	/* draw flag stick */
	glPushMatrix();
	glTranslatef(flag.getX(), flag.getY(), 0);

	bindTexture("stick.jpg");
	gluCylinder(quad, flag.getStickR(), flag.getStickR(), flag.getZ(), 12, 6);
	releaseTexture();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(flag.getX(), flag.getY(), flag.getZ());
	glRotatef(-90.0f, 1, 0, 0);
	float theta = 57.3f*atan2f(wind.y(), wind.x());
	glRotatef(theta, 0, -1, 0);
	glScalef(flag.getWid() / flag.RESO, flag.getHgt() / flag.RESO, flag.getBend());
	glTranslatef(0, 0, -flag.at(0, 0, 2));	// stick the flag to the stick

	/* load flag textures */
	switch (flag.getType()){
	case Flag::GRYFFINDOR:
		bindTexture("gryf.jpg");
		break;
	case Flag::SLYTHERIN:
		bindTexture("slyt.jpg");
		break;
	case Flag::HUFFLEPUFF:
		bindTexture("huff.jpg");
		break;
	case Flag::RAVENCLAW:
		bindTexture("rave.jpg");
		break;
	default:
		QVector3D color = flag.getColor();
		glColor3f(color.x(), color.y(), color.z());
		break;
	}
	/* use nurbs surface */
	/* nurbs points */
	//static const int sn = 8, tn = 4;	// surface and texture's u&v array size
	//static GLfloat texpts1[4 * 2] = { 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };
	//static GLfloat texpts2[4 * 2] = { 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f };
	//static GLfloat knots[sn] = { 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f };
	//static GLfloat tknots[tn] = { 0.0f, 0.0f, 1.0f, 1.0f };
	///* front */
	//static int n = Flag::RESO;
	//gluBeginSurface(nurbs);
	//gluNurbsSurface(nurbs, tn, tknots, tn, tknots, 2 * 2, 2, texpts1, tn / 2, tn / 2, GL_MAP2_TEXTURE_COORD_2);
	//gluNurbsSurface(nurbs, sn, knots, sn, knots, 3 * n, 3, flag.getPoints(), sn / 2, sn / 2, GL_MAP2_VERTEX_3);
	//gluEndSurface(nurbs);
	///* back */
	//glFrontFace(GL_CW);
	//gluBeginSurface(nurbs);
	//gluNurbsSurface(nurbs, tn, tknots, tn, tknots, 2 * 2, 2, texpts2, tn / 2, tn / 2, GL_MAP2_TEXTURE_COORD_2);
	//gluNurbsSurface(nurbs, sn, knots, sn, knots, 3 * n, 3, flag.getPoints(), sn / 2, sn / 2, GL_MAP2_VERTEX_3);
	//gluEndSurface(nurbs);
	//glFrontFace(GL_CCW);

	/* use quadrics */
	float x1, y1, x2, y2;
	int n = Flag::RESO - 1;
	glBegin(GL_QUADS);
	for (int x = 0; x < n; x++){
		for (int y = 0; y < n; y++){
			x1 = float(x) / n;
			y1 = float(y) / n;
			x2 = float(x + 1) / n;
			y2 = float(y + 1) / n;

			glTexCoord2f(x1, y1);
			glVertex3f(flag.at(x, y, 0), flag.at(x, y, 1), flag.at(x, y, 2));
			glTexCoord2f(x1, y2);
			glVertex3f(flag.at(x, y + 1, 0), flag.at(x, y + 1, 1), flag.at(x, y + 1, 2));
			glTexCoord2f(x2, y2);
			glVertex3f(flag.at(x + 1, y + 1, 0), flag.at(x + 1, y + 1, 1), flag.at(x + 1, y + 1, 2));
			glTexCoord2f(x2, y1);
			glVertex3f(flag.at(x + 1, y, 0), flag.at(x + 1, y, 1), flag.at(x + 1, y, 2));
		}
	}
	glEnd();

	glBegin(GL_QUADS);
	for (int x = 0; x < n; x++){
		for (int y = 0; y < n; y++){
			x1 = float(x) / n;
			y1 = float(y) / n;
			x2 = float(x + 1) / n;
			y2 = float(y + 1) / n;

			glTexCoord2f(x1, y1);
			glVertex3f(flag.at(n - x, y, 0), flag.at(n - x, y, 1), flag.at(n - x, y, 2));
			glTexCoord2f(x1, y2);
			glVertex3f(flag.at(n - x, y + 1, 0), flag.at(n - x, y + 1, 1), flag.at(n - x, y + 1, 2));
			glTexCoord2f(x2, y2);
			glVertex3f(flag.at(n - 1 - x, y + 1, 0), flag.at(n - 1 - x, y + 1, 1), flag.at(n - 1 - x, y + 1, 2));
			glTexCoord2f(x2, y1);
			glVertex3f(flag.at(n - 1 - x, y, 0), flag.at(n - 1 - x, y, 1), flag.at(n - 1 - x, y, 2));
		}
	}
	glEnd();
	releaseTexture();
	glPopMatrix();
}

/* render particle system */
void Quiddiards::renderPS(){
	glDisable(GL_LIGHTING);
	for (vector<Particle>::iterator it = ps->getParticles().begin(); it != ps->getParticles().end(); it++){
		float alpha = 1 - it->age / it->life;	//calculate alpha according to particle age
		if (alpha <= 0){
			continue;
		}
		QVector3D pos = it->getCenter();
		QVector3D color = it->color;
		glColor4f(color.x(), color.y(), color.z(), alpha);
		//glColor3f(color.x(), color.y(), color.z());
		glPushMatrix();
		glTranslatef(pos.x(), pos.y(), pos.z());
		gluSphere(quad, it->size, 6, 6);
		glPopMatrix();
	}
	glColor3f(1.0f, 1.0f, 1.0f);
	glEnable(GL_LIGHTING);
}

void Quiddiards::initializeGL(){
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glEnable(GL_AUTO_NORMAL);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//glDisable(GL_BLEND);
	//glBlendFunc(GL_ONE, GL_ZERO);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	/* camera */
	camera = FOLLOW;
	theta = 45;
	phi = 45;
	radius = MAXRANGE / 2;

	initObjects();	// init objects first, since others may base on them
	loadResources();
	initLights();


	ambLight = true;
	sunLight = true;
	spotLight = true;
	pause = false;

	/* flag nurbs object */
	nurbs = gluNewNurbsRenderer();
	gluNurbsProperty(nurbs, GLU_SAMPLING_TOLERANCE, 25.0);
	gluNurbsProperty(nurbs, GLU_DISPLAY_MODE, GLU_FILL);
	/* quadric object */
	quad = gluNewQuadric();
	gluQuadricDrawStyle(quad, GLU_FILL);
	gluQuadricNormals(quad, GLU_SMOOTH);
	gluQuadricTexture(quad, true);

	timer = new QTimer;
	connect(timer, SIGNAL(timeout()), SLOT(animate()));
	timer->start(interval);
	msec = clock();

	/* particle system */
	ps = new ParticleSystem(200, -10.0f);
}

void Quiddiards::paintGL(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClear(GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/* set camera */
	QMatrix4x4 mat;				// identity matrix
	mat.rotate(phi, 0, 0, 1);	// rotate phi around z axis
	mat.rotate(theta, 1, 0, 0);	// rotate around x axis
	mat.rotate(theta, 0, 1, 0);	// rotate around y axis
	eye = mat * QVector3D(0, 0, radius);
	QVector3D up = QVector3D::crossProduct(eye, { -eye.y(), eye.x(), 0.0f });	// note: coord system is left-handwise
	switch (camera){
	case FOLLOW:
		center = cueball.getCenter();
		break;
	case OVER:
		center = { 0, 0, table.getDamHgt() };
		break;
	case FREE:
		break;
	default:
		break;
	}

	gluLookAt(eye.x() + center.x(), eye.y() + center.y(), eye.z() + center.z(), center.x(), center.y(), center.z(), up.x(), up.y(), up.z());

	/* enable or disable lights based on setting */
	if (ambLight){
		static GLfloat modAmb[] = { 0.4f, 0.4f, 0.4f, 1.0f };
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, modAmb);
	}
	else{
		static GLfloat modAmb[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, modAmb);
	}
	if (sunLight){
		static GLfloat sunPos[] = { 1.0f, 1.0f, 1.0f, 0.0f };
		glLightfv(GL_LIGHT0, GL_POSITION, sunPos);
		glEnable(GL_LIGHT0);
	}
	else{
		glDisable(GL_LIGHT0);
	}
	if (spotLight){
		spotPos = { cueball.getX(), cueball.getY(), cueball.getZ() + 5.0f, 1.0f };
		QVector3D dir = cueball.getCenter() - spotPos.toVector3D();
		glLightfv(GL_LIGHT1, GL_POSITION, (float*)&spotPos);
		glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, (float*)&dir);
		glEnable(GL_LIGHT1);
	}
	else{
		glDisable(GL_LIGHT1);
	}

	/* start drawing */
	glEnable(GL_TEXTURE_2D);
	/* draw scene */
	drawSkybox();
	drawGround();
	//drawTable();
	/* draw quaffles */
	for (unsigned i = 0; i < QUAFNUM; i++){
		drawBall(quaffles[i]);
	}
	/* draw bludgers */
	for (unsigned i = 0; i < BLUGNUM; i++){
		drawBall(bludgers[i]);
	}
	/* draw snitch */
	drawBall(snitch);
	/* draw cueball */
	drawBall(cueball);
	//if (operable){
	//	drawCueBroom();
	//}
	for (int i = 0; i < FLAGNUM; i++){
		drawFlag(flags[i]);
	}
	glDisable(GL_TEXTURE_2D);
	if (ps->isActive()){
		renderPS();
	}
}

void Quiddiards::resizeGL(int width, int height){
	if (height == 0)
	{
		height = 1;
	}
	glViewport(0, 0, (GLint)width, (GLint)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (GLfloat)width / (GLfloat)height, 0.1, MAXRANGE*3.5f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Quiddiards::keyPressEvent(QKeyEvent *event){
	switch (event->key()) {
	case Qt::Key_F2:
		actStart();
		break;
	case Qt::Key_P:
		actPause();
		break;
	case Qt::Key_Escape:
	case Qt::Key_Q:
		close();
		break;
	case Qt::Key_Space:
	case Qt::Key_Enter:
	case Qt::Key_Return:
		break;
	case Qt::Key_W:{
		/* forward cueball */
		QVector3D n = -eye;
		n.setZ(0);
		n.normalize();
		n += cueball.getVelocity();
		if (n.length() > cueball.getSpeed()){
			n = cueball.getSpeed()*n.normalized();
		}
		cueball.setVelocity(n);
		break;
	}
	case Qt::Key_A:
		phi += 10;
		break;
	case Qt::Key_S:{
		QVector3D n = eye;
		n.setZ(0);
		n.normalize();
		n += cueball.getVelocity();
		if (n.length() > cueball.getSpeed()){
			n = cueball.getSpeed()*n.normalized();
		}
		cueball.setVelocity(n);
		break;
	}
	case Qt::Key_D:
		phi -= 10;
		break;
	case Qt::Key_Tab:
		//camera = CAMERA((camera + 1) % 2);
		break;
	default:
		return;
	}
	update();
}

void Quiddiards::mousePressEvent(QMouseEvent *event){
	lastMousePosition = event->pos();
	event->accept();
}

void Quiddiards::mouseMoveEvent(QMouseEvent *event){
	if (event->buttons() & Qt::RightButton) {
		int dX = event->x() - lastMousePosition.x();
		int dY = event->y() - lastMousePosition.y();
		phi -= dX;
		//while (phi < 0) {
		//	phi += 360;
		//}
		//while (phi >= 360) {
		//	phi -= 360;
		//}
		theta -= dY;
		if (theta < 1) {
			theta = 1;
		}
		if (theta >= 89) {
			theta = 89;
		}
		update();
	}
	lastMousePosition = event->pos();
	event->accept();
}

void Quiddiards::wheelEvent(QWheelEvent *event){
	if (event->orientation() == Qt::Vertical) {
		int delta = event->delta();
		if (delta < 0) {
			radius *= 1.2;
		}
		else if (delta > 0) {
			radius *= 0.8;
		}
		if (radius > MAXRANGE){
			radius = MAXRANGE;
		}
		else if (radius < 2){
			radius = 2;
		}
		update();
	}
	event->accept();
}

bool Quiddiards::conflict(const Ball& ball){
	for (unsigned i = 0; i < balls.size(); i++){
		if (ball.collide(*balls[i])){
			return true;
		}
	}
	return false;
}

void Quiddiards::handleCollision(){
	for (unsigned i = 0; i < balls.size(); i++){
		Ball* bp = balls[i];
		// first test collision with bound
		if (bp->getX() > 0 && bp->getX() + bp->getR() > COURTRANGE && bp->getDX() > 0){
			bp->setX(COURTRANGE - bp->getR());
			bp->setDX(-bp->getDX());
		}
		if (bp->getX() < 0 && bp->getX() - bp->getR() < -COURTRANGE && bp->getDX() < 0){
			bp->setX(-COURTRANGE + bp->getR());
			bp->setDX(-bp->getDX());
		}
		if (bp->getY() > 0 && bp->getY() + bp->getR() > COURTRANGE && bp->getDY() > 0){
			bp->setY(COURTRANGE - bp->getR());
			bp->setDY(-bp->getDY());
		}
		if (bp->getY() < 0 && bp->getY() - bp->getR() < -COURTRANGE && bp->getDY() < 0){
			bp->setY(-COURTRANGE + bp->getR());
			bp->setDY(-bp->getDY());
		}
		//if (bp->getZ() - bp->getR() < table.getRugHgt() && bp->getDZ() < 0){
		//	bp->setZ(table.getRugHgt() + bp->getR());
		//	//bp->setDZ(-0.9*bp->getDZ());
		//	if (bp->getType() == Ball::SNITCH){
		//		if (((Snitch*)bp)->isSleep()){
		//			bp->setVelocity({ 0.0f, 0.0f, 0.0f });
		//		}
		//		else{
		//			bp->setDZ(-bp->getDZ());
		//		}
		//	}
		//	else{
		//		bp->setDZ(0);
		//	}
		//}
		//if (bp->getZ() + bp->getR() > 2 * table.getRugHgt() && bp->getDZ() > 0){
		//	bp->setDZ(-bp->getDZ());
		//}

		/* test collision with other balls, once for each pair */
		for (unsigned j = i + 1; j < balls.size(); j++){
			if (!bp->collide(*balls[j])){
				collision.erase(std::make_pair(i, j));
			}
			else{
				if (collision.find(std::make_pair(i, j)) != collision.end()){
					continue;
				}
				else{
					/* collide, show spark */
					collision.emplace(std::make_pair(i, j));
					ps->spark((bp->getCenter() + balls[j]->getCenter()) / 2);
				}
				//float dz = 0;
				//if (bp->getType() == Ball::SNITCH){
				//	dz = bp->getDZ();
				//	bp->setDZ(0);
				//}
				//if (balls[j]->getType() == Ball::SNITCH){
				//	dz = balls[j]->getDZ();
				//	balls[j]->setDZ(0);
				//}
				static float e = 0.9;	// coefficient of restitution
				QVector3D axis = balls[j]->getCenter() - bp->getCenter();
				axis.normalize();
				QVector3D v1 = bp->getVelocity(), v2 = balls[j]->getVelocity();
				float s1 = QVector3D::dotProduct(v1, axis), s2 = QVector3D::dotProduct(v2, axis),
					m1 = bp->getMass(), m2 = balls[j]->getMass();
				float sf1 = (m1*s1 + m2*s2 + e*m2*(s2 - s1)) / (m1 + m2);
				float sf2 = (m1*s1 + m2*s2 + e*m1*(s1 - s2)) / (m1 + m2);
				bp->setVelocity(v1 - (s1 - sf1)*axis);
				balls[j]->setVelocity(v2 - (s2 - sf2)*axis);

				//if (bp->getType() == Ball::SNITCH){
				//	bp->setDZ(-dz);
				//}
				//if (balls[j]->getType() == Ball::SNITCH){
				//	balls[j]->setDZ(-dz);
				//}
				if (balls[j]->getType() == Ball::SNITCH){
					balls[j]->setDZ(-balls[j]->getDZ());
				}
			}
		}
	}
}

void Quiddiards::animate(){
	if (pause){
		return;
	}
	handleCollision();
	clock_t gap = clock() - msec;
	msec += gap;
	float sec = gap / 1000.0f;
	ground->wave(sec);
	/* wave flags */
	for (int i = 0; i < FLAGNUM; i++){
		flags[i].wave(sec * wind.length());
	}
	/* move balls */
	for (unsigned i = 0; i < balls.size(); i++){
		balls[i]->move(sec);
		balls[i]->grav(sec);
		/* test touch with ground */
		float depth;
		if ((depth = ground->depth(balls[i]->getBottom())) > 0){
			balls[i]->floa(depth, sec);
		}
		//if (balls[i]->getType() == Ball::CUEBALL){
		//	operable = (balls[i]->getVelocity().length() < FLT_EPSILON);
		//	if (operable){
		//		cuebroom.setAim(balls[i]->getCenter());
		//	}
		//	else{
		//		/* change spotlight direction according to cueball movement */
		//		//glLoadIdentity();
		//		//spotDir = (cueball.getCenter() - spotPos.toVector3D()).normalized();
		//		//glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, (float*)&spotDir);
		//		//spotPos = { cueball.getX(), cueball.getY(), 15.0f ,1.0};
		//		//glLightfv(GL_LIGHT1, GL_POSITION, (float*)&spotPos);
		//	}
		//}
	}
	handleCollision();

	/* manage snitch states */
	static float cnt = 0;
	cnt += sec;
	if (cnt > snitch.getInterval()){
		cnt = 0;
		if (snitch.isSleep()){
			snitch.setVelocity({ 0, 0, snitch.getSpeed() / 5 });
			snitch.setSleep(false);
		}
		else{
			snitch.setVelocity({ 0, 0, -snitch.getSpeed() });
			snitch.setSleep(true);
		}
	}
	/* particle system */
	ps->simulate(sec);
	update();
}

void Quiddiards::actAbout(){
	QMessageBox box(QMessageBox::NoIcon,
		"About",
		"This is a simple demo for computer graphics with opengl",
		QMessageBox::Ok);
	box.setVisible(true);
	box.exec();
}

void Quiddiards::actAmbient(){
	if (ui.actionAmbient->isChecked()){
		ambLight = true;
	}
	else{
		ambLight = false;
	}
	update();
}

void Quiddiards::actControl(){
	QMessageBox box(QMessageBox::NoIcon,
		"Control",
		"Hold right mouse button - rotate view\nEsc/Q - exit\nArrows - move ball",
		QMessageBox::Ok);
	box.setVisible(true);
	box.exec();
}

void Quiddiards::actPause(){
	if (pause){
		timer->start(interval);
		msec = clock();
	}
	else{
		timer->stop();
	}
	pause = !pause;
	ui.actionPause->setChecked(pause);
}

void Quiddiards::actQuit(){
	close();
}

void Quiddiards::actSpotlight(){
	if (ui.actionSpotlight->isChecked()){
		spotLight = true;
	}
	else{
		spotLight = false;
	}
	update();
}

void Quiddiards::actStart(){
	pause = true;
	initObjects();
	pause = false;
	update();
}

void Quiddiards::actSunlight(){
	if (ui.actionSunlight->isChecked()){
		sunLight = true;
	}
	else{
		sunLight = false;
	}
	update();
}

void Quiddiards::actWind(){
	float x = QInputDialog::getDouble(this, tr("Set Wind Direction and Intensity"),
		tr("X:"), wind.x(), -2.0f, 2.0f);
	float y = QInputDialog::getDouble(this, tr("Set Wind Direction and Intensity"),
		tr("Y:"), wind.y(), -2.0f, 2.0f);
	wind = { x, y };
}