#include <QtWidgets>

#include "MainWindow.h"

int main(int argc, char **argv)
{
  const int width = 1024, height = 768;
  qDebug() << "Qt Version:" << QT_VERSION_STR;
  QApplication app(argc, argv);
  MainWindow qWin(width, height);
  qWin.setWindowTitle(
    QString::fromUtf8(
      "No-GL 3D Renderer (If you ask \"Why?\" you've missed the point.)"));
#if 0
  qWin.context3d.setCamMat(
    Mat4x4f(InitTrans, Vec3f(0.0f, 0.0f, 2.5f)));
#endif // 0
  qWin.show();
  return app.exec();
}