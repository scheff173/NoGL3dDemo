#ifndef RENDER_WIDGET_H
#define RENDER_WIDGET_H

#include <functional>
#include <vector>

#include <QWidget>

#include "RenderContext.h"


class RenderWidget: public QWidget {
  private:
    RenderContext &_context;
    Vec3f _centerOrbit;
    QPoint _posNavLast;

  public:
    std::vector<std::function<void()>> sigMatCamChanged;

  public:
    explicit RenderWidget(
      RenderContext &context, QWidget *pQParent = nullptr):
      QWidget(pQParent),
      _context(context),
      _centerOrbit(0.0f, 0.0f, 0.0f)
    { }
    virtual ~RenderWidget() = default;
    RenderWidget(const RenderWidget&) = delete;
    RenderWidget& operator=(const RenderWidget&) = delete;

  public:
    virtual QSize sizeHint() const override
    {
      return QSize(
        _context.getViewportWidth(), _context.getViewportHeight());
    }

  protected:
    virtual void paintEvent(QPaintEvent *pQEvent) override;
    virtual void mousePressEvent(QMouseEvent *pQEvent) override;
    virtual void mouseMoveEvent(QMouseEvent *pQEvent) override;
    //virtual void mouseReleaseEvent(QMouseEvent *pQEvent) override;
};

#endif // RENDER_WIDGET_H
