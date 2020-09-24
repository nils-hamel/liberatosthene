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

    //void le_operator_get_ssd( le_array_t * const le_pri, le_array_t * const le_sec ) {
    le_void_t le_operator_get_ssd_( le_byte_t * const le_pri_base, le_size_t const le_pri_size, le_byte_t * const le_sec_base, le_size_t const le_sec_size ) {

        /* array head pointer */
        le_byte_t * le_pri_head = le_pri_base;
        le_byte_t * le_sec_head = le_sec_base;
        le_byte_t * le_sec_push = le_sec_base;

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
            le_sec_head = le_sec_push;

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

                    /* assign detection flag (inverted flags) - push last detection offset */
                    if ( ( le_flag = le_galf ) ) le_sec_push = le_sec_head;

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

                /* update head */
                le_pri_head += le_pri_type;

            }

        }

    }

    void le_operator_get_ssd( le_array_t * const le_pri, le_array_t * const le_sec ) {

        /* array sizes */
        le_size_t le_pri_size = le_array_get_size( le_pri );
        le_size_t le_sec_size = le_array_get_size( le_sec );

        /* array base pointer */
        le_byte_t * le_pri_base = le_array_get_byte( le_pri );
        le_byte_t * le_sec_base = le_array_get_byte( le_sec );

        /* array head pointer */
        le_byte_t * le_pri_head = le_pri_base;
        le_byte_t * le_sec_head = le_sec_base;
        le_byte_t * le_sec_push = le_sec_base;

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
            le_sec_head = le_sec_push;

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

                    /* assign detection flag (inverted flags) - push last detection offset */
                    if ( ( le_flag = le_galf ) ) le_sec_push = le_sec_head;

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

                /* update head */
                le_pri_head += le_pri_type;

            }

        }

    }

