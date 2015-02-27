#include <QCoreApplication>
#include <QStringList>
#include "VolumeReconstructor.h"


int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    QStringList params = app.arguments();
    VolumeReconstructor volumereconstructor;
    volumereconstructor.generateVolume(params.at(1));

    qDebug() << "Cerrando el programa";
    //return app.exec();
    app.exit();
}
