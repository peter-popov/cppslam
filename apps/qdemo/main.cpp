#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include "SensorSettingsModel.hpp"	

int main(int argc, char *argv[])
{	

	qmlRegisterType<SensorSettingsModel>("qdemo.models", 1, 0, "SensorSettingsModel");


    QApplication app(argc, argv);
    QQmlApplicationEngine engine("mainwindow.qml");
    return app.exec();
}