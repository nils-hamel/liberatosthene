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

    # include "eratosthene-address.h"

/*
    source - accessor methods
 */

    le_byte_t le_address_get_size( le_address_t const * const le_address ) {

        /* return size */
        return( le_address->as_size );

    }

    le_byte_t le_address_get_mode( le_address_t const * const le_address ) {

        /* return convolution mode */
        return( le_address->as_mode & 0x0f );

    }

    le_byte_t le_address_get_query( le_address_t const * const le_address ) {

        /* return query mode */
        return( le_address->as_mode >> 4 );

    }

    le_time_t le_address_get_time( le_address_t const * const le_address, le_size_t const le_offset ) {

        /* return specified time */
        return( le_address->as_times[le_offset] );

    }

    le_byte_t le_address_get_digit( le_address_t const * const le_address, le_size_t const le_offset ) {

        /* return specified digit */
        return( le_address->as_digit[le_offset] );

    }

    le_byte_t le_address_get_span( le_address_t const * const le_address ) {

        /* return span */
        return( le_address->as_span );

    }

    le_enum_t le_address_get_equal( le_address_t const * const le_addra, le_address_t const * const le_addrb ) {

        /* size comparison */
        if ( le_addra->as_size != le_addrb->as_size ) {

            /* send message */
            return( _LE_FALSE );

        }

        /* mode comparison */
        if ( le_addra->as_mode != le_addrb->as_mode ) {

            /* send message */
            return( _LE_FALSE );

        }

        /* span comparison */
        if ( le_addra->as_span != le_addrb->as_span ) {

            /* send message */
            return( _LE_FALSE );

        }

        /* times array comparison */
        if ( memcmp( le_addra->as_times, le_addrb->as_times, sizeof( le_time_t ) * _LE_USE_TIMES ) != 0 ) {

            /* send message */
            return( _LE_FALSE );

        }

        /* digit array comparison */
        if ( memcmp( le_addra->as_digit, le_addrb->as_digit, le_addra->as_size ) != 0 ) {

            /* send message */
            return( _LE_FALSE );

        }

        /* send message */
        return( _LE_TRUE );

    }

    le_enum_t le_address_get_equal_index( le_address_t const * const le_addra, le_address_t const * const le_addrb ) {

        /* size comparison */
        if ( le_addra->as_size != le_addrb->as_size ) {

            /* send message */
            return( _LE_FALSE );

        }

        /* digit array comparison */
        if ( memcmp( le_addra->as_digit, le_addrb->as_digit, le_addra->as_size ) != 0 ) {

            /* send message */
            return( _LE_FALSE );

        }

        /* send message */
        return( _LE_FALSE );

    }

    le_enum_t le_address_get_greater( le_real_t const * const le_fpose, le_real_t const * const le_spose, le_size_t const le_size ) {

        /* vector variable */
        le_real_t le_fdual[3] = {

        /* coordinates normalisation on [0,1[ range */
        ( le_fpose[0] - LE_ADDRESS_MIN_L ) * LE_ADDRESS_IRN_L,
        ( le_fpose[1] - LE_ADDRESS_MIN_A ) * LE_ADDRESS_IRN_A,
        ( le_fpose[2] - LE_ADDRESS_MIN_H ) * LE_ADDRESS_IRN_H

        };

        /* vector variable */
        le_real_t le_sdual[3] = {

        /* coordinates normalisation on [0,1[ range */
        ( le_spose[0] - LE_ADDRESS_MIN_L ) * LE_ADDRESS_IRN_L,
        ( le_spose[1] - LE_ADDRESS_MIN_A ) * LE_ADDRESS_IRN_A,
        ( le_spose[2] - LE_ADDRESS_MIN_H ) * LE_ADDRESS_IRN_H

        };

        /* digit variable */
        le_byte_t le_fdigit = 0;

        /* digit variable */
        le_byte_t le_sdigit = 0;

        /* composing address */
        for ( le_size_t le_parse = 0 ; le_parse < le_size; le_parse ++ ) {

            /* update and check dimension value */
            if ( ( le_fdual[0] *= 2.0 ) >= 1.0 ) {

                /* update dimension value */
                le_fdual[0] -= 1.0;

                /* assign digit component */
                le_fdigit = 0x01;

            /* initialise digit component */
            } else { le_fdigit = 0x00; }

            /* update and check dimension value */
            if ( ( le_sdual[0] *= 2.0 ) >= 1.0 ) {

                /* update dimension value */
                le_sdual[0] -= 1.0;

                /* assign digit component */
                le_sdigit = 0x01;

            /* initialise digit component */
            } else { le_sdigit = 0x00; }

            /* asynchronous dimension management */
            if ( le_parse >= LE_ADDRESS_DEPTH_P ) {

                /* update and check dimension value */
                if ( ( le_fdual[1] *= 2.0 ) >= 1.0 ) {

                    /* update dimension value */
                    le_fdual[1] -= 1.0;

                    /* assign digit component */
                    le_fdigit |= 0x02;

                }

                /* update and check dimension value */
                if ( ( le_sdual[1] *= 2.0 ) >= 1.0 ) {

                    /* update dimension value */
                    le_sdual[1] -= 1.0;

                    /* assign digit component */
                    le_sdigit |= 0x02;

                }

                /* asynchronous dimension management */
                if ( le_parse >= LE_ADDRESS_DEPTH_A ) {

                    /* update and check dimension value */
                    if ( ( le_fdual[2] *= 2.0 ) >= 1.0 ) {

                        /* update dimension value */
                        le_fdual[2] -= 1.0;

                        /* assign digit component */
                        le_fdigit |= 0x04;

                    }

                    /* update and check dimension value */
                    if ( ( le_sdual[2] *= 2.0 ) >= 1.0 ) {

                        /* update dimension value */
                        le_sdual[2] -= 1.0;

                        /* assign digit component */
                        le_sdigit |= 0x04;

                    }

                }

            }

            /* digit comparison */
            if ( le_fdigit > le_sdigit ) {

                /* send message */
                return( _LE_TRUE );

            } else if ( le_fdigit < le_sdigit ) {

                /* send message */
                return( _LE_FALSE );

            }

        }

        /* send message */
        return( _LE_FALSE );

    }

    le_size_t le_address_get_dist( le_address_t const * const le_address, le_address_t const * const le_origin, le_size_t const le_size ) {

        /* parsing variable */
        le_size_t le_parse = 0;

        /* order variable */
        le_size_t le_sign = 0;

        /* arithmetic variable */
        le_size_t le_remain = 0;

        /* arithmetic variable */
        le_size_t le_digit = 0;

        /* returned value variable */
        le_size_t le_return = 0;

        /* parsing digit */
        while ( ( le_parse < le_size ) && ( le_sign == 0 ) ) {

            /* digit comparison */
            if ( le_address->as_digit[le_parse] != le_origin->as_digit[le_parse] ) {

                /* digit comparison */
                if ( le_address->as_digit[le_parse] > le_origin->as_digit[le_parse] ) {

                    /* update sign */
                    le_sign = +1;

                } else {

                    /* update sign */
                    le_sign = -1;

                }

            /* update parser */
            } else { le_parse ++; }

        }

        /* case study */
        if ( le_sign == 0 ) {

            /* return distance */
            return( le_size );

        }

        /* reset parser */
        le_parse = le_size;

        /* parsing digit */
        while ( ( -- le_parse ) >= 0 ) {

            /* compute digit */
            le_digit = ( le_address->as_digit[le_parse] - le_origin->as_digit[le_parse] ) * le_sign + le_remain;

            /* remainder broadcasting */
            le_remain = ( le_digit < 0 ) ? -1 : 0;

            /* check digit */
            if ( le_digit != 0 ) {

                /* update distance */
                le_return = le_parse;

            }

        }

        /* return distance */
        return( le_return );

    }

    le_void_t le_address_get_pose_( le_address_t const * const le_address, le_size_t le_size, le_real_t * const le_pose ) {

        /* scales variable */
        le_real_t le_scale[3] = { 1.0, 1.0, 1.0 };

        /* initialise spatial coordinates */
        le_pose[0] = 0.0;
        le_pose[1] = 0.0;
        le_pose[2] = 0.0;

        /* decomposing address */
        for ( le_size_t le_parse = 0; le_parse < le_size; le_parse ++ ) {

            /* update dimension scale */
            le_scale[0] *= 0.5;

            /* analyse address digit */
            if ( ( le_address->as_digit[le_parse] & 0x01 ) != 0 ) {

                /* update coordinate */
                le_pose[0] += le_scale[0];

            }

            /* asynchronous dimension */
            if ( le_parse >= LE_ADDRESS_DEPTH_P ) {

                /* update dimension scale */
                le_scale[1] *= 0.5;

                /* analyse address digit */
                if ( ( le_address->as_digit[le_parse] & 0x02 ) != 0 ) {

                    /* update coordinate */
                    le_pose[1] += le_scale[1];

                }

                /* asynchronous dimension */
                if ( le_parse >= LE_ADDRESS_DEPTH_A ) {

                    /* update dimension scale */
                    le_scale[2] *= 0.5;

                    /* analyse address digit */
                    if ( ( le_address->as_digit[le_parse] & 0x04 ) != 0 ) {

                        /* update coordinate */
                        le_pose[2] += le_scale[2];

                    }

                }

            }

        }

        /* coordinates de-normalisation */
        le_pose[0] = LE_ADDRESS_MIN_L + ( le_pose[0] * LE_ADDRESS_RAN_L );
        le_pose[1] = LE_ADDRESS_MIN_A + ( le_pose[1] * LE_ADDRESS_RAN_A );
        le_pose[2] = LE_ADDRESS_MIN_H + ( le_pose[2] * LE_ADDRESS_RAN_H );

    }

