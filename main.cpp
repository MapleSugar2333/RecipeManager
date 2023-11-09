#include "Utils.h"
#include "RecipeManager.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[]) {
	QApplication::setAttribute(Qt::AA_Use96Dpi);
	qputenv("QT_SCALE_FACTOR", QString::number(1.0).toLatin1());
	QApplication a(argc, argv);
	if (Utils::initDBConnection()) {
		RecipeManager w;
		w.show();
		return a.exec();
	}
	return 1;
}
