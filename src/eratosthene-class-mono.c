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

    # include "eratosthene-class-mono.h"

/*
    source - constructor/destructor methods
 */

    le_mono_t le_mono_create( le_void_t ) {

        /* created structure variable */
        le_mono_t le_mono = LE_MONO_C;

        /* initialise class descriptor */
        le_class_create( le_mono.mc_data + LE_MONO_HEADER );

        /* return created structure */
        return( le_mono );

    }

    le_void_t le_mono_reset( le_mono_t * const le_mono ) {

        /* reset class descriptor */
        le_class_create( le_mono->mc_data + LE_MONO_HEADER );

        /* reset counter */
        le_mono->mc_size = 0;

        /* reset accumulator */
        le_mono->mc_push[0] = 0.0;
        le_mono->mc_push[1] = 0.0;
        le_mono->mc_push[2] = 0.0;

    }

    le_void_t le_mono_delete( le_mono_t * const le_mono ) {

        /* deleted structure variable */
        le_mono_t le_delete = LE_MONO_C;

        /* delete structure */
        ( * le_mono ) = le_delete;

    }

/*
    source - accessor methods
 */

    le_size_t le_mono_get_offset( le_mono_t const * const le_mono, le_size_t const le_index ) {

        /* extract and return offset */
        return( le_class_get_offset( le_mono->mc_data + LE_MONO_HEADER, le_index ) );

    }

/*
    source - mutator methods
 */

    le_void_t le_mono_set_offset( le_mono_t * const le_mono, le_size_t const le_index, le_size_t const le_offset ) {

        /* assign offset */
        le_class_set_offset( le_mono->mc_data + LE_MONO_HEADER, le_index, le_offset );

    }

    le_void_t le_mono_set_push( le_mono_t * const le_mono, le_byte_t const * const le_data ) {

        /* push components */
        le_mono->mc_push[0] += ( le_real_t ) le_data[0];
        le_mono->mc_push[1] += ( le_real_t ) le_data[1];
        le_mono->mc_push[2] += ( le_real_t ) le_data[2];

        /* update counter */
        le_mono->mc_size ++;

    }

/*
    source - i/o methods
 */

    le_enum_t le_mono_io_read( le_mono_t * const le_mono, le_size_t const le_offset, le_file_t const le_stream ) {

        /* stream offset */
        if ( fseek( le_stream, le_offset, SEEK_SET ) != 0 ) {

            /* critical error tracking */
            # ifdef _LE_CRITICAL
            fprintf( stderr, "E, C, %s, %d, %li\n", __FILE__, __LINE__, pthread_self() );
            # endif

            /* send message */
            return( LE_ERROR_IO_SEEK );

        } else {

            /* read class extended header */
            if ( fread( le_mono->mc_data, sizeof( le_byte_t ), LE_MONO_EXTEND, le_stream ) != LE_MONO_EXTEND ) {

                /* critical error tracking */
                # ifdef _LE_CRITICAL
                fprintf( stderr, "E, C, %s, %d, %li\n", __FILE__, __LINE__, pthread_self() );
                # endif

                /* send message */
                return( LE_ERROR_IO_READ );

            } else {

                /* read class and broadcast message */
                return( le_class_io_read( le_mono->mc_data + LE_MONO_HEADER, le_stream ) );

            }

        }

    }

    le_enum_t le_mono_io_read_fast( le_mono_t * const le_mono, le_size_t const le_offset, le_file_t const le_stream ) {

        /* stream offset */
        if ( fseek( le_stream, le_offset + LE_MONO_HEADER, SEEK_SET ) != 0 ) {

            /* critical error tracking */
            # ifdef _LE_CRITICAL
            fprintf( stderr, "E, C, %s, %d, %li\n", __FILE__, __LINE__, pthread_self() );
            # endif

            /* send message */
            return( LE_ERROR_IO_SEEK );

        } else {

            /* read class descriptor */
            if ( fread( le_mono->mc_data + LE_MONO_HEADER, sizeof( le_byte_t ), LE_CLASS_HEADER, le_stream ) != LE_CLASS_HEADER ) {

                /* critical error tracking */
                # ifdef _LE_CRITICAL
                fprintf( stderr, "E, C, %s, %d, %li\n", __FILE__, __LINE__, pthread_self() );
                # endif

                /* send message */
                return( LE_ERROR_IO_READ );

            } else {

                /* read class and broadcast message */
                return( le_class_io_read( le_mono->mc_data + LE_MONO_HEADER, le_stream ) );

            }

        }

    }

    le_enum_t le_mono_io_write( le_mono_t * const le_mono, le_size_t const le_offset, le_file_t const le_stream ) {

        /* check offset */
        if ( le_offset != _LE_OFFS_NULL ) {

            /* stream offset */
            if ( fseek( le_stream, le_offset, SEEK_SET ) != 0 ) {

                /* critical error tracking */
                # ifdef _LE_CRITICAL
                fprintf( stderr, "E, C, %s, %d, %li\n", __FILE__, __LINE__, pthread_self() );
                # endif

                /* send message */
                return( LE_ERROR_IO_SEEK );

            }

        }

        /* compose class data */
        le_mono->mc_data[0] = le_mono->mc_push[0] / ( le_real_t ) le_mono->mc_size;
        le_mono->mc_data[1] = le_mono->mc_push[1] / ( le_real_t ) le_mono->mc_size;
        le_mono->mc_data[2] = le_mono->mc_push[2] / ( le_real_t ) le_mono->mc_size;

        /* write class extended header */
        if ( fwrite( le_mono->mc_data, sizeof( le_byte_t ), LE_MONO_EXTEND, le_stream ) != LE_MONO_EXTEND ) {

            /* critical error tracking */
            # ifdef _LE_CRITICAL
            fprintf( stderr, "E, C, %s, %d, %li\n", __FILE__, __LINE__, pthread_self() );
            # endif

            /* send message */
            return( LE_ERROR_IO_WRITE );

        } else {

            /* write class and broadcast message */
            return( le_class_io_write( le_mono->mc_data + LE_MONO_HEADER, le_stream ) );

        }

    }

    le_size_t le_mono_io_offset( le_size_t const le_offset, le_size_t const le_index, le_file_t const le_stream ) {

        /* stream offset */
        if ( fseek( le_stream, le_offset + LE_MONO_HEADER, SEEK_SET ) != 0 ) {

            /* critical error tracking */
            # ifdef _LE_CRITICAL
            fprintf( stderr, "E, C, %s, %d, %li\n", __FILE__, __LINE__, pthread_self() );
            # endif

            /* send message */
            return( LE_ERROR_IO_SEEK );

        } else {

            /* extract and return offset */
            return( le_class_io_offset( le_index, le_stream ) );

        }

    }

    le_enum_t le_mono_io_data( le_size_t const le_offset, le_byte_t * const le_data, le_file_t const le_stream ) {

        /* stream offset */
        if ( fseek( le_stream, le_offset, SEEK_SET ) != 0 ) {

            /* critical error tracking */
            # ifdef _LE_CRITICAL
            fprintf( stderr, "E, C, %s, %d, %li\n", __FILE__, __LINE__, pthread_self() );
            # endif

            /* send message */
            return( LE_ERROR_IO_SEEK );

        } else {

            /* import data */
            if ( fread( ( le_void_t * ) le_data, sizeof( le_byte_t ), LE_MONO_HEADER, le_stream ) != LE_MONO_HEADER ) {

                /* critical error tracking */
                # ifdef _LE_CRITICAL
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

