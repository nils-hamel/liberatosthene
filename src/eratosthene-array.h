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

    /*! \file   eratosthene-array.h
     *  \author Nils Hamel <nils.hamel@bluewin.ch>
     *
     *  liberatosthene - array
     */

/*
    header - inclusion guard
 */

    # ifndef __LE_ARRAY__
    # define __LE_ARRAY__

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

/*
    header - external includes
 */

/*
    header - preprocessor definitions
 */

    /* define pseudo-constructor */
    # define LE_ARRAY_C               { 0, NULL, 0, NULL }

    /* define array step */
    # define LE_ARRAY_STEP            ( 65536 )

    /* define array header size */
    # define LE_ARRAY_HEADER_SIZE     ( sizeof( le_size_t ) )
    # define LE_ARRAY_HEADER_MODE     ( sizeof( le_byte_t ) )
    # define LE_ARRAY_HEADER          ( LE_ARRAY_HEADER_SIZE + LE_ARRAY_HEADER_MODE )

    /* define array-size - authorise */
    # define LE_ARRAY_AUTH            ( sizeof( le_size_t ) + sizeof( le_time_t ) )

    /* define array-size - inject */
    # define LE_ARRAY_INJE            ( sizeof( le_time_t ) )

    /* define array-size - optimise */
    # define LE_ARRAY_OPTM            ( sizeof( le_time_t ) )

    /* define array-size - address */
    # define LE_ARRAY_ADDR_TIME       ( sizeof( le_time_t ) * _LE_USE_TIMES )
    # define LE_ARRAY_ADDR_DESC       ( sizeof( le_byte_t ) * 3 )
    # define LE_ARRAY_ADDR            ( LE_ARRAY_ADDR_TIME + LE_ARRAY_ADDR_DESC + ( _LE_USE_DEPTH >> 1 ) )

    /* define array-size - uv3 */
    # define LE_ARRAY_DATA_POSE       ( sizeof( le_real_t ) * 3 )
    # define LE_ARRAY_DATA_TYPE       ( sizeof( le_data_t ) )
    # define LE_ARRAY_DATA_DATA       ( sizeof( le_data_t ) * 3 )
    # define LE_ARRAY_DATA            ( LE_ARRAY_DATA_POSE + LE_ARRAY_DATA_TYPE + LE_ARRAY_DATA_DATA )

/*
    header - preprocessor macros
 */

    /* define stack macro */
    # define le_array_mac_create(p,s) { for ( le_size_t _i_ = 0; _i_ < s; _i_ ++ ) p[_i_] = le_array_create(); }

    /* define stack macro */
    # define le_array_mac_delete(p,s) { for ( le_size_t _i_ = 0; _i_ < s; _i_ ++ ) le_array_delete( p + _i_ ); }

    /* define array access macro */
    # define le_array_mac_byte(a,i)   ( ( a )->ar_vbyte + i )

    /* define array access macro */
    # define le_array_mac_back(a,s)   ( ( a )->ar_vbyte + ( a )->ar_vsize - ( s ) )

    /* define array access macro */
    # define le_array_mac_pose(a,i)   ( ( le_real_t * ) ( ( a )->ar_vbyte + i ) )

    /* define array access macro */
    # define le_array_mac_type(a,i)   ( ( le_data_t * ) ( ( a )->ar_vbyte + i + LE_ARRAY_DATA_POSE ) )

    /* define array access macro */
    # define le_array_mac_data(a,i)   ( ( le_data_t * ) ( ( a )->ar_vbyte + i + LE_ARRAY_DATA_POSE + LE_ARRAY_DATA_TYPE ) )

    /* define array access macro */
    # define le_array_mac_lpose(a)    ( ( le_real_t * ) ( ( a )->ar_vbyte + ( a )->ar_vsize - LE_ARRAY_DATA ) )

    /* define array access macro */
    # define le_array_mac_ltype(a)    ( ( le_data_t * ) ( ( a )->ar_vbyte + ( a )->ar_vsize - LE_ARRAY_DATA_DATA - LE_ARRAY_DATA_TYPE ) )

    /* define array access macro */
    # define le_array_mac_ldata(a)    ( ( le_data_t * ) ( ( a )->ar_vbyte + ( a )->ar_vsize - LE_ARRAY_DATA_DATA ) )

