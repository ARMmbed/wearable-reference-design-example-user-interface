/*
 * Copyright (c) 2016, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __UIF_UI_WRD_TABLE_VIEW_H__
#define __UIF_UI_WRD_TABLE_VIEW_H__

#include "core-util/SharedPointer.h"

#include "UIFramework/UIView.h"
#include "UIFramework/UITextMonitorView.h"
#include "UIFramework/UITableKineticView.h"

#include "wrd-touch/AnalogSlider.h"

#define SCREEN_WIDTH        128
#define SCREEN_HEIGHT       128
#define TITLE_BAR_HEIGHT    11
#define LEFT_SIDE_MARGIN    8
#define SLIDER_RESOLUTION   4000


/*  Generic menu. It has title path at the top, a highlighted field in the middle, a thin margin
    to the left, and a scrolling kinetic menu with left aligned text fields. White text is shown on a black
    background. The higlight field is an inversion of the bits beneath.

    The customization is done in the table object conforming to the UIView::Array protocol.
*/
class WRDTableView : public UITableKineticView
{
public:
    WRDTableView(SharedPointer<UIView::Array>& _table,
                  SharedPointer<AnalogSlider>& _slider);

    // UIView
    virtual uint32_t fillFrameBuffer(SharedPointer<FrameBuffer>& buffer, int16_t xOffset, int16_t yOffset);

    virtual void suspend();
    virtual void resume();
    virtual SharedPointer<UIView::Action> getAction();

private:

    void onPressTask();
    void onChangeTask();
    void onReleaseTask();

    SharedPointer<UIView::Array>    table;
    SharedPointer<AnalogSlider>     slider;
    SharedPointer<UITextView>       titlebar;

    volatile bool                   notSuspended;
};

#endif // __UIF_UI_WRD_TABLE_VIEW_H__
