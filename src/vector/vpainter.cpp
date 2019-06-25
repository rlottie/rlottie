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

#include "vpainter.h"
#include <algorithm>
#include "vdrawhelper.h"

V_BEGIN_NAMESPACE

class VPainterImpl {
public:
    void drawRle(const VPoint &pos, const VRle &rle);
    void drawRle(const VRle &rle, const VRle &clip);
    void setCompositionMode(VPainter::CompositionMode mode)
    {
        mSpanData.mCompositionMode = mode;
    }
    void drawBitmapUntransform(const VRect &target, const VBitmap &bitmap,
                               const VRect &source, uint8_t const_alpha);

public:
    VRasterBuffer mBuffer;
    VSpanData     mSpanData;
};

void VPainterImpl::drawRle(const VPoint &, const VRle &rle)
{
    if (rle.empty()) return;
    // mSpanData.updateSpanFunc();

    if (!mSpanData.mUnclippedBlendFunc) return;

    // do draw after applying clip.
    rle.intersect(mSpanData.clipRect(), mSpanData.mUnclippedBlendFunc,
                  &mSpanData);
}

void VPainterImpl::drawRle(const VRle &rle, const VRle &clip)
{
    if (rle.empty() || clip.empty()) return;

    if (!mSpanData.mUnclippedBlendFunc) return;

    rle.intersect(clip, mSpanData.mUnclippedBlendFunc, &mSpanData);
}

static void fillRect(const VRect &r, VSpanData *data)
{
    auto x1 = std::max(r.x(), 0);
    auto x2 = std::min(r.x() + r.width(), data->mDrawableSize.width());
    auto y1 = std::max(r.y(), 0);
    auto y2 = std::min(r.y() + r.height(), data->mDrawableSize.height());

    if (x2 <= x1 || y2 <= y1) return;

    const int  nspans = 256;
    VRle::Span spans[nspans];

    int y = y1;
    while (y < y2) {
        int n = std::min(nspans, y2 - y);
        int i = 0;
        while (i < n) {
            spans[i].x = short(x1);
            spans[i].len = ushort(x2 - x1);
            spans[i].y = short(y + i);
            spans[i].coverage = 255;
            ++i;
        }

        data->mUnclippedBlendFunc(n, spans, data);
        y += n;
    }
}

void VPainterImpl::drawBitmapUntransform(const VRect &  target,
                                         const VBitmap &bitmap,
                                         const VRect &  source,
                                         uint8_t        const_alpha)
{
    mSpanData.initTexture(&bitmap, const_alpha, VBitmapData::Plain, source);
    if (!mSpanData.mUnclippedBlendFunc) return;
    mSpanData.dx = float(-target.x());
    mSpanData.dy = float(-target.y());

    VRect rr = source.translated(target.x(), target.y());

    fillRect(rr, &mSpanData);
}

VPainter::~VPainter()
{
    delete mImpl;
}

VPainter::VPainter()
{
    mImpl = new VPainterImpl;
}

VPainter::VPainter(VBitmap *buffer)
{
    mImpl = new VPainterImpl;
    begin(buffer);
}
bool VPainter::begin(VBitmap *buffer)
{
    mImpl->mBuffer.prepare(buffer);
    mImpl->mSpanData.init(&mImpl->mBuffer);
    // TODO find a better api to clear the surface
    mImpl->mBuffer.clear();
    return true;
}
void VPainter::end() {}

void VPainter::setDrawRegion(const VRect &region)
{
    mImpl->mSpanData.setDrawRegion(region);
}

void VPainter::setBrush(const VBrush &brush)
{
    mImpl->mSpanData.setup(brush);
}

void VPainter::setCompositionMode(CompositionMode mode)
{
    mImpl->setCompositionMode(mode);
}

void VPainter::drawRle(const VPoint &pos, const VRle &rle)
{
    mImpl->drawRle(pos, rle);
}

void VPainter::drawRle(const VRle &rle, const VRle &clip)
{
    mImpl->drawRle(rle, clip);
}

VRect VPainter::clipBoundingRect() const
{
    return mImpl->mSpanData.clipRect();
}

void VPainter::drawBitmap(const VPoint &point, const VBitmap &bitmap,
                          const VRect &source, uint8_t const_alpha)
{
    if (!bitmap.valid()) return;

    drawBitmap(VRect(point, bitmap.size()),
               bitmap, source, const_alpha);
}

void VPainter::drawBitmap(const VRect &target, const VBitmap &bitmap,
                          const VRect &source, uint8_t const_alpha)
{
    if (!bitmap.valid()) return;

    // clear any existing brush data.
    setBrush(VBrush());

    if (target.size() == source.size()) {
        mImpl->drawBitmapUntransform(target, bitmap, source, const_alpha);
    } else {
        // @TODO scaling
    }
}

void VPainter::drawBitmap(const VPoint &point, const VBitmap &bitmap,
                          uint8_t const_alpha)
{
    if (!bitmap.valid()) return;

    drawBitmap(VRect(point, bitmap.size()),
               bitmap, bitmap.rect(),
               const_alpha);
}

void VPainter::drawBitmap(const VRect &rect, const VBitmap &bitmap,
                          uint8_t const_alpha)
{
    if (!bitmap.valid()) return;

    drawBitmap(rect, bitmap, bitmap.rect(),
               const_alpha);
}

V_END_NAMESPACE
