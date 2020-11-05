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

    # include "eratosthene-array.h"

/*
    source - constructor/destructor methods
 */

    le_array_t le_array_create( le_void_t ) {

        /* created structure variable */
        le_array_t le_array = LE_ARRAY_C;

        /* initialise array */
        le_array_set_size( & le_array, 0 );

        /* return created structure */
        return( le_array );

    }

    le_void_t le_array_delete( le_array_t * const le_array ) {

        /* deleted structure variable */
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

    le_byte_t * le_array_get_byte( le_array_t const * const le_array ) {

        /* return bytes pointer */
        return( ( le_byte_t * ) le_array->ar_vbyte );

    }

/*
    source - mutator methods
 */

    le_void_t le_array_set_header( le_array_t * const le_array, le_byte_t const le_mode ) {

        /* serialise size */
        * ( ( le_size_t * ) le_array->ar_rbyte ) = le_array->ar_vsize;

        /* serialise mode */
        * ( le_array->ar_rbyte + LE_ARRAY_HEADER_SIZE ) = le_mode;

    }

    le_byte_t le_array_set_array( le_array_t * const le_array ) {

        /* restore array memory allocation */
        if ( le_array_set_size( le_array, * ( ( le_size_t * ) le_array->ar_rbyte ) ) == _LE_FALSE ) {

            /* send message */
            return( LE_MODE_NULL );

        }

        /* return mode */
        return( * ( le_array->ar_rbyte + LE_ARRAY_HEADER_SIZE ) );

    }

    le_enum_t le_array_set( le_array_t * const le_array, le_size_t le_length ) {

        /* swap pointer variables */
        le_byte_t * le_swap = NULL;

        /* target size */
        le_size_t le_real = ( le_array->ar_vsize + le_length ) + LE_ARRAY_HEADER;

        /* update array size */
        le_array->ar_vsize += le_length;

        /* check requirements */
        if ( le_array->ar_rsize >= le_real ) {

            /* send message */
            return( _LE_TRUE );

        }

        /* compute ideal size */
        le_real = le_get_ideal( le_real, 1 << 20 );

        /* reallocate array memory */
        if ( ( le_swap = ( le_byte_t * ) realloc( ( le_void_t * ) le_array->ar_rbyte, le_real ) ) == NULL ) {

            /* critical error tracking */
            # ifdef _LE_CRITICAL
            fprintf( stderr, "E, C, %s, %d, %li\n", __FILE__, __LINE__, pthread_self() );
            # endif

            /* restore array size */
            le_array->ar_vsize -= le_length;

            /* send message */
            return( _LE_FALSE );

        }

        /* update structure references */
        le_array->ar_rbyte = le_swap;

        /* update structure references */
        le_array->ar_vbyte = le_array->ar_rbyte + LE_ARRAY_HEADER;

        /* update array size */
        le_array->ar_rsize = le_real;

        /* send message */
        return( _LE_TRUE );

    }

    le_enum_t le_array_set_size( le_array_t * const le_array, le_size_t const le_size ) {

        /* swap pointer variables */
        le_byte_t * le_swap = NULL;

        /* memory size variables */
        le_size_t le_real = le_size + LE_ARRAY_HEADER;

        /* check requirements */
        if ( le_array->ar_rsize >= le_real ) {

            /* update array size */
            le_array->ar_vsize = le_size;

            /* send message */
            return( _LE_TRUE );

        }

        /* allocate array memory */
        if ( ( le_swap = ( le_byte_t * ) realloc( ( le_void_t * ) le_array->ar_rbyte, le_real ) ) == NULL ) {

            /* critical error tracking */
            # ifdef _LE_CRITICAL
            fprintf( stderr, "E, C, %s, %d, %li\n", __FILE__, __LINE__, pthread_self() );
            # endif

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

    le_void_t le_array_set_append( le_array_t * const le_array, le_array_t const * const le_append ) {

        /* array size */
        le_size_t le_size = 0;

        /* check array size */
        if ( le_append->ar_vsize > 0 ) {

            /* retrieve array size */
            le_size = le_array_get_size( le_array );

            /* update array size */
            le_array_set( le_array, le_append->ar_vsize );

            /* append array */
            memcpy( le_array->ar_vbyte + le_size, le_append->ar_vbyte, le_append->ar_vsize );

        }

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

        /* array size variables */
        le_size_t le_size = LE_ARRAY_HEADER + le_array->ar_vsize;

        /* socket i/o variables */
        le_size_t le_head = 0;
        le_size_t le_sent = 0;

        /* create array header */
        le_array_set_header( le_array, le_mode );

        /* array writing */
        while ( le_head < le_size ) {

            /* send array */
            if ( ( le_sent = write( le_socket, le_array->ar_rbyte + le_head, le_size - le_head ) ) > 0 ) {

                /* update head */
                le_head += le_sent;

            } else {

                /* send message */
                return( LE_MODE_NULL );

            }

        }

        /* return array mode */
        return( le_mode );

    }

    le_byte_t le_array_io_read( le_array_t * const le_array, le_sock_t const le_socket ) {

        /* array size variable */
        le_size_t le_size = LE_ARRAY_HEADER;

        /* socket i/o variable */
        le_size_t le_head = 0;
        le_size_t le_read = 0;

        /* array mode variable */
        le_byte_t le_mode = LE_MODE_NULL;

        /* array size */
        le_array_set_size( le_array, 0 );

        /* array reading */
        while ( le_head < le_size ) {

            /* read array */
            if ( ( le_read = read( le_socket, le_array->ar_rbyte + le_head, le_size - le_head ) ) > 0 ) {

                /* update head */
                le_head += le_read;

                /* socket array size */
                if ( ( le_mode == LE_MODE_NULL ) && ( le_head >= LE_ARRAY_HEADER ) ) {

                    /* rebuild array */
                    if ( ( le_mode = le_array_set_array( le_array ) ) != LE_MODE_NULL ) {

                        /* update read size */
                        le_size += le_array->ar_vsize;

                    /* send message */
                    } else { return( LE_MODE_NULL ); }

                }

            } else {

                /* error detection */
                if ( le_read <= 0 ) {

                    /* error management */
                    if ( ( errno != EFAULT ) && ( errno != EINTR ) ) {

                        /* send message */
                        return( LE_MODE_NULL );

                    }

                }

            }

        }

        /* return array mode */
        return( le_mode );

    }