/*
    source - mutator methods
 */

    le_void_t le_address_set_size( le_address_t * const le_address, le_byte_t const le_size ) {

        /* assign size */
        le_address->as_size = le_size;

    }

    le_void_t le_address_set_mode( le_address_t * const le_address, le_byte_t const le_mode ) {

        /* assign convolution mode */
        le_address->as_mode = ( le_address->as_mode & 0xf0 ) | ( le_mode & 0x0f );

    }

    le_void_t le_address_set_query( le_address_t * const le_address, le_byte_t const le_query ) {

        /* assign query mode */
        le_address->as_mode = ( le_address->as_mode & 0x0f ) | ( le_query << 4 );

    }

    le_void_t le_address_set_time( le_address_t * const le_address, le_size_t const le_offset, le_time_t const le_time ) {

        /* assign specified time */
        le_address->as_times[le_offset] = le_time;

    }

    le_void_t le_address_set_digit( le_address_t * const le_address, le_size_t const le_offset, le_byte_t const le_digit ) {

        /* assign specified digit */
        le_address->as_digit[le_offset] = le_digit;

    }

    le_void_t le_address_set_span( le_address_t * const le_address, le_byte_t const le_span ) {

        /* assign span */
        le_address->as_span = le_span;

    }

    le_void_t le_address_set_pose( le_address_t * const le_address, le_real_t const * const le_pose ) {

        /* vector variable */
        le_real_t le_vector[3] = {

        /* coordinates normalisation on [0,1[ range */
        ( le_pose[0] - LE_ADDRESS_MIN_L ) * LE_ADDRESS_IRN_L,
        ( le_pose[1] - LE_ADDRESS_MIN_A ) * LE_ADDRESS_IRN_A,
        ( le_pose[2] - LE_ADDRESS_MIN_H ) * LE_ADDRESS_IRN_H

        };

        /* composing address */
        for ( le_size_t le_parse = 0 ; le_parse < le_address->as_size; le_parse ++ ) {

            /* update and check dimension value */
            if ( ( le_vector[0] *= 2.0 ) >= 1.0 ) {

                /* update dimension value */
                le_vector[0] -= 1.0;

                /* assign address digit component */
                le_address->as_digit[le_parse] = 0x01;

            } else {

                /* assign address digit component */
                le_address->as_digit[le_parse] = 0x00;

            }

            /* asynchronous dimension management */
            if ( le_parse >= LE_ADDRESS_DEPTH_P ) {

                /* update and check dimension value */
                if ( ( le_vector[1] *= 2.0) >= 1.0 ) {

                    /* update dimension value */
                    le_vector[1] -= 1.0;

                    /* assign address digit component */
                    le_address->as_digit[le_parse] |= 0x02;

                }

                /* asynchronous dimension management */
                if ( le_parse >= LE_ADDRESS_DEPTH_A ) {

                    /* update and check dimension value */
                    if ( ( le_vector[2] *= 2.0 ) >= 1.0 ) {

                        /* update dimension value */
                        le_vector[2] -= 1.0;

                        /* assign address digit component */
                        le_address->as_digit[le_parse] |= 0x04;

                    }

                }

            }

        }

    }

