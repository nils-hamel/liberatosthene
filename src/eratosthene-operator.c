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

        /* primitive type */
        le_byte_t le_pri_type = 0;
        le_byte_t le_sec_type = 0;

        /* detection flag - opposition */
        le_byte_t le_flag = 0x00;
        le_byte_t le_galf = 0x00;

        /* comparison index */
        le_size_t le_index = 0;

        /* parsing primary */
        while ( ( le_pri_head - le_pri_base ) < le_pri_size ) {

            /* extract primitive type */
            le_pri_type = * ( le_pri_head + LE_ARRAY_DATA_POSE );

            /* reset detection flag */
            le_flag = 0x00;

            /* reset head */
            le_sec_head = le_sec_base;

            /* parsing secondary */
            while ( ( le_flag == 0x00 ) && ( ( le_sec_head - le_sec_base ) < le_sec_size ) ) {

                /* extract primitive type */
                le_sec_type = * ( le_sec_head + LE_ARRAY_DATA_POSE );

                /* check type compatibility */
                if ( le_pri_type == le_sec_type ) {

                    /* reset detection flag */
                    le_galf = 0xff;

                    /* reset comparison index */
                    le_index = 0;

                    /* parsing primitive vertex */
                    while ( ( le_galf == 0xff ) && ( le_sec_type > 0 ) ) {

                        /* primitive identity detection procedure */
                        if ( ( ( le_real_t * ) ( le_sec_head + le_index ) )[0] == ( ( le_real_t * ) ( le_pri_head + le_index ) )[0] ) {
                        if ( ( ( le_real_t * ) ( le_sec_head + le_index ) )[1] == ( ( le_real_t * ) ( le_pri_head + le_index ) )[1] ) {

                            /* update detection flag if last component is different */
                            if ( ( ( le_real_t * ) ( le_sec_head + le_index ) )[2] != ( ( le_real_t * ) ( le_pri_head + le_index ) )[2] ) le_galf = 0x00;

                        /* update detection flag */
                        } else { le_galf = 0x00; }
                        } else { le_galf = 0x00; }

                        /* update comparison index */
                        le_index += LE_ARRAY_DATA;

                        /* update primitive stack */
                        le_sec_type --;

                    }

                    /* assign detection flag - inverted flags */
                    le_flag = le_galf;

                    /* update head */
                    le_sec_head += le_pri_type * LE_ARRAY_DATA;

                } else {

                    /* update head */
                    le_sec_head += le_sec_type * LE_ARRAY_DATA;

                }

            }

            /* check detection flag */
            if ( le_flag == 0xff ) {

                /* modulate primitive color */
                while ( le_pri_type > 0 ) {

                    /* update head - to data segment */
                    le_pri_head += LE_ARRAY_DATA_POSE;

                    /* modulate color components */
                    * ( ++ le_pri_head ) = 32;
                    * ( ++ le_pri_head ) = 32;
                    * ( ++ le_pri_head ) = 32;

                    /* update head - to next vertex */
                    le_pri_head ++;

                    /* update primitive stack */
                    le_pri_type --;

                }

            } else {

                /* update head */
                le_pri_head += le_pri_type * LE_ARRAY_DATA;

            }

        }

    }

    void le_operator_get_ssd_remove_2( le_array_t * const le_array, le_array_t * const le_compare ) {

        /* array sizes */
        le_size_t le_pri_size = le_array_get_size( le_pri );
        le_size_t le_sec_size = le_array_get_size( le_sec );

        /* array base pointer */
        le_byte_t * le_pri_base = le_array_get_byte( le_pri );
        le_byte_t * le_sec_base = le_array_get_byte( le_sec );

        /* array head pointer */
        le_byte_t * le_pri_head = le_pri_base;
        le_byte_t * le_pri_diff = NULL;
        le_byte_t * le_sec_head = le_sec_base;

        /* primitive type */
        le_byte_t le_pri_type = 0;
        le_byte_t le_sec_type = 0;

        /* detection flag */
        le_byte_t le_flag = 0x00;

        /* parsing primary */
        while ( ( le_pri_head - le_pri_base ) < le_pri_size ) {

            /* extract primitive type */
            le_pri_type = * ( le_pri_head + LE_ARRAY_DATA_POSE );

            /* reset detection flag */
            le_flag = 0x00;

            /* reset secondary head */
            le_sec_head = le_sec_base;

            /* parsing secondary */
            while ( ( le_flag == 0x00 ) && ( ( le_sec_head - le_sec_base ) < le_sec_size ) ) {

                /* extract primitive type */
                le_sec_type = * ( le_sec_head + LE_ARRAY_DATA_POSE );

                /* check type compatibility */
                if ( le_pri_type == le_sec_type ) {

                    /* similarity check */
                    if ( memcmp( le_pri_head, le_sec_head, le_pri_type * LE_ARRAY_DATA ) == 0 ) {

                        /* update detection flag */
                        le_flag = 0xff;

                    }

                }

                /* update head */
                le_sec_head += le_sec_type * LE_ARRAY_DATA;

            }

            /* analyse flag */
            if ( le_flag == 0xff ) {

                /* initialise color pointer */
                le_pri_diff = le_pri_head + LE_ARRAY_DATA_POSE;

                /* extinguish primitive color */
                while ( le_pri_type != 0 ) {

                    /* assign color components */
                    * ( ++ le_pri_diff ) = 32;
                    * ( ++ le_pri_diff ) = 32;
                    * ( ++ le_pri_diff ) = 32;

                    /* update color pointer */
                    le_pri_diff = le_pri_diff + LE_ARRAY_DATA - 3;

                    /* update primitive stack */
                    le_pri_type --;

                }

            }

            /* update head */
            le_pri_head += le_pri_type * LE_ARRAY_DATA;

        }

    }

    void le_operator_get_ssd_remove_1( le_array_t * const le_array, le_array_t * const le_compare ) {

        /* array size */
        le_size_t le_asize = le_array_get_size( le_array );
        le_size_t le_csize = le_array_get_size( le_compare );

        /* primitive type */
        le_size_t le_atype = 0;
        le_size_t le_ctype = 0;

        /* array pointer */
        le_byte_t * le_ahead = NULL;
        le_byte_t * le_chead = NULL;

        /* search index */
        le_size_t le_parse = 0;
        le_size_t le_index = 0;

        /* detection flag */
        le_byte_t le_flag = 0x00;

        /* parsing primary */
        while ( le_parse < le_asize ) {

            /* compute array pointer */
            le_ahead = le_array_get_byte( le_array ) + le_parse;

            /* extract primitive type */
            le_atype = * ( le_ahead + LE_ARRAY_DATA_POSE );

            /* reset detection flag */
            le_flag = 0x00;

            /* reset search index */
            le_index = 0;

            /* parsing secondary */
            while ( ( le_flag == 0x00 ) && ( le_index < le_csize ) ) {

                /* compute array pointer */
                le_chead = le_array_get_byte( le_compare ) + le_index;

                /* extract primitive type */
                le_ctype = * ( le_chead + LE_ARRAY_DATA_POSE );

                /* detect matching type */
                if ( le_atype == le_ctype ) {

                    /* detection test */
                    if ( memcmp( le_ahead, le_chead, LE_ARRAY_DATA_POSE ) == 0 ) {

                        /* update detection flag */
                        le_flag = 0xff;

                    /* update search index */
                    } else { le_index += LE_ARRAY_DATA; }

                /* update search index */
                } else { le_index += ( le_ctype + LE_ARRAY_DATA ); }

            }

            /* detection result analysis */
            if ( le_flag == 0xff ) {

                /* compute array pointer */
                le_ahead += LE_ARRAY_DATA_POSE;

                /* primary color modulation */
                le_ahead[1] = 32;
                le_ahead[2] = 32;
                le_ahead[3] = 32;

            }

            /* update index */
            le_parse += ( le_atype * LE_ARRAY_DATA );

        }

    }

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
                le_head[1] = 32;
                le_head[2] = 32;
                le_head[3] = 32;

            }

        }

    }

