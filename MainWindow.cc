#include <chrono>

#include <QtWidgets>

#include "color.h"
#include "MainWindow.h"

MainWindow::MainWindow(
  uint width, uint height, QWidget *pQParent):
  QWidget(pQParent),
  context3d(width, height),
  _xCam(0.0f), _yCam(0.0f), _zCam(2.5f),
  _hCam(0.0f), _pCam(0.0f), _rCam(0.0f),
  _lockCam(false),
  _fov(30.0f), _dNear(0.01f), _dFar(100.0f),
  _headLight(true),
  _dtSum(0.0), _nFPS(0), _iFPS(0),
  _resSphere(4),
  _qSliderAmbient(Qt::Horizontal),
  _qView3d(context3d),
  _angle(0), _stepAngle(5),
  _iImg(0)
{
  // prepare texture
  { // load image (using Qt image loader plug-in)
    QImage qImg
      = QImage("Earth.jpg")
        .mirrored() // flip vertical to make lower, left corner texel (0, 0)
        .convertToFormat(QImage::Format_RGBA8888);
    const uint width = (uint)qImg.width(), height = (uint)qImg.height();
    uint32 *img = (uint32*)qImg.bits();
    // convert blue (picked in GIMP) to alpha
    std::vector<uint32> imgA(img, img + width * height);
    colorToAlpha(width, height, imgA.data(),
      rgbaToColor(0xffb17a4f).xyz(), 100.0f);
    // merge original and processed image to weaken the alpha effect
    mergeColors(width, height, img, img, imgA.data(), 0.5f, 0.5f);
#if 0 // test:
    // save image to file (for check in GIMP)
    qImg.save("Earth-alpha.png");
#endif // 0
    // load image as texture
    uint idTex = context3d.loadTex(
      qImg.width(), qImg.height(), (const uint32*)qImg.bits());
    assert(idTex > 0);
    // bind image
    context3d.setTex(idTex);
  }
  // init render context
  context3d.setClearColor(Vec4f(0.5f, 0.75f, 1.0f, 1.0f));
  updateCamMat(false); updateProjMat(false);
  // build GUI
  _qTxtDuration.setReadOnly(true);
  _qForm.addRow(new QLabel(QString::fromUtf8("<b>Camera:</b>")));
  _qTxtCamX.setRange(-1000.0, 1000.0);
  _qTxtCamX.setSingleStep(1.0);
  _qTxtCamX.setValue(_xCam);
  _qForm.addRow(QString::fromUtf8("X:"), &_qTxtCamX);
  _qTxtCamY.setRange(-1000.0, 1000.0);
  _qTxtCamY.setSingleStep(1.0);
  _qTxtCamY.setValue(_yCam);
  _qForm.addRow(QString::fromUtf8("Y:"), &_qTxtCamY);
  _qTxtCamZ.setRange(-1000.0, 1000.0);
  _qTxtCamZ.setSingleStep(1.0);
  _qTxtCamZ.setValue(_zCam);
  _qForm.addRow(QString::fromUtf8("Z:"), &_qTxtCamZ);
  _qTxtCamH.setRange(-180.0, 180.0);
  _qTxtCamH.setSingleStep(1.0);
  _qTxtCamH.setValue(_hCam);
  _qForm.addRow(QString::fromUtf8("Heading:"), &_qTxtCamH);
  _qTxtCamP.setRange(-180.0, 180.0);
  _qTxtCamP.setSingleStep(1.0);
  _qTxtCamP.setValue(_pCam);
  _qForm.addRow(QString::fromUtf8("Pitch:"), &_qTxtCamP);
  _qTxtCamR.setRange(-180.0, 180.0);
  _qTxtCamR.setSingleStep(1.0);
  _qTxtCamR.setValue(_rCam);
  _qForm.addRow(QString::fromUtf8("Roll:"), &_qTxtCamR);
  _qTxtFOV.setRange(5.0, 90.0);
  _qTxtFOV.setSingleStep(1.0);
  _qTxtFOV.setValue(_fov);
  _qForm.addRow(QString::fromUtf8("Field of View:"), &_qTxtFOV);
  _qTxtDNear.setText(QLocale().toString(_dNear, 'f'));
  _qForm.addRow(QString::fromUtf8("Near Clip Distance:"), &_qTxtDNear);
  _qTxtDFar.setText(QLocale().toString(_dFar, 'f'));
  _qForm.addRow(QString::fromUtf8("Far Clip Distance:"), &_qTxtDFar);
  _qForm.addRow(new QLabel(QString::fromUtf8("<b>Statistics:</b>")));
  _qTxtTrisVtcs.setReadOnly(true);
  _qForm.addRow(QString::fromUtf8("Geometry:"), &_qTxtTrisVtcs);
  _qForm.addRow(QString::fromUtf8("Duration:"), &_qTxtDuration);
  _qForm.addRow(new QLabel(QString::fromUtf8("<b>Settings:</b>")));
  _qSpinBoxResSphere.setRange(0, 4);
  _qSpinBoxResSphere.setValue(_resSphere);
  _qForm.addRow(QString::fromUtf8("Res. of Sphere:"), &_qSpinBoxResSphere);
#define CHECK_BOX(MODE, TEXT) \
  _qTgl##MODE.setChecked( \
    context3d.isEnabled(RenderContext::MODE)); \
  _qForm.addRow(QString::fromUtf8(TEXT), &_qTgl##MODE)
  CHECK_BOX(FrontSide, "Show front sides:");
  CHECK_BOX(BackSide, "Show back sides:");
  CHECK_BOX(DepthBuffer, "Depth Buffering:");
  CHECK_BOX(DepthTest, "Depth Test:");
  CHECK_BOX(Smooth, "Smooth:");
  CHECK_BOX(Blending, "Alpha Blending:");
  CHECK_BOX(Texturing, "Textures:");
  CHECK_BOX(Lighting, "Lighting:");
#undef CHECK_BOX
  _qSpinBoxAmbient.setRange(0.0, 1.0);
  _qSpinBoxAmbient.setSingleStep(0.1);
  _qSpinBoxAmbient.setValue(context3d.getAmbient());
  _qVBoxAmbient.addWidget(&_qSpinBoxAmbient);
  _qSliderAmbient.setRange(0, 100);
  _qSliderAmbient.setSingleStep(1);
  _qSliderAmbient.setPageStep(10);
  _qSliderAmbient.setValue(context3d.getAmbient() * 100.0f);
  _qVBoxAmbient.addWidget(&_qSliderAmbient);
  _qForm.addRow(QString::fromUtf8("Ambient:"), &_qVBoxAmbient);
  _qTglAnim.setChecked(false);
  _qForm.addRow(QString::fromUtf8("Animation:"), &_qTglAnim);
  _qHBox.addLayout(&_qForm);
  _qHBox.addWidget(&_qView3d);
  setLayout(&_qHBox);
  _qTimerAnim.setInterval(50); // ms
  // install signal handlers
  context3d.setRenderCallback(
    [&](RenderContext &context) { cbRender(context); });
  connect(&_qTxtCamX,
    (void(QDoubleSpinBox::*)(double))&QDoubleSpinBox::valueChanged,
    [&](double value) { _xCam = (float)value; updateCamMat(true); });
  connect(&_qTxtCamY,
    (void(QDoubleSpinBox::*)(double))&QDoubleSpinBox::valueChanged,
    [&](double value) { _yCam = (float)value; updateCamMat(true); });
  connect(&_qTxtCamZ,
    (void(QDoubleSpinBox::*)(double))&QDoubleSpinBox::valueChanged,
    [&](double value) { _zCam = (float)value; updateCamMat(true); });
  connect(&_qTxtCamH,
    (void(QDoubleSpinBox::*)(double))&QDoubleSpinBox::valueChanged,
    [&](double value) { _hCam = (float)value; updateCamMat(true); });
  connect(&_qTxtCamP,
    (void(QDoubleSpinBox::*)(double))&QDoubleSpinBox::valueChanged,
    [&](double value) { _pCam = (float)value; updateCamMat(true); });
  connect(&_qTxtCamR,
    (void(QDoubleSpinBox::*)(double))&QDoubleSpinBox::valueChanged,
    [&](double value) { _rCam = (float)value; updateCamMat(true); });
  connect(&_qTxtFOV,
    (void(QDoubleSpinBox::*)(double))&QDoubleSpinBox::valueChanged,
    [&](double value) { _fov = (float)value; updateProjMat(true); });
  connect(&_qTxtDNear, &QLineEdit::editingFinished,
    [&]() {
      bool ok;
      const float dNear = QLocale().toFloat(_qTxtDNear.text(), &ok);
      if (ok && dNear > 0.0f && dNear < _dFar) {
        _dNear = dNear; updateProjMat(true);
      } else _qTxtDNear.setText(QLocale().toString(_dNear, 'f'));
    });
  connect(&_qTxtDFar, &QLineEdit::editingFinished,
    [&]() {
      bool ok;
      const float dFar = QLocale().toFloat(_qTxtDFar.text(), &ok);
      if (ok && dFar > _dNear) {
        _dFar = _dFar; updateProjMat(true);
      } else _qTxtDFar.setText(QLocale().toString(_dFar, 'f'));
    });
  connect(&_qSpinBoxResSphere,
    (void(QSpinBox::*)(int))&QSpinBox::valueChanged,
    [&](int resSphere) {
      _resSphere = (uint)resSphere;
      _mesh.vtcs.clear(); // force re-build
      context3d.render();
    });
#define CHECK_BOX(MODE) \
  connect(&_qTgl##MODE, &QCheckBox::toggled, \
    [&](bool enable) { \
      context3d.enable(RenderContext::MODE, enable); \
      context3d.render(); \
    })
  CHECK_BOX(FrontSide);
  CHECK_BOX(BackSide);
  CHECK_BOX(DepthBuffer);
  CHECK_BOX(DepthTest);
  CHECK_BOX(Smooth);
  CHECK_BOX(Blending);
  CHECK_BOX(Texturing);
  CHECK_BOX(Lighting);
#undef CHECK_BOX
  connect(&_qSpinBoxAmbient,
    (void(QDoubleSpinBox::*)(double))&QDoubleSpinBox::valueChanged,
    [&](double ambient) {
      context3d.setAmbient(ambient);
      _qSliderAmbient.setValue(100 * context3d.getAmbient());
      context3d.render();
    });
  connect(&_qSliderAmbient, &QSlider::valueChanged,
    [&](int ambient) {
      context3d.setAmbient(0.01f * ambient);
      _qSpinBoxAmbient.setValue(context3d.getAmbient());
      context3d.render();
    });
  connect(&_qTglAnim, &QCheckBox::toggled,
    [&](bool enable) {
      enable ? _qTimerAnim.start() : _qTimerAnim.stop();
    });
  _qView3d.sigMatCamChanged.push_back(
    [&]() { updateCamMatWidgets(); });
  connect(&_qTimerAnim, &QTimer::timeout,
    [&]() {
      (_angle += _stepAngle) %= 360;
      context3d.getModelMat()
        = Mat4x4f(InitRotY, degToRad((float)_angle));
      context3d.render();
#if 0 // record image sequence
      if (_iImg < 36 && _angle % 10 == 0) {
        const QImage qImg((uchar*)context3d.getRGBA(),
          context3d.getViewportWidth(), context3d.getViewportHeight(),
          QImage::Format_RGBA8888);
        QString file
          = QString("snap-NoGL3d-%1.png").arg(_iImg, 2, 10, QChar('0'));
        qDebug() << "Save:" << file;
        if (qImg.save(file)) ++_iImg;
      }
#endif // 0
    });
}

