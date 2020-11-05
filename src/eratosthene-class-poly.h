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

    /*! \file   eratosthene-class-poly.h
     *  \author Nils Hamel <nils.hamel@bluewin.ch>
     *
     *  liberatosthene - class-poly (poly-vertex class)
     */

/*
    header - inclusion guard
 */

    # ifndef __LE_CLASS_POLY__
    # define __LE_CLASS_POLY__

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
    # include "eratosthene-class.h"
    # include "eratosthene-array.h"

/*
    header - external includes
 */

/*
    header - preprocessor definitions
 */

    /* define pseudo-constructor */
    # define LE_POLY_C      { 0, NULL, { 0 } }

    /* define class structure */
    # define LE_POLY_HEADER ( _LE_USE_OFFSET )

    /* define class structure */
    # define LE_POLY_EXTEND ( LE_POLY_HEADER + LE_CLASS_HEADER )

    /* define class structure */
    # define LE_POLY_CLASS  ( LE_CLASS_MEMORY )

    /* define class structure */
    # define LE_POLY_MEMORY ( LE_POLY_HEADER + LE_POLY_CLASS )

    /* define class memory step */
    # define LE_POLY_STEP   ( 1024 )

/*
    header - preprocessor macros
 */

/*
    header - type definition
 */

/*
    header - structures
 */

    /*! \struct le_poly_struct
     *  \brief poly-vertex class structure
     *
     *  This structure holds the definition and required elements to access poly
     *  vertex storage representation in door tree structure.
     *
     *  Three main elements are hold by the structure and its tree structure
     *  storage representation.
     *
     *  The first main element is the amount of element hold by the poly-vertex
     *  class seen as a primitive container. The value is stored in the storage
     *  representation on a \b _LE_USE_OFFSET byte long integer.
     *
     *  The last main element hold by the structure storage representation is
     *  the content of its implicit class that gives access to its daughter
     *  links to classes one scale below. These links are simple offsets managed
     *  by its implicit class.
     *
     *  The second main element hold by the structure is the array containing the
     *  offsets of the primitive stored by the class. These offset corresponds
     *  to offset of the primitive in the kept injected uv3 stream.
     *
     *  The storage representation of one poly-vertex class can then be seen as
     *  follows :
     *
     *      [link_count][implicit class bytes][link_0][link_1] ...
     *
     *  It follows that the storage representation size of a poly-vertex class
     *  is a function of its content.
     *
     *  The first elements of the storage representation are hold in the
     *  \b pc_data array while the seconds, the links, are hold in the dynamic
     *  memory allocation pointed by \b pc_link.
     *
     *  An additional size is also kept in the structure, \b pc_size, that keep
     *  track of the memory allocation size reserved for links to the graphical
     *  primitives. This allows to keep the memory allocation during usage of
     *  the structure.
     *
     *  \var le_poly_struct::pc_size
     *  Size, in link count, of the links array
     *  \var le_poly_struct::pc_link
     *  Base pointer of the links array
     *  \var le_poly_struct::pc_data
     *  Buffer used to read storage class representation (without links array)
     */

    typedef struct le_poly_struct {

        le_size_t   pc_size;
        le_byte_t * pc_link;
        le_byte_t   pc_data[LE_POLY_MEMORY];

    } le_poly_t;

