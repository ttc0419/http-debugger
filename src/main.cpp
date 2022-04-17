#include "MainWidget.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QTranslator translator;
	MainWidget widget;

	const QStringList uiLanguages = QLocale::system().uiLanguages();
	for (const QString &locale : uiLanguages) {
		if (translator.load(":/i18n/" + QLocale(locale).name())) {
			app.installTranslator(&translator);
			widget.setLocale(QLocale(locale));
			break;
		}
	}

	widget.show();
	return app.exec();
}
