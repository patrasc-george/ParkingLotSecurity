#include "mainwindow.h"
#include "httpServer.h"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	MainWindow window;

	if (window.getIsPassword())
	{
		window.show();

		Server server;

		return app.exec();
	}

	return 0;
}
