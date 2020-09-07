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

    void le_operator_get_diff( le_array_t * const le_array, le_array_t * const le_compare ) {

        /* array size */
        le_size_t le_asize = le_array_get_size( le_array );

        /* array size */
        le_size_t le_csize = le_array_get_size( le_compare );

        /* array pointer */
        le_real_t * le_apose = NULL;

        /* array pointer */
        le_real_t * le_cpose = NULL;

        /* array pointer */
        le_byte_t * le_adata = NULL;

        /* detection flag */
        le_byte_t le_flag = 0x00;

        /* parsing principal */
        for ( le_size_t le_parse = 0; le_parse < le_asize; le_parse += LE_ARRAY_DATA ) {

            /* assign array pointer */
            le_apose = ( le_real_t * ) ( le_array_get_byte( le_array ) + le_parse );

            /* reset flag */
            le_flag = 0x00;

            /* parsing secondary */
            for ( le_size_t le_index = 0; le_index < le_csize; le_index += LE_ARRAY_DATA ) {

                /* assign array pointer */
                le_cpose = ( le_real_t * ) ( le_array_get_byte( le_compare ) + le_index );

                /* experimental fast strict differences detection */
                if ( memcmp( le_apose, le_cpose, LE_ARRAY_DATA_POSE ) == 0 ) {

                    /* update flag */
                    le_flag = 0xff;

                }

            }

            /* check detection flag */
            if ( le_flag == 0xff ) {

                /* assign array pointer */
                le_adata = ( ( le_byte_t * ) le_apose ) + LE_ARRAY_DATA_POSE;

                /* modulate color */
                le_adata[1] = 32;
                le_adata[2] = 32;
                le_adata[3] = 32;

            }

        }

    }

