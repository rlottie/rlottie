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

#include"lottieview.h"

using namespace rlottie;

static void
_image_update_cb(void *data, Evas_Object *obj EINA_UNUSED)
{
   RenderStrategy *info = (RenderStrategy *)data;
   info->dataCb();
}

void RenderStrategy::addCallback(){
    evas_object_image_pixels_get_callback_set(_renderObject, _image_update_cb, this);
}

static Eina_Bool
animator(void *data , double pos)
{
    LottieView *view = static_cast<LottieView *>(data);

    view->seek(pos);
    if (pos == 1.0) {
      view->mAnimator = NULL;
      view->finished();
      return EINA_FALSE;
    }
    return EINA_TRUE;
}

LottieView::LottieView(Evas *evas, Strategy s) {
    mPalying = false;
    mReverse = false;
    mRepeatCount = 0;
    mRepeatMode = LottieView::RepeatMode::Restart;
    mLoop = false;
    mSpeed = 1;

    switch (s) {
    case Strategy::renderCpp: {
        mRenderDelegate = std::make_unique<RlottieRenderStrategy_CPP>(evas);
        break;
    }
    case Strategy::renderCppAsync: {
        mRenderDelegate = std::make_unique<RlottieRenderStrategy_CPP_ASYNC>(evas);
        break;
    }
    case Strategy::renderC: {
        mRenderDelegate = std::make_unique<RlottieRenderStrategy_C>(evas);
        break;
    }
    case Strategy::renderCAsync: {
        mRenderDelegate = std::make_unique<RlottieRenderStrategy_C_ASYNC>(evas);
        break;
    }
    case Strategy::eflVg: {
        mRenderDelegate = std::make_unique<EflVgRenderStrategy>(evas);
        break;
    }
    default:
        mRenderDelegate = std::make_unique<RlottieRenderStrategy_CPP>(evas);
        break;
    }
}

LottieView::~LottieView()
{
    if (mAnimator) ecore_animator_del(mAnimator);
}

Evas_Object *LottieView::getImage() {
    return mRenderDelegate->renderObject();
}

void LottieView::show()
{
    mRenderDelegate->show();
    seek(0);
}

void LottieView::hide()
{
    mRenderDelegate->hide();
}

void LottieView::seek(float pos)
{
    if (!mRenderDelegate) return;


    mPos = mapProgress(pos);

    // check if the pos maps to the current frame
    if (mCurFrame == mRenderDelegate->frameAtPos(mPos)) return;

    mCurFrame = mRenderDelegate->frameAtPos(mPos);

    mRenderDelegate->render(mCurFrame);
}

float LottieView::getPos()
{
   return mPos;
}

void LottieView::setFilePath(const char *filePath)
{
    mRenderDelegate->loadFromFile(filePath);
}

void LottieView::loadFromData(const std::string &jsonData, const std::string &key, const std::string &resourcePath)
{
    mRenderDelegate->loadFromData(jsonData, key, resourcePath);
}

void LottieView::setSize(int w, int h)
{
    mRenderDelegate->resize(w, h);
}

void LottieView::setPos(int x, int y)
{
    mRenderDelegate->setPos(x, y);
}

void LottieView::finished()
{
    restart();
}

void LottieView::loop(bool loop)
{
    mLoop = loop;
}

void LottieView::setRepeatCount(int count)
{
    mRepeatCount = count;
}

void LottieView::setRepeatMode(LottieView::RepeatMode mode)
{
    mRepeatMode = mode;
}

void LottieView::play()
{
    if (mAnimator) ecore_animator_del(mAnimator);
    mAnimator = ecore_animator_timeline_add(duration()/mSpeed, animator, this);
    mReverse = false;
    mCurCount = mRepeatCount;
    mPalying = true;
}

void LottieView::pause()
{

}

void LottieView::stop()
{
    mPalying = false;
    if (mAnimator) {
        ecore_animator_del(mAnimator);
        mAnimator = NULL;
    }
}

void LottieView::restart()
{
    mCurCount--;
    if (mLoop || mRepeatCount) {
        if (mRepeatMode == LottieView::RepeatMode::Reverse)
            mReverse = !mReverse;
        else
            mReverse = false;

        if (mAnimator) ecore_animator_del(mAnimator);
        mAnimator = ecore_animator_timeline_add(duration()/mSpeed, animator, this);
    }
}
