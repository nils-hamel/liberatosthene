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

    # include "eratosthene-operator.h"

/*
    source - accessor methods
 */

    void le_operator_get_marker( le_array_t * const le_array, le_array_t * const le_beacon, le_size_t const le_offset ) {

        /* parsing head */
        le_byte_t * le_head = le_array_get_byte( le_array ) + le_offset;

        /* array edge pointer */
        le_byte_t * le_edge = le_array_get_byte( le_array ) + le_array_get_size( le_array );

        /* primitive type */
        le_byte_t le_type = 0;

        /* parsing array */
        while ( le_head < le_edge ) {

            /* extract primitive type */
            le_type = * ( le_head + LE_ARRAY_DATA_POSE ) * LE_ARRAY_DATA;

            /* update beacon array size */
            le_array_set( le_beacon, LE_ARRAY_DATA );

            /* copy vertex */
            memcpy( le_array_mac_lpose( le_beacon ), le_head, LE_ARRAY_DATA );

            /* update primitive type */
            ( * le_array_mac_ltype( le_beacon ) ) = LE_UV3_LINE;

            /* vertex height modulation */
            le_array_mac_lpose( le_beacon )[2] += 5;
            
            /* update beacon array size */
            le_array_set( le_beacon, LE_ARRAY_DATA );

            /* copy vertex */
            memcpy( le_array_mac_lpose( le_beacon ), le_head, LE_ARRAY_DATA );

            /* update primitive type */
            ( * le_array_mac_ltype( le_beacon ) ) = LE_UV3_LINE;

            /* vertex height modulation */
            le_array_mac_lpose( le_beacon )[2] += 505;            

            /* update head */
            le_head += le_type;

        }

    }

    void le_operator_get_dd( le_array_t * const le_pri, le_array_t * const le_sec, le_array_t * const le_beacon, le_size_t const le_pri_offset, le_size_t const le_sec_offset ) {

        /* array sizes */
        le_size_t le_pri_size = le_array_get_size( le_pri ) - le_pri_offset;
        le_size_t le_sec_size = le_array_get_size( le_sec ) - le_sec_offset;

        /* array base pointer */
        le_byte_t * le_pri_base = le_array_get_byte( le_pri ) + le_pri_offset;
        le_byte_t * le_sec_base = le_array_get_byte( le_sec ) + le_sec_offset;

        /* array head pointer */
        le_byte_t * le_pri_head = le_pri_base;
        le_byte_t * le_sec_head = le_sec_base;

        /* primitive type */
        le_byte_t le_pri_type = 0;
        le_byte_t le_sec_type = 0;

        /* detection flag - opposition */
        le_byte_t le_flag = LE_OPER_OFF;
        le_byte_t le_galf = LE_OPER_OFF;

        /* comparison index */
        le_size_t le_index = 0;

        /* parsing primary */
        while ( ( le_pri_head - le_pri_base ) < le_pri_size ) {

            /* extract primitive type */
            le_pri_type = * ( le_pri_head + LE_ARRAY_DATA_POSE ) * LE_ARRAY_DATA;

            /* reset detection flag */
            le_flag = LE_OPER_OFF;

            /* reset head */
            le_sec_head = le_sec_base;

            /* parsing secondary */
            while ( ( ! le_flag ) && ( ( le_sec_head - le_sec_base ) < le_sec_size ) ) {

                /* extract primitive type */
                le_sec_type = * ( le_sec_head + LE_ARRAY_DATA_POSE ) * LE_ARRAY_DATA;

                /* check type compatibility */
                if ( le_pri_type == le_sec_type ) {

                    /* reset detection flag */
                    le_galf = LE_OPER_SET;

                    /* reset comparison index */
                    le_index = 0;

                    /* parsing primitive vertex */
                    while ( ( le_galf ) && ( le_sec_type ) ) {

                        /* primitive identity detection procedure */
                        if ( ( ( le_real_t * ) ( le_sec_head + le_index ) )[0] == ( ( le_real_t * ) ( le_pri_head + le_index ) )[0] ) {
                        if ( ( ( le_real_t * ) ( le_sec_head + le_index ) )[1] == ( ( le_real_t * ) ( le_pri_head + le_index ) )[1] ) {

                            /* update detection flag if last component is different */
                            if ( ( ( le_real_t * ) ( le_sec_head + le_index ) )[2] != ( ( le_real_t * ) ( le_pri_head + le_index ) )[2] ) le_galf = LE_OPER_OFF;

                        /* update detection flag */
                        } else { le_galf = LE_OPER_OFF; }
                        } else { le_galf = LE_OPER_OFF; }

                        /* update comparison index */
                        le_index += LE_ARRAY_DATA;

                        /* update primitive stack */
                        le_sec_type -= LE_ARRAY_DATA;

                    }

                    /* assign detection flag - flags in opposition */
                    le_flag = le_galf;

                    /* update head */
                    le_sec_head += le_pri_type;

                } else {

                    /* update head */
                    le_sec_head += le_sec_type;

                }

            }

            /* check detection flag */
            if ( le_flag ) {

                /* modulate primitive color */
                while ( le_pri_type ) {

                    /* update head - to data segment */
                    le_pri_head += LE_ARRAY_DATA_POSE;

                    /* modulate color components */
                    * ( ++ le_pri_head ) = 32;
                    * ( ++ le_pri_head ) = 32;
                    * ( ++ le_pri_head ) = 32;

                    /* update head - to next vertex */
                    le_pri_head ++;

                    /* update primitive stack */
                    le_pri_type -= LE_ARRAY_DATA;

                }

            } else {

                /* check beacon array availability */
                if ( le_beacon != NULL ) {

                    /* update beacon array size */
                    le_array_set( le_beacon, LE_ARRAY_DATA );

                    /* copy vertex */
                    memcpy( le_array_mac_lpose( le_beacon ), le_pri_head, LE_ARRAY_DATA );

                    /* update primitive type */
                    ( * le_array_mac_ltype( le_beacon ) ) = LE_UV3_LINE;

                    /* vertex height modulation */
                    le_array_mac_lpose( le_beacon )[2] += 5;
                    
                    /* update beacon array size */
                    le_array_set( le_beacon, LE_ARRAY_DATA );

                    /* copy vertex */
                    memcpy( le_array_mac_lpose( le_beacon ), le_pri_head, LE_ARRAY_DATA );

                    /* update primitive type */
                    ( * le_array_mac_ltype( le_beacon ) ) = LE_UV3_LINE;

                    /* vertex height modulation */
                    le_array_mac_lpose( le_beacon )[2] += 505;

                }

                /* update head */
                le_pri_head += le_pri_type;

            }

        }

    }

