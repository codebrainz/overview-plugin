/*
 * overviewui.h - This file is part of the Geany Overview plugin
 *
 * Copyright (c) 2015 Matthew Brush <mbrush@codebrainz.ca>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 */

#ifndef OVERVIEWUI_H_
#define OVERVIEWUI_H_

#include "overviewprefs.h"
#include <gtk/gtk.h>

void       overview_ui_init                     (OverviewPrefs *prefs);
void       overview_ui_deinit                   (void);
GtkWidget *overview_ui_get_menu_item            (void);
gboolean   overview_ui_supports_left_position   (void);

#endif // OVERVIEWUI_H_