void MainWindow::updateCamMat(bool render)
{
  if (_lockCam) return;
  Lock lock(_lockCam);
  Mat4x4f mat = makeEuler(RotYXZ,
    degToRad(_hCam), degToRad(_pCam), degToRad(_rCam));
  mat._03 = _xCam; mat._13 = _yCam; mat._23 = _zCam;
  context3d.setCamMat(mat);
  if (render) context3d.render();
}

void MainWindow::updateProjMat(bool render)
{
  context3d.getProjMat()
    = makePersp(degToRad(30.0f),
      (float)context3d.getViewportWidth() / context3d.getViewportHeight(),
      _dNear, _dFar);
  if (render) context3d.render();
}

void MainWindow::loadTex(const QString &file)
{
  const QImage qImg = QImage(file).convertToFormat(QImage::Format_RGBA8888);
  uint idTex = context3d.loadTex(
    qImg.width(), qImg.height(), (const uint32*)qImg.bits());
  context3d.setTex(idTex);
}

void MainWindow::showEvent(QShowEvent *pQEvent)
{
  QWidget::showEvent(pQEvent);
  context3d.render();
}

typedef std::chrono::high_resolution_clock Clock;
typedef Clock::time_point Time;
typedef std::chrono::microseconds MuS;
inline long long duration(const Time &from, const Time &to)
{
  return std::chrono::duration_cast<MuS>(to - from).count();
}

