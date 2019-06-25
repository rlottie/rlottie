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
#include "vbrush.h"

V_BEGIN_NAMESPACE

VGradient::VGradient(VGradient::Type type)
    : mType(type)
{
    if (mType == Type::Linear)
        linear.x1 = linear.y1 = linear.x2 = linear.y2 = 0.0f;
    else
        radial.cx = radial.cy = radial.fx =
        radial.fy = radial.cradius = radial.fradius = 0.0f;
}

void VGradient::setStops(const VGradientStops &stops)
{
    mStops = stops;
}

VBrush::VBrush(const VColor &color) : mType(VBrush::Type::Solid), mColor(color)
{
}

VBrush::VBrush(uchar r, uchar g, uchar b, uchar a)
    : mType(VBrush::Type::Solid), mColor(r, g, b, a)

{
}

VBrush::VBrush(const VGradient *gradient)
{
    if (!gradient) return;

    mGradient = gradient;

    if (gradient->mType == VGradient::Type::Linear) {
        mType = VBrush::Type::LinearGradient;
    } else if (gradient->mType == VGradient::Type::Radial) {
        mType = VBrush::Type::RadialGradient;
    }
}

VBrush::VBrush(const VBitmap &texture)
{
    if (!texture.valid()) return;

    mType = VBrush::Type::Texture;
    mTexture = texture;
}

void VBrush::setMatrix(const VMatrix &m)
{
    mMatrix = m;
}

V_END_NAMESPACE