/*
    source - serialisation methods
 */

    le_size_t le_address_serial( le_address_t * const le_address, le_array_t * const le_array, le_size_t const le_offset, le_enum_t const le_mode ) {

        /* serialisation variables */
        le_byte_t * le_base = le_array_get_byte( le_array ) + le_offset;

        /* serialisation variables */
        le_time_t * le_time = ( le_time_t * ) le_base;

        /* check serialisation mode */
        if ( le_mode == _LE_GET ) {

            /* serialise times */
            le_address->as_times[0] = le_time[0];
            le_address->as_times[1] = le_time[1];
            le_address->as_times[2] = le_time[2];

            /* update serial base */
            le_base += LE_ARRAY_ADDR_TIME;

            /* serialise address configuration */
            le_address->as_size = le_base[0];
            le_address->as_mode = le_base[1];
            le_address->as_span = le_base[2];

            /* update serial base */
            le_base += LE_ARRAY_ADDR_DESC;

            /* address digit micro-compression */
            for ( le_size_t le_msb = ( _LE_USE_DEPTH >> 1 ), le_lsb = 0; le_msb < _LE_USE_DEPTH; le_msb ++, le_lsb ++, le_base ++ ) {

                /* inflate address digit */
                le_address->as_digit[le_msb] = ( * le_base ) >> 4;
                le_address->as_digit[le_lsb] = ( * le_base ) & 0x0f;

            }

        } else {

            /* serialise times */
            le_time[0] = le_address->as_times[0];
            le_time[1] = le_address->as_times[1];
            le_time[2] = le_address->as_times[2];

            /* update serial base */
            le_base += LE_ARRAY_ADDR_TIME;

            /* serialise address configuration */
            le_base[0] = le_address->as_size;
            le_base[1] = le_address->as_mode;
            le_base[2] = le_address->as_span;

            /* update serial base */
            le_base += LE_ARRAY_ADDR_DESC;

            /* address digit micro-compression */
            for ( le_size_t le_msb = ( _LE_USE_DEPTH >> 1 ), le_lsb = 0; le_msb < _LE_USE_DEPTH; le_msb ++, le_lsb ++, le_base ++ ) {

                /* deflate address digits */
                ( * le_base ) = ( le_address->as_digit[le_msb] << 4 ) | ( le_address->as_digit[le_lsb] );

            }

        }

        /* return updated offset */
        return( le_offset + LE_ARRAY_ADDR );

    }

