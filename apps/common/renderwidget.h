// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'typeset' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef LIBTYPESET_APPCOMMON_RENDERWIDGET_H
#define LIBTYPESET_APPCOMMON_RENDERWIDGET_H

#include <QWidget>

#include <QMargins>

#include "tex/box.h"

namespace tex
{
class Rule;
} // namespace tex

class QPainter;

class RenderWidget : public QWidget
{
  Q_OBJECT
public:
  explicit RenderWidget(QWidget* parent = nullptr);

  void center(bool on);
  bool centered() const;

  void setMargins(QMargins margins);
  const QMargins& margins() const;

  void setBox(std::shared_ptr<tex::Box> box);

protected:
  void paintEvent(QPaintEvent* ev) override;

protected:
  friend class LayoutVisitor;

  void visit(QPainter& painter, std::shared_ptr<tex::Box> box);

  static QRectF getRect(const QPointF& pos, const tex::Box& box);

  virtual void paint(QPainter& painter, const std::shared_ptr<tex::Box>& box, const QPointF& pos);
  virtual void paint(QPainter& painter, const std::shared_ptr<tex::Rule>& rule, const QPointF& pos);

private:
  bool m_center = false;
  QMargins m_margins;
  std::shared_ptr<tex::Box> m_box;
};

#endif // LIBTYPESET_APPCOMMON_RENDERWIDGET_H
