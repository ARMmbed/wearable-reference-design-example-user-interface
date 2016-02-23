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

#include "WRDTableView.h"

#if 0
#include "swo/swo.h"
#define printf(...) { swoprintf(__VA_ARGS__); }
#else
#define printf(...)
#endif

/*  Generic menu. It has title path at the top, a highlighted field in the middle, a thin margin
    to the left, and a scrolling kinetic menu with left aligned text fields. White text is shown on a black
    background. The higlight field is an inversion of the bits beneath.

    The customization is done in the table object conforming to the UIView::Array protocol.
*/
WRDTableView::WRDTableView(SharedPointer<UIView::Array>& _table,
                             SharedPointer<AnalogSlider>& _slider)
    :   UITableKineticView(_table,                              // table source
                           (SCREEN_WIDTH - LEFT_SIDE_MARGIN),   // UIView width
                           (SCREEN_HEIGHT - TITLE_BAR_HEIGHT),  // UIView height
                           -(TITLE_BAR_HEIGHT / 2)),            // UIView offset
        table(_table),
        slider(_slider),
        notSuspended(false)
{
    // generic title is black text on white background
    titlebar = SharedPointer<UITextView>(new UITextView(table->getTitle(), &Font_Breadcrumbs));

    // generic menu is white text on black background
    UITableKineticView::setInverse(true);

    printf("WRDTableView: %p %s\r\n", this, table->getTitle());
}


void WRDTableView::suspend()
{
    printf("WRDTableView: %s suspend\r\n", table->getTitle());

    slider->setCallOnPress(NULL);
    slider->setCallOnChange(NULL);
    slider->setCallOnRelease(NULL);

    notSuspended = false;
}

void WRDTableView::resume()
{
    printf("WRDTableView: %s resume\r\n", table->getTitle());

    slider->setCallOnPress(this, &WRDTableView::onPressTask);
    slider->setCallOnChange(this, &WRDTableView::onChangeTask);
    slider->setCallOnRelease(this, &WRDTableView::onReleaseTask);

    notSuspended = true;
}

void WRDTableView::onPressTask()
{
    printf("WRDTableView: onpress %s %d\r\n", table->getTitle(), notSuspended);

    /*  The notSuspended check is to ensure that the table does not receive updates when suspended.
    */
    if (notSuspended)
    {
        UITableKineticView::sliderPressed();
    }
}

void WRDTableView::onChangeTask()
{
    printf("WRDTableView: onchange %s %d\r\n", table->getTitle(), notSuspended);

    /*  The notSuspended check is to ensure that the table does not receive updates when suspended.
    */
    if (notSuspended)
    {
        int speed = slider->getSpeed();
        speed = (speed * SCREEN_HEIGHT)/(SLIDER_RESOLUTION);

        UITableKineticView::sliderChangedWithSpeed(speed);
    }
}

void WRDTableView::onReleaseTask()
{
    printf("WRDTableView: onrelease %s %d\r\n", table->getTitle(), notSuspended);

    /*  The notSuspended check is to ensure that the table does not receive updates when suspended.
    */
    if (notSuspended)
    {
        int speed = slider->getSpeed();
        speed = (speed * SCREEN_HEIGHT)/(SLIDER_RESOLUTION);

        UITableKineticView::sliderReleasedWithSpeed(speed);
    }
}


SharedPointer<UIView::Action> WRDTableView::getAction()
{
    /* Find selected cell index */
    unsigned int index = UITableKineticView::getMiddleIndex();

    /* Get action for current cell from table. Side effects in table are allowed. */
    SharedPointer<UIView::Action> returnObject = table->actionAtIndex(index);

    /*  If the returned object is a table itself, use it to create a new menu
        and use that as a return value instead. If the table has a title concatenate
        it to the current table to create a path.
        If the returned object is not recognized, return it to caller.
    */
    /* TODO: if actionObject is a UIViewAlert object we can show it on top of everything. */
    if (returnObject->getType() == UIView::Action::Array)
    {
        SharedPointer<UIView::Array> _table = returnObject->getArray();

        SharedPointer<UIView> menu(new WRDTableView(_table, slider));
        returnObject = SharedPointer<UIView::Action>(new UIView::Action(menu));
    }

    return returnObject;
}

