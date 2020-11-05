/*
 *  liberatosthene
 *
 *      Nils Hamel - nils.hamel@alumni.epfl.ch
 *      Copyright (c) 2016-2020 DHLAB, EPFL
 *      Copyright (c) 2020 Republic and Canton of Geneva
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

    /*! \file   eratosthene-geodesy.h
     *  \author Nils Hamel <nils.hamel@bluewin.ch>
     *
     *  liberatosthene - geodesy
     */

/*
    header - inclusion guard
 */

    # ifndef __LE_GEODESY__
    # define __LE_GEODESY__

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
    # include "eratosthene-address.h"

/*
    header - external includes
 */

/*
    header - preprocessor definitions
 */

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

    /*! \brief mutator methods
     *
     *  This function allows to compute the optimal scale at which a poly-vertex
     *  primitive has to appear for the first time according to the size of the
     *  edge of the primitive.
     *
     *  The function starts by computing the cartesian length of the primitive
     *  edge from the geographical coordinates. Based on the computed edge
     *  length, the function determine the optimal scale at which the primitive
     *  should appear.
     *
     *  \param le_vertex Primitive vertex
     *  \param le_origin Primitive vertex
     *
     *  \return Returns the primitive optimal injection scale
     */

    le_size_t le_geodesy_get_optimal_scale( le_real_t const * const le_vertex, le_real_t const * const le_origin );

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

