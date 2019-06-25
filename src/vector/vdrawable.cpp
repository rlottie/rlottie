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
#include "vdrawable.h"
#include "vdasher.h"
#include "vraster.h"

void VDrawable::preprocess(const VRect &clip)
{
    if (mFlag & (DirtyState::Path)) {
        if (mStroke.enable) {
            if (mStroke.mDash.size()) {
                VDasher dasher(mStroke.mDash.data(), mStroke.mDash.size());
                mPath.clone(dasher.dashed(mPath));
            }
            mRasterizer.rasterize(std::move(mPath), mStroke.cap, mStroke.join,
                                  mStroke.width, mStroke.miterLimit, clip);
        } else {
            mRasterizer.rasterize(std::move(mPath), mFillRule, clip);
        }
        mPath = {};
        mFlag &= ~DirtyFlag(DirtyState::Path);
    }
}

VRle VDrawable::rle()
{
    return mRasterizer.rle();
}

void VDrawable::setStrokeInfo(CapStyle cap, JoinStyle join, float miterLimit,
                              float strokeWidth)
{
    if ((mStroke.cap == cap) && (mStroke.join == join) &&
        vCompare(mStroke.miterLimit, miterLimit) &&
        vCompare(mStroke.width, strokeWidth))
        return;

    mStroke.enable = true;
    mStroke.cap = cap;
    mStroke.join = join;
    mStroke.miterLimit = miterLimit;
    mStroke.width = strokeWidth;
    mFlag |= DirtyState::Path;
}

void VDrawable::setDashInfo(std::vector<float> &dashInfo)
{
    bool hasChanged = false;

    if (mStroke.mDash.size() == dashInfo.size()) {
        for (uint i = 0; i < dashInfo.size(); i++) {
            if (!vCompare(mStroke.mDash[i], dashInfo[i])) {
                hasChanged = true;
                break;
            }
        }
    } else {
        hasChanged = true;
    }

    if (!hasChanged) return;

    mStroke.mDash = dashInfo;

    mFlag |= DirtyState::Path;
}

void VDrawable::setPath(const VPath &path)
{
    mPath = path;
    mFlag |= DirtyState::Path;
}
