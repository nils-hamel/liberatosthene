/*
 *  liberatosthene
 *
 *      Nils Hamel - nils.hamel@bluewin.ch
 *      Copyright (c) 2016-2017 DHLAB, EPFL
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

    # include "eratosthene-array.h"

/*
    source - constructor/destructor methods
 */

    le_array_t le_array_create( le_void_t ) {

        /* created structure variables */
        le_array_t le_array = LE_ARRAY_C;

        /* return created structure */
        return( le_array );

    }

    le_void_t le_array_delete( le_array_t * const le_array ) {

        /* deleted structure variables */
        le_array_t le_delete = LE_ARRAY_C;

        /* check array pointer */
        if ( le_array->ar_rbyte != NULL ) {

            /* release array memory */
            free( le_array->ar_rbyte );

        }

        /* delete structure */
        ( * le_array ) = le_delete;

    }

/*
    source - accessor methods
 */

    le_size_t le_array_get_size( le_array_t const * const le_array ) {

        /* return size */
        return( le_array->ar_vsize );

    }

    le_byte_t * le_array_get_byte( le_array_t * const le_array ) {

        /* return bytes pointer */
        return( le_array->ar_vbyte );

    }

/*
    source - mutator methods
 */

    le_enum_t le_array_set( le_array_t * const le_array, le_size_t const le_length ) {

        /* swap pointer variables */
        le_byte_t * le_swap = NULL;

        /* check requierments */
        if ( le_array->ar_rsize >= ( ( le_array->ar_vsize += le_length ) + LE_ARRAY_HEADER ) ) {

            /* send message */
            return( _LE_TRUE );

        }

        /* reallocate array memory */
        if ( ( le_swap = ( le_byte_t * ) realloc( ( le_void_t * ) le_array->ar_rbyte, le_array->ar_rsize += LE_ARRAY_STEP ) ) == NULL ) {

            /* restore array size */
            le_array->ar_rsize -= LE_ARRAY_STEP;

            /* restore array size */
            le_array->ar_vsize -= le_length;

            /* send message */
            return( _LE_FALSE );

        }

        /* update structure references */
        le_array->ar_rbyte = le_swap;

        /* update structure references */
        le_array->ar_vbyte = le_array->ar_rbyte + LE_ARRAY_HEADER;

        /* send message */
        return( _LE_TRUE );

    }

    le_enum_t le_array_set_size( le_array_t * const le_array, le_size_t const le_size ) {

        /* swap pointer variables */
        le_byte_t * le_swap = NULL;

        /* memory size variables */
        le_size_t le_real = le_size + LE_ARRAY_HEADER;

        /* reset entropic size */
        le_array->ar_csize = 0;

        /* check requirements */
        if ( le_array->ar_rsize >= le_real ) {

            /* update array size */
            le_array->ar_vsize = le_size;

            /* send message */
            return( _LE_TRUE );

        }

        /* allocate array memory */
        if ( ( le_swap = ( le_byte_t * ) realloc( ( le_void_t * ) le_array->ar_rbyte, le_real ) ) == NULL ) {

            /* send message */
            return( _LE_FALSE );

        }

        /* update structure references */
        le_array->ar_rbyte = le_swap;

        /* update structure references */
        le_array->ar_vbyte = le_array->ar_rbyte + LE_ARRAY_HEADER;

        /* update array size */
        le_array->ar_rsize = le_real;

        /* update array size */
        le_array->ar_vsize = le_size;

        /* send message */
        return( _LE_TRUE );

    }

/*
    source - serialisation methods
 */

    le_size_t le_array_serial( le_array_t * const le_array, le_void_t * const le_bytes, le_size_t const le_length, le_size_t const le_offset, le_enum_t const le_mode ) {

        /* check serialisation mode */
        if ( le_mode == _LE_GET ) {

            /* serialisation */
            memcpy( le_bytes, le_array->ar_vbyte + le_offset, le_length );

        } else {

            /* serialisation */
            memcpy( le_array->ar_vbyte + le_offset, le_bytes, le_length );

        }

        /* return updated offset */
        return( le_offset + le_length );

    }

