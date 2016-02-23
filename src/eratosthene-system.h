/*
 *  liberatosthene - geodetic system
 *
 *      Nils Hamel - nils.hamel@bluewin.ch
 *      Copyright (c) 2016 EPFL CDH DHLAB
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

    /*! \file   eratosthene-system.h
     *  \author Nils Hamel <n.hamel@bluewin.ch>
     *
     *  System module
     */

/*
    header - inclusion guard
 */

    # ifndef __LE_SYSTEM_INJECT__
    # define __LE_SYSTEM_INJECT__

/*
    header - C/C++ compatibility
 */

    # ifdef __cplusplus
    extern "C" {
    # endif

/*
    header - includes
 */

    # include "eratosthene.h"

/*
    header - preprocessor definitions
 */

    /* Define pseudo-constructor */
    # define LE_SYSTEM__ { 0, 0, NULL, 0xFFFFFFFFFFFFFFFF }

/*
    header - preprocessor macros
 */

/*
    header - type definition
 */

/*
    header - structures
 */

    typedef struct le_system_struct {

        le_time_t sm_terl;
        le_size_t sm_depth;
        FILE **   sm_scale;
        le_time_t sm_time;

    } le_system_t;

/*
    header - function prototypes
 */

    le_enum_t le_system_open( le_system_t * const le_system, le_time_t const le_time, le_char_t const * const le_root );
    le_void_t le_system_close( le_system_t * const le_system );

/*
    header - C/C++ compatibility
 */

    # ifdef __cplusplus
    }
    # endif

/*
    header - inclusion guard
 */

    # endif

