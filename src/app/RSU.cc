//
// Copyright (C) 2016 David Eckhoff <david.eckhoff@fau.de>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "app/RSU.h"

#include "app/messages/HelpMessage_m.h"

using namespace tirocinio;

Define_Module(tirocinio::RSU);

void RSU::onWSA(veins::DemoServiceAdvertisment* wsa)
{
    // Do something when receiving a WSA
}

void RSU::onWSM(veins::BaseFrame1609_4* frame)
{
    // Do something when receiving a WSM
}
