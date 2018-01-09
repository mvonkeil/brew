/**
 *
 *  |_  _ _
 *  |_)| (/_VV
 *
 *  Copyright 2015-2018 [insert fancy company name here]
 *
 *  Created on: 08.01.18
 *
 */

#include <brew/video/ExtendViewport.h>
#include <cmath>

namespace brew {

ExtendViewport::ExtendViewport(Real minVirtualWidth, Real minVirtualHeight, Real maxVirtualWidth, Real maxVirtualHeight,
                               std::shared_ptr<Camera> camera)
        : Viewport(minVirtualWidth, minVirtualHeight, camera) {
    minSize.set(minVirtualWidth, minVirtualHeight);
    maxSize.set(maxVirtualWidth, maxVirtualHeight);
}

ExtendViewport::ExtendViewport(const Vec2& minSize, const Vec2& maxSize, std::shared_ptr<Camera> camera)
        : Viewport(minSize, camera) {
    this->minSize = minSize;
    this->maxSize = maxSize;
}

void ExtendViewport::update(SizeT physicalWidth, SizeT physicalHeight) {
    Viewport::update(physicalWidth, physicalHeight);

    Vec2 virtualSize = getMinimumVirtualSize();

    Real physicalRatio = physicalHeight / physicalWidth;
    Real virtualRatio = getVirtualSize().y / getVirtualSize().x;

    Real scale =
            physicalRatio > virtualRatio ? physicalWidth / getVirtualSize().x : physicalHeight / getVirtualSize().y;

    Vec2 scaled = minSize * scale;

    // Extend in the short direction.
    auto viewportWidth = static_cast<SizeT>(std::round(scaled.x));
    auto viewportHeight = static_cast<SizeT>(std::round(scaled.y));

    if (viewportWidth < physicalWidth) {
        Real toViewportSpace = viewportHeight / virtualSize.y;
        Real toWorldSpace = virtualSize.y / viewportHeight;
        Real lengthen = (physicalWidth - viewportWidth) * toWorldSpace;

        if (maxSize.x > 0) {
            lengthen = std::min(lengthen, maxSize.x - minSize.x);
        }

        virtualSize.x += lengthen;
        viewportWidth += std::round(lengthen * toViewportSpace);
    } else if (viewportHeight < physicalHeight) {
        Real toViewportSpace = viewportWidth / virtualSize.x;
        Real toWorldSpace = virtualSize.x / viewportWidth;
        Real lengthen = (physicalHeight - viewportHeight) * toWorldSpace;

        if (maxSize.y > 0) {
            lengthen = std::min(lengthen, maxSize.y - minSize.y);
        }

        virtualSize.y += lengthen;
        viewportHeight += std::round(lengthen * toViewportSpace);
    }

    setVirtualSize(virtualSize);

    // Center.
    setPhysicalBounds(static_cast<SizeT>((physicalWidth - viewportWidth) / 2),
                      static_cast<SizeT>((physicalHeight - viewportHeight) / 2),
                      viewportWidth,
                      viewportHeight);
}

void ExtendViewport::setMinimumVirtualSize(const Vec2& minSize) {
    this->minSize = minSize;
}

void ExtendViewport::setMaximumVirtualSize(const Vec2& maxSize) {
    this->maxSize = maxSize;
}

} /* namespace brew */