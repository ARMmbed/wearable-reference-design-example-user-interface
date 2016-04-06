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

#ifndef __UI_H__
#define __UI_H__

#include "core-util/SharedPointer.h"

#include "UIFramework/UIView.h"
#include "UIFramework/UIViewStack.h"

#include "wrd-touch/AnalogButton.h"
#include "wrd-touch/AnalogSlider.h"

class UI : public UIView
{
public:
    UI(void);
    ~UI(void);

    // UIView
    virtual uint32_t fillFrameBuffer(SharedPointer<FrameBuffer>& buffer, int16_t xOffset, int16_t yOffset);
    void setWakeupCallback(FunctionPointer& wakeup);

private:
    void goBackPressISR(void);
    void goBackReleaseISR(void);
    void goBackTask(void);
    void goWayBackTask(void);

    void buttonPressISR(void);
    void buttonReleaseISR(void);
    void buttonTask(void);
    void resetTask(void);

    void wakeupTask(void);

private:
    SharedPointer<UIViewStack> stack;
    SharedPointer<UIView> currentCell;
    SharedPointer<UIView> mainMenu;

    SharedPointer<AnalogSlider> slider;

    InterruptIn forwardButton;

#if defined(YOTTA_CFG_HARDWARE_WRD_TOUCH_SLIDER_BACK)
    AnalogButton backButton;
#else
    InterruptIn backButton;
#endif

    int32_t forwardCounter;
    int32_t backwardCounter;

    uint32_t buttonLastPress;
    minar::callback_handle_t buttonHandle;
    minar::callback_handle_t goBackHandle;
    minar::callback_handle_t goWayBackHandle;
    minar::callback_handle_t resetHandle;
};


#endif // __UI_H__
