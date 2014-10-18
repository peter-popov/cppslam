#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include "SensorSettingsModel.hpp"
#include "SimulationModel.hpp"


int main(int argc, char *argv[])
{	

	qmlRegisterType<SensorSettingsModel>("qdemo.models", 1, 0, "SensorSettingsModel");
	qmlRegisterType<Simulation>("qdemo.models", 1, 0, "Simulation");
	qmlRegisterType<MclSettings>("qdemo.models", 1, 0, "MclSettings");
	qmlRegisterType<Pose>("qdemo.models", 1, 0, "Pose");
	qmlRegisterType<Particle>("qdemo.models", 1, 0, "Particle");
	qmlRegisterType<VelocityControl>("qdemo.models", 1, 0, "VelocityControl");
	qmlRegisterType<PointW>("qdemo.models", 1, 0, "PointW");


    QApplication app(argc, argv);
    QQmlApplicationEngine engine("mainwindow.qml");
    return app.exec();
}