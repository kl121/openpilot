#include "selfdrive/ui/qt/offroad/settings.h"

#include <cassert>
#include <string>

#include <QDebug>

#ifndef QCOM
#include "selfdrive/ui/qt/offroad/networking.h"
#endif

#ifdef ENABLE_MAPS
#include "selfdrive/ui/qt/maps/map_settings.h"
#endif

#include "selfdrive/common/params.h"
#include "selfdrive/common/util.h"
#include "selfdrive/hardware/hw.h"
#include "selfdrive/ui/qt/widgets/controls.h"
#include "selfdrive/ui/qt/widgets/input.h"
#include "selfdrive/ui/qt/widgets/scrollview.h"
#include "selfdrive/ui/qt/widgets/ssh_keys.h"
#include "selfdrive/ui/qt/widgets/toggle.h"
#include "selfdrive/ui/ui.h"
#include "selfdrive/ui/qt/util.h"
#include "selfdrive/ui/qt/qt_window.h"

TogglesPanel::TogglesPanel(QWidget *parent) : ListWidget(parent) {

  addItem(new ParamControl("OpenpilotEnabledToggle",
                                  "오픈파일럿 사용",
                                  "어댑티브 크루즈 컨트롤 및 차선 유지 지원을 위해 오픈파일럿 시스템을 사용하십시오. 이 기능을 사용하려면 항상 주의를 기울여야 합니다. 이 설정을 변경하는 것은 자동차의 전원이 꺼졌을 때 적용됩니다.",
                                  "../assets/offroad/icon_openpilot.png",
                                  this));
  addItem(new ParamControl("IsLdwEnabled",
                                  "차선이탈 경보 사용",
                                  "50km/h이상의 속도로 주행하는 동안 방향 지시등이 활성화되지 않은 상태에서 차량이 감지된 차선 위를 넘어갈 경우 원래 차선으로 다시 방향을 전환하도록 경고를 보냅니다.",
                                  "../assets/offroad/icon_warning.png",
                                  this));
  addItem(new ParamControl("IsRHD",
                                  "우핸들 운전방식 사용",
                                  "오픈파일럿이 좌측 교통 규칙을 준수하도록 허용하고 우측 운전석에서 운전자 모니터링을 수행하십시오.",
                                  "../assets/offroad/icon_openpilot_mirrored.png",
                                  this));
  addItem(new ParamControl("IsMetric",
                                  "미터법 사용",
                                  "mi/h 대신 km/h 단위로 속도를 표시합니다.",
                                  "../assets/offroad/icon_metric.png",
                                  this));
  addItem(new ParamControl("CommunityFeaturesToggle",
                                  "커뮤니티 기능 사용",
                                  "comma.ai에서 유지 또는 지원하지 않고 표준 안전 모델에 부합하는 것으로 확인되지 않은 오픈 소스 커뮤니티의 기능을 사용하십시오. 이러한 기능에는 커뮤니티 지원 자동차와 커뮤니티 지원 하드웨어가 포함됩니다. 이러한 기능을 사용할 때는 각별히 주의해야 합니다.",
                                  "../assets/offroad/icon_shell.png",
                                  this));

  addItem(new ParamControl("UploadRaw",
                                    "업로드 raw 로그",
                                    "와이파이 연결 시 로그와 영상 전체를 업로드 합니다.",
                                  "../assets/offroad/icon_network.png",
                                  this));

  ParamControl *record_toggle = new ParamControl("RecordFront",
                                                 "운전자 영상 녹화 및 업로드",
                                                 "운전자 모니터링 카메라에서 데이터를 업로드하고 운전자 모니터링 알고리즘을 개선하십시오.",
                                                 "../assets/offroad/icon_monitoring.png",
                                                 this);
  addItem(record_toggle);
  addItem(new ParamControl("EndToEndToggle",
                                  "\U0001f96c 차선이 없을 때 사용 버전 알파",
                                  "차선이 없는 곳에서 사람과 같이 운전을 하는 것을 목표합니다.",
                                  "../assets/offroad/icon_road.png",
                                  this));

#ifdef ENABLE_MAPS
  addItem(new ParamControl("NavSettingTime24h",
                                  "Show ETA in 24h format",
                                  "Use 24h format instead of am/pm",
                                  "../assets/offroad/icon_metric.png",
                                  this));
#endif

  bool record_lock = Params().getBool("RecordFrontLock");
  record_toggle->setEnabled(!record_lock);
}

