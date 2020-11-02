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

    # include "eratosthene-class-poly.h"

/*
    source - constructor/destructor methods
 */

    le_poly_t le_poly_create( le_void_t ) {

        /* created structure variable */
        le_poly_t le_poly = LE_POLY_C;

        /* initialise class descriptor */
        le_class_create( le_poly.pc_data + LE_POLY_HEADER );

        /* return created structure */
        return( le_poly );

    }

    le_void_t le_poly_reset( le_poly_t * const le_poly ) {

        /* reset class descriptor */
        le_class_create( le_poly->pc_data + LE_POLY_HEADER );

        /* reset class size */
        le_poly_set_size( le_poly, 0 );

    }

    le_void_t le_poly_delete( le_poly_t * const le_poly ) {

        /* deleted structure variable */
        le_poly_t le_delete = LE_POLY_C;

        /* check memory */
        if ( le_poly->pc_size > 0 ) {

            /* release memory */
            free( le_poly->pc_link );

        }

        /* delete structure */
        ( * le_poly ) = le_delete;

    }

/*
    source - accessor methods
 */

    le_size_t le_poly_get_size( le_poly_t const * const le_poly ) {

        /* return class links array size */
        return( le_class_mac_cast( le_poly->pc_data ) & _LE_OFFS_NULL );

    }

    le_size_t le_poly_get_link( le_poly_t const * const le_poly, le_size_t const le_index ) {

        /* return link value */
        return( ( * ( ( le_size_t * ) ( le_poly->pc_link + ( le_index * _LE_USE_OFFSET ) ) ) ) & _LE_OFFS_NULL );

    }

    le_size_t le_poly_get_offset( le_poly_t const * const le_poly, le_size_t const le_index ) {

        /* extract and return offset */
        return( le_class_get_offset( le_poly->pc_data + LE_POLY_HEADER, le_index ) );

    }

/*
    source - mutator methods
 */

    le_void_t le_poly_set_size( le_poly_t * const le_poly, le_size_t const le_size ) {

        /* reset and assign links array size */
        le_class_mac_cast( le_poly->pc_data ) = ( le_class_mac_cast( le_poly->pc_data ) & ( ~ _LE_OFFS_NULL ) ) | ( le_size & _LE_OFFS_NULL );

    }

    le_void_t le_poly_set_offset( le_poly_t * const le_poly, le_size_t const le_index, le_size_t const le_offset ) {

        /* assign offset */
        le_class_set_offset( le_poly->pc_data + LE_POLY_HEADER, le_index, le_offset );

    }

    le_enum_t le_poly_set_push( le_poly_t * const le_poly, le_size_t const le_link ) {

        /* size variable */
        le_size_t le_size = le_poly_get_size( le_poly );

        /* offset variable */
        le_size_t * le_offset = NULL;

        /* check memory allocation */
        if ( le_size == le_poly->pc_size ) {

            /* update memory */
            if ( le_poly_set_memory( le_poly, le_poly->pc_size + LE_POLY_STEP ) != LE_ERROR_SUCCESS ) {

                /* send message */
                return( LE_ERROR_MEMORY );

            }

        }

        /* update size */
        le_poly_set_size( le_poly, le_size + 1 );

        /* compute link array offset */
        le_offset = ( le_size_t * ) ( le_poly->pc_link + ( le_size * _LE_USE_OFFSET ) );

        /* assign link value in array */
        ( * le_offset ) = ( ( * le_offset ) & ( ~ _LE_OFFS_NULL ) ) | ( le_link & _LE_OFFS_NULL );

        /* send message */
        return( LE_ERROR_SUCCESS );

    }

    le_enum_t le_poly_set_memory( le_poly_t * const le_poly, le_size_t const le_size ) {

        /* swap variable */
        le_void_t * le_swap = NULL;

        /* re-allocate memory */
        if ( ( le_swap = realloc( le_poly->pc_link, ( le_size * _LE_USE_OFFSET ) + LE_CLASS_PADDING ) ) == NULL ) {

            /* critical error tracking */
            # ifdef _LE_FATAL
            fprintf( stderr, "E, C, %s, %d, %li\n", __FILE__, __LINE__, pthread_self() );
            # endif

            /* send message */
            return( LE_ERROR_MEMORY );

        }

        /* update size */
        le_poly->pc_size = le_size;

        /* validate memory allocation */
        le_poly->pc_link = le_swap;

        /* send message */
        return( LE_ERROR_SUCCESS );

    }

    le_void_t le_poly_set_release( le_poly_t * const le_poly ) {

        /* check memory allocation */
        if ( le_poly->pc_link != NULL ) {

            /* release memory */
            free( le_poly->pc_link );

        }

    }

