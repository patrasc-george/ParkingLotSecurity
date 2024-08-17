#include "mainwindow.h"
#include "httpServer.h"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	MainWindow window;
	window.show();

	Server server;

	return app.exec();
}