/*
    header - function prototypes
 */

    /*! \brief constructor/destructor methods
     *
     *  This function creates and initialises the content of a poly-vertex class
     *  structure.
     *
     *  The function initialises the structure fields with default values and
     *  uses the implicit class specialised function to initialise its own
     *  content.
     *
     *  \return Returns the created structure
     */

    le_poly_t le_poly_create( le_void_t );

    /*! \brief constructor/destructor methods
     *
     *  This function allows the reset the provided poly-vertex class structure
     *  to restore its state as it would have been created by the pseudo
     *  constructor function \b le_poly_create().
     *
     *  The links array is emptied but its memory allocation is kept.
     *
     *  \param le_poly Poly-vertex class structure
     */

    le_void_t le_poly_reset( le_poly_t * const le_poly );

    /*! \brief constructor/destructor methods
     *
     *  This function deletes the content of the provided poly-vertex class
     *  structure.
     *
     *  The links array is emptied and its memory allocation is released. The
     *  fields of the structure are reset using default values.
     *
     *  \param le_poly Poly-vertex class structure
     */

    le_void_t le_poly_delete( le_poly_t * const le_poly );

    /*! \brief accessor methods
     *
     *  This function allows to query the size of the links array of the
     *  provided poly-vertex class structure. This size corresponds to the
     *  amount of graphical primitives hold by the class.
     *
     *  \param le_poly Poly-vertex class structure
     *
     *  \return Returns the size of the class links array
     */

    le_size_t le_poly_get_size( le_poly_t const * const le_poly );

    /*! \brief accessor methods
     *
     *  This function allows to retrieve the offset value of the elements of the
     *  links array of the provided poly-vertex class structure. These offset
     *  values are usually used to find the graphical primitives definition in
     *  the door storage structure specific uv3 file.
     *
     *  \param le_poly  Poly-vertex class structure
     *  \param le_index Index of the link in the links array
     *
     *  \return Returns the link value found at the desired index
     */

    le_size_t le_poly_get_link( le_poly_t const * const le_poly, le_size_t const le_index );

    /*! \brief accessor methods
     *
     *  This function allows the query the offset value stored in the provided
     *  poly-vertex class structure. The provided index indicates which offset
     *  to return and has to be in the [0,7] range.
     *
     *  The function uses the specialised implicit class function to access the
     *  desired offset.
     *
     *  \param le_poly  Poly-vertex class structure
     *  \param le_index Offset index
     *
     *  \return Returns the extracted offset from the class structure
     */

    le_size_t le_poly_get_offset( le_poly_t const * const le_poly, le_size_t const le_index );

    /*! \brief mutator methods
     *
     *  This function allows to update the size of the primitive links array of
     *  the provided class.
     *
     *  \param le_poly Poly-vertex class structure
     *  \param le_size Size of the link array
     */

    le_void_t le_poly_set_size( le_poly_t * const le_poly, le_size_t const le_size );

    /*! \brief mutator methods
     *
     *  This function allows to update the offset of the provided poly-vertex
     *  class structure using the provided value. The index indicates which
     *  offset to update and has to be in the [0,7] range.
     *
     *  The function uses the specialised implicit class function to update the
     *  desired offset.
     *
     *  \param le_poly   Poly-vertex class structure
     *  \param le_index  Offset index
     *  \param le_offset Offset value
     */

    le_void_t le_poly_set_offset( le_poly_t * const le_poly, le_size_t const le_index, le_size_t const le_offset );

    /*! \brief mutator methods
     *
     *  This function allows to add a link to the provided poly-vertex class
     *  structure links array.
     *
     *  The function checks the state of the links array memory allocation and
     *  proceed to a correction when needed. The provided link is then added to
     *  the end of the array. The size of the links array is also updated by the
     *  function.
     *
     *  \param le_poly Poly-vertex class structure
     *  \param le_link Link value
     *
     *  \return Returns LE_ERROR_SUCCESS on success, an error code otherwise
     */

    le_enum_t le_poly_set_push( le_poly_t * const le_poly, le_size_t const le_link );

    /*! \brief mutator methods
     *
     *  This function is used to update the poly-vertex class structure links
     *  array memory allocation.
     *
     *  The provided size indicates the amount of links required. The function
     *  re-allocate the memory without checking any condition. The remaining
     *  content of the previous memory allocation is kept, accordingly to the
     *  new memory allocation size.
     *
     *  \param le_poly Poly-vertex class structure
     *  \param le_size Size of the desired links array, in links count
     *
     *  \return Returns LE_ERROR_SUCCESS on success, an error code otherwise
     */

    le_enum_t le_poly_set_memory( le_poly_t * const le_poly, le_size_t const le_size );

    /*! \brief mutator methods - missing documentation */

    le_void_t le_poly_set_release( le_poly_t * const le_poly );

    /*! \brief i/o methods
     *
     *  This function is used to read the class representation from a storage
     *  tree structure in the provided poly-vertex class structure. The function
     *  reads the entire representation in the storage structure in one single
     *  reading, including the links array and its implicit class structure.
     *
     *  The provided offset gives the offset in the provided stream to start
     *  the reading to the class representation. The provided stream has to be
     *  an already opened stream in binary read mode.
     *
     *  \param le_poly   Poly-vertex class structure
     *  \param le_offset Class reading offset, in bytes
     *  \param le_stream Steam descriptor
     *
     *  \return Returns LE_ERROR_SUCCESS on success, an error code otherwise
     */

    le_enum_t le_poly_io_read( le_poly_t * const le_poly, le_size_t const le_offset, le_file_t const le_stream );

    /*! \brief i/o methods
     *
     *  This function allows the read a poly-vertex class storage representation
     *  reading only the fixed-length header, that are the links array size and
     *  the descriptor of its implicit class. Its implicit class is then read
     *  using its specialised function.
     *
     *  This function is used for efficiency purpose when process does not need
     *  the read the content of the links array of the class.
     *
     *  The provided offset gives the offset in the provided stream to start
     *  the reading to the class representation. The provided stream has to be
     *  an already opened stream in binary read mode.
     *
     *  \param le_poly   Poly-vertex class structure
     *  \param le_offset Class reading offset, in bytes
     *  \param le_stream Stream descriptor
     *
     *  \return Returns LE_ERROR_SUCCESS on success, an error code otherwise
     */

    le_enum_t le_poly_io_read_fast( le_poly_t * const le_poly, le_size_t const le_offset, le_file_t const le_stream );

    /*! \brief i/o methods
     *
     *  This function allows to read a poly-vertex class storage representation
     *  remaining links array after a call to \b le_poly_io_read_fast().
     *
     *  The function uses the size of the links array, read using the previous
     *  function, and update the provided class structure links array memory
     *  allocation before to read the content of the links array.
     *
     *  The function assumes that the position in the provided stream is where
     *  the previous function left it, that is on the links array first byte.
     *  The provided stream has to be an already opened stream in binary read
     *  mode.
     *
     *  \param le_poly   Poly-vertex class structure
     *  \param le_stream Stream descriptor
     *
     *  \return Returns LE_ERROR_SUCCESS on success, an error code otherwise
     */

    le_enum_t le_poly_io_read_next( le_poly_t * const le_poly, le_file_t const le_stream );

    /*! \brief i/o methods
     *
     *  This function is used to write the provided poly-vertex class structure
     *  storage representation.
     *
     *  The function starts by writing the fixed-length header and implicit
     *  class before to export the content of the links array.
     *
     *  If a null offset is provided, the function assumes that the class has to
     *  be written at the current position of the provided stream. The provided
     *  stream has to be an already opened stream in binary write mode.
     *
     *  \param le_poly   Poly-vertex class structure
     *  \param le_offset Class writing offset, in bytes
     *  \param le_stream Stream descriptor
     *
     *  \return Returns LE_ERROR_SUCCESS on success, an error code otherwise
     */

    le_enum_t le_poly_io_write( le_poly_t const * const le_poly, le_size_t const le_offset, le_file_t const le_stream );

    /*! \brief i/o methods (detached)
     *
     *  Note :
     *
     *  Detached methods are specific functions associated to a structure but
     *  without operating directly on it. Such function are used in optimisation
     *  strategies.
     *
     *  This function allows to read a single offset from a poly-vertex class
     *  representation in a tree storage structure. The function invokes the
     *  specialised implicit class function to read the desired offset.
     *
     *  The provided offset value indicates where to start the representation
     *  reading in the provided stream. The provided stream has to be an already
     *  opened stream in binary read mode.
     *
     *  \param le_offset Class reading offset, in bytes
     *  \param le_index  Class index
     *  \param le_stream Stream descriptor
     *
     *  \return Returns the class desired offset on success, _LE_OFFS_NULL
     *  otherwise
     */

    le_size_t le_poly_io_offset( le_size_t const le_offset, le_size_t const le_index, le_file_t const le_stream );

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

