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

    # include "eratosthene-operator.h"

/*
    source - accessor methods
 */

    void le_operator_get_diff_strict( le_array_t * const le_array, le_array_t * const le_compare ) {

        /* array size */
        le_size_t le_asize = le_array_get_size( le_array );

        /* array size */
        le_size_t le_csize = le_array_get_size( le_compare );

        /* array pointer */
        le_byte_t * le_head = NULL;

        /* search index */
        le_size_t le_index = 0;

        /* detection flag */
        le_byte_t le_flag = 0x00;

        /* parsing primary */
        for ( le_size_t le_parse = 0; le_parse < le_asize; le_parse += LE_ARRAY_DATA ) {

            /* compute array pointer */
            le_head = le_array_get_byte( le_array ) + le_parse;

            /* reset detection flag */
            le_flag = 0x00;

            /* reset search index */
            le_index = 0;

            /* parsing secondary */
            while ( ( le_flag == 0x00 ) && ( le_index < le_csize ) ) {

                /* detection test */
                if ( memcmp( le_head, le_array_get_byte( le_compare ) + le_index, LE_ARRAY_DATA_POSE ) == 0 ) {

                    /* update detection flag */
                    le_flag = 0xff;

                /* update parser */
                } else { le_index += LE_ARRAY_DATA; }

            }

            /* detection result analysis */
            if ( le_flag == 0xff ) {

                /* compute array pointer */
                le_head += LE_ARRAY_DATA_POSE;

                /* primary color modulation */
                le_head[1] = 48;
                le_head[2] = 48;
                le_head[3] = 48;

            }

        }

    }