/*
    header - type definition
 */

/*
    header - structures
 */

    /*! \struct le_array_struct
     *  \brief array structure
     *
     *  This structure holds the definition and content of a bytes array. These
     *  arrays are commonly used to transmit data through TCP/IP sockets. These
     *  arrays of bytes are commonly interpreted through serialisation, access
     *  macro and specialised functions.
     *
     *  More specifically, the structure holds an array in an array. The first
     *  array, corresponding to the allocated memory, has a virtual size usually
     *  greater than the actual data array size contain in it.
     *
     *  A second array is defined through its base pointer (\b ar_vbyte) and its
     *  size (\b ar_vsize). This pseudo array contains the proper data. From
     *  a memory point of view, one can represent the data array as follows :
     *
     *      R...RD...DR...R
     *
     *  with R designating bytes of the memory array and D the bytes of the data
     *  array contained in it.
     *
     *  This is done so for two main reasons : the first one is the possibility
     *  to have a header in which the sizes and array mode can be packed and
     *  sent over TCP/IP without additional memory allocation and copy. The
     *  second reason is the possibility to only reallocate memory when the
     *  real array is not sufficient to holds the data array, again saving
     *  memory allocation and copies.
     *
     *  When the array is written on TCP/IP socket, the header is filled as
     *  follows :
     *
     *      [ar_vsize][mode][array_data] ... [array_data]
     *
     *  As the array is received by the remote computer, the header contains
     *  all the information needed to rebuild the array.
     *
     *  As an array is written or read from a socket, it is always associated
     *  to a transfer mode value. This value is used to define the type of the
     *  data sent through the array and to perform consistency checks.
     *
     *  \var le_array_struct::ar_rsize
     *  Array real memory size, in bytes
     *  \var le_array_struct::ar_rbyte
     *  Array real memory base pointer
     *  \var le_array_struct::ar_vsize
     *  Size, in bytes, of the data stored in the array
     *  \var le_array_struct::ar_vbyte
     *  Base pointer of the data stored in the array
     */

    typedef struct le_array_struct {

        le_size_t   ar_rsize;
        le_byte_t * ar_rbyte;

        le_size_t   ar_vsize;
        le_byte_t * ar_vbyte;

    } le_array_t;