DevicePanel::DevicePanel(QWidget* parent) : ListWidget(parent) {
  setSpacing(50);
  Params params = Params();
  addItem(new LabelControl("Dongle ID", getDongleId().value_or("N/A")));

  QString serial = QString::fromStdString(params.get("HardwareSerial", false));
  addItem(new LabelControl("시리얼", serial));

  auto dcamBtn = new ButtonControl("운전자 영상", "미리보기",
                                   "운전자 영상이 제대로 작동하는지 확인을 합니다. 차량 시동을 끄고 사용하도록 하십시오.");
  connect(dcamBtn, &ButtonControl::clicked, [=]() { emit showDriverView(); });

  QString resetCalibDesc = "오픈파일럿은 좌우로 4° 위아래로 5° 의 오차범위내에 장착되어야 합니다. 오픈파일럿이 항상 캘리브레이션을 하기때문에 리셋은 대부분의 경우 불필요합니다.";
  auto resetCalibBtn = new ButtonControl("캘리브레이션 리셋", "리셋", resetCalibDesc);
  connect(resetCalibBtn, &ButtonControl::clicked, [=]() {
      if (ConfirmationDialog::confirm("캘리브레이션을 리셋하시겠습니까?", this)) {
        Params().remove("CalibrationParams");
      }
  });
  connect(resetCalibBtn, &ButtonControl::showDescription, [=]() {
      QString desc = resetCalibDesc;
      std::string calib_bytes = Params().get("CalibrationParams");
      if (!calib_bytes.empty()) {
        try {
          AlignedBuffer aligned_buf;
          capnp::FlatArrayMessageReader cmsg(aligned_buf.align(calib_bytes.data(), calib_bytes.size()));
          auto calib = cmsg.getRoot<cereal::Event>().getLiveCalibration();
          if (calib.getCalStatus() != 0) {
            double pitch = calib.getRpyCalib()[1] * (180 / M_PI);
            double yaw = calib.getRpyCalib()[2] * (180 / M_PI);
            desc += QString(" Your device is pointed %1° %2 and %3° %4.")
                    .arg(QString::number(std::abs(pitch), 'g', 1), pitch > 0 ? "up" : "down",
                         QString::number(std::abs(yaw), 'g', 1), yaw > 0 ? "right" : "left");
          }
        } catch (kj::Exception) {
          qInfo() << "invalid CalibrationParams";
        }
      }
      resetCalibBtn->setDescription(desc);
  });

  ButtonControl *retrainingBtn = nullptr;
  if (!params.getBool("Passive")) {
    retrainingBtn = new ButtonControl("트레이닝 가이드", "가이드 보기", "오픈파일럿의 제한적 상황과 규정을 확인");
    connect(retrainingBtn, &ButtonControl::clicked, [=]() {
        if (ConfirmationDialog::confirm("트레이닝 가이드를 확인하시겠습니까?", this)) {
          emit reviewTrainingGuide();
        }
    });
  }

  ButtonControl *regulatoryBtn = nullptr;
  if (Hardware::TICI()) {
    regulatoryBtn = new ButtonControl("Regulatory", "VIEW", "");
    connect(regulatoryBtn, &ButtonControl::clicked, [=]() {
        const std::string txt = util::read_file(ASSET_PATH.toStdString() + "/offroad/fcc.html");
        RichTextDialog::alert(QString::fromStdString(txt), this);
    });
  }

  for (auto btn : {dcamBtn, resetCalibBtn, retrainingBtn, regulatoryBtn}) {
    if (btn) {
      connect(parent, SIGNAL(offroadTransition(bool)), btn, SLOT(setEnabled(bool)));
      addItem(btn);
    }
  }

  // power buttons
  QHBoxLayout *power_layout = new QHBoxLayout();
  power_layout->setSpacing(30);

  QPushButton *reboot_btn = new QPushButton("Reboot");
  reboot_btn->setObjectName("reboot_btn");
  power_layout->addWidget(reboot_btn);
  QObject::connect(reboot_btn, &QPushButton::clicked, [=]() {
      if (ConfirmationDialog::confirm("Are you sure you want to reboot?", this)) {
        Hardware::reboot();
      }
  });

#ifdef QCOM
  QPushButton *reboot_rmprebuilt_btn = new QPushButton("빌드부팅");
  power_layout->addWidget(reboot_rmprebuilt_btn);
  QObject::connect(reboot_rmprebuilt_btn, &QPushButton::clicked, [=]() {
      if (ConfirmationDialog::confirm("prebuilt 파일을 임시 삭제하고 부팅합니다.",this)) {
        Hardware::update_reboot();
      }
  });

  QPushButton *cleanbuild_btn = new QPushButton("클린 빌드부팅");
  power_layout->addWidget(cleanbuild_btn);
  QObject::connect(cleanbuild_btn, &QPushButton::clicked, [=]() {
      if (ConfirmationDialog::confirm("완전에 가까운재설치를 합니다. 약 30분 소요됩니다.",this)) {
        Hardware::clean_build_reboot();
      }
  });
#endif

  QPushButton *poweroff_btn = new QPushButton("Power Off");
  poweroff_btn->setObjectName("poweroff_btn");
  power_layout->addWidget(poweroff_btn);
  QObject::connect(poweroff_btn, &QPushButton::clicked, [=]() {
      if (ConfirmationDialog::confirm("Are you sure you want to power off?", this)) {
        Hardware::poweroff();
      }
  });

  setStyleSheet(R"(
    QPushButton {
      height: 120px;
      border-radius: 15px;
    }
    #reboot_btn { background-color: #393939; }
    #reboot_btn:pressed { background-color: #4a4a4a; }
    #poweroff_btn { background-color: #E22C2C; }
    #poweroff_btn:pressed { background-color: #FF2424; }
  )");




  addItem(power_layout);
}