/*
    source - i/o methods
 */

    le_enum_t le_poly_io_read( le_poly_t * const le_poly, le_size_t const le_offset, le_file_t const le_stream ) {

        /* size variable */
        le_size_t le_size = 0;

        /* stream offset */
        if ( fseek( le_stream, le_offset, SEEK_SET ) != 0 ) {

            /* critical error tracking */
            # ifdef _LE_FATAL
            fprintf( stderr, "E, C, %s, %d, %li\n", __FILE__, __LINE__, pthread_self() );
            # endif

            /* send message */
            return( LE_ERROR_IO_SEEK );

        } else {

            /* read class extended header */
            if ( fread( le_poly->pc_data, sizeof( le_byte_t ), LE_POLY_EXTEND, le_stream ) != LE_POLY_EXTEND ) {

                /* critical error tracking */
                # ifdef _LE_FATAL
                fprintf( stderr, "E, C, %s, %d, %li\n", __FILE__, __LINE__, pthread_self() );
                # endif

                /* send message */
                return( LE_ERROR_IO_READ );

            } else {

                /* read class */
                if ( le_class_io_read( le_poly->pc_data + LE_POLY_HEADER, le_stream ) != LE_ERROR_SUCCESS ) {

                    /* send message */
                    return( LE_ERROR_IO_READ );

                } else {

                    /* extract size */
                    le_size = le_poly_get_size( le_poly );

                    /* memory management */
                    if ( le_poly_set_memory( le_poly, le_size ) != LE_ERROR_SUCCESS ) {

                        /* send message */
                        return( LE_ERROR_MEMORY );

                    } else {

                        /* read class links */
                        if ( fread( le_poly->pc_link, _LE_USE_OFFSET, le_size, le_stream ) != le_size ) {

                            /* critical error tracking */
                            # ifdef _LE_FATAL
                            fprintf( stderr, "E, C, %s, %d, %li\n", __FILE__, __LINE__, pthread_self() );
                            # endif

                            /* send message */
                            return( LE_ERROR_IO_READ );

                        } else {

                            /* send message */
                            return( LE_ERROR_SUCCESS );

                        }

                    }

                }

            }

        }

    }

    le_enum_t le_poly_io_read_fast( le_poly_t * const le_poly, le_size_t const le_offset, le_file_t const le_stream ) {

        /* stream offset */
        if ( fseek( le_stream, le_offset, SEEK_SET ) != 0 ) {

            /* critical error tracking */
            # ifdef _LE_FATAL
            fprintf( stderr, "E, C, %s, %d, %li\n", __FILE__, __LINE__, pthread_self() );
            # endif

            /* send message */
            return( LE_ERROR_IO_SEEK );

        } else {

            /* read class extended header */
            if ( fread( le_poly->pc_data, sizeof( le_byte_t ), LE_POLY_EXTEND, le_stream ) != LE_POLY_EXTEND ) {

                /* send message */
                return( LE_ERROR_IO_READ );

            } else {

                /* read class and broadcast message */
                return( le_class_io_read( le_poly->pc_data + LE_POLY_HEADER, le_stream ) );

            }

        }

    }

    le_enum_t le_poly_io_read_next( le_poly_t * const le_poly, le_file_t const le_stream ) {

        /* size variable */
        le_size_t le_size = le_poly_get_size( le_poly );

        /* memory management */
        if ( le_poly_set_memory( le_poly, le_size ) != LE_ERROR_SUCCESS ) {

            /* send message */
            return( LE_ERROR_MEMORY );

        } else {

            /* read class links */
            if ( fread( le_poly->pc_link, _LE_USE_OFFSET, le_size, le_stream ) != le_size ) {

                /* critical error tracking */
                # ifdef _LE_FATAL
                fprintf( stderr, "E, C, %s, %d, %li\n", __FILE__, __LINE__, pthread_self() );
                # endif

                /* send message */
                return( LE_ERROR_IO_READ );

            } else {

                /* send message */
                return( LE_ERROR_SUCCESS );

            }

        }

    }

    le_enum_t le_poly_io_write( le_poly_t const * const le_poly, le_size_t const le_offset, le_file_t const le_stream ) {

        /* size variable */
        le_size_t le_size = le_poly_get_size( le_poly );

        /* check offset */
        if ( le_offset != _LE_OFFS_NULL ) {

            /* stream offset */
            if ( fseek( le_stream, le_offset, SEEK_SET ) != 0 ) {

                /* critical error tracking */
                # ifdef _LE_FATAL
                fprintf( stderr, "E, C, %s, %d, %li\n", __FILE__, __LINE__, pthread_self() );
                # endif

                /* send message */
                return( LE_ERROR_IO_SEEK );

            }

        }

        /* write class extended header */
        if ( fwrite( le_poly->pc_data, sizeof( le_byte_t ), LE_POLY_EXTEND, le_stream ) != LE_POLY_EXTEND ) {

            /* critical error tracking */
            # ifdef _LE_FATAL
            fprintf( stderr, "E, C, %s, %d, %li\n", __FILE__, __LINE__, pthread_self() );
            # endif

            /* send messsage */
            return( LE_ERROR_IO_WRITE );

        } else {

            /* write class */
            if ( le_class_io_write( le_poly->pc_data + LE_POLY_HEADER, le_stream ) != LE_ERROR_SUCCESS ) {

                /* send message */
                return( LE_ERROR_IO_WRITE );

            } else {

                /* write class links */
                if ( fwrite( le_poly->pc_link, _LE_USE_OFFSET, le_size, le_stream ) != le_size ) {

                    /* critical error tracking */
                    # ifdef _LE_FATAL
                    fprintf( stderr, "E, C, %s, %d, %li\n", __FILE__, __LINE__, pthread_self() );
                    # endif

                    /* send message */
                    return( LE_ERROR_IO_WRITE );

                } else {

                    /* send message */
                    return( LE_ERROR_SUCCESS );

                }

            }

        }

    }

    le_size_t le_poly_io_offset( le_size_t const le_offset, le_size_t const le_index, le_file_t const le_stream ) {

        /* stream offset */
        if ( fseek( le_stream, le_offset + LE_POLY_HEADER, SEEK_SET ) != 0 ) {

            /* critical error tracking */
            # ifdef _LE_FATAL
            fprintf( stderr, "E, C, %s, %d, %li\n", __FILE__, __LINE__, pthread_self() );
            # endif

            /* send message */
            return( LE_ERROR_IO_SEEK );

        } else {

            /* extract and return offset */
            return( le_class_io_offset( le_index, le_stream ) );

        }

    }

