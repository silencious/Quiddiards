#include "quiddiards.h"
#include <QApplication>
#include <exception>

int main(int argc, char *argv[])
{
	try{
		QApplication app(argc, argv);
		app.setApplicationName("Quiddiards");
		app.setApplicationVersion("1.0");
		Q_INIT_RESOURCE(quiddiards);

		Quiddiards window;
		window.show();
		return app.exec();
	}
	catch (exception& e){
		
		return 1;
	}
}