SoftwarePanel::SoftwarePanel(QWidget* parent) : ListWidget(parent) {
  gitBranchLbl = new LabelControl("Git Branch");
  gitCommitLbl = new LabelControl("Git Commit");
  osVersionLbl = new LabelControl("OS Version");
  versionLbl = new LabelControl("Version", "", QString::fromStdString(params.get("ReleaseNotes")).trimmed());
  lastUpdateLbl = new LabelControl("Last Update Check", "", "The last time openpilot successfully checked for an update. The updater only runs while the car is off.");
  updateBtn = new ButtonControl("Check for Update", "");
  connect(updateBtn, &ButtonControl::clicked, [=]() {
    if (params.getBool("IsOffroad")) {
      fs_watch->addPath(QString::fromStdString(params.getParamPath("LastUpdateTime")));
      fs_watch->addPath(QString::fromStdString(params.getParamPath("UpdateFailedCount")));
      updateBtn->setText("CHECKING");
      updateBtn->setEnabled(false);
    }
    std::system("pkill -1 -f selfdrive.updated");
  });


  auto uninstallBtn = new ButtonControl("Uninstall " + getBrand(), "UNINSTALL");
  connect(uninstallBtn, &ButtonControl::clicked, [=]() {
    if (ConfirmationDialog::confirm("Are you sure you want to uninstall?", this)) {
      Params().putBool("DoUninstall", true);
    }
  });
  connect(parent, SIGNAL(offroadTransition(bool)), uninstallBtn, SLOT(setEnabled(bool)));

  QWidget *widgets[] = {versionLbl, lastUpdateLbl, updateBtn, gitBranchLbl, gitCommitLbl, osVersionLbl, uninstallBtn};
  for (QWidget* w : widgets) {
    addItem(w);
  }

  fs_watch = new QFileSystemWatcher(this);
  QObject::connect(fs_watch, &QFileSystemWatcher::fileChanged, [=](const QString path) {
    int update_failed_count = params.get<int>("UpdateFailedCount").value_or(0);
    if (path.contains("UpdateFailedCount") && update_failed_count > 0) {
      lastUpdateLbl->setText("failed to fetch update");
      updateBtn->setText("CHECK");
      updateBtn->setEnabled(true);
    } else if (path.contains("LastUpdateTime")) {
      updateLabels();
    }
  });
}