/*
    source - conversion methods
 */

    le_void_t le_address_ct_string( le_address_t const * const le_address, le_char_t * const le_string ) {

        /* conversion buffer variables */
        le_char_t le_data[_LE_USE_DEPTH + 1] = { 0 };

        /* convert address */
        for ( le_size_t le_parse = 0; le_parse < le_address->as_size; le_parse ++ ) {

            /* digit/character conversion */
            le_data[le_parse] = le_address->as_digit[le_parse] + 48;

        }

        /* compose address string */
        sprintf( ( char * ) le_string, "/%" _LE_BYTE_P "/%" _LE_TIME_P ",%" _LE_TIME_P "/%" _LE_TIME_P "/%s/%" _LE_BYTE_P, le_address->as_mode, le_address->as_times[0], le_address->as_times[1], le_address->as_times[2], le_data, le_address->as_span );

    }

    le_void_t le_address_cf_string( le_address_t * const le_address, le_char_t const * const le_string ) {

        /* conversion buffer variables */
        le_char_t le_data[_LE_USE_DEPTH + 1] = { 0 };

        /* decompose address string */
        sscanf( ( char * ) le_string, "/%" _LE_BYTE_S "/%" _LE_TIME_S ",%" _LE_TIME_S "/%" _LE_TIME_S "/%[^/]/%" _LE_BYTE_S, & le_address->as_mode, le_address->as_times, le_address->as_times + 1, le_address->as_times + 2, le_data, & le_address->as_span );

        /* compute address size */
        le_address->as_size = strlen( ( char * ) le_data );

        /* convert address */
        for ( le_size_t le_parse = 0; le_parse < le_address->as_size; le_parse ++ ) {

            /* digit/character conversion */
            le_address->as_digit[le_parse] = le_data[le_parse] - 48;

        }

    }

