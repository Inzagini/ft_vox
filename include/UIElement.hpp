#pragma once

class UIElement {
  public:
    virtual void draw() = 0;
    virtual ~UIElement() = default;
};
