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

    /*! \file   eratosthene-uv3.h
     *  \author Nils Hamel <nils.hamel@bluewin.ch>
     *
     *  liberatosthene - uv3
     */

/*
    header - inclusion guard
 */

    # ifndef __LE_UV3__
    # define __LE_UV3__

/*
    header - C/C++ compatibility
 */

    # ifdef __cplusplus
    extern "C" {
    # endif

/*
    header - internal includes
 */

    # include "eratosthene.h"
    # include "eratosthene-address.h"

/*
    header - external includes
 */

/*
    header - preprocessor definitions
 */

    /* define uv3 primitive */
    # define LE_UV3_POINT         ( 1 )
    # define LE_UV3_LINE          ( 2 )
    # define LE_UV3_TRIANGLE      ( 3 )

    /* define standard chunk size */
    # define LE_UV3_CHUNK         ( 2396745 )

/*
    header - preprocessor macros
 */

    /* uv3 record access */
    # define le_uv3_get_type( r ) ( * ( ( le_byte_t * ) ( r + LE_ARRAY_DATA_POSE ) ) )

/*
    header - type definition
 */

/*
    header - structures
 */

/*
    header - function prototypes
 */

    /*! \brief accessor methods
     *
     *  This function allows to copy the spatial coordinates of an uv3 record in
     *  the provided vector.
     *
     *  The \b le_record pointer has to point to a valid uv3 record first byte  
     *  and the \b le_pose has to be the base-pointer of a 3-value vector.
     *
     *  \param le_record Record pointer
     *  \param le_pose   Coordinates vector
     */

    le_void_t le_uv3_get_record_pose( le_byte_t const * const le_record, le_real_t * const le_pose );

    /*! \brief mutator methods (implicit)
     */

    le_void_t le_uv3_set_sort_circle( le_byte_t * const le_primitive, le_size_t const le_type, le_size_t const le_scfg );

    /*! \brief mutator methods (implicit)
     */

    le_byte_t * le_uv3_set_sort_primitive( le_byte_t * const le_buffer, le_size_t const le_size, le_size_t const le_scfg );

    /*! \brief mutator methods (implicit)
     *
     *  Note :
     *
     *  Implicit methods are similar to the detached methods in the way the
     *  subsequent structure is not explicitly defined. In this case, the
     *  structure is replaced by data implicitly provided in a specific format.
     *
     *  Note :
     *
     *  Both \b le_uv3_set_sort() and \b le_uv3_set_merge() implement a global
     *  merge-sort algorithm adapted to massive uv3 files.
     *
     *  This function implements a merge sort algorithm on uv3 data array. The
     *  comparison is made possible on three dimensional elements as the
     *  comparison function is made on the corresponding spatial index of each
     *  elements.
     *
     *  A few constraints have to be fulfilled concerning the input uv3 data
     *  array. In the first place the poly-vertex have to be specified in a
     *  single rows, with no other data between their vertex (such as mono
     *  vertex elements). The second constrain is that poly-vertex can not be
     *  truncated at the end of the uv3 data.
     *
     *  The function implements a primitive type agnostic merging process in the
     *  way mono-vertex and poly-vertex are not treated separately. In case of
     *  poly-vertex, the first vertex is considered as the relevant vertex for
     *  index-based comparison and sorting. The remaining vertex are ignored
     *  from the comparison point of view and the poly-vertex primitives are
     *  sorted as single blocs.
     *
     *  The result of the index-based merge-sort process is a uv3 data array,
     *  with the same size as the input uv3 data array, but containing mono and
     *  poly-vertex sorted from their spatial index point of view.
     *
     *  This function is usually used prior to data storage structure creation
     *  as data are injected in the server. This allows more efficient processes
     *  to take place and it also improves the efficiency with which data are
     *  accessed.
     *
     *  Two sizes are provided to the function that are the actual size, in
     *  bytes of the input uv3 chunk and the size of the memory allocation that
     *  holds the data. This allows to have larger memory allocation than the
     *  data it holds and having a function that always returns an array
     *  contained in a memory allocation that have the same size as the input
     *  one.
     *
     *  \param le_buffer Pointer to the first byte of the uv3 data
     *  \param le_size   Size of the uv3 data, in bytes
     *  \param le_memory Size of the uv3 data memory allocation, in bytes
     *  \param le_scfg   Server spatial configuration parameter
     *
     *  \return Returns a pointer to the resulting data or a NULL pointer
     */

    le_byte_t * le_uv3_set_sort( le_byte_t * const le_buffer, le_size_t const le_size, le_size_t le_memory, le_size_t const le_scfg );

    /*! \brief mutator methods (implicit)
     *
     *  Note :
     *
     *  Implicit methods are similar to the detached methods in the way the
     *  subsequent structure is not explicitly defined. In this case, the
     *  structure is replaced by data implicitly provided in a specific format.
     *
     *  Note :
     *
     *  Both \b le_uv3_set_sort() and \b le_uv3_set_merge() implement a global
     *  merge-sort algorithm adapted to massive uv3 files.
     *
     *  This function implements a merge sort algorithm at the level of large
     *  files. It implements the same merge sort algorithm as the function
     *  \b le_uv3_set_sort() but considering two files as input rather than
     *  a single memory buffer. In this way, it implements the continuation of
     *  the merge sort process of the previous function.
     *
     *  The function reads the two input files uv3 records and exports them in
     *  the output uv3 files respecting the comparison performed on the spatial
     *  index to obtain a unique sorted file from the two input files. It
     *  assumes then that the content of the two input files is already sorted,
     *  usually using the previous function.
     *
     *  Using this function and the \b le_uv3_set_sort() function allows to sort
     *  very large uv3 files, based on the spatial index comparison, without
     *  consuming large amount of memory.
     *
     *  \param le_pfile Path to the first uv3 file
     *  \param le_qfile Path to the second uv3 file
     *  \param le_file  Path to the output uv3 file
     *  \param le_scfg  Server spatial configuration parameter
     *
     *  \return Returns LE_ERROR_SUCCESS on success, an error code otherwise
     */

    le_enum_t le_uv3_set_merge( le_char_t const * const le_pfile, le_char_t const * const le_qfile, le_char_t const * const le_file, le_size_t const le_scfg );

/*
    header - C/C++ compatibility
 */

    # ifdef __cplusplus
    }
    # endif

/*
    header - inclusion guard
 */

    # endif

