/* Copyright (c) 2018 Samsung Electronics Co., Ltd. All rights reserved.

 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include"evasapp.h"
#include "lottieview.h"
#include <memory>
#include<vector>
#include<string>

class UxSampleTest
{
public:
  UxSampleTest(EvasApp *app, bool renderMode) {
      mApp = app;
      mRenderMode = renderMode;
      mResourceList = EvasApp::jsonFiles(std::string(DEMO_DIR) + "UXSample_1920x1080/");
      mRepeatMode = LottieView::RepeatMode::Restart;
  }

  void showPrev() {
      if (mResourceList.empty()) return;
      mCurIndex--;
      if (mCurIndex < 0)
          mCurIndex = mResourceList.size() - 1;
      show();
  }

  void showNext() {
    if (mResourceList.empty()) return;

    mCurIndex++;
    if (mCurIndex >= int(mResourceList.size()))
        mCurIndex = 0;
    show();
  }

  void resize() {
      if (mView) {
          mView->setSize(mApp->width(), mApp->height());
      }
  }

private:
  void show() {
      mView = std::make_unique<LottieView>(mApp->evas(), Strategy::renderCAsync);
      mView->setFilePath(mResourceList[mCurIndex].c_str());
      mView->setPos(0, 0);
      mView->setSize(mApp->width(), mApp->height());
      mView->show();
      mView->play();
      mView->loop(true);
      mView->setRepeatMode(mRepeatMode);
  }

public:
  EvasApp                    *mApp;
  bool                        mRenderMode = false;
  int                         mCurIndex = -1;
  std::vector<std::string>    mResourceList;
  std::unique_ptr<LottieView> mView;
  LottieView::RepeatMode      mRepeatMode;
};

static void
onExitCb(void *data, void */*extra*/)
{
    UxSampleTest *view = (UxSampleTest *)data;
    delete view;
}

static void
onKeyCb(void *data, void *extra)
{
    UxSampleTest *view = (UxSampleTest *)data;
    char *keyname = (char *)extra;

    if (!strcmp(keyname, "Right") || !strcmp(keyname, "n")) {
        view->showNext();
    } else if (!strcmp(keyname, "Left") || !strcmp(keyname, "p")) {
        view->showPrev();
    } else if (!strcmp(keyname,"r")) {
        if (view->mRepeatMode == LottieView::RepeatMode::Restart) {
            view->mRepeatMode = LottieView::RepeatMode::Reverse;
        } else
            view->mRepeatMode = LottieView::RepeatMode::Restart;
        if (view->mView)
            view->mView->setRepeatMode(view->mRepeatMode);
    }
}

static void
onResizeCb(void *data, void */*extra*/)
{
    UxSampleTest *view = (UxSampleTest *)data;
    view->resize();
}

int
main(int argc, char **argv)
{
   EvasApp *app = new EvasApp(800, 800);
   app->setup();

   bool renderMode = true;
   if (argc > 1) {
       if (!strcmp(argv[1],"--disable-render"))
           renderMode = false;
   }
   UxSampleTest *view = new UxSampleTest(app, renderMode);
   view->showNext();

   app->addExitCb(onExitCb, view);
   app->addKeyCb(onKeyCb, view);
   app->addResizeCb(onResizeCb, view);

   app->run();
   delete app;
   return 0;
}
