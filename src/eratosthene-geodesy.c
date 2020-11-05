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

    # include "eratosthene-geodesy.h"

/*
    source - mutator methods
 */

    le_size_t le_geodesy_get_optimal_scale( le_real_t const * const le_vertex, le_real_t const * const le_origin ) {

        /* position variable */
        le_real_t le_origin_c[3] = { 0.0 };
        le_real_t le_vertex_c[3] = { 0.0 };

        /* distance variable */
        le_real_t le_comp = 0.0;
        le_real_t le_dist = 0.0;

        /* compute radial position */
        le_origin_c[2] = le_origin[2] + LE_ADDRESS_WGS_A;
        le_vertex_c[2] = le_vertex[2] + LE_ADDRESS_WGS_A;

        /* compute coordinates conversion */
        le_origin_c[1] = le_origin_c[2] * sin( le_origin[1] );
        le_origin_c[2] = le_origin_c[2] * cos( le_origin[1] );
        le_origin_c[0] = le_origin_c[2] * sin( le_origin[0] );
        le_origin_c[2] = le_origin_c[2] * cos( le_origin[0] );
        le_vertex_c[1] = le_vertex_c[2] * sin( le_vertex[1] );
        le_vertex_c[2] = le_vertex_c[2] * cos( le_vertex[1] );
        le_vertex_c[0] = le_vertex_c[2] * sin( le_vertex[0] );
        le_vertex_c[2] = le_vertex_c[2] * cos( le_vertex[0] );

        /* compute distance components */
        le_comp  = le_origin_c[0] - le_vertex_c[0];
        le_dist += le_comp * le_comp;
        le_comp  = le_origin_c[1] - le_vertex_c[1];
        le_dist += le_comp * le_comp;
        le_comp  = le_origin_c[2] - le_vertex_c[2];
        le_dist += le_comp * le_comp;

        /* vertex identity management */
        if ( le_dist > 0. ) {

            /* compute and return optimal scale */
            return( ( le_size_t ) ceil( log( ( LE_2P * LE_ADDRESS_WGS_A ) / sqrt( le_dist ) ) / log( 2. ) ) );

        } else {

            /* return maximum depth */
            return( _LE_USE_DEPTH );

        }

    }

