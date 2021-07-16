#pragma once

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

#include "selfdrive/common/params.h"
#include "selfdrive/ui/qt/widgets/toggle.h"

#include <fstream>
#include <cstdio>

#include "selfdrive/hardware/hw.h"
QFrame *horizontal_line(QWidget *parent = nullptr);

class ElidedLabel : public QLabel {
  Q_OBJECT

 public:
  explicit ElidedLabel(QWidget *parent = 0);
  explicit ElidedLabel(const QString &text, QWidget *parent = 0);

 protected:
  void paintEvent(QPaintEvent *event) override;
  void resizeEvent(QResizeEvent* event) override;
  QString lastText_, elidedText_;
};

class AbstractControl : public QFrame {
  Q_OBJECT

public:
  void setDescription(const QString &desc) {
    if(description) description->setText(desc);
  }

signals:
  void showDescription();

protected:
  AbstractControl(const QString &title, const QString &desc = "", const QString &icon = "", QWidget *parent = nullptr);
  void hideEvent(QHideEvent *e) override;

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
  LabelControl(const QString &title, const QString &text = "", const QString &desc = "", QWidget *parent = nullptr) : AbstractControl(title, desc, "", parent) {
    label.setText(text);
    label.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    hlayout->addWidget(&label);
  }
  void setText(const QString &text) { label.setText(text); }

private:
  ElidedLabel label;
};

// widget for a button with a label
class ButtonControl : public AbstractControl {
  Q_OBJECT

public:
  ButtonControl(const QString &title, const QString &text, const QString &desc = "", QWidget *parent = nullptr);
  inline void setText(const QString &text) { btn.setText(text); }
  inline QString text() const { return btn.text(); }

signals:
  void released();

public slots:
  void setEnabled(bool enabled) { btn.setEnabled(enabled); };

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
  ParamControl(const QString &param, const QString &title, const QString &desc, const QString &icon, QWidget *parent = nullptr) : ToggleControl(title, desc, icon, false, parent) {
    if (params.getBool(param.toStdString().c_str())) {
      toggle.togglePosition();
    }
    QObject::connect(this, &ToggleControl::toggleFlipped, [=](bool state) {
      params.putBool(param.toStdString().c_str(), state);
    });
  }

protected:
  Params params;
};
//prebuilt param control class, this only uses for prebuilt toggle button.
class PrebuiltParamControl : public ParamControl {
  Q_OBJECT



public:
  PrebuiltParamControl(const QString &param, const QString &title, const QString &desc, const QString &icon, QWidget *parent = nullptr) :
          ParamControl(param, title,desc, icon, parent) {

    //when instantiate object
    if (params.getBool(param.toStdString().c_str())) {
        std::ofstream output("/data/openpilot/prebuilt"); //touch prebuilt
    } else {
        std::remove("/data/openpilot/prebuilt"); //rm prebuilt
    }
    QObject::connect(this, &ToggleControl::toggleFlipped, [=](bool state) {
          if (state ) {
            std::ofstream output("/data/openpilot/prebuilt");
        } else {
            std::remove("/data/openpilot/prebuilt");
        }
    });
 }
};

//Lateral Control Selection class, this only uses for INDI_Selectd toggle button.
class INDISelection : public ToggleControl {
  Q_OBJECT

public:
  INDISelection() : ToggleControl("Select INDI for Lateral Control", "INDI 방식으로 조향제어를 합니다 (If you select this option, EON controls the steering using INDI)", "../assets/offroad/icon_checkmark.png", Params().getBool("INDI_Selected")) {
    QObject::connect(this, &INDISelection::toggleFlipped, [=](int state) {
      char value = state ? '1' : '0';
      Params().put("INDI_Selected", &value, 1);
    });
  }
};

//Lateral Control Selection class, this only uses for LQR_Selectd toggle button.
class LQRSelection : public ToggleControl {
  Q_OBJECT

public:
  LQRSelection() : ToggleControl("Select LQR for Lateral Control", "LQR 방식으로 조향제어를 합니다 (If you select this option, EON controls the steering using LQR)", "../assets/offroad/icon_checkmark.png", Params().getBool("LQR_Selected")) {
    QObject::connect(this, &LQRSelection::toggleFlipped, [=](int state) {
      char value = state ? '1' : '0';
      Params().put("LQR_Selected", &value, 1);
    });
  }
};
