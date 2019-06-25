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
#include "evasapp.h"
#include "lottieview.h"
#include<iostream>
#include <stdio.h>
#include <fstream>
#include <sstream>
using namespace std;

static void
onExitCb(void *data, void */*extra*/)
{
    LottieView *view = (LottieView *)data;
    delete view;
}

int
main(void)
{
   EvasApp *app = new EvasApp(800, 800);
   app->setup();

   std::string filePath = DEMO_DIR;
   filePath +="circuit.json";

   LottieView *view = new LottieView(app->evas());
   view->setFilePath(filePath.c_str());
   if (view->player()) {
       view->player()->setValue<rlottie::Property::FillColor>("**",
           [](const rlottie::FrameInfo& info) {
                if (info.curFrame() < 15 )
                    return rlottie::Color(0, 1, 0);
                else {
                    return rlottie::Color(1, 0, 0);
                }
            });
   }
   view->setPos(0, 0);
   view->setSize(800, 800);
   view->show();
//   view->setMinProgress(0.5);
//   view->setMaxProgress(0.0);
   view->play();
   view->loop(true);
   view->setRepeatMode(LottieView::RepeatMode::Reverse);

   app->addExitCb(onExitCb, view);
   app->run();
   delete app;
   return 0;
}





