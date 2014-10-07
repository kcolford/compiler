/**
 * @file   attributes.h
 * @author Kieran Colford <colfordk@gmail.com>
 * 
 * @brief  This is the header file for attribute declarations.
 * 
 * Copyright (C) 2014 Kieran Colford
 *
 * This file is part of Mongoose.
 *
 * Mongoose is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mongoose is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Mongoose; see the file COPYING.  If not see
 * <http://www.gnu.org/licenses/>.
 *
 */

#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#if defined __GNUC__ && 2 < __GNUC__
#define ATTRIBUTE(ALIST) __attribute__ (ALIST)
#else
#define ATTRIBUTE(ALIST)	/* empty */
#endif

#define ATTRIBUTE_PURE ATTRIBUTE ((__pure__))
#define ATTRIBUTE_CONST ATTRIBUTE ((__const__))
#define ATTRIBUTE_MALLOC ATTRIBUTE ((__malloc__))
#define ATTRIBUTE_NONNULL(...) ATTRIBUTE ((__nonnull__ (__VA_ARGS__)))

#endif