/*
    header - function prototypes
 */

    /*! \brief constructor/destructor methods
     *
     *  This function initialise and returns an empty socket-array structure.
     *  This function is mandatory before usage of array method on any array
     *  structure.
     *
     *  \return Returns the created structure
     */

    le_array_t le_array_create( le_void_t );

    /*! \brief constructor/destructor methods
     *
     *  This function un-initialises the provided array structure. It checks the
     *  memory allocation state and release it when necessary. The fields of the
     *  structure are then cleared using default values.
     *
     *  \param le_array Array structure
     */

    le_void_t le_array_delete( le_array_t * const le_array );

    /*! \brief accessor methods
     *
     *  This function returns the size, in bytes, of the data stored in the
     *  provided array structure.
     *
     *  \param le_array Array structure
     *
     *  \return Array data size, in bytes
     */

    le_size_t le_array_get_size( le_array_t const * const le_array );

    /*! \brief accessor methods
     *
     *  This function returns the array data memory base pointer.
     *
     *  \param le_array Array structure
     *
     *  \return Returns data array memory base pointer
     */

    le_byte_t * le_array_get_byte( le_array_t const * const le_array );

    /*! \brief mutator methods
     *
     *  This function is used to build the array header part. It packs the array
     *  size in the header reserved field. In addition, the function also packs
     *  the array transfer mode value in the header.
     *
     *  This function is usually used before to write the array on a specific
     *  socket.
     *
     *  \param le_array Array structure
     *  \param le_mode  Array transfer mode
     */

    le_void_t le_array_set_header( le_array_t * const le_array, le_byte_t const le_mode );

    /*! \brief mutator methods
     *
     *  This function extract the array size and transfer mode from the header
     *  of the provided socket-array. It sets back the field of the array
     *  structure accordingly to the values extracted from the header. It also
     *  re-allocate the socket-array memory according to the extracted size.
     *  The function returns the socket-array transfer mode.
     *
     *  This function is usually used while a socket-array is read from a given
     *  socket.
     *
     *  \param le_array Array structure
     *
     *  \return Returns the read socket-array transfer mode
     */

    le_byte_t le_array_set_array( le_array_t * const le_array );

    /*! \brief mutator methods
     *
     *  This function checks if the memory size of the provided array structure
     *  allows the insertion of an element of size \b le_length, given in bytes.
     *
     *  If the memory size allows the insertion, the function update the array
     *  data size. Otherwise, it updates the size of the array and also the data
     *  size. In this case, the array memory is re-allocated by the function
     *  according to the updated memory size.
     *
     *  When the memory allocation of the array is increased, this function
     *  always uses default increasing step value in order to minimise the
     *  amount of required memory re-allocations through time.
     *
     *  \param le_array  Array structure
     *  \param le_length Length, in bytes, of inserted element
     *
     *  \return Return _LE_TRUE on success, _LE_FALSE otherwise
     */

    le_enum_t le_array_set( le_array_t * const le_array, le_size_t le_length );

    /*! \brief mutator methods
     *
     *  This function is used to force the data size of the provided array to a
     *  specific value. The function checks if the memory size remains greater
     *  than the specified size. In this case, the array memory allocation is
     *  not changed. Otherwise, the memory size of the array is aligned on the
     *  provided size and the function reallocates the array memory.
     *
     *  \param le_array  Array structure
     *  \param le_size   Size of the data array, in bytes
     *
     *  \return Return _LE_TRUE on success, _LE_FALSE otherwise
     */

    le_enum_t le_array_set_size( le_array_t * const le_array, le_size_t const le_size );

    /*! \brief mutator methods
     *
     *  This function allows to append the content of the second array at the
     *  end of the first one. The first array size is increased according to
     *  the second array size and the content of the second array is append at
     *  the end of the first one.
     *
     *  The second array is left unchanged. As the size of the first array is
     *  modified, its memory allocation is modified (base pointer).
     *
     *  \param le_array  First array structure
     *  \param le_append Second array structure
     */

    le_void_t le_array_set_append( le_array_t * const le_array, le_array_t const * const le_append );

    /*! \brief serialisation methods
     *
     *  This function is used to pack or unpack atomic variables in the data
     *  array stored in the provided array structure. It receives a variable and
     *  its size and pack or unpack it in the data array at the specified offset
     *  according to the provided mode.
     *
     *  \param le_array  Array structure
     *  \param le_bytes  Pointer to the atomic variable to pack/unpack
     *  \param le_length Size, in bytes, of the atomic variable to pack/unpack
     *  \param le_offset Pack/unpack offset in the data array, in bytes
     *  \param le_mode   Serialisation mode (_LE_SET or _LE_GET)
     *
     *  \return Offset of the byte following the variable in the data array
     */

    le_size_t le_array_serial( le_array_t * const le_array, le_void_t * const le_bytes, le_size_t const le_length, le_size_t const le_offset, le_enum_t const le_mode );

    /*! \brief i/o methods
     *
     *  This function writes the provided array bytes in the socket pointed by
     *  the provided socket descriptor. This function also sets the array
     *  structure header before to send it over TCP/IP.
     *
     *  In addition to the data array size, the header also contains a transfer
     *  mode value to indicate the type of data carried by the array. This mode
     *  value is set according to the provided \b le_mode parameter.
     *
     *  \param le_array  Array structure
     *  \param le_mode   Array tranfert mode
     *  \param le_socket Socket descriptor
     *
     *  \return Returns the array mode on success, LE_MODE_NULL otherwise
     */

    le_byte_t le_array_io_write( le_array_t * const le_array, le_byte_t le_mode, le_sock_t const le_socket );

    /*! \brief i/o methods
     *
     *  This function reads the provided array bytes from the socket pointed by
     *  the provided socket descriptor. The reading is based on the analysis
     *  of the read array header and the received data.
     *
     *  A timeout condition is implemented for transmission failures. As a chunk
     *  read fails, the function retries the read until it succeed or until the
     *  timeout is reached.
     *
     *  \param le_array  Array structure
     *  \param le_socket Socket descriptor
     *
     *  \return Returns the array mode on success, LE_MODE_NULL otherwise
     */

    le_byte_t le_array_io_read( le_array_t * const le_array, le_sock_t const le_socket );

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