/*
    source - i/o methods
 */

    le_byte_t le_array_io_write( le_array_t * const le_array, le_byte_t le_mode, le_sock_t const le_socket ) {

        /* socket-array size variables */
        le_size_t le_size = LE_ARRAY_HEADER + le_array->ar_vsize;

        /* socket i/o variables */
        le_size_t le_head = 0;
        le_size_t le_sent = 0;
        le_size_t le_fail = 0;

        /* serialise sizes */
        ( ( le_size_t * ) le_array->ar_rbyte )[0] = le_array->ar_vsize;
        ( ( le_size_t * ) le_array->ar_rbyte )[1] = le_array->ar_csize;

        /* serialise array mode */
        le_array->ar_rbyte[LE_ARRAY_HEADER_SIZE] = le_mode;

        /* socket-array writing */
        while ( ( le_head < le_size ) && ( le_fail < _LE_USE_RETRY ) ) {

            /* socket-array content */
            if ( ( le_sent = write( le_socket, le_array->ar_rbyte, le_size - le_head ) ) > 0 ) {

                /* update head */
                le_head += le_sent;

            /* update failure */
            } else { le_fail ++; }

        }

        /* check failure */
        if ( le_fail < _LE_USE_RETRY ) {

            /* return array mode */
            return( le_mode );

        /* send message */
        } else { return( LE_MODE_NULL ); }

    }

    le_byte_t le_array_io_read_beta( le_array_t * const le_array, le_sock_t const le_socket ) {

        /* reading size variables */
        le_size_t le_size = LE_ARRAY_HEADER;

        /* reading head variables */
        le_size_t le_head = 0;

        /* socket i/o variables */
        le_size_t le_read = 0;
        le_size_t le_fail = 0;

        le_enum_t le_flag = _LE_FALSE;

        /* socket-array size */
        le_array_set_size( le_array, LE_ARRAY_HEADER );

        /* socket-array reading */
        while ( ( le_head < le_size ) && ( le_fail < _LE_USE_RETRY ) ) {

            /* read socket-array chunk */
            if ( ( le_read = read( le_socket, le_array->ar_rbyte + le_head, le_size - le_head ) ) > 0 ) {

                /* update head */
                le_head += le_read;

            /* update failure */
            } else { le_fail ++; }

            /* dynamic size management */
            if ( ( le_flag == _LE_FALSE ) && ( le_head >= sizeof( le_size_t ) ) ) {

                /* update reading size */
                le_size += * ( ( le_size_t * ) le_array->ar_rbyte );

                /* update array size */
                le_array_set_size( le_array, * ( ( le_size_t * ) le_array->ar_rbyte ) );

                /* update management flag */
                le_flag = _LE_TRUE;

            }

        }

        /* check failure */
        if ( le_fail < _LE_USE_RETRY ) {

            /* return array mode */
            return( le_array->ar_rbyte[LE_ARRAY_HEADER_SIZE] );

        /* send message */
        } else { return( LE_MODE_NULL ); }

    }

    le_byte_t le_array_io_read( le_array_t * const le_array, le_sock_t const le_socket ) {

        /* reading size variables */
        le_size_t le_size = sizeof( le_size_t );

        /* reading head variables */
        le_size_t le_head = 0;

        /* socket i/o variables */
        le_size_t le_read = 0;
        le_size_t le_fail = 0;

        /* socket array reading */
        while ( ( le_head < le_size ) && ( le_fail < _LE_USE_RETRY ) ) {

            /* socket-array size */
            if ( ( le_read = read( le_socket, ( ( le_byte_t * ) & le_array->ar_vsize ) + le_head, le_size - le_head ) ) > 0 ) {

                /* update head */
                le_head += le_read;

            /* update failure */
            } else { le_fail ++; }

        }

        /* check failure */
        if ( le_fail == _LE_USE_RETRY ) {

            /* send message */
            return( LE_MODE_NULL );

        }

        /* socket-array memory */
        if ( le_array_set_size( le_array, le_array->ar_vsize ) == _LE_FALSE ) {

            /* send message */
            return( LE_MODE_NULL );

        }

        /* socket-array size */
        le_size = le_array->ar_vsize + LE_ARRAY_HEADER;

        /* socket-array reading */
        while ( ( le_head < le_size ) && ( le_fail < _LE_USE_RETRY ) ) {

            /* socket-array content */
            if ( ( le_read = read( le_socket, le_array->ar_rbyte + le_head, le_size - le_head ) ) > 0 ) {

                /* update head */
                le_head += le_read;

            /* update failure */
            } else { le_fail ++; }

        }

        /* check failure */
        if ( le_fail == _LE_USE_RETRY ) {

            /* send message */
            return( LE_MODE_NULL );

        }

        /* serialise array size */
        le_array->ar_csize = ( ( le_size_t * ) le_array->ar_rbyte )[1];

        /* serialise array mode */
        return( le_array->ar_rbyte[LE_ARRAY_HEADER_SIZE] );

    }

