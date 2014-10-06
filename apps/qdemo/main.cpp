#include <QApplication>
#include <QQmlApplicationEngine>



class SensorModel
{
	//QPROPERTY a0, a1, a2, a3, lambda, sigma, max range
	//QPROPERTY samples, read-only
public:
	const QVector<double>& samples();
};

int main(int argc, char *argv[])
{	
    QApplication app(argc, argv);
    QQmlApplicationEngine engine("mainwindow.qml");
    return app.exec();
}