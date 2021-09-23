# @hanabi95님의 코드를 기반으로 하고있습니다. 항상 감사드립니다.

### 일반 사용자분들은 "release" 브랜치를 사용하세요. 버전이 올라가도 최신을 그곳에 유지하겠습니다.



- 볼트EV 대상 한국어 오픈파일럿 포크입니다.
- hanabi95(@hanabi95) 님의 작업을 기본으로 하고 있습니다. 항상 감사드립니다.
- 블랙순정판다, Lepro, 볼트EV 2019 프리미터 세이프티 차량에서 공기압 계기판 기준 250으로  타고있습니다.

문의사항은 오카방 혹은 이슈 생성으로 주십시오. 감사합니다.

rkjnice@gmail.com



To use openpilot in a car, you need four things
* This software. It's free and available right here.
* One of [the 140+ supported cars](docs/CARS.md). We support Honda, Toyota, Hyundai, Nissan, Kia, Chrysler, Lexus, Acura, Audi, VW, and more. If your car is not supported, but has adaptive cruise control and lane keeping assist, it's likely able to run openpilot.
* A supported device to run this software. This can be a [comma two](https://comma.ai/shop/products/two), [comma three](https://comma.ai/shop/products/three), or if you like to experiment, a [Ubuntu computer with webcams](https://github.com/commaai/openpilot/tree/master/tools/webcam).
* A way to connect to your car. With a comma two or three, you need only a [car harness](https://comma.ai/shop/products/car-harness). With an EON Gold or PC, you also need a [black panda](https://comma.ai/shop/products/panda).



###for record purpose, beware mereging upstream, check those files.
* controls.hpp	Conflicted (Both modified)	selfdrive/ui/qt/widgets
* paint.cc	Conflicted (Both modified)	selfdrive/ui
* ui.cc	Conflicted (Both modified)	selfdrive/ui
* ui.hpp	Conflicted (Both modified)	selfdrive/ui
* latcontrol_pid.py	Conflicted (Both modified)	selfdrive/controls/lib
* lateral_planner.py	Conflicted (Both modified)	selfdrive/controls/lib
* controlsd.py	Conflicted (Both modified)	selfdrive/controls
* interface.py	Conflicted (Both modified)	selfdrive/car/gm
* values.py	Conflicted (Both modified)	selfdrive/car/gm
* interfaces.py	Conflicted (Both modified)	selfdrive/car
* boardd.cc	Conflicted (Both modified)	selfdrive/boardd
* pigeon.cc	Conflicted (Both modified)	selfdrive/boardd
* SConstruct	Conflicted (Both modified)
* latcontrol_lqr.py	Staged	selfdrive/controls/lib
