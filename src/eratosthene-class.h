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

    /*! \file   eratosthene-class.h
     *  \author Nils Hamel <nils.hamel@bluewin.ch>
     *
     *  liberatosthene - class
     */

/*
    header - inclusion guard
 */

    # ifndef __LE_CLASS__
    # define __LE_CLASS__

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
    # include "eratosthene-class-access.h"

/*
    header - external includes
 */

/*
    header - preprocessor definitions
 */

    /* define class structure */
    # define LE_CLASS_HEADER  ( sizeof( le_byte_t ) )

    /* define class structure */
    # define LE_CLASS_TARGET  ( _LE_USE_OFFSET * _LE_USE_BASE )

    /* define class structure */
    # define LE_CLASS_PADDING ( sizeof( le_size_t ) - _LE_USE_OFFSET )

    /* define class structure */
    # define LE_CLASS_ALIGNED ( LE_CLASS_TARGET + LE_CLASS_PADDING )

    /* define class structure */
    # define LE_CLASS_MEMORY  ( LE_CLASS_HEADER + LE_CLASS_ALIGNED )

    /* define configuration count */
    # define LE_CLASS_COUNT   ( 1 << _LE_USE_BASE )

/*
    header - preprocessor macros
 */

    /* define cast macro */
    # define le_class_mac_cast( o ) ( * ( ( le_size_t * ) ( o ) ) )

/*
    header - type definition
 */

    /*! \brief generic implicit class structure
     *
     *  Implicit class structures are used for both mono-vertex and poly-vertex
     *  storage strategy. They allows the management of the mono and poly-vertex
     *  class offset storage.
     *
     *  The main role of implicit class structure is to hold, write, read and
     *  give access to the management of the class offsets. They are implicitly
     *  constituted of a sequence of bytes storing the required information. It
     *  follows that these classes are not defined through a proper structure
     *  due to their simplicity, explaining why their are called implicit.
     *
     *  The byte structure of the class is architectures as follows :
     *
     *      [descriptor][offset_i]...[offset_j]
     *
     *  The class descriptor holds a bit pattern giving the content of the class
     *  itself. If the first bit is set to one, it indicates that the first link
     *  offset is active in the class. The second bit give the same information
     *  for the second offset. Doing so allows to only store the active offset,
     *  the other being implicitly assumed to be null (\b _LE_OFFS_NULL).
     *
     *  To access a class offset, the descriptor has to be read in the first
     *  place to determine if the offset is active. Depending of the content of
     *  the descriptor, optimised array are used to determine the position of
     *  the offset before to read it.
     *
     *  If an offset has to be set, the descriptor has to be read in the first
     *  place to determine if the offset is already active in the class. If not,
     *  the content of the class has to be modified to be able to insert the
     *  offset at the correct position. Again, optimised arrays are used for
     *  such operation.
     *
     *  The descriptor is stored on a single byte while the active offset values
     *  are coded on a number of byte given by the \b _LE_USE_OFFSET definition.
     *
     *  Considering such classes in common for both mono and poly-vertex allows
     *  to implement the same optimisation that will affect both storage
     *  strategy all at once. This also allows to avoid code redundancy as mono
     *  and poly-vertex works in the same way in term of managing the access
     *  offsets.
     */

    /* implicit class structure */
    typedef le_byte_t le_class_t;

/*
    header - structures
 */

/*
    header - function prototypes
 */

    /*! \brief constructor/destructor methods
     *
     *  This function initialise the bytes structure of the provided implicit
     *  class structure.
     *
     *  The function simply sets the class descriptor to zero indicating that
     *  the class does not store any offset.
     *
     *  \param le_class Class bytes structure
     */

    le_void_t le_class_create( le_class_t * const le_class );

    /*! \brief accessor methods
     *
     *  This function allows to get the offset stored in the provided implicit
     *  class structure.
     *
     *  The function starts by reading the class descriptor to detect if the
     *  required offset, pointed by its index \b le_index, is available or not.
     *  As the offset is available, the function returns its value.
     *
     *  If the desired offset is missing in the class, the function returns the
     *  \b _LE_OFFS_NULL value.
     *
     *  \param le_class Class bytes structure
     *  \param le_index Offset index
     *
     *  \return Return the class offset on success, _LE_OFFS_NULL otherwise
     */

    le_size_t le_class_get_offset( le_byte_t const * le_class, le_size_t const le_index );

    /*! \brief mutator methods
     *
     *  This function allows to set the value of the offset of the provided
     *  class of index \b le_index.
     *
     *  The function starts by checking if the offset is already present in the
     *  class. In such case, its value is simply overridden.
     *
     *  If the offset is not available in the provided class, the function
     *  starts by inserting the offset moving the previous content, when
     *  necessary.
     *
     *  \param le_class  Class bytes structure
     *  \param le_index  Offset index
     *  \param le_offset Offset value
     *
     *  \return Returns the class offset on success, _LE_OFFS_NULL otherwise
     */

    le_void_t le_class_set_offset( le_class_t * le_class, le_size_t const le_index, le_size_t const le_offset );

    /*! \brief i/o methods
     *
     *  This function is used to read the content of the provided implicit class
     *  structure from the provided stream.
     *
     *  The function assumes, for subsequent efficiency reason, that the class
     *  descriptor has already been read and stored in the provided class. The
     *  function then reads the remaining elements of the class based on the
     *  value of the descriptor.
     *
     *  The provided stream has to be an already open stream in binary read
     *  mode.
     *
     *  \param le_class  Class byte structure
     *  \param le_stream Stream descriptor
     *
     *  \return Returns LE_ERROR_SUCCESS on success, an error code otherwise
     */

    le_enum_t le_class_io_read( le_class_t * le_class, le_file_t const le_stream );

    /*! \brief i/o methods
     *
     *  This function is used to write the content of the provided implicit
     *  class structure un the provided stream.
     *
     *  The function assumes, for subsequent efficiency reason, that the class
     *  descriptor has already been written in the provided stream. The function
     *  then writes the remaining elements of the class based on the exported
     *  descriptor.
     *
     *  The provided stream has to be an already open stream in binary write
     *  mode.
     *
     *  \param le_class  Class byte structure
     *  \param le_stream Stream descriptor
     *
     *  \return Returns LE_ERROR_SUCCESS on success, an error code otherwise
     */

    le_enum_t le_class_io_write( le_byte_t const * le_class, le_file_t const le_stream );

    /*! \brief i/o methods (detached)
     *
     *  Note :
     *
     *  Detached methods are specific functions associated to a structure but
     *  without operating directly on it. Such function are used in optimisation
     *  strategies.
     *
     *  This function allows to retrieve the offset value of a class stored in
     *  the provided stream. This function allows to read a single offset out of
     *  a storage structure without having to read to entire class.
     *
     *  The function starts by reading the implicit class structure descriptor
     *  from the provided stream. The availability of the offset of index
     *  \b le_index is then checked. If the offset is available, the function
     *  reads and returns it.
     *
     *  On error or if the offset is not available, the function returns the
     *  \b _LE_OFFS_NULL value.
     *
     *  The function assumes that the provided stream is an already open stream
     *  in binary read mode and that the reading offset of the stream is on the
     *  implicit class descriptor.
     *
     *  \param le_index Offset index
     *  \param le_stream Stream descriptor
     *
     *  \return Returns the class offset on success, _LE_OFFS_NULL otherwise
     */

    le_size_t le_class_io_offset( le_size_t const le_index, le_file_t const le_stream );

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

