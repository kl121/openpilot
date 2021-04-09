#pragma once

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "common/params.h"
#include "toggle.hpp"

QFrame *horizontal_line(QWidget *parent = nullptr);

class AbstractControl : public QFrame {
  Q_OBJECT

protected:
  AbstractControl(const QString &title, const QString &desc = "", const QString &icon = "", QWidget *parent = nullptr);

  QSize minimumSizeHint() const override {
    QSize size = QFrame::minimumSizeHint();
    size.setHeight(120);
    return size;
  };

  QHBoxLayout *hlayout;
  QPushButton *title_label;
  QLabel *description = nullptr;
};

// widget to display a value
class LabelControl : public AbstractControl {
  Q_OBJECT

public:
  LabelControl(const QString &title, const QString &text, const QString &desc = "", QWidget *parent = nullptr) : AbstractControl(title, desc, "", parent) {
    label.setText(text);
    label.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    hlayout->addWidget(&label);
  }
  void setText(const QString &text) { label.setText(text); }

private:
  QLabel label;
};

// widget for a button with a label
class ButtonControl : public AbstractControl {
  Q_OBJECT

public:
  template <typename Functor>
  ButtonControl(const QString &title, const QString &text, const QString &desc, Functor functor, const QString &icon = "", QWidget *parent = nullptr) : AbstractControl(title, desc, icon, parent) {
    btn.setText(text);
    btn.setStyleSheet(R"(
      QPushButton {
        padding: 0;
        border-radius: 50px;
        font-size: 35px;
        font-weight: 500;
        color: #E4E4E4;
        background-color: #393939;
      }
      QPushButton:disabled {
        color: #33E4E4E4;
      }
    )");
    btn.setFixedSize(250, 100);
    QObject::connect(&btn, &QPushButton::released, functor);
    hlayout->addWidget(&btn);
  }
  void setText(const QString &text) { btn.setText(text); }

public slots:
  void setEnabled(bool enabled) {
    btn.setEnabled(enabled);
  };

private:
  QPushButton btn;
};

class ToggleControl : public AbstractControl {
  Q_OBJECT

public:
  ToggleControl(const QString &title, const QString &desc = "", const QString &icon = "", const bool state = false, QWidget *parent = nullptr) : AbstractControl(title, desc, icon, parent) {
    toggle.setFixedSize(150, 100);
    if (state) {
      toggle.togglePosition();
    }
    hlayout->addWidget(&toggle);
    QObject::connect(&toggle, &Toggle::stateChanged, this, &ToggleControl::toggleFlipped);
  }

  void setEnabled(bool enabled) { toggle.setEnabled(enabled); }

signals:
  void toggleFlipped(bool state);

protected:
  Toggle toggle;
};

// widget to toggle params
class ParamControl : public ToggleControl {
  Q_OBJECT

public:
  ParamControl(const QString &param, const QString &title, const QString &desc, const QString &icon, QWidget *parent = nullptr) : ToggleControl(title, desc, icon, parent) {
    // set initial state from param
    if (Params().read_db_bool(param.toStdString().c_str())) {
      toggle.togglePosition();
    }
    QObject::connect(this, &ToggleControl::toggleFlipped, [=](int state) {
      char value = state ? '1' : '0';
      Params().write_db_value(param.toStdString().c_str(), &value, 1);
    });
  }
};

class PrebuiltParamControl : public ParamControl {
  Q_OBJECT



 public PrebuiltParamControl(const QString &param, const QString &title, const QString &desc, const QString &icon, QWidget *parent = nullptr) :
                ParamControl(const QString &param, const QString &title, const QString &desc, const QString &icon, QWidget *parent = nullptr)
 {
    if (Params().read_db_bool(param.toStdString().c_str())) {
    //touch prebuilt
        std::ofstream output("/data/openpilot/prebuilt");

    } else {
    //remove prebuilt
        std::remove("/data/openpilot/prebuilt");
    }
    QObject::connect(this, &ToggleControl::toggleFlipped, [=](int state) {
//      char value = state ? '1' : '0';
        if (state == 1 ) {
            std::ofstream output("/data/openpilot/prebuilt");
        } else {
            std::remove("/data/openpilot/prebuilt");
        }
//      Params().write_db_value(param.toStdString().c_str(), &value, 1);
    });
 }
}