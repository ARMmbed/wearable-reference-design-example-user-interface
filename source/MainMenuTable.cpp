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

#include "MainMenuTable.h"

#include "UIFramework/UIImageView.h"
#include "UIFramework/UITextView.h"
#include "UIFramework/UITextMonitorView.h"
#include "UIFramework/UITableView.h"

#include "uif-ui-watch-face-wrd/SetTimeMenuTable.h"
#include "uif-ui-touch-calibration-wrd/TouchCalibrationView.h"

typedef enum {
    CELL_TOP_FILLER = 0,
    CELL_SETTINGS,
    CELL_TOUCH,
    CELL_END_FILLER
} entries_t;

SharedPointer<UIView> MainMenuTable::viewAtIndex(uint32_t index) const
{
    UIView* cell = NULL;

    switch(index)
    {
        // top filler
        case CELL_TOP_FILLER:
                            cell = new UIImageView(NULL);
                            break;

        // settings menu
        case CELL_SETTINGS:
                            cell = new UITextView("Settings", &Font_Menu);
                            break;

        // debug menu
        case CELL_TOUCH:
                            cell = new UITextView("Touch UI", &Font_Menu);
                            break;

        // bottom filler
        case CELL_END_FILLER:
        default:
                            cell = new UIImageView(NULL);
                            break;
    }

    if (cell)
    {
        cell->setHorizontalAlignment(UIView::ALIGN_LEFT);
        cell->setVerticalAlignment(UIView::VALIGN_MIDDLE);
        cell->setWidth(0);
        cell->setHeight(0);
    }

    return SharedPointer<UIView>(cell);
}

uint32_t MainMenuTable::getSize() const
{
    return CELL_END_FILLER + 1;
}

uint32_t MainMenuTable::widthAtIndex(uint32_t index) const
{
    (void) index;

    return 128;
}

uint32_t MainMenuTable::heightAtIndex(uint32_t index) const
{
    uint32_t ret = 0;

    if ( (index == CELL_TOP_FILLER) || (index == CELL_END_FILLER) )
    {
        ret = 128;
    }
    else
    {
        ret = 35;
    }

    return ret;
}

SharedPointer<UIView::Action> MainMenuTable::actionAtIndex(uint32_t index)
{
    SharedPointer<UIView::Action> returnObject;

    switch (index)
    {
        case CELL_SETTINGS:
                            {
                                SharedPointer<UIView::Array> table(new SetTimeMenuTable());
                                returnObject = SharedPointer<UIView::Action>(new UIView::Action(table));
                            }
                            break;

        case CELL_TOUCH:
                            {
                                SharedPointer<UIView> touch(new TouchCalibrationView());
                                returnObject = SharedPointer<UIView::Action>(new UIView::Action(touch));
                            }
                            break;

        default:
                            returnObject = SharedPointer<UIView::Action>(new UIView::Action);
                            break;
    }

    return returnObject;
}


const char* MainMenuTable::getTitle() const
{
    return "menu";
}

uint32_t MainMenuTable::getFirstIndex() const
{
    return CELL_TOP_FILLER + 1;
}

uint32_t MainMenuTable::getLastIndex() const
{
    return CELL_END_FILLER - 1;
}

uint32_t MainMenuTable::getDefaultIndex() const
{
    return CELL_SETTINGS;
}