/*
    source - uf3-specific methods
 */

    le_enum_t le_array_uf3_encode( le_array_t const * const le_src, le_array_t * const le_dst ) {

        /* bitmask variables */
        le_mask_t le_mask = 0x0;

        /* array pointer variables */
        le_byte_t * le_head = NULL;
        le_byte_t * le_line = NULL;

        /* array pointer variables */
        le_byte_t * le_tail = NULL;
        le_byte_t * le_desc = NULL;

        /* check array state */
        if ( le_src->ar_vsize <= ( LE_ARRAY_UF3 * 2 ) ) {

            /* update array size */
            if ( le_array_set_size( le_dst, le_src->ar_vsize ) != _LE_TRUE ) {

                /* send message */
                return( LE_ERROR_MEMORY );

            }

            /* update array content */
            memcpy( le_dst->ar_vbyte, le_src->ar_vbyte, le_src->ar_vsize );

        } else {

            /* update array size */
            if ( le_array_set_size( le_dst, le_array_entropy( le_src ) ) != _LE_TRUE ) {

                /* send message */
                return( LE_ERROR_MEMORY );

            }

            /* update array content */
            memcpy( le_dst->ar_vbyte, le_src->ar_vbyte, LE_ARRAY_UF3 );

            /* initialise array pointers */
            le_line = ( le_head = le_src->ar_vbyte + LE_ARRAY_UF3 );

            /* initialise array pointers */
            le_tail = ( le_desc = le_dst->ar_vbyte + LE_ARRAY_UF3 );

            /* array encoding */
            while ( ( le_head - le_src->ar_vbyte ) < le_src->ar_vsize ) {

                /* flags initialisation */
                ( * le_tail ++ ) = 0;
                ( * le_tail ++ ) = 0;
                ( * le_tail ++ ) = 0;

                /* reset bitmask */
                le_mask = 0x1;

                /* encode uf3 record - pose */
                while ( ( le_head - le_line ) < LE_ARRAY_UF3_POSE ) {

                    /* inequality detection */
                    if ( ( * le_head ) != ( * ( le_head - LE_ARRAY_UF3 ) ) ) {

                        /* encode byte */
                        ( * le_tail ++ ) = ( * le_head );

                        /* update flags */
                        ( * ( le_mask_t * ) le_desc ) |= le_mask;

                    }

                    /* update bitmask and head */
                    le_mask <<= 1, le_head ++;

                }

                /* encode uf3 record - data */
                ( * le_tail ++ ) = ( * le_head ++ );
                ( * le_tail ++ ) = ( * le_head ++ );
                ( * le_tail ++ ) = ( * le_head ++ );

                /* update array pointer */
                le_desc = le_tail;

                /* update array pointer */
                le_line = le_head;

            }

            /* update array size */
            le_dst->ar_vsize = le_tail - le_dst->ar_vbyte;

            /* update array entropic size */
            le_dst->ar_csize = le_src->ar_vsize;

        }

        /* send message */
        return( LE_ERROR_SUCCESS );

    }

    le_enum_t le_array_uf3_decode( le_array_t const * const le_src, le_array_t * const le_dst ) {

        /* bitmask variables */
        le_mask_t le_mask = 0x0;

        /* array pointer variables */
        le_byte_t * le_head = NULL;
        le_byte_t * le_line = NULL;

        /* array pointer variables */
        le_byte_t * le_tail = NULL;
        le_byte_t * le_desc = NULL;

        /* check array state */
        if ( le_src->ar_csize == 0 ) {

            /* update array size */
            if ( le_array_set_size( le_dst, le_src->ar_vsize ) != _LE_TRUE ) {

                /* send message */
                return( LE_ERROR_MEMORY );

            }

            /* update array content */
            memcpy( le_dst->ar_vbyte, le_src->ar_vbyte, le_src->ar_vsize );

        } else {

            /* update array size */
            if ( le_array_set_size( le_dst, le_src->ar_csize ) != _LE_TRUE ) {

                /* send message */
                return( LE_ERROR_MEMORY );

            }

            /* update array content */
            memcpy( le_dst->ar_vbyte, le_src->ar_vbyte, LE_ARRAY_UF3 );

            /* initialise array pointers */
            le_line = ( le_head = le_dst->ar_vbyte + LE_ARRAY_UF3 );

            /* initialise array pointers */
            le_tail = ( le_desc = le_src->ar_vbyte + LE_ARRAY_UF3 );

            /* array decoding */
            while ( ( le_head - le_dst->ar_vbyte ) < le_dst->ar_vsize ) {

                /* reset bitmask */
                le_mask = 0x1, le_tail += 3;

                /* decode uf3 record - pose */
                while ( ( le_head - le_line ) < LE_ARRAY_UF3_POSE ) {

                    /* flags analysis */
                    if ( ( ( * ( le_mask_t * ) le_desc ) & le_mask ) != 0 ) {

                        /* decode byte */
                        ( * le_head ) = ( * le_tail ++ );

                    } else {

                        /* decode byte */
                        ( * le_head ) = ( * ( le_head - LE_ARRAY_UF3 ) );

                    }

                    /* update bitmask and head */
                    le_mask <<= 1, le_head ++;

                }

                /* decode uf3 record - data */
                ( * le_head ++ ) = ( * le_tail ++ );
                ( * le_head ++ ) = ( * le_tail ++ );
                ( * le_head ++ ) = ( * le_tail ++ );

                /* update array pointer */
                le_desc = le_tail;

                /* update array pointer */
                le_line = le_head;

            }

        }

        /* send message */
        return( LE_ERROR_SUCCESS );

    }

