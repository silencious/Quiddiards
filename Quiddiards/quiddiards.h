#pragma once

#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <qtimer.h>
#include <QOpenGLWidget>
#include <QOpenGLTexture>
/* it is required to include windows.h before glu.h */
#include <windows.h>
#include <GL\GLU.h>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <ctime>
#include "ui_quiddiards.h"
#include "ball.h"
#include "table.h"
#include "cuebroom.h"
#include "flag.h"
#include "particle.h"
#include "ground.h"

class Quiddiards : public QOpenGLWidget
{
	Q_OBJECT

public:
	Quiddiards(QWidget* parent = 0);
	~Quiddiards();

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);

	void keyPressEvent(QKeyEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);

private:
	void initUI();
	void initLights();
	void initObjects();
	void initGround();
	void loadResources();
	void setShaders();
	void perlin(QImage& image, QColor color, float factor);
	inline void bindTexture(const std::string& filename);
	inline void releaseTexture();
	void drawTable();
	void drawBall(const Ball& ball);
	void drawCueBroom();
	void drawGround();
	void drawSkybox();
	void drawFlag(Flag& flag);
	void renderPS();

	//collision related
	bool conflict(const Ball& ball);
	void handleCollision();

private:
	// constant parameters
	const float WAVEHGT = 1.0f;
	static const int QUAFNUM = 6;
	static const int BLUGNUM = 6;
	static const int TEXNUM = 21;
	const unsigned interval = 5;
	const float MAXRANGE = 50.0f;
	const float COURTRANGE = 18.0f;
	const float FLAGHGT = 20.0f;
	static const int FLAGNUM = 4;

	//gui
	Ui::QuiddiardsClass ui;
	float theta;		// angle with z axis
	float phi;			// angle between shadow and x axis
	float radius;		// distance from the center
	QPoint lastMousePosition;
	QVector3D center;
	QVector3D eye;
	enum CAMERA{OVER, FOLLOW, FREE};
	CAMERA camera;

	bool ambLight;
	bool sunLight;
	bool spotLight;

	//scene
	Ground* ground;
	Table table;
	CueBroom cuebroom;
	Quaffle quaffles[QUAFNUM];
	Bludger bludgers[BLUGNUM];
	Snitch snitch;
	CueBall cueball;
	std::vector<Ball*> balls;

	Flag flags[FLAGNUM];
	GLUnurbs* nurbs;
	GLUquadric* quad;
	QVector4D spotPos;

	static std::string texFiles[TEXNUM];
	std::string texFile;
	std::map<std::string, QOpenGLTexture*> textures;
	QVector2D wind;

	//animation
	QTimer* timer;
	clock_t msec;
	std::set<std::pair<unsigned, unsigned>> collision;
	bool operable;
	bool pause;
	ParticleSystem* ps;

	private slots:
	void animate();
	void actAbout();
	void actAmbient();
	void actControl();
	void actPause();
	void actQuit();
	void actSpotlight();
	void actStart();
	void actSunlight();
	void actWind();
};