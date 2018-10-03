#include <QtWidgets>

#include "RenderWidget.h"

void RenderWidget::paintEvent(QPaintEvent*)
{
  QPainter qPainter(this);
  const QImage qImg((uchar*)_context.getRGBA(),
    _context.getViewportWidth(), _context.getViewportHeight(),
    QImage::Format_RGBA8888);
  qPainter.drawImage(0, 0, qImg);
}

void RenderWidget::mousePressEvent(QMouseEvent *pQEvent)
{
  if (pQEvent->button() == Qt::LeftButton) {
    _posNavLast = pQEvent->pos();
    pQEvent->accept();
  } else QWidget::mousePressEvent(pQEvent);
}

void RenderWidget::mouseMoveEvent(QMouseEvent *pQEvent)
{
  if (pQEvent->buttons() & Qt::LeftButton) {
    // normalized distance of mouse since last event
    const Vec2f d(
      (float)(pQEvent->pos().x() - _posNavLast.x()) / width(),
      (float)(pQEvent->pos().y() - _posNavLast.y()) / height());
    if (1.0f * manhattan(d) > 1E-10) {
      // retrieve camera matrix
      const Mat4x4f &mat = _context.getCamMat();
      const Vec3f right(mat._00, mat._10, mat._20);
      const Vec3f up(mat._01, mat._11, mat._21);
      // determine rotation axis for orbit navigation
      const Vec3f axis = normalize(d.y * right + d.x * up);
      _context.setCamMat(
        Mat4x4f(InitTrans, _centerOrbit)
        * Mat4x4f(InitRot, axis, -2.0f * (float)Pi * length(d))
        * Mat4x4f(InitTrans, -_centerOrbit)
        * mat);
      _context.render();
      // signal change of camera matrix
      for (const auto &func : sigMatCamChanged) if (func) func();
      // post-processing
      _posNavLast = pQEvent->pos();
    }
    pQEvent->accept();
  } else QWidget::mousePressEvent(pQEvent);

}