void SoftwarePanel::showEvent(QShowEvent *event) {
  updateLabels();
}

void SoftwarePanel::updateLabels() {
  QString lastUpdate = "";
  auto tm = params.get("LastUpdateTime");
  if (!tm.empty()) {
    lastUpdate = timeAgo(QDateTime::fromString(QString::fromStdString(tm + "Z"), Qt::ISODate));
  }

  versionLbl->setText(getBrandVersion());
  lastUpdateLbl->setText(lastUpdate);
  updateBtn->setText("CHECK");
  updateBtn->setEnabled(true);
  gitBranchLbl->setText(QString::fromStdString(params.get("GitBranch")));
  gitCommitLbl->setText(QString::fromStdString(params.get("GitCommit")).left(10));
  osVersionLbl->setText(QString::fromStdString(Hardware::get_os_version()).trimmed());
}

QWidget * network_panel(QWidget * parent) {
#ifdef QCOM
  QWidget *w = new QWidget(parent);
  QVBoxLayout *layout = new QVBoxLayout(w);
  layout->setContentsMargins(50, 0, 50, 0);

  ListWidget *list = new ListWidget();
  list->setSpacing(30);
  // wifi + tethering buttons
  auto wifiBtn = new ButtonControl("WiFi Settings", "OPEN");
  QObject::connect(wifiBtn, &ButtonControl::clicked, [=]() { HardwareEon::launch_wifi(); });
  list->addItem(wifiBtn);

  auto tetheringBtn = new ButtonControl("Tethering Settings", "OPEN");
  QObject::connect(tetheringBtn, &ButtonControl::clicked, [=]() { HardwareEon::launch_tethering(); });
  list->addItem(tetheringBtn);

  // SSH key management
  list->addItem(new SshToggle());
  list->addItem(new SshControl());

  layout->addWidget(list);
  layout->addStretch(1);
#else
  Networking *w = new Networking(parent);
#endif
  return w;
}

void SettingsWindow::showEvent(QShowEvent *event) {
  panel_widget->setCurrentIndex(0);
  nav_btns->buttons()[0]->setChecked(true);
}


QWidget * community_panel(QWidget * parent) {
  QWidget *w = new QWidget(parent);
  QVBoxLayout *toggles_list = new QVBoxLayout(w);
  toggles_list->setSpacing(30);

  toggles_list->addWidget(new ParamControl("AutoLaneChangeEnabled",
                                            "자동 차선변경 사용",
                                            "자동 차선 변경. 사용에 주의 하십시오",
                                            "../assets/offroad/icon_road.png"
                                              ));
  toggles_list->addWidget(horizontal_line());
  toggles_list->addWidget(new PrebuiltParamControl("PrebuiltEnabled",
                                            "Enable Prebuilt File",
                                            "완전 정상주행 2회 이후 활성화하세요. prebuilt 파일이 있는경우 새로 빌드하지 않습니다. 업데이트창이 뜰때 내용을 확인하세요.",
                                            "../assets/offroad/icon_checkmark.png"
                                            ));

//  toggles_list->addWidget(horizontal_line());
//  toggles_list->addWidget(new LQRSelection());
//  toggles_list->addWidget(horizontal_line());
//  toggles_list->addWidget(new INDISelection());

  QWidget *widget = new QWidget;
  widget->setLayout(toggles_list);
  return widget;
}