void MainWindow::cbRender(RenderContext &context)
{
  // build sphere if not yet done
  if (_mesh.vtcs.empty()) {
#if 1 // regular:
    makeSphereMesh(_mesh, _resSphere);
#else // used for debugging
    _mesh.vtcs.emplace_back(
      Vec3f(0.0f, 1.0f, -0.5f), Vec3f(0.0f, 0.0f, 1.0f), Vec2f(0.0f, 0.0f));
    _mesh.vtcs.emplace_back(
      Vec3f(-1.0f, 0.0f, -0.5f), Vec3f(0.0f, 0.0f, 1.0f), Vec2f(0.0f, 0.0f));
    _mesh.vtcs.emplace_back(
      Vec3f(0.0f, -1.0f, -0.5f), Vec3f(0.0f, 0.0f, 1.0f), Vec2f(0.0f, 0.0f));
#endif // 1
    _qTxtTrisVtcs.setText(
      QString("%1 Tris, %2 Vtcs").arg(
        QString::number(_mesh.vtcs.size() / 3),
        QString::number(_mesh.vtcs.size())));
  }
  // start stop-watch
  const Time tStart = Clock::now();
  // clear buffers
  context.clear(true, true);
  // render sphere
  context.setColor(Vec4f(1.0f, 1.0f, 1.0f, 1.0f));
  for (size_t i = 0, n = _mesh.vtcs.size(); i < n; ++i) {
    const VertexCNT &vtx = _mesh.vtcs[i];
    context.setNormal(vtx.normal);
    context.setTexCoord(vtx.texCoord);
    context.drawVertex(vtx.coord);
  }
  // stop stop-watch
  const Time tEnd = Clock::now();
  const double dt = 1E-6 * duration(tStart, tEnd);
  if (_nFPS < NFPS) _dtFPS[_nFPS++] = dt;
  else {
    _dtSum -= _dtFPS[_iFPS]; _dtFPS[_iFPS] = dt;
    _iFPS = (_iFPS + 1) % NFPS;
  }
  _dtSum += dt;
  const uint fps = _nFPS / _dtSum;
  _qTxtDuration.setText(
    QString("%1 s (%2 fps)").arg(
    QString::number(dt, 'f', 6), QString::number(fps)));
  // update 3d view
  _qView3d.update();
}

void MainWindow::updateCamMatWidgets()
{
  if (_lockCam) return;
  Lock lock(_lockCam);
  const Mat4x4f &matCam = context3d.getCamMat();
  // set x, y, z
  _qTxtCamX.setValue(_xCam = matCam._03);
  _qTxtCamY.setValue(_yCam = matCam._13);
  _qTxtCamZ.setValue(_zCam = matCam._23);
  decompose(matCam, RotYXZ, _hCam, _pCam, _rCam);
  _qTxtCamH.setValue(_hCam *= 180.0f / (float)Pi);
  _qTxtCamP.setValue(_pCam *= 180.0f / (float)Pi);
  _qTxtCamR.setValue(_rCam *= 180.0f / (float)Pi);
}