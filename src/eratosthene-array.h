/*
 *  liberatosthene - geodetic system
 *
 *      Nils Hamel - nils.hamel@bluewin.ch
 *      Copyright (c) 2016 EPFL CDH DHLAB
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

    /*! \file   eratosthene-array.h
     *  \author Nils Hamel <n.hamel@bluewin.ch>
     *
     *  Array module
     */

/*
    header - inclusion guard
 */

    # ifndef __LE_SYSTEM_ARRAY__
    # define __LE_SYSTEM_ARRAY__

/*
    header - C/C++ compatibility
 */

    # ifdef __cplusplus
    extern "C" {
    # endif

/*
    header - includes
 */

    # include "eratosthene.h"
    # include "eratosthene-network.h"

/*
    header - preprocessor definitions
 */

    /* Define pseudo-constructor */
    # define LE_ARRAY_C    { 0, 0, NULL }

    /* Define array entry length (bytes) */
    # define LE_ARRAY_LINE ( ( sizeof( le_real_t ) + sizeof( le_data_t ) ) * 3 + sizeof( le_time_t ) )

    /* Define array step (bytes) */
    # define LE_ARRAY_STEP ( 4096 )

/*
    header - preprocessor macros
 */

/*
    header - type definition
 */

/*
    header - structures
 */

    /*! \struct le_array_struct
     *  \brief array structure
     *
     *  This structure simply holds the definition of an unidemensional array
     *  storing four dimensionnal colorimetric points.
     *
     *  The byte array memory pointed by the structure field is arranged in
     *  successive lines containing seven values each : three spatial 
     *  components, a time component and three colorimetric components.
     *
     *  The spatial components are stored using \b le_real_t type when time is
     *  coded using \b le_time_t type. The three colorimetric components are
     *  stored using the \b le_data_t type.
     *
     *  In addition to the array size, giving the number of colorimetric points
     *  stored in the array, the structure holds an asynchronous size used to
     *  reduce the amount of memory dynamic reallocation.
     *
     *  \var le_array_struct::ar_vsze
     *  Array asynchronous size, in number of points
     *  \var le_array_struct::ar_size
     *  Array size, in number of points
     *  \var le_array_struct::ar_byte
     *  Pointer to array bytes.
     */

    typedef struct le_array_struct {

        le_size_t   ar_vsze;
        le_size_t   ar_size;
        le_byte_t * ar_byte;

    } le_array_t;

/*
    header - function prototypes
 */

    /*! \brief constructor/destructor methods
     *
     *  This function simply returns an array structure initialised with default
     *  values.
     *
     *  \return Returns initialised structure
     */

    le_array_t le_array_create( le_void_t );

    /*! \brief constructor/destructor methods
     *
     *  This function uninitialise the provided array structure. It unallocates
     *  memory when required and clears the structure field with default values.
     *
     *  \param le_array Array structure
     */

    le_void_t le_array_delete( le_array_t * const le_array );

    /*! \brief accessor methods
     *
     *  Returns the size, in element count, of the array contained in the array
     *  structure.
     *
     *  \param le_array Array structure
     *
     *  \return Array element count
     */

    le_size_t le_array_get_size( le_array_t const * const le_array );

    /*! \brief accessor methods
     *
     *  Returns the pointer to bytes of the array contained in the array
     *  structure.
     *
     *  \param le_array Array structure
     *
     *  \return Returns pointer to array first byte
     */

    le_byte_t * le_array_get_byte( le_array_t const * const le_array );

    /*! \brief mutator methods
     *
     *  This function allows to push a new element in the array contained in
     *  the array structure.
     *
     *  \param le_array Array structure
     *  \param le_pose  Array containing the point position
     *  \param le_time  Time of the point
     *  \param le_data  Array containing the point color
     *
     *  \return Returns LE_ERROR_SUCCESS on success, LE_ERROR_MEMORY on memory
     *  allocation failure
     */

    le_enum_t le_array_set_push( le_array_t * const le_array, le_real_t const * const le_pose, le_time_t const le_time, le_data_t const * const le_data );

    /*! \brief i/o methods
     *
     *  This function writes the byte content of the array structure in the
     *  provided socket. The socket has to be an already opened socket.
     *
     *  \param le_array  Array structure
     *  \param le_socket Opened socket
     *
     *  \return Returns LE_ERROR_SUCCESS on sucess, LE_ERROR_IO_SOCKET for an
     *  invalid socket and LE_ERROR_IO_WRITE on socket i/o failure.
     */

    le_enum_t le_array_io_write( le_array_t const * const le_array, le_sock_t const le_socket );

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