static bool triangle(uint16_t x, uint16_t y)
{
    return (TITLE_BAR_HEIGHT - x) > y;
}

uint32_t WRDTableView::fillFrameBuffer(SharedPointer<FrameBuffer>& canvas, int16_t xOffset, int16_t yOffset)
{
    unsigned int callInterval = ULONG_MAX;

    int xBase = (xOffset < 0) ? xOffset : 0;
    int yBase = (yOffset < 0) ? yOffset : 0;

    /*  Title Bar

        Draw title bar, black on white.
    */
    /* draw bottom part of triangle */
    int triXPos = xBase + SCREEN_WIDTH - 32 - TITLE_BAR_HEIGHT;
    SharedPointer<FrameBuffer> triCanvas = canvas->getFrameBuffer(triXPos,
                                                                  yBase,
                                                                  TITLE_BAR_HEIGHT,
                                                                  TITLE_BAR_HEIGHT);
    int triXOffset = (triXPos < 0) ? triXPos : 0;
    triCanvas->drawFunction(&triangle, triXOffset, yBase, inverse);

    /* draw rectangle to the right of triangle */
    canvas->drawRectangle((xBase + SCREEN_WIDTH - 31 - 1), (xBase + SCREEN_WIDTH),
                          yBase, (yBase + TITLE_BAR_HEIGHT),
                          !inverse);

    /* draw rectangle beneath title */
    canvas->drawRectangle(xBase, (xBase + 85),
                          yBase, (yBase + TITLE_BAR_HEIGHT),
                          inverse);

    /* draw text */
    SharedPointer<FrameBuffer> topCanvas =
                            canvas->getFrameBuffer(xBase + LEFT_SIDE_MARGIN,
                                                   yBase + 2,
                                                   (SCREEN_WIDTH - LEFT_SIDE_MARGIN),
                                                   TITLE_BAR_HEIGHT);
    int titlebarXOffset = ((xBase + LEFT_SIDE_MARGIN) < 0) ? xBase + LEFT_SIDE_MARGIN : 0;
    int titlebarYOffset = ((yBase + 2) < 0) ? yBase + 2 : 0;

    // propate color inversion
    titlebar->setInverse(!inverse);

    titlebar->fillFrameBuffer(topCanvas, titlebarXOffset, titlebarYOffset);


    /*  Menu - Kinetic scrolling table
    */

    /*  propate color inversion
    */
    canvas->drawRectangle(0, LEFT_SIDE_MARGIN, TITLE_BAR_HEIGHT, canvas->getHeight(), !inverse);

    /* propate color inversion to table */
    UITableKineticView::setInverse(inverse);

    /* draw table at (8,8) leaving room for title bar and left margin. */
    int tableXPos = xBase + LEFT_SIDE_MARGIN;
    int tableYPos = yBase + TITLE_BAR_HEIGHT;
    int tableXOffset = (tableXPos < 0) ? tableXPos : 0;
    int tableYOffset = (tableYPos < 0) ? tableYPos : 0;

    SharedPointer<FrameBuffer> subcanvas =
                            canvas->getFrameBuffer(tableXPos,
                                                   tableYPos,
                                                   (SCREEN_WIDTH - LEFT_SIDE_MARGIN),
                                                   (SCREEN_HEIGHT - TITLE_BAR_HEIGHT));
    callInterval = UITableKineticView::fillFrameBuffer(subcanvas, tableXOffset, tableYOffset);

    /*  Selection box

        Draw a selection bar on the middle of the screen by inverting all pixels.
    */
    unsigned int invertMinY = tableYOffset + (SCREEN_HEIGHT / 2) - 18;
    invertMinY = (invertMinY > 0) ? invertMinY : 0;

    unsigned int invertMaxY = tableYOffset + (SCREEN_HEIGHT / 2) + 17;
    invertMaxY = (invertMaxY > 0) ? invertMaxY : 0;

    canvas->invertRectangle(0, SCREEN_WIDTH, invertMinY, invertMaxY);

    return callInterval;
}


