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
#include <dirent.h>
#include <algorithm>

static void
_on_resize(Ecore_Evas *ee)
{
   EvasApp *app = (EvasApp *)ecore_evas_data_get(ee, "app");
   int w, h;
   ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);
   app->resize(w, h);
   if (app->mResizeCb)
       app->mResizeCb(app->mResizeData, nullptr);
}

static void
_on_delete(Ecore_Evas *ee)
{
   EvasApp *app = (EvasApp *)ecore_evas_data_get(ee, "app");
   if (app->mExitCb)
       app->mExitCb(app->mExitData, nullptr);

   ecore_main_loop_quit();
   ecore_evas_free(ee);
}

static Eina_Bool
on_key_down(void *data, int /*type*/, void *event)
{
    Ecore_Event_Key *keyData = (Ecore_Event_Key *)event;

    EvasApp *app = (EvasApp *) data;
    if (app && app->mKeyCb)
        app->mKeyCb(app->mKeyData, (void *)keyData->key);

    return false;
}

static void
on_pre_render(Ecore_Evas *ee)
{
    EvasApp *app = (EvasApp *)ecore_evas_data_get(ee, "app");
    if (app->mRenderPreCb)
        app->mRenderPreCb(app->mRenderPreData, nullptr);
}

static void
on_post_render(Ecore_Evas *ee)
{
    EvasApp *app = (EvasApp *)ecore_evas_data_get(ee, "app");
    if (app && app->mRenderPostCb)
        app->mRenderPostCb(app->mRenderPostData, nullptr);
}

EvasApp::EvasApp(int w, int h)
{
    if (!ecore_evas_init())
     return;
    mw = w;
    mh = h;
    //setenv("ECORE_EVAS_ENGINE", "opengl_x11", 1);
    mEcoreEvas = ecore_evas_new(NULL, 0, 0, mw, mh, NULL);
    if (!mEcoreEvas) return;
}

void
EvasApp::setup()
{
    ecore_evas_data_set(mEcoreEvas, "app", this);
    ecore_evas_callback_resize_set(mEcoreEvas, _on_resize);
    ecore_evas_callback_delete_request_set(mEcoreEvas, _on_delete);
    ecore_event_handler_add(ECORE_EVENT_KEY_DOWN, on_key_down, this);
    ecore_evas_callback_pre_render_set(mEcoreEvas, on_pre_render);
    ecore_evas_callback_post_render_set(mEcoreEvas, on_post_render);

    ecore_evas_show(mEcoreEvas);
    mEvas = ecore_evas_get(mEcoreEvas);
    mBackground = evas_object_rectangle_add(mEvas);
    evas_object_color_set(mBackground, 70, 70, 70, 255);
    evas_object_show(mBackground);
}

void
EvasApp::resize(int w, int h)
{
    evas_object_resize(mBackground, w, h);
    mw = w;
    mh = h;
}

void EvasApp::run()
{
    resize(mw, mh);
    ecore_main_loop_begin();
    ecore_evas_shutdown();
}

static bool isJsonFile(const char *filename) {
  const char *dot = strrchr(filename, '.');
  if(!dot || dot == filename) return false;
  return !strcmp(dot + 1, "json");
}

std::vector<std::string>
EvasApp::jsonFiles(const std::string &dirName, bool /*recurse*/)
{
    DIR *d;
    struct dirent *dir;
    std::vector<std::string> result;
    d = opendir(dirName.c_str());
    if (d) {
      while ((dir = readdir(d)) != NULL) {
        if (isJsonFile(dir->d_name))
          result.push_back(dirName + dir->d_name);
      }
      closedir(d);
    }

    std::sort(result.begin(), result.end(), [](auto & a, auto &b){return a < b;});

    return result;
}

