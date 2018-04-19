/*
 *  liberatosthene
 *
 *      Nils Hamel - nils.hamel@bluewin.ch
 *      Copyright (c) 2016-2018 DHLAB, EPFL
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

    # include "eratosthene-class.h"

/*
    source - constructor/destructor methods
 */

    le_class_t le_class_create( le_data_t const * const le_data ) {

        /* created structure variables */
        le_class_t le_class = LE_CLASS_C_DATA( le_data[0], le_data[1], le_data[2] );

        /* initialise offsets */
        memset( le_class.cs_data + _LE_USE_DATA, 0xff, _LE_USE_OFFSET * _LE_USE_BASE );

        /* return constructed structure */
        return( le_class );

    }

    le_void_t le_class_delete( le_class_t * const le_class ) {

        /* deleted structure variables */
        le_class_t le_delete = LE_CLASS_C;

        /* delete structure */
        ( * le_class ) = le_delete;

    }

/*
    source - accessor methods
 */

    le_size_t le_class_get_offset( le_class_t const * const le_class, le_size_t const le_index ) {

        /* extract and return offset */
        return( ( * le_class_mac_offset( le_class, le_index ) ) & _LE_OFFS_NULL );

    }

/*
    source - mutator methods
 */

    le_void_t le_class_set_offset( le_class_t * const le_class, le_size_t const le_index, le_size_t const le_offset ) {

        /* pointer variable */
        le_size_t * le_base = le_class_mac_offset( le_class, le_index );

        /* assign offset */
        ( * le_base ) = ( ( * le_base ) & LE_CLASS_MASK ) | le_offset;

    }

    le_void_t le_class_set_push( le_class_t * const le_class, le_data_t const * const le_data ) {

        /* heap variable */
        le_real_t le_heap = ( ( le_real_t ) le_class->cs_data[3] ) + 1.0;

        /* data pseudo-accumulation */
        le_class->cs_data[0] = ( ( le_heap * le_class->cs_data[0] ) + le_data[0] ) / ( le_heap + 1.0 ) + 0.5;
        le_class->cs_data[1] = ( ( le_heap * le_class->cs_data[1] ) + le_data[1] ) / ( le_heap + 1.0 ) + 0.5;
        le_class->cs_data[2] = ( ( le_heap * le_class->cs_data[2] ) + le_data[2] ) / ( le_heap + 1.0 ) + 0.5;

        /* check heap value */
        if ( le_class->cs_data[3] < _LE_BYTE_MAX ) {

            /* update heap value */
            le_class->cs_data[3] ++;

        }

    }

/*
    source - i/o methods
 */

    le_enum_t le_class_io_read( le_class_t * const le_class, le_size_t const le_offset, le_file_t const le_stream ) {

        /* set stream position */
        if ( fseek( le_stream, le_offset, SEEK_SET ) != 0 ) {

            /* send message */
             return( LE_ERROR_IO_SEEK );

        }

        /* read class buffer */
        if ( fread( ( le_void_t * ) le_class->cs_data, 1, LE_CLASS_ARRAY, le_stream ) != LE_CLASS_ARRAY ) {

            /* send message */
            return( LE_ERROR_IO_READ );

        } else {

            /* send message */
            return( LE_ERROR_SUCCESS );

        }

    }

    le_enum_t le_class_io_readf( le_class_t * const le_class, le_size_t const le_offset, le_file_t const le_stream ) {

        /* set stream position */
        if ( fseek( le_stream, le_offset + _LE_USE_DATA, SEEK_SET ) != 0 ) {

            /* send message */
             return( LE_ERROR_IO_SEEK );

        }

        /* read class buffer */
        if ( fread( ( le_void_t * ) ( le_class->cs_data + _LE_USE_DATA ), 1, LE_CLASS_OFFSET, le_stream ) != LE_CLASS_OFFSET ) {

            /* send message */
            return( LE_ERROR_IO_READ );

        } else {

            /* send message */
            return( LE_ERROR_SUCCESS );

        }

    }

    le_enum_t le_class_io_write( le_class_t const * const le_class, le_size_t const le_offset, le_file_t const le_stream ) {

        /* set stream position */
        if ( fseek( le_stream, le_offset, SEEK_SET ) != 0 ) {

            /* send message */
            return( LE_ERROR_IO_SEEK );

        }

        /* write class buffer */
        if ( fwrite( ( le_void_t * ) le_class->cs_data, 1, LE_CLASS_ARRAY, le_stream ) != LE_CLASS_ARRAY ) {

            /* send message */
            return( LE_ERROR_IO_WRITE );

        } else {

            /* send message */
            return( LE_ERROR_SUCCESS );

        }

    }

/*
    source - detached methods
 */

    le_size_t le_class_det_offset( le_size_t const le_offset, le_size_t const le_index, le_file_t const le_stream ) {

        /* returned value variable */
        le_size_t le_return = _LE_OFFS_NULL;

        /* set stream position */
        if ( fseek( le_stream, le_offset + _LE_USE_DATA + ( le_index * _LE_USE_OFFSET ), SEEK_SET ) != 0 ) {

            /* send message */
            return( _LE_OFFS_NULL );

        }

        /* read class offset */
        if ( fread( ( le_void_t * ) & le_return, 1, _LE_USE_OFFSET, le_stream ) != _LE_USE_OFFSET ) {

            /* send message */
            return( _LE_OFFS_NULL );

        }

        /* return offset */
        return( le_return );

    }

    le_enum_t le_class_det_data( le_size_t const le_offset, le_data_t * const le_data, le_file_t const le_stream ) {

        /* set stream position */
        if ( fseek( le_stream, le_offset, SEEK_SET ) != 0 ) {

            /* send message */
            return( LE_ERROR_IO_SEEK );

        }

        /* read class data */
        if ( fread( ( le_void_t * ) le_data, 1, 3, le_stream ) != 3 ) {

            /* send message */
            return( LE_ERROR_IO_READ );

        } else {

            /* send message */
            return( LE_ERROR_SUCCESS );

        }

    }

