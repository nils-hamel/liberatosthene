/*
 *  liberatosthene
 *
 *      Nils Hamel - nils.hamel@bluewin.ch
 *      Copyright (c) 2016-2020 DHLAB, EPFL
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
 */

    /*! \file   eratosthene-uv3.h
     *  \author Nils Hamel <nils.hamel@bluewin.ch>
     *
     *  liberatosthene - operator
     */

/*
    header - inclusion guard
 */

    # ifndef __LE_OPERATOR__
    # define __LE_OPERATOR__

/*
    header - C/C++ compatibility
 */

    # ifdef __cplusplus
    extern "C" {
    # endif

/*
    header - internal includes
 */

    # include "eratosthene.h"
    # include "eratosthene-array.h"
    # include "eratosthene-uv3.h"

/*
    header - external includes
 */

/*
    header - preprocessor definitions
 */

    /* define detection flag */
    # define LE_OPER_OFF ( 0x00 )
    # define LE_OPER_SET ( 0xff )

/*
    header - preprocessor macros
 */

/*
    header - type definition
 */

/*
    header - structures
 */

/*
    header - function prototypes
 */

    /*! \brief accessor methods - missing documentation */

    void le_operator_get_marker( le_array_t * const le_array, le_array_t * const le_beacon, le_size_t const le_offset );

    /*! \brief accessor methods - missing documentation */

    void le_operator_get_dd( le_array_t * const le_pri, le_array_t * const le_sec, le_array_t * const le_beacon, le_size_t const le_pri_offset, le_size_t const le_sec_offset );

    /*! \brief removal */

    void le_operator_get_dd_( le_array_t * const le_pri, le_array_t * const le_sec, le_array_t * const le_mark );

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