SettingsWindow::SettingsWindow(QWidget *parent) : QFrame(parent) {

  // setup two main layouts
  sidebar_widget = new QWidget;
  QVBoxLayout *sidebar_layout = new QVBoxLayout(sidebar_widget);
  sidebar_layout->setMargin(0);
  panel_widget = new QStackedWidget();
  panel_widget->setStyleSheet(R"(
    border-radius: 30px;
    background-color: #292929;
  )");

  // close button
  QPushButton *close_btn = new QPushButton("×");
  close_btn->setStyleSheet(R"(
    QPushButton {
      font-size: 140px;
      padding-bottom: 20px;
      font-weight: bold;
      border 1px grey solid;
      border-radius: 100px;
      background-color: #292929;
      font-weight: 400;
    }
    QPushButton:pressed {
      background-color: #3B3B3B;
    }
  )");
  close_btn->setFixedSize(200, 200);
  sidebar_layout->addSpacing(45);
  sidebar_layout->addWidget(close_btn, 0, Qt::AlignCenter);
  QObject::connect(close_btn, &QPushButton::clicked, this, &SettingsWindow::closeSettings);

  // setup panels
  DevicePanel *device = new DevicePanel(this);
  QObject::connect(device, &DevicePanel::reviewTrainingGuide, this, &SettingsWindow::reviewTrainingGuide);
  QObject::connect(device, &DevicePanel::showDriverView, this, &SettingsWindow::showDriverView);

  QList<QPair<QString, QWidget *>> panels = {
    {"장치", device},
    {"네트워크", network_panel(this)},
    {"토글메뉴", new TogglesPanel(this)},
    {"개발자", new SoftwarePanel(this)},
    {"커뮤니티", community_panel(this)},
  };

#ifdef ENABLE_MAPS
  if (!Params().get("MapboxToken").empty()) {
    auto map_panel = new MapPanel(this);
    panels.push_back({"Navigation", map_panel});
    QObject::connect(map_panel, &MapPanel::closeSettings, this, &SettingsWindow::closeSettings);
  }
#endif
  const int padding = 35;

  nav_btns = new QButtonGroup();
  for (auto &[name, panel] : panels) {
    QPushButton *btn = new QPushButton(name);
    btn->setCheckable(true);
    btn->setChecked(nav_btns->buttons().size() == 0);
    btn->setStyleSheet(QString(R"(
      QPushButton {
        color: grey;
        border: none;
        background: none;
        font-size: 65px;
        font-weight: 500;
        padding-top: %1px;
        padding-bottom: %1px;
      }
      QPushButton:checked {
        color: white;
      }
      QPushButton:pressed {
        color: #ADADAD;
      }
    )").arg(padding));

    nav_btns->addButton(btn);
    sidebar_layout->addWidget(btn, 0, Qt::AlignRight);

    const int lr_margin = name != "Network" ? 50 : 0;  // Network panel handles its own margins
    panel->setContentsMargins(lr_margin, 25, lr_margin, 25);

    ScrollView *panel_frame = new ScrollView(panel, this);
    panel_widget->addWidget(panel_frame);

    QObject::connect(btn, &QPushButton::clicked, [=, w = panel_frame]() {
      btn->setChecked(true);
      panel_widget->setCurrentWidget(w);
    });
  }
  sidebar_layout->setContentsMargins(50, 50, 100, 50);

  // main settings layout, sidebar + main panel
  QHBoxLayout *main_layout = new QHBoxLayout(this);

  sidebar_widget->setFixedWidth(500);
  main_layout->addWidget(sidebar_widget);
  main_layout->addWidget(panel_widget);

  setStyleSheet(R"(
    * {
      color: white;
      font-size: 48px;
    }
    SettingsWindow {
      background-color: black;
    }
  )");
}

void SettingsWindow::hideEvent(QHideEvent *event) {
#ifdef QCOM
  HardwareEon::close_activities();
#endif
}
