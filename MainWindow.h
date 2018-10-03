#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QBoxLayout>
#include <QCheckBox>
#include <QFormLayout>
#include <QSlider>
#include <QSpinBox>
#include <QWidget>
#include <QTimer>

#include "Mesh.h"
#include "RenderWidget.h"
#include "Sphere.h"

class MainWindow: public QWidget {
  public:
    RenderContext context3d;
  private:
    float _xCam, _yCam, _zCam;
    float _hCam, _pCam, _rCam;
    bool _lockCam;
    float _fov;
    float _dNear, _dFar;
    bool _headLight;
    enum { NFPS = 100 };
    double _dtFPS[NFPS], _dtSum;
    uint _nFPS, _iFPS;
    MeshT<VertexCNT> _mesh;
    uint _resSphere;
    QHBoxLayout _qHBox;
    QFormLayout _qForm;
    QDoubleSpinBox _qTxtCamX, _qTxtCamY, _qTxtCamZ;
    QDoubleSpinBox _qTxtCamH, _qTxtCamP, _qTxtCamR;
    QDoubleSpinBox _qTxtFOV;
    QLineEdit _qTxtDNear, _qTxtDFar;
    QLineEdit _qTxtDuration;
    QSpinBox _qSpinBoxResSphere;
    QLineEdit _qTxtTrisVtcs;
    QCheckBox _qTglFrontSide;
    QCheckBox _qTglBackSide;
    QCheckBox _qTglDepthBuffer;
    QCheckBox _qTglDepthTest;
    QCheckBox _qTglSmooth;
    QCheckBox _qTglBlending;
    QCheckBox _qTglTexturing;
    QCheckBox _qTglLighting;
    QVBoxLayout _qVBoxAmbient;
    QDoubleSpinBox _qSpinBoxAmbient;
    QSlider _qSliderAmbient;
    QCheckBox _qTglAnim;
    RenderWidget _qView3d;
    std::function<void(RenderContext&)> _cbRender;
    uint _angle, _stepAngle;
    QTimer _qTimerAnim;
    uint _iImg;
  public:
    MainWindow(
      uint width, uint height, QWidget *pQParent = nullptr);
    virtual ~MainWindow() = default;
    MainWindow(const MainWindow&) = delete;
    MainWindow& operator=(const MainWindow&) = delete;

    void updateCamMat(bool render);

    void updateProjMat(bool render);

    void loadTex(const QString &file);

  protected:

    virtual void showEvent(QShowEvent *pQEvent) override;

  private:

    void cbRender(RenderContext &context);

    void updateCamMatWidgets();
};

#endif // MAIN_WINDOW_H
